#ifndef __UTILS_CRT
#define __UTILS_CRT

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>

using u8 = uint8_t;
using u32 = uint32_t;
using i32 = int32_t;

template <typename T>
struct vec3
{
    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        struct
        {
            T r;
            T g;
            T b;
        };
    };

    bool operator==(const vec3<T> &rhs) const
    {
        return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
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

template <typename T, vec2<size_t> size>
class Matrix
{
    std::unique_ptr<T[]> arr;

public:
    Matrix() : arr(new T[size.volume()])
    {
    }

    Matrix(const Matrix &rhs) : arr(new T[size.volume()])
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
    vec2<size_t> pos;
    Anchor anchor;

protected:
    Shape(vec2<size_t> _pos) : pos(_pos), anchor(Anchor::CENTER)
    {
    }
    Shape(vec2<size_t> _pos, Anchor _anchor) : pos(_pos), anchor(_anchor)
    {
    }
public:
    [[nodiscard]] vec2<size_t> getPosition() const {
        return pos;
    }
};

class Recht : public Shape
{
    vec2<size_t> size;

public:
    Recht(vec2<size_t> _size, vec2<size_t> _pos) : Shape(_pos), size(_size)
    {
    }
};

class Circle : public Shape
{
    size_t radius;

public:
    Circle(size_t _radius, vec2<size_t> _pos) : Shape(_pos), radius(_radius)
    {
    }
    [[nodiscard]] size_t getRadius() const {
        return radius;
    }
};

const Color RED = {255, 0, 0};
const Color GREEN = {0, 255, 0};
const Color BLUE = {0, 0, 255};

#endif //__UTILS_CRT
