#pragma once

#include "Sphere.h"
#include "Light.h"

#include <vector>

namespace JYKim
{
    using namespace std;
    
    class RayTracer
    {
    public:
        int width, height;
		Light light;
        shared_ptr<Sphere> sphere;

    public:
        RayTracer(const int width, const int height)
            : width(width), height(height)
        {
            sphere = make_shared<Sphere>(Vector3(0.0f, 0.0f, 0.5f), 0.4f, Color(1.0f, 1.0f, 1.0f, 1.0f));
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
                return sphere->color * hit.distance; // 깊이(Ray를 쏜 pixel로부터의 거리 d)를 곱해서 입체감 만들기(거리가 가까우면 어둡고 멀면 밝음)
                //현재는 조명, 원근감 등이 표현되지 않았기 때문에 현실감이 조금 떨어지는 상태
                //return sphere->color / (1.0f + hit.distance); // 가까이 있는 걸 밝게, 멀리 있는 건 어둡게 나타내 더 자연스러움
            }
        }

        void Render(vector<Color>& pixels) const
        {
            ImGui::Begin("Sphere");
            ImGui::SliderFloat3("Center", sphere->GetCenterFloatAddress(), -1.0f, 1.0f);
            ImGui::SliderFloat("Radius", sphere->GetRadiusAddress(), 0.0f, 1.0f);
            ImGui::SliderFloat3("RGB", sphere->GetColorFloatAddress(), 0.0f, 1.0f);
            ImGui::End();

            fill(pixels.begin(), pixels.end(), Color());

#pragma omp parallel for
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                {
                    const Vector3 pixelWorldPos = TransformScreenToWorld(Vector2(float(x), float(y)));

                    const auto rayDir = Vector3(0.0f, 0.0f, 1.0f);

                    Ray pixelRay{ pixelWorldPos, rayDir };

                    pixels[size_t(x + width * y)] = TraceRay(pixelRay);
                }
        }
    };
}