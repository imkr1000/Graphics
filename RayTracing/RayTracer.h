#pragma once

#include "Sphere.h"
#include "Light.h"
#include "Triangle.h"
#include "Square.h"

#include <vector>

namespace JYKim
{
    using namespace std;
    
    class RayTracer
    {
    public:
        int width, height;
		Light light;
        vector<shared_ptr<Object>> objects;
        shared_ptr<Object> tempObject;

    public:
        RayTracer(const int width, const int height)
            : width(width), height(height)
        {
			auto sphere1 = make_shared<Sphere>(Vector3(1.0f, 0.0f, 1.5f), 0.4f);
			sphere1->SetAmbient(Vector3(0.2f));
			sphere1->SetDiffuse(Vector3(1.0f, 0.2f, 0.2f));
			sphere1->SetSpecular(Vector3(0.5f));
			//sphere1->SetAlpha(50.0f);

			objects.push_back(sphere1);

			//auto triangle1 = make_shared<Triangle>(Vector3(-2.0f, -2.0f, 2.0f), Vector3(-2.0f, 2.0f, 2.0f), Vector3(2.0f, 2.0f, 2.0f));
			//
			//triangle1->amb = Vector3(0.2f);
			//triangle1->diff = Vector3(0.5f);
			//triangle1->spec = Vector3(0.5f);
			//triangle1->alpha = 5.0f;

			//objects.push_back(triangle1);

			auto triangle = make_shared<Triangle>(Vector3(-2.0f, -2.0f, 2.0f), Vector3(-2.0f, 2.0f, 2.0f), Vector3(2.0f, 2.0f, 2.0f));
			triangle->SetAmbient(Vector3(1.0f));
			triangle->SetDiffuse(Vector3(0.0f));
			triangle->SetSpecular(Vector3(0.0f));

			tempObject = triangle; // ���� ó���ϱ� ���� �ӽ÷� ����

			objects.push_back(triangle);

			light = Light{ Vector3(0.0f, 1.0f, 0.5f) }; // ȭ�� ����
        }

		Hit FindClosestCollision(const Ray& ray) const
		{
			float closestD = 1000.0f; // inf
			Hit closestHit = Hit{ -1.0f, Vector3(), Vector3() };

			for (const auto& obj : objects)
			{
				auto hit = obj->CheckRayCollision(ray);

				if (hit.distance >= 0.0f && hit.distance < closestD)
				{
					closestD = hit.distance;
					hit.obj = obj;
					closestHit = hit;

					// Barycentric coordinated ����
					closestHit.w = hit.w;

					// �ؽ��� ��ǥ
					// closestHit.uv = hit.uv;
				}
			}

			return closestHit;
		}

        Vector3 TransformScreenToWorld(const Vector2& screenPosition) const
        {
            // ���⼭ ��ǥ�� ��ȯ ����
            // ��ũ�� ��ǥ��� [0, width-1] x [0, height-1]
            // ���⼭ �츮�� ������ ����(World) ��ǥ��� [-aspect, +aspect] x [-1, +1]
            // ȭ����� aspect = float(width) / height
            float aspectRatio = float(width) / height;
            //float transformedX = screenPosition.x / (width - 1) * aspectRatio * 2.0f - aspectRatio;
            float transformedX = aspectRatio * (screenPosition.x / (width - 1) * 2.0f - 1.0f);
            float transformedY = -screenPosition.y / (height - 1) * 2.0f + 1.0f;

            // 3���� �������� Ȯ�� (z��ǥ�� 0.0���� ����)
            return Vector3(transformedX, transformedY, 0.0f);
        }

        // ������ ��ü�� ������ �� ��ü�� �� ��ȯ
        Color TraceRay(const Ray& ray) const
        {
			// Render first hit
			const Hit& hit = FindClosestCollision(ray);

			if (hit.distance >= 0.0f)
			{
				Color pointColor;

				// Amibent
				pointColor = Color(hit.obj->amb);

				if (hit.obj == this->tempObject) // �ӽ÷� �ﰢ���� ���� ���� ����
				{
					// Barycentric coordinates�� �̿��� �� ����(interpolation)
					const auto color0 = Color(1.0f, 0.0f, 0.0f, 1.0f);
					const auto color1 = Color(0.0f, 1.0f, 0.0f, 1.0f);
					const auto color2 = Color(0.0f, 0.0f, 1.0f, 1.0f);

					const float w0 = hit.w.x;
					const float w1 = hit.w.y;
					const float w2 = 1.0f - w0 - w1;

					pointColor = color0 * w0 + color1 * w1 + color2 * w2;
				}

				// Diffuse
				Vector3 dirToLight = light.pos - hit.point;
				dirToLight.Normalize();

				// �׸��� ������
				//Ray shadowRay = { hit.point + 1e-4f * dirToLight, dirToLight };
				//if (FindClosestCollision(shadowRay).obj != nullptr
				//	&& FindClosestCollision(shadowRay).distance <= (light.pos - hit.point).Length())
				//	return Color(hit.obj->amb);

				{
					const float diff = max(hit.normal.Dot(dirToLight), 0.0f);

					// Specular
					const Vector3 reflectDir = 2.0f * hit.normal.Dot(dirToLight) * hit.normal - dirToLight;
					const float specualr = pow(max(-ray.dir.Dot(reflectDir), 0.0f), hit.obj->alpha);

					pointColor += Color(diff * hit.obj->diff);
					pointColor += Color(hit.obj->spec * specualr);
				}
				
				return pointColor;
			}

			return Color();
        }

        void Render(vector<Color>& pixels) const
        {
			fill(pixels.begin(), pixels.end(), Color());

			const Vector3 eyePos(0.0f, 0.0f, -1.5f);

#pragma warning (disable : 6993)
#pragma omp parallel for
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                {
                    const Vector3 pixelWorldPos = TransformScreenToWorld(Vector2(float(x), float(y)));

					// ������ ���� ����
					// ��ũ���� ������ z����, ���밪 1.0�� ���� ����
					// Orthographic projection (������) vs perspective projection (��������)
					// ����� ���������̱� ������ ���ٰ��� �������� �ʴ´�(�� ��ü�� �ȼ��� �Ÿ��� ���� ũ�� ���̰� ����)
                    //const auto rayDir = Vector3(0.0f, 0.0f, 1.0f); //��������
					auto rayDir = pixelWorldPos - eyePos;
					rayDir.Normalize();

                    Ray pixelRay{ pixelWorldPos, rayDir };
					
					// index���� size_t�� ��� (index�� ������ ���� �����ϱ�)
					// traceRay()�� ��ȯ���� vec3 (RGB), A�� ���ʿ�
                    pixels[size_t(x + width * y)] = TraceRay(pixelRay);
                }
#pragma warning (default : 6993)
        }
    };
}