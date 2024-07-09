#pragma once

class Circle
{
private:
    Vector2 center = Vector2();
    float radius = 0.0f;
    float radiusSquared = 0.0f;
    Color color;

public:
    //Circle() : Circle(Vector2(), 0.0f, Color()) {}
    Circle(const Vector2& center, const float radius, const Color& color)
        : center(center), color(color), radius(radius), radiusSquared(radius* radius)
    {}

public:
    Color GetColor() const { return color; }

public:
    bool IsInside(const Vector2& v)
    {
        //Á÷°üÀû
        //if ((v - center).Length() <= radius)
        //    return true;

        if ((v - center).LengthSquared() <= radiusSquared)
            return true;

        return false;
    }
};