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

			tempObject = triangle; // 따로 처리하기 위해 임시로 저장

			objects.push_back(triangle);

			light = Light{ Vector3(0.0f, 1.0f, 0.5f) }; // 화면 뒷쪽
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

					// Barycentric coordinated 복사
					closestHit.w = hit.w;

					// 텍스쳐 좌표
					// closestHit.uv = hit.uv;
				}
			}

			return closestHit;
		}

        Vector3 TransformScreenToWorld(const Vector2& screenPosition) const
        {
            // 여기서 좌표계 변환 구현
            // 스크린 좌표계는 [0, width-1] x [0, height-1]
            // 여기서 우리가 정의한 월드(World) 좌표계는 [-aspect, +aspect] x [-1, +1]
            // 화면비율 aspect = float(width) / height
            float aspectRatio = float(width) / height;
            //float transformedX = screenPosition.x / (width - 1) * aspectRatio * 2.0f - aspectRatio;
            float transformedX = aspectRatio * (screenPosition.x / (width - 1) * 2.0f - 1.0f);
            float transformedY = -screenPosition.y / (height - 1) * 2.0f + 1.0f;

            // 3차원 공간으로 확장 (z좌표는 0.0으로 가정)
            return Vector3(transformedX, transformedY, 0.0f);
        }

        // 광선이 물체에 닿으면 그 물체의 색 반환
        Color TraceRay(const Ray& ray) const
        {
			// Render first hit
			const Hit& hit = FindClosestCollision(ray);

			if (hit.distance >= 0.0f)
			{
				Color pointColor;

				// Amibent
				pointColor = Color(hit.obj->amb);

				if (hit.obj == this->tempObject) // 임시로 삼각형만 색을 직접 결정
				{
					// Barycentric coordinates를 이용한 색 섞기(interpolation)
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

				// 그림자 꺼놓음
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

					// 광선의 방향 벡터
					// 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
					// Orthographic projection (정투영) vs perspective projection (원근투영)
					// 현재는 직교투영이기 때문에 원근감이 느껴지지 않는다(즉 물체와 픽셀의 거리에 따른 크기 차이가 없다)
                    //const auto rayDir = Vector3(0.0f, 0.0f, 1.0f); //직교투영
					auto rayDir = pixelWorldPos - eyePos;
					rayDir.Normalize();

                    Ray pixelRay{ pixelWorldPos, rayDir };
					
					// index에는 size_t형 사용 (index가 음수일 수는 없으니까)
					// traceRay()의 반환형은 vec3 (RGB), A는 불필요
                    pixels[size_t(x + width * y)] = TraceRay(pixelRay);
                }
#pragma warning (default : 6993)
        }
    };
}