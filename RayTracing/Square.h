#pragma once

#include "Triangle.h"

namespace JYKim
{
	class Square : public Object
	{
	public:
		Triangle triangle1, triangle2;

		Square(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3)
			: triangle1(v0, v1, v2), triangle2(v0, v2, v3)
		{
		}

		Hit CheckRayCollision(const Ray& ray) const override
		{
			auto hit1 = triangle1.CheckRayCollision(ray);
			auto hit2 = triangle2.CheckRayCollision(ray);

			if (hit1.distance >= 0.0f && hit2.distance >= 0.0f)
			{
				return hit1.distance < hit2.distance ? hit1 : hit2;
			}
			else if (hit1.distance >= 0.0f)
			{
				return hit1;
			}
			else
			{
				return hit2;
			}
		}
	};
}