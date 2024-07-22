#pragma once

#include "Hit.h"
#include "Ray.h"

namespace JYKim
{
    class Sphere
    {
    public:
        Vector3 center;     //center.z가 음수 -> 화면 바깥 쪽 방향에 물체가 있다(그려지지 않는다)
        float radius;
        
		// 퐁 쉐이딩(Phong shading)을 위한 재질(material)
		Vector3 amb = Vector3();	// Ambient
		Vector3 diff = Vector3();	// Diffuse
		Vector3 spec = Vector3();	// Specular
		float ks = 0.0f;
		float alpha = 0.0f;
		//float reflection_ = 0.0;
		//float transparency = 0.0;

    public:
        Sphere(const Vector3& center, const float radius, const Color& color)
            : center(center), radius(radius)
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
            Hit hit = Hit{ -1.0f, Vector3(0.0f), Vector3(0.0f) };   // d가 음수이면 충돌을 안한 것으로 가정

            /*
            * hit.d = ... // 광선의 시작점으로부터 충돌지점까지의 거리 (float)
            * hit.point = ... // 광선과 구가 충돌한 지점의 위치 (vec3)
            * hit.normal = .. // 충돌 지점에서 구의 단위 법선 벡터(unit normal vector)
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

            // const float a = ray.dir.Dot(ray.dir); // dir이 unit vector라면 a는 1.0f라서 생략 가능

            return hit;
        }
    };
}
