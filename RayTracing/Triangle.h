#pragma once

#include "Object.h"

namespace JYKim
{
	class Triangle : public Object
	{
	public:
		Vector3 v0, v1, v2;
		// Vector2 uv0, uv1, uv2; // �ڿ��� �ؽ��� ��ǥ��� ���

	public:
		Triangle() : v0(Vector3()), v1(Vector3()), v2(Vector3())
		{
		}

		Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2)
			: v0(v0), v1(v1), v2(v2)
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

				// Barycentric coordinates Ȯ�ο�
				hit.w = Vector2(w0, w1);

				// �ؽ��縵(texturing)���� ���
				// hit.uv = uv0 * u + uv1 * v + uv2 * (1.0f - u - v);
			}

			return hit;
		}

		// ���� ���α׷����� �����Ͻô� �е��� ���� �����غ��ø� ����,
		// ��κ��� ���丸 �����صνð� Ȱ���ϴ� �������� �����ϼŵ� ����մϴ�.
		// �� ���ذ� ���� �ʴ´ٸ� ���� �ڷ�� ���� �����ϸ鼭 �����Ͻô� �����
		// �����ϴ�. ����:
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
		bool IntersectRayTriangle(const Vector3& origin, const Vector3& dir,
			const Vector3& v0, const Vector3& v1, const Vector3 v2,
			Vector3& point, Vector3& faceNormal,
			float& t, float& w0, float& w1) const
		{
			/*
			 * �⺻ ����
			 * - �ﰢ���� �����ִ� ���� ������ ������ ã��,
			 * - �� ������ �ﰢ�� �ȿ� �ִ��� �ۿ� �ִ����� �Ǵ��Ѵ�.
			 */

			 /* 1. �ﰢ���� ���� �ִ� ����� ���� ���� ��� */
			 // faceNormal = ...;
			 //����: �ﰢ���� ���̰� 0�� ��쿡�� ����� �� ����
			faceNormal = (v1 - v0).Cross(v2 - v0);
			faceNormal.Normalize();

			// �ﰢ�� �޸��� �׸��� ���� ���� ��� (Backface culling)
			// if (... < 0.0f) return false;
			if (-dir.Dot(faceNormal) < 0.0f) return false;

			// ���� ������ ���� �ſ� �����ٸ� �浹���� ���ϴ� ������ �Ǵ�
			// if (... < 1e-2f) return false; // t ���� 0���� ������ ����
			if (abs(dir.Dot(faceNormal)) < 1e-2f) return false;

			/* 2. ������ ����� �浹 ��ġ ��� */
			// t = ...
			t = (v0.Dot(faceNormal) - origin.Dot(faceNormal)) / dir.Dot(faceNormal);

			// ������ ������ ������ �浹�Ѵٸ� �������� �ʿ� ����
			// if (...) return false;
			if (t < 0.0f) return false;

			// point = origin + t * dir; // �浹��
			point = origin + t * dir;

			/* 3. �� �浹 ��ġ�� �ﰢ�� �ȿ� ��� �ִ� �� Ȯ�� */

			// ���� �ﰢ���� 3���� normal ���
			// const vec3 normal0 = ...
			// const vec3 normal1 = ...
			// const vec3 normal2 = ...
			// ���⸸ Ȯ���ϸ� �Ǳ� ������ normalize() ���� ����
			const Vector3 cross0 = (point - v2).Cross(v1 - v2);
			const Vector3 cross1 = (point - v0).Cross(v2 - v0);
			const Vector3 cross2 = (point - v1).Cross(v0 - v1);
			// �Ʒ����� cross product�� ���밪���� ���� �ﰢ������ ���� ���

			// if (dot(normal0, faceNormal) < 0.0f) return false;
			// if (dot(normal1, faceNormal) < 0.0f) return false;
			// if (dot(normal2, faceNormal) < 0.0f) return false;

			if (cross0.Dot(faceNormal) < 0.0f) return false;
			if (cross1.Dot(faceNormal) < 0.0f) return false;
			if (cross2.Dot(faceNormal) < 0.0f) return false;

			// Barycentric coordinates ���
			// �ؽ��縵(texturing)���� ���
			// �Ʒ����� cross product�� ���밪���� ���� �ﰢ������ ���� ���

			// const float area0 = ...
			// const float area1 = ...
			// const float area2 = ...
			//const float area0 = cross0.Length() * 0.5f;
			//const float area1 = cross1.Length() * 0.5f;
			//const float area2 = cross2.Length() * 0.5f;
			//�����̱� ������ * 0.5 ���� ����
			const float area0 = cross0.Length();
			const float area1 = cross1.Length();
			const float area2 = cross2.Length();

			// const float areaSum = ...

			// ��ȣ�� alpha, beta, gamma �Ǵ� u, v, w ���� ����ϱ⵵ ��
			//w0 = 0.0f; //�ӽ� ��
			//w1 = 0.0f; //�ӽ� ��
			const float areaSum = area0 + area1 + area2;

			w0 = area0 / areaSum;
			w1 = area1 / areaSum;

			return true;
		}
	};
}