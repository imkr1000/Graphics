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
    Vector2 GetCenter() const { return center; }
    float* GetCenterFloatAddress() { return &center.x; }
    Color GetColor() const { return color; }
    float* GetColorFloatAddress() { return &color.x; }
    float GetRadius() const { return radius; }
    float* GetRadiusAddress() { return &radius; }
    float GetRadiusSqaured() const { return radiusSquared; }

    void SetRadiusSqaured(float radiusSquared) { this->radiusSquared = radiusSquared; }

public:
    bool IsInside(const Vector2& v)
    {
        //流包利
        //if ((v - center).Length() <= radius)
        //    return true;

        //弥利拳
        if ((v - center).LengthSquared() <= radiusSquared)
            return true;

        return false;
    }
};