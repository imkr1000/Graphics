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

			light = Light{ Vector3(0.0f, 0.0f, -1.0f) }; // ȭ�� ����
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
				//�տ��� �� �� ����
                //return sphere->GetColor() * hit.distance; // ����(Ray�� �� pixel�κ����� �Ÿ� d)�� ���ؼ� ��ü�� �����(�Ÿ��� ������ ��Ӱ� �ָ� ����)
                //����� ����, ���ٰ� ���� ǥ������ �ʾұ� ������ ���ǰ��� ���� �������� ����
                //return sphere->color / (1.0f + hit.distance); // ������ �ִ� �� ���, �ָ� �ִ� �� ��Ӱ� ��Ÿ�� �� �ڿ�������

				// ���⼭ �� ��(Phong reflection model)���� ���� ȿ�� ���
				// ���� �ڷ�
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