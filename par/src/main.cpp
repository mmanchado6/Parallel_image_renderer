#include "image.hpp"
#include <random>
#include <span>
#include <string>

#include "../../common/include/configuration_parser.hpp"
#include "../../common/include/motor_intersecciones.hpp"
#include "../../common/include/scene_parser.hpp"
#include "../include/image_par.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <tuple>
#include <utility>

#include "ray.hpp"
#include "scene.hpp"
#include "vector.hpp"

#include <iostream>
#include <oneapi/tbb/blocked_range2d.h>
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_for.h>
#include <tbb/task_arena.h>

#include <functional>
#include <memory>
#include <tbb/partitioner.h>
#include <thread>

namespace {

  struct Config {
    std::string config_file;
    std::string scene_file;
    std::string output_file;
    int num_threads         = 128;
    std::string partitioner = "simple";
    size_t grain_size       = 2;
  };

  struct RenderingContext {
    image::imagePAR image;
    imagec::Pov viewpoint;
    render::scene scene;
    imagec::imageConfig config;
  };

  struct RenderParams {
    render::point O{0, 0, 0};
    render::vector delta_x{0, 0, 0};
    render::vector delta_y{0, 0, 0};
    render::point cam_pos{0, 0, 0};
    render::scene const * scene        = nullptr;
    imagec::imageConfig const * config = nullptr;
    int samples                        = 0;
  };

  // Estructura para generadores de números aleatorios por hilo
  struct ThreadLocalRNG {
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> pixel_dist{-0.5, 0.5};

    ThreadLocalRNG(std::uint_fast64_t seed) : rng(seed) { }
  };

  // Estructura para los parámetros de cálculo de color
  struct SampleComputeParams {
    int row;
    int col;
    int samples;
    RenderParams const * params;
    ThreadLocalRNG * thread_rng;

    SampleComputeParams(int row_val, int col_val, int samples_val, RenderParams const & params_ref,
                        ThreadLocalRNG & rng_ref)
        : row(row_val), col(col_val), samples(samples_val), params(&params_ref),
          thread_rng(&rng_ref) { }
  };

  Config parse_arguments(std::span<char *> args) {
    if (args.size() < 4) {
      std::exit(1);
    }

    Config config{std::string(args[1]), std::string(args[2]), std::string(args[3])};

    // Parsear argumentos opcionales para paralelización
    for (size_t i = 4; i < args.size(); ++i) {
      std::string const arg = args[i];
      if (arg == "--threads" and i + 1 < args.size()) {
        config.num_threads = std::stoi(args[++i]);
      } else if (arg == "--partitioner" and i + 1 < args.size()) {
        config.partitioner = args[++i];
      } else if (arg == "--grain-size" and i + 1 < args.size()) {
        config.grain_size = static_cast<size_t>(std::stoul(args[++i]));
      }
    }

    return config;
  }

  RenderingContext initialize_rendering(Config const & cfg) {
    // Función utilizada para obtener todo lo relacionado con la configuración de la imagen
    imagec::imageConfig config = configurationParser::parse(cfg.config_file);
    render::scene scene        = sceneParser::parse(cfg.scene_file);

    int const image_width  = config.width();
    int const image_height = config.height();
    image::imagePAR image(image_width, image_height);

    auto [cam_pos, cam_target, cam_north, fov] = config.paramsView();

    imagec::Pov const viewpoint(cam_pos, cam_target, cam_north, static_cast<double>(fov),
                                image_width, image_height,
                                static_cast<std::uint_fast64_t>(config.ray_seed()));

    return {std::move(image), viewpoint, std::move(scene), std::move(config)};
  }

  std::tuple<int, int, int> apply_post_processing(double r, double g, double b,
                                                  RenderingContext const & ctx) {
    // Aplicar el postprocesado a los colores
    double const inv_samples = 1.0 / static_cast<double>(ctx.config.samples_per_pixel());

    r *= inv_samples;
    g *= inv_samples;
    b *= inv_samples;

    double const gamma_inv = 1.0 / ctx.config.gamma();

    r = std::pow(r, gamma_inv);
    g = std::pow(g, gamma_inv);
    b = std::pow(b, gamma_inv);

    double r_scaled = r * 255.0;
    double g_scaled = g * 255.0;
    double b_scaled = b * 255.0;

    r_scaled = std::clamp(r_scaled, 0.0, 255.0);
    g_scaled = std::clamp(g_scaled, 0.0, 255.0);
    b_scaled = std::clamp(b_scaled, 0.0, 255.0);

    return {static_cast<int>(r_scaled), static_cast<int>(g_scaled), static_cast<int>(b_scaled)};
  }

  // Configurar parámetros de renderizado
  RenderParams setup_render_params(RenderingContext & ctx) {
    return RenderParams{ctx.viewpoint.origin_proj_win(),
                        ctx.viewpoint.displacement_x(),
                        ctx.viewpoint.displacement_y(),
                        ctx.viewpoint.position(),
                        &ctx.scene,
                        &ctx.config,
                        ctx.config.samples_per_pixel()};
  }

  // Función para calcular color de una muestra
  std::tuple<double, double, double> compute_sample_color(
      SampleComputeParams const & compute_params) {
    double r = 0.0, g = 0.0, b = 0.0;

    for (int s = 0; s < compute_params.samples; ++s) {
      // Generar offsets bajo demanda
      double const ox = compute_params.thread_rng->pixel_dist(compute_params.thread_rng->rng);
      double const oy = compute_params.thread_rng->pixel_dist(compute_params.thread_rng->rng);

      render::point const Q = compute_params.params->O +
                              compute_params.params->delta_x * (compute_params.col + ox) +
                              compute_params.params->delta_y * (compute_params.row + oy);
      ray::ray const ray(compute_params.params->cam_pos,
                         render::vector::get_vector_from_points(compute_params.params->cam_pos, Q));

      auto hit = render::motorIntersecciones::find_nearest_intersection(
          ray, *compute_params.params->scene);

      auto color =
          hit ? render::motorIntersecciones::color_contribution(
                    hit.value(), compute_params.params->config->max_depth(),
                    *compute_params.params->scene, *compute_params.params->config)
              : render::motorIntersecciones::background_color(ray, *compute_params.params->config);

      r += std::get<0>(color);
      g += std::get<1>(color);
      b += std::get<2>(color);
    }

    return {r, g, b};
  }

  // Estructura para los parámetros de procesamiento de píxeles
  struct PixelRangeParams {
    RenderingContext * ctx;
    RenderParams const * params;
    tbb::enumerable_thread_specific<ThreadLocalRNG> * tls_rng;
  };

  // Procesar un rango de píxeles. Función principal que asignaremos a cada hilo de ejecución
  void process_pixel_range(tbb::blocked_range2d<int> const & range,
                           PixelRangeParams const & pixel_params) {
    // Obtenemos el generador ya inicializado para este hilo
    ThreadLocalRNG & thread_rng = pixel_params.tls_rng->local();

    int const SAMPLES = pixel_params.params->samples;

    for (int row = range.rows().begin(); row < range.rows().end(); ++row) {
      for (int col = range.cols().begin(); col < range.cols().end(); ++col) {
        SampleComputeParams const compute_params(row, col, SAMPLES, *pixel_params.params,
                                                 thread_rng);
        auto color = compute_sample_color(compute_params);
        pixel_params.ctx->image.setPixel(
            row, col,
            apply_post_processing(std::get<0>(color), std::get<1>(color), std::get<2>(color),
                                  *pixel_params.ctx));
      }
    }
  }

  // Configurar RNG thread-local
  tbb::enumerable_thread_specific<ThreadLocalRNG> setup_thread_local_rng(
      std::uint_fast64_t base_seed) {
    return tbb::enumerable_thread_specific<ThreadLocalRNG>([base_seed] {
      auto thread_id_hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
      return ThreadLocalRNG(base_seed xor thread_id_hash);
    });
  }

  // Configurar parámetros de píxeles
  PixelRangeParams setup_pixel_params(RenderingContext & ctx, RenderParams const & params,
                                      tbb::enumerable_thread_specific<ThreadLocalRNG> & tls_rng) {
    PixelRangeParams pixel_params{};
    pixel_params.ctx     = &ctx;
    pixel_params.params  = &params;
    pixel_params.tls_rng = &tls_rng;
    return pixel_params;
  }

  // Función para lanzar el bucle paralelo con la estrategia de división de trabajo elegida
  void execute_with_partitioner(
      tbb::blocked_range2d<int> const & range,
      std::function<void(tbb::blocked_range2d<int> const &)> const & process_body,
      std::string const & partitioner) {
    if (partitioner == "simple") {
      tbb::parallel_for(range, process_body, tbb::simple_partitioner());
    } else if (partitioner == "static") {
      tbb::parallel_for(range, process_body, tbb::static_partitioner());
    } else {
      tbb::parallel_for(range, process_body, tbb::auto_partitioner());
    }
  }

  // Ejecución del renderizado paralelo
  void execute_parallel_rendering(RenderingContext & ctx, Config const & cfg,
                                  RenderParams const & params) {
    int const height = ctx.image.height();
    int const width  = ctx.image.width();

    // Preparación del rng. Se crean una vez por hilo, fuera del bucle
    std::uint_fast64_t const base_seed = ctx.viewpoint.seed();
    auto tls_rng                       = setup_thread_local_rng(base_seed);

    auto pixel_params = setup_pixel_params(ctx, params, tls_rng);

    // Lambda simple para llamar a nuestra funcion
    auto process_body = [&pixel_params](tbb::blocked_range2d<int> const & range) {
      process_pixel_range(range, pixel_params);
    };

    // Lanzar la función auxiliar para ejecutar el parallel_for con el partitioner elegido
    size_t const effective_grain = cfg.grain_size;
    tbb::blocked_range2d<int> const range(0, height, effective_grain, 0, width, effective_grain);
    execute_with_partitioner(range, process_body, cfg.partitioner);
  }

  // Renderizar imagen principal usando TBB
  void render_image_parallel(RenderingContext & ctx, Config const & cfg) {
    RenderParams const params = setup_render_params(ctx);
    execute_parallel_rendering(ctx, cfg, params);
  }

}  // namespace

int main(int argc, char * argv[]) {
  try {
    // Parseamos los archivos de entrada
    Config const config = parse_arguments(std::span(argv, static_cast<size_t>(argc)));

    // Configurar control global de hilos si se especifica
    std::unique_ptr<tbb::global_control> thread_control;
    if (config.num_threads > 0) {
      thread_control = std::make_unique<tbb::global_control>(
          tbb::global_control::max_allowed_parallelism, config.num_threads);
    }

    // Configuramos todos los párametros
    RenderingContext ctx = initialize_rendering(config);

    // Iniciamos el rng
    imagec::imageConfig::init_material_rng(
        static_cast<std::uint_fast64_t>(ctx.config.material_seed()));

    // Renderizamos la imagen en paralelo
    render_image_parallel(ctx, config);

    // Escribimos en el archivo de salida
    ctx.image.write_ppm(config.output_file);
  } catch (std::exception const & e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
