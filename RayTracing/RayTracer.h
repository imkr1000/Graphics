#pragma once

#include "Sphere.h"
#include "Light.h"

#include <vector>

namespace JYKim
{
    using namespace std;
    
    class RayTracer
    {
    private:
        int width, height;
		Light light;
        shared_ptr<Sphere> sphere;

    public:
        RayTracer(const int width, const int height)
            : width(width), height(height)
        {
            sphere = make_shared<Sphere>(Vector3(0.0f, 0.0f, 0.5f), 0.5f, Color(1.0f, 1.0f, 1.0f, 1.0f));
			sphere->SetDiffuse(Vector3(0.0f, 0.0f, 1.0f));
			sphere->SetSpecular(Vector3(1.0f, 1.0f, 1.0f));
			sphere->SetKs(0.8f);
			sphere->SetAlpha(9.0f);

			light = Light{ Vector3(0.0f, 0.0f, -1.0f) }; // 화면 뒷쪽
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
        Color TraceRay(Ray& ray) const
        {
            const Hit& hit = sphere->IntersectRayCollision(ray);

            if (hit.distance < 0.0f)
            {
                return Color();
            }
            else
            {
				//앞에서 쓴 색 결정
                //return sphere->GetColor() * hit.distance; // 깊이(Ray를 쏜 pixel로부터의 거리 d)를 곱해서 입체감 만들기(거리가 가까우면 어둡고 멀면 밝음)
                //현재는 조명, 원근감 등이 표현되지 않았기 때문에 현실감이 조금 떨어지는 상태
                //return sphere->color / (1.0f + hit.distance); // 가까이 있는 걸 밝게, 멀리 있는 건 어둡게 나타내 더 자연스러움

				// 여기서 퐁 모델(Phong reflection model)으로 조명 효과 계산
				// 참고 자료
				// https://en.wikipedia.org/wiki/Phong_reflection_model
				// https://www.scratchapixel.com/lessons/3d-basic-rendering/phong-shader-BRDF/phong-illumination-models-brdf.html

				// TODO:
				// Diffuse
				// const Vector3 dirToLight = ...
				const Vector3 n = hit.normal;
				const Vector3 l = light.pos - hit.point;
				const_cast<Vector3&>(l).Normalize();
				const float diff = max(n.Dot(l), 0.0f);
				
				// Specular
				// const Vector3 reflectDir = ... // r = 2 (n dot l) n - l
				const Vector3 r = 2.0f * n.Dot(l) * n - l;
				const float specular = pow(max((-ray.dir).Dot(r), 0.0f), sphere->GetAlpha());

				//return sphere->GetAmbient();
				//return sphere->GetDiffuse() * diff;
				//return sphere->GetSpecular() * specular * sphere->GetKs();

				return Color(sphere->GetAmbient() + sphere->GetDiffuse() * diff + sphere->GetSpecular() * specular * sphere->GetKs());
            }
        }

        void Render(vector<Color>& pixels) const
        {
            ImGui::Begin("Sphere");
            ImGui::SliderFloat3("Center", sphere->GetCenterFloatAddress(), -1.0f, 1.0f);
            ImGui::SliderFloat("Radius", sphere->GetRadiusAddress(), 0.0f, 1.0f);
			ImGui::SliderFloat3("Light", const_cast<float*>(&light.pos.x), -2.0f, 2.0f);
            ImGui::SliderFloat3("Ambient Color", sphere->GetAmbientFloatAddress(), 0.0f, 1.0f);
            ImGui::SliderFloat3("Diffuse Color", sphere->GetDiffuseFloatAddress(), 0.0f, 1.0f);
            ImGui::SliderFloat3("Specular Color", sphere->GetSpecularFloatAddress(), 0.0f, 1.0f);
			ImGui::SliderFloat("Specular Power", sphere->GetAlphaAddress(), 0.0f, 100.0f);
			ImGui::SliderFloat("Specular coeff", sphere->GetKsAddress(), 0.0f, 1.0f);
            ImGui::End();

            fill(pixels.begin(), pixels.end(), Color());

#pragma omp parallel for
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                {
                    const Vector3 pixelWorldPos = TransformScreenToWorld(Vector2(float(x), float(y)));

					// 광선의 방향 벡터
					// 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
					// Orthographic projection (정투영) vs perspective projection (원근투영)
					// 현재는 직교투영이기 때문에 원근감이 느껴지지 않는다(즉 물체와 픽셀의 거리에 따른 크기 차이가 없다)
                    const auto rayDir = Vector3(0.0f, 0.0f, 1.0f);

                    Ray pixelRay{ pixelWorldPos, rayDir };

					// index에는 size_t형 사용 (index가 음수일 수는 없으니까)
					// traceRay()의 반환형은 vec3 (RGB), A는 불필요
                    pixels[size_t(x + width * y)] = TraceRay(pixelRay);
                }
        }
    };
}