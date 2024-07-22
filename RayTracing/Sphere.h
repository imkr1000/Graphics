#pragma once

#include "Hit.h"
#include "Ray.h"

namespace JYKim
{
    class Sphere
    {
    public:
        Vector3 center;     //center.z�� ���� -> ȭ�� �ٱ� �� ���⿡ ��ü�� �ִ�(�׷����� �ʴ´�)
        float radius;
        Color color;        // �ڿ��� '����(material)'�� Ȯ��

    public:
        Sphere(const Vector3& center, const float radius, const Color& color)
            : center(center), color(color), radius(radius)
        {
        }

    public:
        Vector3 GetCenter() const { return center; }
        float* GetCenterFloatAddress() { return &center.x; }
        Color GetColor() const { return color; }
        float* GetColorFloatAddress() { return &color.x; }
        float GetRadius() const { return radius; }
        float* GetRadiusAddress() { return &radius; }

		// Wikipedia Line-sphere intersection
		// https://en.wikipedia.org/wiki/Line?sphere_intersection
        Hit IntersectRayCollision(const Ray& ray) const
        {
            Hit hit = Hit{ -1.0f, Vector3(0.0f), Vector3(0.0f) };   // d�� �����̸� �浹�� ���� ������ ����

            /*
            * hit.d = ... // ������ ���������κ��� �浹���������� �Ÿ� (float)
            * hit.point = ... // ������ ���� �浹�� ������ ��ġ (vec3)
            * hit.normal = .. // �浹 �������� ���� ���� ���� ����(unit normal vector)
            */

            const float b = 2.0f * ray.dir.Dot(ray.start - center);
            const float c = (ray.start - center).LengthSquared() - radius * radius;

            const float nabla = b * b / 4.0f - c;

            if (nabla >= 0.0f)
            {
                const float d1 = -b / 2.0f + sqrt(nabla);
                const float d2 = -b / 2.0f - sqrt(nabla);

                //hit.distance = d1 < d2 ? d1 : d2;
                hit.distance = min(d1, d2);
                //hit.distance = d2 >= 0 ? d2 : d1;

                hit.point = ray.start + ray.dir * hit.distance;
                hit.normal = hit.point - center;
                hit.normal.Normalize();
            }

            // const float a = ray.dir.Dot(ray.dir); // dir�� unit vector��� a�� 1.0f�� ���� ����

            return hit;
        }
    };
}
