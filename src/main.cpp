#include "utils.hpp"
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <numbers>
#include <random>
#include <string>
#include <vector>

std::random_device r;
std::default_random_engine randEngine(r());
std::uniform_int_distribution<u8> randColor{0, 255};

std::vector<u32> generateWeights(std::vector<Color> colors) {
  size_t size = colors.size();
  std::vector<u32> result(size);
  for (auto i = 0; i < size; ++i) {
    result[i] = size - i;
  }
  return result;
}

// Homework 2 part 1
inline void createRechtsImage(const vec2<size_t> resolution) {

  constexpr size_t ROWS = 10;
  constexpr size_t COLS = 10;

  const vec2<size_t> rechtsAmount = {ROWS, COLS};
  static std::bernoulli_distribution randBool{0.75};

  struct ColoredRecht : Recht {
    std::vector<Color> colors;

    ColoredRecht(vec2<size_t> size) : Recht(size, {0, 0}) {
      colors.push_back(Color{
          .x = randColor(randEngine),
          .y = randColor(randEngine),
          .z = randColor(randEngine),
      });
      bool newColor = true;
      while (newColor) {
        colors.push_back(Color{
            .x = randColor(randEngine),
            .y = randColor(randEngine),
            .z = randColor(randEngine),
        });
        newColor = randBool(randEngine);
      }
    }
  };

  std::vector<ColoredRecht> rechts;
  rechts.reserve(ROWS * COLS);
  for (u32 i = 0; i < ROWS * COLS; ++i) {
    rechts.emplace_back(resolution / rechtsAmount);
  }

  std::ofstream image("imageRechts.ppm", std::ios::trunc | std::ios::out);
  image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";

  for (size_t y = 0; y < resolution.y; ++y) {
    for (size_t x = 0; x < resolution.x; ++x) {
      const size_t recht_x = x * ROWS / resolution.x;
      const size_t recht_y = y * COLS / resolution.y;
      const ColoredRecht &recht = rechts.at(recht_y * ROWS + recht_x);

      auto weights = generateWeights(recht.colors);
      std::discrete_distribution<> d(weights.begin(), weights.end());
      Color c = recht.colors[d(randEngine)];
      image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " "
            << static_cast<u32>(c.z) << " ";
    }
  }
  image.close();
}

// Homework 2 part 2
void createShapeAccent(const vec2<size_t> resolution) {
  constexpr size_t radius = 300;
  Circle circle(radius, resolution / 2);

  std::ofstream image("imageShapeAccent.ppm", std::ios::trunc | std::ios::out);
  image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";

  for (size_t y = 0; y < resolution.y; ++y) {
    for (size_t x = 0; x < resolution.x; ++x) {
      const size_t relative_x = x > circle.getPosition().x
                                    ? x - circle.getPosition().x
                                    : circle.getPosition().x - x;
      const size_t relative_y = y > circle.getPosition().y
                                    ? y - circle.getPosition().y
                                    : circle.getPosition().y - y;
      if ((relative_x * relative_x) + (relative_y * relative_y) >
          circle.getRadius() * circle.getRadius()) {
        image << 128 << " " << 128 << " " << 128 << " ";
      } else {
        image << 0 << " " << 128 << " " << 0 << " ";
      }
    }
  }
}

// Homework 3
Matrix<Ray> rayGeneration(const vec2<size_t> resolution) {
  Matrix<Ray> raster(resolution);

  for (size_t y = 0; y < resolution.y; ++y) {
    for (size_t x = 0; x < resolution.x; ++x) {
      // get the center of the pixel;
      double world_x = x + .5;
      double world_y = y + .5;

      // convert to Normalised Device Coordinate space
      world_x /= resolution.x;
      world_y /= resolution.y;

      // convert to Screen space
      world_x = world_x * 2 - 1;
      world_y = 1 - world_y * 2;

      // align to aspect ratio
      world_x *= double(resolution.x) / resolution.y;
      vec3<double> direction({world_x, world_y, -1.0});
      direction.normalise();
      raster.get(x, y) =
          Ray({.origin = vec3<double>{.0, .0, .0}, .direction = direction});
    }
  }
  return raster;
}

// Homework 3
void createImageFromRayDirections(const vec2<size_t> resolution) {
  auto rays = rayGeneration(resolution);

  std::ofstream image("RayDirection.ppm", std::ios::trunc | std::ios::out);
  image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";
  for (size_t y = 0; y < resolution.y; ++y) {
    for (size_t x = 0; x < resolution.x; ++x) {
      Color c{.x = static_cast<u8>(std::abs(rays.get(x, y).direction.x) * 255),
              .y = static_cast<u8>(std::abs(rays.get(x, y).direction.y) * 255),
              .z = static_cast<u8>(rays.get(x, y).direction.z * -255)};
      image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " "
            << static_cast<u32>(c.z) << " ";
    }
  }
}

// Homework 4
void crossProductTests() {
  std::vector<vec3<double>> storageVectors;
  std::vector<double> storageScalars;
  // Task 2
  {
    storageVectors.push_back(
        crossProduct(vec3<double>{.x = 3.5, .y = .0, .z = .0},
                     vec3<double>{.x = 1.75, .y = 3.5, .z = .0}));
    storageVectors.push_back(
        crossProduct(vec3<double>{.x = 3, .y = -3, .z = 1},
                     vec3<double>{.x = 4, .y = 9, .z = 3}));
    storageScalars.push_back(crossProduct(vec3<double>{.x = 3, .y = -3, .z = 1},
                                          vec3<double>{.x = 4, .y = 9, .z = 3})
                                 .length());
    storageScalars.push_back(
        crossProduct(vec3<double>{.x = 3, .y = -3, .z = 1},
                     vec3<double>{.x = -12, .y = 12, .z = -4})
            .length());
  }
  // Task 3
  {
    Triangle t1({.x = -1.75, .y = -1.75, .z = -3},
                {.x = 1.75, .y = -1.75, .z = -3},
                {.x = -0, .y = 1.75, .z = -3});
    Triangle t2{{.x = 0, .y = 0, .z = -1},
                {.x = 1, .y = 0, .z = 1},
                {.x = -1, .y = 0, .z = 1}};
    Triangle t3{{.x = 0.56, .y = 1.11, .z = 1.23},
                {.x = 0.44, .y = -2.368, .z = -0.54},
                {.x = -1.56, .y = 0.15, .z = -1.92}};
    storageVectors.push_back(t1.getNormal());
    storageVectors.push_back(t2.getNormal());
    storageVectors.push_back(t3.getNormal());

    storageScalars.push_back(t1.getArea());
    storageScalars.push_back(t2.getArea());
    storageScalars.push_back(t3.getArea());
  }
}

// Homework 5
void createTriangleImages(const vec2<size_t> resolution) {
  {
    // TASK 1
    Triangle triangle{{.x = -1.75, .y = -1.75, .z = -3},
                      {.x = 1.75, .y = -1.75, .z = -3},
                      {.x = 0, .y = 1.75, .z = -3}};
    auto rays = rayGeneration(resolution);

    std::ofstream image("RayTriangleIntersection1.ppm",
                        std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";
    for (size_t y = 0; y < resolution.y; ++y) {
      for (size_t x = 0; x < resolution.x; ++x) {
        Color c = !std::isnan(rays.get(x, y).intersects(triangle))
                      ? Colors::Green
                      : Colors::Black;
        image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " "
              << static_cast<u32>(c.z) << " ";
      }
    }
  }
  {
    // TASK 2
    Triangle triangle{{.x = -3, .y = 0, .z = -2},
                      {.x = 5, .y = 0, .z = -3},
                      {.x = 1, .y = 3, .z = -4}};
    auto rays = rayGeneration(resolution);

    std::ofstream image("RayTriangleIntersection2.ppm",
                        std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";
    for (size_t y = 0; y < resolution.y; ++y) {
      for (size_t x = 0; x < resolution.x; ++x) {
        Color c = !std::isnan(rays.get(x, y).intersects(triangle))
                      ? Colors::Red
                      : Colors::Black;
        image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " "
              << static_cast<u32>(c.z) << " ";
      }
    }
  }
  {
    // TASK 3
    // Make triangles intersect for the fun of it
    Triangle triangle1{{.x = -1.75, .y = -1.75, .z = -3},
                       {.x = 1.75, .y = -1.75, .z = -3},
                       {.x = 0, .y = 1.75, .z = -3}};
    Triangle triangle2{{.x = -3, .y = 0, .z = -2},
                       {.x = 5, .y = 0, .z = -3},
                       {.x = 1, .y = 3, .z = -4}};
    auto rays = rayGeneration(resolution);

    std::ofstream image("RayTriangleIntersection3.ppm",
                        std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";
    for (size_t y = 0; y < resolution.y; ++y) {
      for (size_t x = 0; x < resolution.x; ++x) {
        Color c = Colors::Black;
        auto distance1 = rays.get(x, y).intersects(triangle1);
        auto distance2 = rays.get(x, y).intersects(triangle2);
        if (!std::isnan(distance1)) {
          if (!std::isnan(distance2)) {
            if (distance1 < distance2) {
              c = Colors::Green;
            } else {
              c = Colors::Red *
                  (1 - (distance2 /
                        6)); // try to represent distance with brightness value
            }
          } else {
            c = Colors::Green;
          }
        } else if (!std::isnan(distance2)) {
          c = Colors::Red *
              (1 - (distance2 /
                    6)); // try to represent distance with brightness value
        }
        image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " "
              << static_cast<u32>(c.z) << " ";
      }
    }
  }
  {
    // TASK 4
    // Make shape from triangles
    double r1 = .5;
    double r2 = 1;
    double r3 = 2;
    double r4 = 4;
    vec3<double> points[4][16];
    vec3<double> center = {.x = 0, .y = 0, .z = -3};

    // calculate point coordinates on circles
    for (size_t i = 0; i < 16; ++i) {
      double theta = 2 * std::numbers::pi / 16 * i;
      points[0][i] = {
          .x = r1 * std::cos(theta), .y = r1 * std::sin(theta), .z = -3};
      points[1][i] = {
          .x = r2 * std::cos(theta), .y = r2 * std::sin(theta), .z = -3};
      points[2][i] = {
          .x = r3 * std::cos(theta), .y = r3 * std::sin(theta), .z = -3};
      points[3][i] = {
          .x = r4 * std::cos(theta), .y = r4 * std::sin(theta), .z = -3};
    }

    Triangle innerTriangles[16];
    for (size_t i = 0; i < 8; ++i) {
      auto tmp = i == 7 ? 0 : 2 + i * 2;
      innerTriangles[i * 2].update(center, points[1][i * 2],
                                   points[0][1 + i * 2]);
      innerTriangles[i * 2 + 1].update(center, points[0][1 + i * 2],
                                       points[1][tmp]);
    }
    Color innerColor = Colors::White;
    Triangle intermediateTriangles[32];
    for (size_t i = 0; i < 8; ++i) {
      auto tmp = i == 7 ? 0 : 2 + i * 2;
      intermediateTriangles[i * 4].update(points[1][i * 2], points[2][i * 2],
                                          points[1][i * 2 + 1]);
      intermediateTriangles[i * 4 + 1].update(
          points[1][i * 2], points[1][i * 2 + 1], points[0][i * 2 + 1]);
      intermediateTriangles[i * 4 + 2].update(
          points[0][i * 2 + 1], points[1][i * 2 + 1], points[1][tmp]);
      intermediateTriangles[i * 4 + 3].update(points[1][i * 2 + 1],
                                              points[2][tmp], points[1][tmp]);
    }
    Color intermediateColor = Colors::Yellow;
    Triangle outerTriangles[32];
    for (size_t i = 0; i < 8; ++i) {
      auto tmp = i == 7 ? 0 : 2 + i * 2;
      outerTriangles[i * 4].update(points[2][i * 2], points[3][i * 2],
                                   points[2][i * 2 + 1]);
      outerTriangles[i * 4 + 1].update(points[2][i * 2], points[2][i * 2 + 1],
                                       points[1][i * 2 + 1]);
      outerTriangles[i * 4 + 2].update(points[1][i * 2 + 1],
                                       points[2][i * 2 + 1], points[2][tmp]);
      outerTriangles[i * 4 + 3].update(points[2][i * 2 + 1], points[3][tmp],
                                       points[2][tmp]);
    }
    Color outerColor = Colors::Red;

    auto rays = rayGeneration(resolution);

    std::ofstream image("RayTriangleIntersection4.ppm",
                        std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";
    for (size_t y = 0; y < resolution.y; ++y) {
      for (size_t x = 0; x < resolution.x; ++x) {
        Color c = Colors::Black;
        {
          double distance = std::numeric_limits<double>::infinity();
          for (size_t i = 0; i < 16; ++i) {
            auto tmp = rays.get(x, y).intersects(innerTriangles[i]);
            if (std::isnan(tmp))
              continue;
            if (tmp < distance) {
              distance = tmp;
              c = innerColor;
            }
          }
          for (size_t i = 0; i < 32; ++i) {
            auto tmp = rays.get(x, y).intersects(intermediateTriangles[i]);
            if (std::isnan(tmp))
              continue;
            if (tmp < distance) {
              distance = tmp;
              c = intermediateColor;
            }
          }
          for (size_t i = 0; i < 32; ++i) {
            auto tmp = rays.get(x, y).intersects(outerTriangles[i]);
            if (std::isnan(tmp))
              continue;
            if (tmp < distance) {
              distance = tmp;
              c = outerColor;
            }
          }
        }
        image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " "
              << static_cast<u32>(c.z) << " ";
      }
    }
  }
}

int main(const int argc, const char **argv) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << "[resolution_x] [resolution_y]"
              << std::endl;
    return 1;
  }

  const size_t RES_X = std::stoul(argv[1]);
  const size_t RES_Y = std::stoul(argv[2]);

  if (errno == ERANGE) {
    std::cerr << "Invalid arguments!\n";
    return 1;
  }

  const vec2<size_t> resolution = {RES_X, RES_Y};

  // createRechtsImage(resolution);
  // createShapeAccent(resolution);
  // createImageFromRayDirections(resolution);
  // crossProductTests();
  createTriangleImages(resolution);

  return 0;
}
