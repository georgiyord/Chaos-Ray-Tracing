#ifndef __UTILS_CRT
#define __UTILS_CRT

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>

using u64   = uint64_t;
using u32   = uint32_t;
using u16   = uint16_t;
using u8    = uint8_t;
using i64   = int64_t;
using i32   = int32_t;
using i16   = int16_t;
using i8    = int8_t;

template <typename T>
struct vec3
{
    T x;
    T y;
    T z;

    bool operator==(const vec3<T> &rhs) const
    {
        return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
    }

    vec3<T> operator-(const vec3<T> &rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    T length(){
        return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
    }

    vec3<T>& normalise(){
        double _length = length();
        if (_length != 0){
            x /= _length;
            y /= _length;
            z /= _length;
        }
        return *this;
    }

    friend vec3<T> crossProduct(const vec3<T>& a, const vec3<T>& b){
        return vec3<T>{.x = a.y * b.z - a.z * b.y, .y = a.z * b.x - a.x * b.z , .z = a.x * b.y - a.y * b.x};
    }

    friend T dotProduct(const vec3<T>& a, const vec3<T>& b){
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    template<typename K>
    vec3<T> operator*(K scalar){
        return vec3<T>{.x = this->x * scalar, .y = this->y * scalar, .z = this->z * scalar};
    }

    template<typename K>
    friend vec3<T> operator*(K scalar, vec3<T> vec){
        return vec3<T>{.x = vec->x * scalar, .y = vec->y * scalar, .z = vec->z * scalar};
    }
};

template <typename T>
struct vec2
{
    T x;
    T y;

    constexpr vec2(T _x, T _y) : x(_x), y(_y)
    {
    }

    [[nodiscard]] constexpr T volume() const
    {
        return x * y;
    }

    constexpr vec2<T> operator/(const vec2<T> &rhs) const
    {
        return vec2<T>(x / rhs.x, y / rhs.y);
    }

    constexpr vec2<T> operator/(size_t rhs) const
    {
        return vec2<T>(x / rhs, y / rhs);
    }

    bool operator==(const vec2 &rhs)
    {
        return x == rhs.x && y == rhs.y;
    }
};

template <typename T>
class Matrix
{
    vec2<size_t> size;
    std::unique_ptr<T[]> arr;

public:
    Matrix(vec2<size_t> _size) : size(_size), arr(new T[size.volume()])
    {
    }

    Matrix(const Matrix &rhs) : size(rhs.size), arr(new T[size.volume()])
    {
        for (u32 i = 0; i < size.volume(); ++i)
        {
            arr[i] = rhs.arr[i];
        }
    }

    Matrix &operator=(const Matrix &rhs)
    {
        if (this != &rhs)
        {
            if (size != rhs.size){
                size = rhs.size;
                arr = new T[size.volume()];
            }
            for (u32 i = 0; i < size.volume(); ++i)
            {
                arr[i] = rhs.arr[i];
            }
        }
        return *this;
    }

    [[nodiscard]] const T *begin() const
    {
        return arr;
    }

    [[nodiscard]] const T *end() const
    {
        return arr + size.volume();
    }

    [[nodiscard]] T *begin()
    {
        return arr;
    }

    [[nodiscard]] T *end()
    {
        return arr + size.volume();
    }

    T &get(const vec2<u32> pos)
    {
        if (pos.x >= size.x || pos.y >= size.y)
        {
            throw std::out_of_range("");
        }
        return arr[(pos.y * size.x) + pos.x];
    }

    T &get(u32 x, u32 y)
    {
        if (x >= size.x || y >= size.y)
        {
            throw std::out_of_range("");
        }
        return arr[(y * size.x) + x];
    }

    [[nodiscard]] const T &get(const vec2<u32> pos) const
    {
        if (pos.x >= size.x || pos.y >= size.y)
        {
            throw std::out_of_range("");
        }
        return arr[(pos.y * size.x) + pos.x];
    }

    [[nodiscard]] const T &get(u32 x, u32 y) const
    {
        if (x >= size.x || y >= size.y)
        {
            throw std::out_of_range("");
        }
        return arr[(y * size.x) + x];
    }

    [[nodiscard]] vec2<size_t> getSize() const
    {
        return size;
    }
};

using Color = vec3<u8>;

enum class Anchor : u8
{
    TOP_RIGHT,
    TOP,
    TOP_LEFT,
    LEFT,
    CENTER,
    RIGHT,
    BOTTOM_LEFT,
    BOTTOM,
    BOTTOM_RIGHT
};

class Shape
{
    vec2<u64> pos;
    Anchor anchor;

protected:
    Shape(vec2<u64> _pos) : pos(_pos), anchor(Anchor::CENTER)
    {
    }
    Shape(vec2<u64> _pos, Anchor _anchor) : pos(_pos), anchor(_anchor)
    {
    }
public:
    [[nodiscard]] vec2<u64> getPosition() const {
        return pos;
    }
};

class Recht : public Shape
{
    vec2<size_t> size;

public:
    Recht(vec2<size_t> _size, vec2<u64> _pos) : Shape(_pos), size(_size)
    {
    }
};

class Circle : public Shape
{
    size_t radius;

public:
    Circle(size_t _radius, vec2<u64> _pos) : Shape(_pos), radius(_radius)
    {
    }
    [[nodiscard]] size_t getRadius() const {
        return radius;
    }
};

const Color RED = {255, 0, 0};
const Color GREEN = {0, 255, 0};
const Color BLUE = {0, 0, 255};


struct Ray{
    vec3<double> origin;
    vec3<double> direction;
};

struct Triangle{
    vec3<double> point1;
    vec3<double> point2;
    vec3<double> point3;

    vec3<double> getNormal(){
        const vec3<double> vec1 = point2 - point1;
        const vec3<double> vec2 = point3 - point1;
        return crossProduct(vec1, vec2).normalise();
    }

    double getArea(){
        const vec3<double> vec1 = point2 - point1;
        const vec3<double> vec2 = point3 - point1;
        return crossProduct(vec1, vec2).length() / 2;
    }
};

#endif //__UTILS_CRT
