#include "utils.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>


std::random_device r;
std::default_random_engine randEngine(r());
std::uniform_int_distribution<u8> randColor{0, 255};

std::vector<u32> generateWeights(std::vector<Color> colors)
{
    size_t size = colors.size();
    std::vector<u32> result(size);
    for (auto i = 0; i < size; ++i)
    {
        result[i] = size - i;
    }
    return result;
}


// Homework 2 part 1
inline void createRechtsImage(const vec2<size_t> resolution)
{

    constexpr size_t ROWS = 10;
    constexpr size_t COLS = 10;

    const vec2<size_t> rechtsAmount = {ROWS, COLS};
    static std::bernoulli_distribution randBool{0.75};

    struct ColoredRecht : Recht
    {
        std::vector<Color> colors;

        ColoredRecht(vec2<size_t> size) : Recht(size, {0, 0})
        {
            colors.push_back(Color{
                .r = randColor(randEngine),
                .g = randColor(randEngine),
                .b = randColor(randEngine),
            });
            bool newColor = true;
            while (newColor)
            {
                colors.push_back(Color{
                    .r = randColor(randEngine),
                    .g = randColor(randEngine),
                    .b = randColor(randEngine),
                });
                newColor = randBool(randEngine);
            }
        }
    };

    std::vector<ColoredRecht> rechts;
    rechts.reserve(ROWS * COLS);
    for (u32 i = 0; i < ROWS * COLS; ++i)
    {
        rechts.emplace_back(resolution / rechtsAmount);
    }

    std::ofstream image("imageRechts.ppm", std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";

    for (size_t y = 0; y < resolution.y; ++y)
    {
        for (size_t x = 0; x < resolution.x; ++x)
        {
            const size_t recht_x = x * ROWS / resolution.x;
            const size_t recht_y = y * COLS / resolution.y;
            const ColoredRecht &recht = rechts.at(recht_y * ROWS + recht_x);

            auto weights = generateWeights(recht.colors);
            std::discrete_distribution<> d(weights.begin(), weights.end());
            Color c = recht.colors[d(randEngine)];
            image << static_cast<u32>(c.r) << " " << static_cast<u32>(c.g) << " " << static_cast<u32>(c.b) << " ";
        }
    }
    image.close();
}

// Homework 2 part 2
void createShapeAccent(const vec2<size_t> resolution){
    constexpr size_t radius = 300;
    Circle circle(radius, resolution / 2);

    std::ofstream image("imageShapeAccent.ppm", std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";

    for (size_t y = 0; y < resolution.y; ++y)
    {
        for (size_t x = 0; x < resolution.x; ++x)
        {
            const size_t relative_x = x > circle.getPosition().x ? x - circle.getPosition().x : circle.getPosition().x - x;
            const size_t relative_y = y > circle.getPosition().y ? y - circle.getPosition().y : circle.getPosition().y - y;
            if ((relative_x * relative_x) + (relative_y * relative_y) > circle.getRadius() * circle.getRadius()){
                image << 128 << " " << 128 << " " << 128 << " ";
            }
            else{
                image << 0 << " " << 128 << " " << 0 << " ";
            }
        }
    }
}

// Homework 3
Matrix<Ray> rayGeneration(const vec2<size_t> resolution){
    Matrix<Ray> raster(resolution);

    for (size_t y = 0; y < resolution.y; ++y)
    {
        for (size_t x = 0; x < resolution.x; ++x)
        {
            //get the center of the pixel;
            double world_x = x + .5;
            double world_y = y + .5;

            // convert to Normalised Device Coordinate space
            world_x /= resolution.x;
            world_y /= resolution.y;

            // convert to Screen space
            world_x = world_x * 2 - 1;
            world_y = 1 - world_y * 2;

            // align to aspect ratio
            world_x *= resolution.x / resolution.y;
            vec3<double> direction({world_x, world_y, -1.0});
            direction.normalise();
            raster.get(x, y) = Ray({.origin=vec3<double>{.0, .0, .0}, .direction=direction});
        }
    }
    return raster;
}

// Homework 3
void createImageFromRayDirections(const vec2<size_t> resolution){
    auto rays = rayGeneration(resolution);

    std::ofstream image("RayDirection.ppm", std::ios::trunc | std::ios::out);
    image << "P3 " << resolution.x << " " << resolution.y << " " << 255 << " ";
    for (size_t y = 0; y < resolution.y; ++y)
    {
        for (size_t x = 0; x < resolution.x; ++x)
        {
            Color c {
                .r = static_cast<u8>(std::abs(rays.get(x, y).direction.x) * 255),
                .g = static_cast<u8>(std::abs(rays.get(x, y).direction.y) * 255),
                .b = static_cast<u8>(rays.get(x, y).direction.z * -255)
            };
            image << static_cast<u32>(c.r) << " " << static_cast<u32>(c.g) << " " << static_cast<u32>(c.b) << " ";
        }
    }

}

int main(const int argc, const char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << "[resolution_x] [resolution_y]" << std::endl;
        return 1;
    }

    const size_t RES_X = std::stoul(argv[1]);
    const size_t RES_Y = std::stoul(argv[2]);

    if (errno == ERANGE)
    {
        std::cerr << "Invalid arguments!\n";
        return 1;
    }

    const vec2<size_t> resolution = {RES_X, RES_Y};

    // createRechtsImage(resolution);
    // createShapeAccent(resolution);
    createImageFromRayDirections(resolution);

    return 0;
}
