#pragma once

#include "Hit.h"
#include "Ray.h"
#include "Texture.h"

namespace JYKim
{
	using namespace DirectX;
	using namespace SimpleMath;

	class Object
	{
	public:
		// 퐁 쉐이딩(Phong shading)을 위한 재질(material)
		Vector3 amb = Vector3();	// Ambient
		Vector3 diff = Vector3();	// Diffuse
		Vector3 spec = Vector3();	// Specular
		float alpha = 10.0f;
		//float ks = 1.0f;			// Specular Reflection, 변수를 줄이기 위해 항상 1.0으로 가정
		//float reflection_ = 0.0;
		//float transparency = 0.0;

	public:
		Vector3 GetAmbient() const { return amb; }
		float* GetAmbientFloatAddress() { return &amb.x; }
		Vector3 GetDiffuse() const { return diff; }
		float* GetDiffuseFloatAddress() { return &diff.x; }
		Vector3 GetSpecular() const { return spec; }
		float* GetSpecularFloatAddress() { return &spec.x; }
		//float GetKs() const { return ks; }
		//float* GetKsAddress() { return &ks; }
		float GetAlpha() const { return alpha; }
		float* GetAlphaAddress() { return &alpha; }
		void SetAmbient(const Vector3& amb) { this->amb = amb; }
		void SetDiffuse(const Vector3& diff) { this->diff = diff; }
		void SetSpecular(const Vector3& spec) { this->spec = spec; }
		//void SetKs(const float ks) { this->ks = ks; }
		void SetAlpha(const float alpha) { this->alpha = alpha; }

	public:
		Object(const Color& color = Color(1))
			: amb(color.ToVector3()), diff(color.ToVector3()), spec(color.ToVector3())
		{
		}

		virtual Hit CheckRayCollision(const Ray& ray) const = 0;
	};
}