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
			auto sphere1 = make_shared<Sphere>(Vector3(0.1f, -0.5f, 2.0f), 1.0f);
			sphere1->SetAmbient(Vector3(0.0f));
			sphere1->SetDiffuse(Vector3(1.0f, 0.0f, 0.0f));
			sphere1->SetSpecular(Vector3(1.0f));
			sphere1->SetAlpha(100.0f);
			sphere1->reflection = 0.6f;
			sphere1->transparency = 0.0f;
			
			objects.push_back(sphere1);

			auto groundTexture = make_shared<Texture>("_Textures/abstract.jpg");

			const float uvRatio = 10.0f;

			auto ground = make_shared<Square>(Vector3(-5.0f, -1.5f, -5.0f), Vector3(-5.0f, -1.5f, 5.0f), Vector3(5.0f, -1.5f, 5.0f), Vector3(5.0f, -1.5f, -5.0f),
				Vector2(0.0f, 0.0f), Vector2(uvRatio, 0.0f), Vector2(uvRatio, uvRatio), Vector2(0.0f, uvRatio));

			ground->amb = Vector3(1.0f);
			ground->diff = Vector3(1.0f);
			ground->spec = Vector3(1.0f);
			ground->alpha = 10.0f;
			ground->reflection = 0.6f;
			ground->transparency = 0.3f;
			ground->ambTexture = groundTexture;
			ground->difTexture = groundTexture;

			objects.push_back(ground);

			auto squareTexturePosX = std::make_shared<Texture>("SaintPetersBasilica/posx.jpg");
			auto squarePosX = make_shared<Square>(Vector3(10.0f, 10.0f, 10.0f), Vector3(10.0f, 10.0f, -10.0f), Vector3(10.0f, -10.0f, -10.0f), Vector3(10.0f, -10.0f, 10.0f),
				Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f));

			squarePosX->amb = Vector3(1.0f);
			squarePosX->diff = Vector3(0.0f);
			squarePosX->spec = Vector3(0.0f);
			squarePosX->alpha = 10.0f;
			squarePosX->reflection = 0.0f;
			squarePosX->ambTexture = squareTexturePosX;
			squarePosX->difTexture = squareTexturePosX;

			objects.push_back(squarePosX);

			auto squareTexturePosY = std::make_shared<Texture>("SaintPetersBasilica/posy.jpg");
			auto squarePosY = make_shared<Square>(Vector3(-10.0f, 10.0f, -10.0f), Vector3(10.0f, 10.0f, -10.0f), Vector3(10.0f, 10.0f, 10.0f), Vector3(-10.0f, 10.0f, 10.0f),
				Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f));

			squarePosY->amb = Vector3(1.0f);
			squarePosY->diff = Vector3(0.0f);
			squarePosY->spec = Vector3(0.0f);
			squarePosY->alpha = 10.0f;
			squarePosY->reflection = 0.0f;
			squarePosY->ambTexture = squareTexturePosY;
			squarePosY->difTexture = squareTexturePosY;

			objects.push_back(squarePosY);

			auto squareTexturePosZ = std::make_shared<Texture>("SaintPetersBasilica/posz.jpg");
			auto squarePosZ = make_shared<Square>(Vector3(-10.0f, 10.0f, 10.0f), Vector3(10.0f, 10.0f, 10.0f), Vector3(10.0f, -10.0f, 10.0f), Vector3(-10.0f, -10.0f, 10.0f),
				Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f));

			squarePosZ->amb = Vector3(1.0f);
			squarePosZ->diff = Vector3(0.0f);
			squarePosZ->spec = Vector3(0.0f);
			squarePosZ->alpha = 10.0f;
			squarePosZ->reflection = 0.0f;
			squarePosZ->ambTexture = squareTexturePosZ;
			squarePosZ->difTexture = squareTexturePosZ;

			objects.push_back(squarePosZ);

			auto squareTextureNegX = std::make_shared<Texture>("SaintPetersBasilica/negx.jpg");
			auto squareNegX = make_shared<Square>(Vector3(-10.0f, 10.0f, -10.0f), Vector3(-10.0f, 10.0f, 10.0f), Vector3(-10.0f, -10.0f, 10.0f), Vector3(-10.0f, -10.0f, -10.0f),
				Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f));

			squareNegX->amb = Vector3(1.0f);
			squareNegX->diff = Vector3(0.0f);
			squareNegX->spec = Vector3(0.0f);
			squareNegX->alpha = 10.0f;
			squareNegX->reflection = 0.0f;
			squareNegX->ambTexture = squareTextureNegX;
			squareNegX->difTexture = squareTextureNegX;

			objects.push_back(squareNegX);

			auto squareTextureNegY = std::make_shared<Texture>("SaintPetersBasilica/negy.jpg");
			auto squareNegY = make_shared<Square>(Vector3(-10.0f, -10.0f, 10.0f), Vector3(10.0f, -10.0f, 10.0f), Vector3(10.0f, -10.0f, -10.0f), Vector3(-10.0f, -10.0f, -10.0f),
				Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f));

			squareNegY->amb = Vector3(1.0f);
			squareNegY->diff = Vector3(0.0f);
			squareNegY->spec = Vector3(0.0f);
			squareNegY->alpha = 10.0f;
			squareNegY->reflection = 0.0f;
			squareNegY->ambTexture = squareTextureNegY;
			squareNegY->difTexture = squareTextureNegY;

			objects.push_back(squareNegY);

			auto squareTextureNegZ = std::make_shared<Texture>("SaintPetersBasilica/negz.jpg");
			auto squareNegZ = make_shared<Square>(Vector3(10.0f, 10.0f, -10.0f), Vector3(-10.0f, 10.0f, -10.0f), Vector3(-10.0f, -10.0f, -10.0f), Vector3(10.0f, -10.0f, -10.0f),
				Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f));

			squareNegZ->amb = Vector3(1.0f);
			squareNegZ->diff = Vector3(0.0f);
			squareNegZ->spec = Vector3(0.0f);
			squareNegZ->alpha = 10.0f;
			squareNegZ->reflection = 0.0f;
			squareNegZ->ambTexture = squareTextureNegZ;
			squareNegZ->difTexture = squareTextureNegZ;

			objects.push_back(squareNegZ);

			light = Light{ Vector3(0.0f, 0.3f, -0.5f) }; // ȭ�� ����
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

					// �ؽ��� ��ǥ
					closestHit.uv = hit.uv;
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
        Color TraceRay(const Ray& ray, int recurseLevel) const
        {
			if (recurseLevel < 0)
				return Color();

			// Render first hit
			const Hit& hit = FindClosestCollision(ray);

			if (hit.distance >= 0.0f)
			{
				Color color;
				Color phongColor;

				// Amibent
				if (hit.obj->ambTexture) // �ӽ÷� �ﰢ���� ���� ���� ����
				{
					//FindClosestCollision�� ���� ��ü�� ray�� �浹 �������� �޾ƿ� uv ���� �Ѿ��,
					//��������� shading�� �� �ؽ��ķκ��� uv��ǥ�� �ش��ϴ� ���� ���� Sampling�� �� ���ȴ�
					//Texture Ŭ������ ó�� �ٷ� �̹��� Ŭ������ ����� �����ϴ�.
					//SmaplePoint �ǽ�
					//pointColor = hit.obj->amb * hit.obj->ambTexture->SamplePoint(hit.uv);
					phongColor += hit.obj->amb * hit.obj->ambTexture->SampleLinear(hit.uv);
				}
				else
				{
					phongColor += Color(hit.obj->amb);
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

				//���� ������ �� color�� phongColor�� reflection�� ���� ������ ��� ����)
				//1.0f(���� ������ ����) - �ݻ緮(����) - ������(����)
				//���⼭�� �������� 0���� ����(��ü�� ������ ��� �������� ����)
				color += phongColor * (1.0f - hit.obj->reflection - hit.obj->transparency);
				
				if (hit.obj->reflection)
				{
					// ���⿡ �ݻ� ����
					// ��ġ ���� ����
					// �ݻ籤�� ��ȯ���� ���� ���� ���� ������ hit.obj->reflection

					//const vec3 reflectedDirection = ...
					//color += ...

					//const vec3 m = ray.dir - glm::dot(hit.normal, ray.dir) * hit.normal;
					//-d + 2m
					//2m = 2.0f * ray.dir - dot(hit.normal, ray.dir) * hit.normal * 2.0f;
					//2m - d = ray.dir - dot(hit.normal, ray.dir) * hit.normal * 2.0f;
					Vector3 reflectedDirection;
					(-ray.dir.Dot(hit.normal) * hit.normal * 2.0f + ray.dir).Normalize(reflectedDirection);
					Ray reflectionRay = { hit.point + reflectedDirection * 1e-4f, reflectedDirection };
					color += hit.obj->reflection * TraceRay(reflectionRay, recurseLevel - 1);
				}

				// ����
				// https://samdriver.xyz/article/refraction-sphere (�׸����� ���ƿ�)
				// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel (��������)
				// https://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/reflection_refraction.pdf (�����̵尡 ���� ������ ������ ��Ȯ�ؿ�)
				if (hit.obj->transparency)
				{
					// ������ ��ü�� ���� ó��

					const float ior = 1.5f; // Index of refraction (����: 1.5, ��: 1.3)
					
					float eta; // sinTheta1 / sinTheta2
					Vector3 normal;

					if (ray.dir.Dot(hit.normal) < 0.0f) // �ۿ��� ������ ���� ��� (��: ����->����)
					{
						eta = ior;
						normal = hit.normal;
					}
					else // �ȿ��� ������ ������ ��� (��: ����->����)
					{
						eta = 1.0f / ior;
						normal = -hit.normal;
					}

					// const float cosTheta1 = ... ;
					// const float sinTheta1 = ... ; // cos^2 + sin^2 = 1
					// const float sinTheta2 = ... ;
					// const float cosTheta2 = ... ;
					const float cosTheta1 = normal.Dot(-ray.dir);
					const float sinTheta1 = sqrtf(1.0f - cosTheta1 * cosTheta1);
					const float sinTheta2 = sinTheta1 / eta;
					const float cosTheta2 = sqrtf(1.0f - sinTheta2 * sinTheta2);

					// const Vector3 m = glm::normalize(...);
					// const Vector3 a = ...;
					// const Vector3 b = ...;
					// const Vector3 refractedDirection = glm::normalize(a + b); // transmission

					Vector3 m;
					(ray.dir - normal.Dot(ray.dir) * normal).Normalize(m);
					const Vector3 a = -normal * cosTheta2;
					const Vector3 b = m * sinTheta2;
					Vector3 refractedDirection;
					(a + b).Normalize(refractedDirection);

					Ray refractedRay{ hit.point + refractedDirection * 1e-4f, refractedDirection };
					// color += ...;
					color += TraceRay(refractedRay, recurseLevel - 1) * hit.obj->transparency;

					// Fresnel ȿ���� �����Ǿ����ϴ�.
				}

				return color;
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
				return TraceRay(myRay, 5);
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
					
					// index���� size_t�� ��� (index�� ������ ���� �����ϱ�)
					// traceRay()�� ��ȯ���� vec3 (RGB), A�� ���ʿ�
                    pixels[size_t(x + width * y)] = TraceRay(pixelRay, 5);

					//SuperSampling ����
					//Vector3 pixelColor;
					//TraceRay2x2(eyePos, pixelWorldPos, dx, 3).ToVector3().Clamp(Vector3(0.0f), Vector3(1.0f), pixelColor);
					//pixels[size_t(x + width * y)] = Color(pixelColor);
                }
#pragma warning (default : 6993)
        }
    };
}