#pragma once

#include "Object.h"

namespace JYKim
{
    class Sphere : public Object
    {
    private:
        Vector3 center;     //center.z�� ���� -> ȭ�� �ٱ� �� ���⿡ ��ü�� �ִ�(�׷����� �ʴ´�)
        float radius;

    public:
        Sphere(const Vector3& center, const float radius, const Color& color = Color(1))
            : center(center), radius(radius), Object(color)
        {
        }

    public:
        Vector3 GetCenter() const { return center; }
        float* GetCenterFloatAddress() { return &center.x; }
        float GetRadius() const { return radius; }
        float* GetRadiusAddress() { return &radius; }

		// Wikipedia Line-sphere intersection
		// https://en.wikipedia.org/wiki/Line?sphere_intersection
		Hit CheckRayCollision(const Ray& ray) const override
		{
			Hit hit = Hit{ -1.0f, Vector3(0.0f), Vector3(0.0f) };   // d�� �����̸� �浹�� ���� ������ ����

			/*
			* hit.d = ... // ������ ���������κ��� �浹���������� �Ÿ� (float)
			* hit.point = ... // ������ ���� �浹�� ������ ��ġ (vec3)
			* hit.normal = .. // �浹 �������� ���� ���� ���� ����(unit normal vector)
			*/

			//const double a = glm::dot(ray.dir_, ray.dir_); // dir�� unit vector��� a�� 1.0
			const float b = 2.0f * ray.dir.Dot(ray.start - center);
			const float c = (ray.start - center).LengthSquared() - radius * radius;

			//const float nabla = b * b / 4.0f - c;
			const float nabla = b * b - 4.0f * c;

			if (nabla >= 0.0f)
			{
				//const float d1 = -b / 2.0f + sqrt(nabla);
				//const float d2 = -b / 2.0f - sqrt(nabla);

				const float d1 = (-b - sqrt(nabla)) * 0.5f;
				const float d2 = (-b + sqrt(nabla)) * 0.5f;

				//hit.distance = d1 < d2 ? d1 : d2;
				hit.distance = min(d1, d2);
				//hit.distance = d2 >= 0 ? d2 : d1;

				// ��ü �ȿ��� �ٽ� ������ �����鼭 �浹 ����
				if (hit.distance < 0.0f)
					hit.distance = max(d1, d2);

				hit.point = ray.start + ray.dir * hit.distance;
				hit.normal = hit.point - center;
				hit.normal.Normalize();
			}

			// const float a = ray.dir.Dot(ray.dir); // dir�� unit vector��� a�� 1.0f�� ���� ����

			return hit;
		}
    };
}
