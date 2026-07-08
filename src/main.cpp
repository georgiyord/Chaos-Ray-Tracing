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
                .x = randColor(randEngine),
                .y = randColor(randEngine),
                .z = randColor(randEngine),
            });
            bool newColor = true;
            while (newColor)
            {
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
            image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " " << static_cast<u32>(c.z) << " ";
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
                .x = static_cast<u8>(std::abs(rays.get(x, y).direction.x) * 255),
                .y = static_cast<u8>(std::abs(rays.get(x, y).direction.y) * 255),
                .z = static_cast<u8>(rays.get(x, y).direction.z * -255)
            };
            image << static_cast<u32>(c.x) << " " << static_cast<u32>(c.y) << " " << static_cast<u32>(c.z) << " ";
        }
    }
}

// Homework 4
void crossProductTests(){
    std::vector<vec3<double>> storageVectors;
    std::vector<double> storageScalars;
    // Task 2
    {
        storageVectors.push_back(crossProduct(vec3<double>{.x=3.5, .y=.0, .z=.0}, vec3<double>{.x=1.75, .y=3.5, .z=.0}));
        storageVectors.push_back(crossProduct(vec3<double>{.x=3, .y=-3, .z=1}, vec3<double>{.x=4, .y=9, .z=3}));
        storageScalars.push_back(crossProduct(vec3<double>{.x=3, .y=-3, .z=1}, vec3<double>{.x=4, .y=9, .z=3}).length());
        storageScalars.push_back(crossProduct(vec3<double>{.x=3, .y=-3, .z=1}, vec3<double>{.x=-12, .y=12, .z=-4}).length());
    }
    // Task 3
    {
        Triangle t1{.point1={.x=-1.75, .y=-1.75, .z=-3}, .point2={.x=1.75, .y=-1.75, .z=-3}, .point3={.x=-0, .y=1.75, .z=-3}};
        Triangle t2{.point1={.x=0, .y=0, .z=-1}, .point2={.x=1 ,.y=0, .z=1}, .point3={.x=-1, .y=0, .z=1}};
        Triangle t3{.point1={.x=0.56, .y=1.11, .z=1.23}, .point2={.x=0.44, .y=-2.368, .z=-0.54}, .point3={.x=-1.56, .y=0.15, .z=-1.92}};
        storageVectors.push_back(t1.getNormal());
        storageVectors.push_back(t2.getNormal());
        storageVectors.push_back(t3.getNormal());

        storageScalars.push_back(t1.getArea());
        storageScalars.push_back(t2.getArea());
        storageScalars.push_back(t3.getArea());
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
    // createImageFromRayDirections(resolution);
    crossProductTests();

    return 0;
}
