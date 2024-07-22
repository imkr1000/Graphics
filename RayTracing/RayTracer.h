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
			sphere->amb
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
        Color TraceRay(Ray& ray) const
        {
            const Hit& hit = sphere->IntersectRayCollision(ray);

            if (hit.distance < 0.0f)
            {
                return Color();
            }
            else
            {
                return sphere->GetColor() * hit.distance; // ����(Ray�� �� pixel�κ����� �Ÿ� d)�� ���ؼ� ��ü�� �����(�Ÿ��� ������ ��Ӱ� �ָ� ����)
                //����� ����, ���ٰ� ���� ǥ������ �ʾұ� ������ ���ǰ��� ���� �������� ����
                //return sphere->color / (1.0f + hit.distance); // ������ �ִ� �� ���, �ָ� �ִ� �� ��Ӱ� ��Ÿ�� �� �ڿ�������
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

					// ������ ���� ����
					// ��ũ���� ������ z����, ���밪 1.0�� ���� ����
					// Orthographic projection (������) vs perspective projection (��������)
					// ����� ���������̱� ������ ���ٰ��� �������� �ʴ´�(�� ��ü�� �ȼ��� �Ÿ��� ���� ũ�� ���̰� ����)
                    const auto rayDir = Vector3(0.0f, 0.0f, 1.0f);

                    Ray pixelRay{ pixelWorldPos, rayDir };

					// index���� size_t�� ��� (index�� ������ ���� �����ϱ�)
					// traceRay()�� ��ȯ���� vec3 (RGB), A�� ���ʿ�
                    pixels[size_t(x + width * y)] = TraceRay(pixelRay);
                }
        }
    };
}