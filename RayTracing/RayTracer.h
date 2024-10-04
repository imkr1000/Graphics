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

    public:
        RayTracer(const int width, const int height)
            : width(width), height(height)
        {
			// sphere1�� ���� ������� �����մϴ�. ���� �ǽ��ϼŵ� �˴ϴ�.
			auto sphere1 = make_shared<Sphere>(Vector3(1.0f, 0.0f, 1.5f), 0.8f);
			sphere1->SetAmbient(Vector3(0.2f));
			sphere1->SetDiffuse(Vector3(1.0f, 0.2f, 0.2f));
			sphere1->SetSpecular(Vector3(0.5f));
			sphere1->SetAlpha(10.0f);
			
			objects.push_back(sphere1);

			//auto imageTexture = make_shared<Texture>("_Textures/abstract.jpg");

			const float uvRatio = 1.0f;

			auto square = make_shared<Square>(Vector3(-2.0f, 2.0f, 2.0f), Vector3(2.0f), Vector3(2.0f, -2.0f, 2.0f), Vector3(-2.0f, -2.0f, 2.0f),
				Vector2(0.0f, 0.0f), Vector2(uvRatio, 0.0f), Vector2(uvRatio, uvRatio), Vector2(0.0f, uvRatio));

			square->amb = Vector3(0.2f);
			square->diff = Vector3(1.0f);
			square->spec = Vector3(0.0f);

			//square->ambTexture = imageTexture;
			//square->difTexture = imageTexture;

			objects.push_back(square);

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
					closestHit.uv = hit.uv;

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
            //float aspectRatio = float(width) / height;
            ////float transformedX = screenPosition.x / (width - 1) * aspectRatio * 2.0f - aspectRatio;
            //float transformedX = aspectRatio * (screenPosition.x / (width - 1) * 2.0f - 1.0f);
            //float transformedY = -screenPosition.y / (height - 1) * 2.0f + 1.0f;
			//
            //// 3���� �������� Ȯ�� (z��ǥ�� 0.0���� ����)
            //return Vector3(transformedX, transformedY, 0.0f);
			const float xScale = 2.0f / this->width;
			const float yScale = 2.0f / this->height;
			const float aspect = float(this->width) / this->height;

			// 3���� �������� Ȯ�� (z��ǥ�� 0.0)
			return Vector3((screenPosition.x * xScale - 1.0f) * aspect, -screenPosition.y * yScale + 1.0f, 0.0f);
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
				if (hit.obj->ambTexture) // �ӽ÷� �ﰢ���� ���� ���� ����
				{
					//FindClosestCollision�� ���� ��ü�� ray�� �浹 �������� �޾ƿ� uv ���� �Ѿ��,
					//��������� shading�� �� �ؽ��ķκ��� uv��ǥ�� �ش��ϴ� ���� ���� Sampling�� �� ���ȴ�
					//Texture Ŭ������ ó�� �ٷ� �̹��� Ŭ������ ����� �����ϴ�.
					//SmaplePoint �ǽ�
					//pointColor = hit.obj->amb * hit.obj->ambTexture->SamplePoint(hit.uv);
					pointColor = hit.obj->amb * hit.obj->ambTexture->SampleLinear(hit.uv);
				}
				else
				{
					pointColor = Color(hit.obj->amb);
				}

				// Diffuse
				Vector3 dirToLight = light.pos - hit.point;
				dirToLight.Normalize();

				// �׸��� ������
				//Ray shadowRay = { hit.point + 1e-4f * dirToLight, dirToLight };
				//if (FindClosestCollision(shadowRay).obj != nullptr
				//	&& FindClosestCollision(shadowRay).distance <= (light.pos - hit.point).Length())
				//	return Color(hit.obj->amb);

				
				const float diff = max(hit.normal.Dot(dirToLight), 0.0f);

				// Specular
				const Vector3 reflectDir = 2.0f * hit.normal.Dot(dirToLight) * hit.normal - dirToLight;
				const float specualr = pow(max(-ray.dir.Dot(reflectDir), 0.0f), hit.obj->alpha);

				if (hit.obj->difTexture)
				{
					pointColor += Color(diff * hit.obj->diff * hit.obj->difTexture->SampleLinear(hit.uv));
				}
				else
				{
					pointColor += Color(diff * hit.obj->diff);
				}
				
				pointColor += Color(hit.obj->spec * specualr);
				
				return pointColor;
			}

			return Color();
        }

		Color TraceRay2x2(const Vector3& eyePos, Vector3 pixelPos, const float dx, const int recursiveLevel) const
		{
			//recursiveLevel : ��� ȣ���� �� �� �� �ų�
			if (recursiveLevel == 0)
			{
				Vector3 rayDir;
				(pixelPos - eyePos).Normalize(rayDir);
				Ray myRay{ pixelPos, rayDir };
				return TraceRay(myRay);
			}

			const float subdx = dx * 0.5f;

			Color pixelColor;
			//ó�� pixelPos�� �ȼ��� ���ϴ� ���� �ȼ��� ������(���� �ȼ� �߾�)���� ��´�
			pixelPos = Vector3(pixelPos.x - subdx * 0.5f, pixelPos.y - subdx * 0.5f, pixelPos.z);
			//[����] ���� ����� �޸� subdx�� 0.5f�� ��������ϴ�.

			// ...
			for (int y = 0; y < 2; ++y)
				for (int x = 0; x < 2; ++x)
				{
					//4���� ���� �ȼ��� ���� ������(�߾� ��ġ)�� ����
					Vector3 subPos(pixelPos.x + x * subdx, pixelPos.y + y * subdx, pixelPos.z);
					//4���� ���� �ȼ��� ���� �ٽ� traceRay2x2
					pixelColor += TraceRay2x2(eyePos, subPos, subdx, recursiveLevel - 1);
				}

			//4���� ���� �ȼ� �� ���� pixelColor�� �����Ǿ����Ƿ� ��հ� ������ ���� 4�� ������
			return pixelColor * 0.25f;
		}

        void Render(vector<Color>& pixels) const
        {
			fill(pixels.begin(), pixels.end(), Color());

			const Vector3 eyePos(0.0f, 0.0f, -1.5f);

			const float dx = 2.0f / height;

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

					//�������� ����
					auto rayDir = pixelWorldPos - eyePos;
					rayDir.Normalize();
					
                    Ray pixelRay{ pixelWorldPos, rayDir };
					//
					//// index���� size_t�� ��� (index�� ������ ���� �����ϱ�)
					//// traceRay()�� ��ȯ���� vec3 (RGB), A�� ���ʿ�
                    //pixels[size_t(x + width * y)] = TraceRay(pixelRay);

					//SuperSampling ����
					//const auto pixelColor = TraceRay2x2(eyePos, pixelWorldPos, dx, 3);
					pixels[size_t(x + width * y)] = TraceRay2x2(eyePos, pixelWorldPos, dx, 3);
                }
#pragma warning (default : 6993)
        }
    };
}