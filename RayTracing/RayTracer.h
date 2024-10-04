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
			auto sphere1 = make_shared<Sphere>(Vector3(0.0f, -0.1f, 1.5f), 1.0f);
			sphere1->SetAmbient(Vector3(0.1f));
			sphere1->SetDiffuse(Vector3(1.0f, 0.0f, 0.0f));
			sphere1->SetSpecular(Vector3(1.0f));
			sphere1->SetAlpha(10.0f);
			sphere1->reflection = 0.5f;
			sphere1->transparency = 0.0f;
			
			objects.push_back(sphere1);

			auto sphere2 = make_shared<Sphere>(Vector3(1.2f, -0.1f, 0.5f), 0.4f);
			sphere2->SetAmbient(Vector3(0.0f));
			sphere2->SetDiffuse(Vector3(0.0f, 0.0f, 1.0f));
			sphere2->SetSpecular(Vector3(1.0f));
			sphere2->SetAlpha(50.0f);
			sphere2->reflection = 0.0f;

			objects.push_back(sphere2);

			auto groundTexture = make_shared<Texture>("_Textures/abstract.jpg");

			const float uvRatio = 1.0f;

			auto ground = make_shared<Square>(Vector3(-10.0f, -1.2f, 0.0f), Vector3(-10.0f, -1.2f, 10.0f), Vector3(10.0f, -1.2f, 10.0f), Vector3(10.0f, -1.2f, 0.0f),
				Vector2(0.0f, 0.0f), Vector2(uvRatio, 0.0f), Vector2(uvRatio, uvRatio), Vector2(0.0f, uvRatio));

			ground->amb = Vector3(1.0f);
			ground->diff = Vector3(1.0f);
			ground->spec = Vector3(1.0f);
			ground->alpha = 10.0f;
			ground->reflection = 0.0f;

			ground->ambTexture = groundTexture;
			ground->difTexture = groundTexture;

			objects.push_back(ground);

			light = Light{ Vector3(0.0f, 0.5f, -0.5f) }; // 화면 뒷쪽
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

					// 텍스쳐 좌표
					closestHit.uv = hit.uv;
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
            //float aspectRatio = float(width) / height;
            ////float transformedX = screenPosition.x / (width - 1) * aspectRatio * 2.0f - aspectRatio;
            //float transformedX = aspectRatio * (screenPosition.x / (width - 1) * 2.0f - 1.0f);
            //float transformedY = -screenPosition.y / (height - 1) * 2.0f + 1.0f;
			//
            //// 3차원 공간으로 확장 (z좌표는 0.0으로 가정)
            //return Vector3(transformedX, transformedY, 0.0f);
			const float xScale = 2.0f / this->width;
			const float yScale = 2.0f / this->height;
			const float aspect = float(this->width) / this->height;

			// 3차원 공간으로 확장 (z좌표는 0.0)
			return Vector3((screenPosition.x * xScale - 1.0f) * aspect, -screenPosition.y * yScale + 1.0f, 0.0f);
        }

        // 광선이 물체에 닿으면 그 물체의 색 반환
        Color TraceRay(const Ray& ray, int recurseLevel) const
        {
			// Render first hit
			const Hit& hit = FindClosestCollision(ray);

			if (hit.distance >= 0.0f)
			{
				Color color;
				Color phongColor;

				// Amibent
				if (hit.obj->ambTexture) // 임시로 삼각형만 색을 직접 결정
				{
					//FindClosestCollision을 통해 물체와 ray의 충돌 지점에서 받아온 uv 값이 넘어와,
					//결과적으로 shading할 때 텍스쳐로부터 uv좌표에 해당하는 색상 값을 Sampling할 때 사용된다
					//Texture 클래스는 처음 다룬 이미지 클래스와 상당히 유사하다.
					//SmaplePoint 실습
					//pointColor = hit.obj->amb * hit.obj->ambTexture->SamplePoint(hit.uv);
					phongColor = hit.obj->amb * hit.obj->ambTexture->SampleLinear(hit.uv);
				}
				else
				{
					phongColor = Color(hit.obj->amb);
				}

				// Diffuse
				Vector3 dirToLight = light.pos - hit.point;
				dirToLight.Normalize();

				// 그림자 꺼놓음
				//Ray shadowRay = { hit.point + 1e-4f * dirToLight, dirToLight };
				//if (FindClosestCollision(shadowRay).obj != nullptr
				//	&& FindClosestCollision(shadowRay).distance <= (light.pos - hit.point).Length())
				//	return Color(hit.obj->amb);

				
				const float diff = max(hit.normal.Dot(dirToLight), 0.0f);

				if (hit.obj->difTexture)
				{
					phongColor += Color(diff * hit.obj->diff * hit.obj->difTexture->SampleLinear(hit.uv));
				}
				else
				{
					phongColor += Color(diff * hit.obj->diff);
				}

				// Specular
				const Vector3 reflectDir = 2.0f * hit.normal.Dot(dirToLight) * hit.normal - dirToLight;
				const float specualr = pow(max(-ray.dir.Dot(reflectDir), 0.0f), hit.obj->alpha);
				
				phongColor += Color(hit.obj->spec * specualr);

				//최종 색상이 될 color에 phongColor와 reflection을 적정 비율로 섞어서 누적)
				//1.0f(원래 광선의 비율) - 반사량(비율) - 투과량(비율)
				//여기서는 투과량은 0으로 가정(물체가 투명할 경우 투과량이 있음)
				color += phongColor * (1.0f - hit.obj->reflection - hit.obj->transparency);
				
				if (hit.obj->reflection)
				{
					// 여기에 반사 구현
					// 수치 오류 주의
					// 반사광이 반환해준 색을 더할 때의 비율은 hit.obj->reflection

					//const vec3 reflectedDirection = ...
					//color += ...

					//const vec3 m = ray.dir - glm::dot(hit.normal, ray.dir) * hit.normal;
					//-d + 2m
					//2m = 2.0f * ray.dir - dot(hit.normal, ray.dir) * hit.normal * 2.0f;
					//2m - d = ray.dir - dot(hit.normal, ray.dir) * hit.normal * 2.0f;
					Vector3 reflectedDirection;
					(ray.dir - ray.dir.Dot(hit.normal) * hit.normal * 2.0f).Normalize(reflectedDirection);
					Ray reflectionRay = { hit.point + reflectedDirection * 1e-4f, reflectedDirection };
					color += hit.obj->reflection * TraceRay(reflectionRay, recurseLevel - 1);
				}

				if (hit.obj->transparency)
				{
					// 투명한 물체의 굴절 처리
				}

				return color;
			}

			return Color();
        }

		Color TraceRay2x2(const Vector3& eyePos, Vector3 pixelPos, const float dx, const int recursiveLevel) const
		{
			//recursiveLevel : 재귀 호출을 몇 번 할 거냐
			if (recursiveLevel == 0)
			{
				Vector3 rayDir;
				(pixelPos - eyePos).Normalize(rayDir);
				Ray myRay{ pixelPos, rayDir };
				return TraceRay(myRay, 5);
			}

			const float subdx = dx * 0.5f;

			Color pixelColor;
			//처음 pixelPos는 픽셀의 좌하단 서브 픽셀의 포지션(서브 픽셀 중앙)으로 잡는다
			pixelPos = Vector3(pixelPos.x - subdx * 0.5f, pixelPos.y - subdx * 0.5f, pixelPos.z);
			//[수정] 강의 영상과 달리 subdx에 0.5f를 곱해줬습니다.

			// ...
			for (int y = 0; y < 2; ++y)
				for (int x = 0; x < 2; ++x)
				{
					//4개의 서브 픽셀에 대한 포지션(중앙 위치)를 결정
					Vector3 subPos(pixelPos.x + x * subdx, pixelPos.y + y * subdx, pixelPos.z);
					//4개의 서브 픽셀에 대해 다시 traceRay2x2
					pixelColor += TraceRay2x2(eyePos, subPos, subdx, recursiveLevel - 1);
				}

			//4개의 서브 픽셀 색 값이 pixelColor에 누적되었으므로 평균값 도출을 위해 4로 나눈다
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

					// 광선의 방향 벡터
					// 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
					// Orthographic projection (정투영) vs perspective projection (원근투영)
					// 현재는 직교투영이기 때문에 원근감이 느껴지지 않는다(즉 물체와 픽셀의 거리에 따른 크기 차이가 없다)
                    //const auto rayDir = Vector3(0.0f, 0.0f, 1.0f); //직교투영

					//원근투영 적용
					auto rayDir = pixelWorldPos - eyePos;
					rayDir.Normalize();
					
                    Ray pixelRay{ pixelWorldPos, rayDir };
					
					// index에는 size_t형 사용 (index가 음수일 수는 없으니까)
					// traceRay()의 반환형은 vec3 (RGB), A는 불필요
                    pixels[size_t(x + width * y)] = TraceRay(pixelRay, 5);

					//SuperSampling 적용
					//Vector3 pixelColor;
					//TraceRay2x2(eyePos, pixelWorldPos, dx, 3).ToVector3().Clamp(Vector3(0.0f), Vector3(1.0f), pixelColor);
					//pixels[size_t(x + width * y)] = Color(pixelColor);
                }
#pragma warning (default : 6993)
        }
    };
}