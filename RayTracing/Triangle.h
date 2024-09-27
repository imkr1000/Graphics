#pragma once

namespace JYKim
{
	class Triangle : public Object
	{
	public:
		Vector3 v0, v1, v2;
		Vector2 uv0, uv1, uv2; // 뒤에서 텍스춰 좌표계로 사용

	public:
		Triangle() : v0(Vector3()), v1(Vector3()), v2(Vector3()), uv0(Vector2(0.0f)), uv1(Vector2(0.0f)), uv2(Vector2(0.0f))
		{
		}

		Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2 uv0 = Vector2(0.0f), Vector2 uv1 = Vector2(0.0f), Vector2 uv2 = Vector2(0.0f))
			: v0(v0), v1(v1), v2(v2), uv0(uv0), uv1(uv1), uv2(uv2)
		{
		}

		Hit CheckRayCollision(const Ray& ray) const override
		{
			Hit hit = Hit{ -1.0, Vector3(), Vector3() };

			Vector3 point, faceNormal;
			float t, w0, w1;

			if (IntersectRayTriangle(ray.start, ray.dir, v0, v1, v2, point, faceNormal, t, w0, w1))
			{
				hit.distance = t;
				hit.point = point; // ray.start + ray.dir * t;
				hit.normal = faceNormal;

				//Barycentric Interpolation를 통해 텍스쳐 좌표를 Interpolation 한다.
				//DX에서는 이미 다 구현되어 있어서 어떻게 동작하는지 알 수 없다.
				//ray가 충돌할 때 uv좌표를 함께 계산
				hit.uv = uv0 * w0 + uv1 * w1 + uv2 * (1.0f - w0 - w1); // 텍스춰 좌표
			}

			return hit;
		}

		// 수학 프로그래밍을 좋아하시는 분들은 직접 구현해보시면 좋고,
		// 대부분은 개념만 이해해두시고 활용하는 방향으로 접근하셔도 충분합니다.
		// 잘 이해가 가지 않는다면 여러 자료로 교차 검증하면서 공부하시는 방법도
		// 좋습니다. 참고:
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
		bool IntersectRayTriangle(const Vector3& origin, const Vector3& dir,
			const Vector3& v0, const Vector3& v1, const Vector3 v2,
			Vector3& point, Vector3& faceNormal,
			float& t, float& w0, float& w1) const
		{
			/*
			 * 기본 전략
			 * - 삼각형이 놓여있는 평면과 광선의 교점을 찾고,
			 * - 그 교점이 삼각형 안에 있는지 밖에 있는지를 판단한다.
			 */

			 /* 1. 삼각형이 놓여 있는 평면의 수직 벡터 계산 */
			 // faceNormal = ...;
			 //주의: 삼각형의 넓이가 0일 경우에는 계산할 수 없음
			faceNormal = (v1 - v0).Cross(v2 - v0);
			faceNormal.Normalize();

			// 삼각형 뒷면을 그리고 싶지 않은 경우 (Backface culling)
			// if (... < 0.0f) return false;
			if (-dir.Dot(faceNormal) < 0.0f) return false;

			// 평면과 광선이 수평에 매우 가깝다면 충돌하지 못하는 것으로 판단
			// if (... < 1e-2f) return false; // t 계산시 0으로 나누기 방지
			if (abs(dir.Dot(faceNormal)) < 1e-2f) return false;

			/* 2. 광선과 평면의 충돌 위치 계산 */
			// t = ...
			t = (v0.Dot(faceNormal) - origin.Dot(faceNormal)) / dir.Dot(faceNormal);

			// 광선의 시작점 이전에 충돌한다면 렌더링할 필요 없음
			// if (...) return false;
			if (t < 0.0f) return false;

			// point = origin + t * dir; // 충돌점
			point = origin + t * dir;

			/* 3. 그 충돌 위치가 삼각형 안에 들어 있는 지 확인 */

			// 작은 삼각형들 3개의 normal 계산
			// const vec3 normal0 = ...
			// const vec3 normal1 = ...
			// const vec3 normal2 = ...
			// 방향만 확인하면 되기 때문에 normalize() 생략 가능
			const Vector3 cross0 = (point - v2).Cross(v1 - v2);
			const Vector3 cross1 = (point - v0).Cross(v2 - v0);
			const Vector3 cross2 = (point - v1).Cross(v0 - v1);
			// 아래에서 cross product의 절대값으로 작은 삼각형들의 넓이 계산

			// if (dot(normal0, faceNormal) < 0.0f) return false;
			// if (dot(normal1, faceNormal) < 0.0f) return false;
			// if (dot(normal2, faceNormal) < 0.0f) return false;

			if (cross0.Dot(faceNormal) < 0.0f) return false;
			if (cross1.Dot(faceNormal) < 0.0f) return false;
			if (cross2.Dot(faceNormal) < 0.0f) return false;

			// Barycentric coordinates 계산
			// 텍스춰링(texturing)에서 사용
			// 아래에서 cross product의 절대값으로 작은 삼각형들의 넓이 계산

			// const float area0 = ...
			// const float area1 = ...
			// const float area2 = ...
			//const float area0 = cross0.Length() * 0.5f;
			//const float area1 = cross1.Length() * 0.5f;
			//const float area2 = cross2.Length() * 0.5f;
			//비율이기 때문에 * 0.5 생략 가능
			const float area0 = cross0.Length();
			const float area1 = cross1.Length();
			const float area2 = cross2.Length();

			// const float areaSum = ...

			// 기호에 alpha, beta, gamma 또는 u, v, w 등을 사용하기도 함
			//w0 = 0.0f; //임시 값
			//w1 = 0.0f; //임시 값
			const float areaSum = area0 + area1 + area2;

			w0 = area0 / areaSum;
			w1 = area1 / areaSum;

			return true;
		}
	};
}