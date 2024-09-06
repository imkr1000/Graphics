#pragma once

#include "stb_image.h"

namespace JYKim
{
	// Bilinear interpolation reference
	// https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/bilinear-filtering

	class Texture
	{
	public:
		int width = 0, height = 0, channels = 0;
		std::vector<uint8_t> image;

		Texture(const std::string& fileName)
		{
			unsigned char* img = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

			image.resize(size_t(width * height * channels));
			memcpy(image.data(), img, image.size() * sizeof(image[0]));

			delete img;
		}

		Texture(const int width, const int height, const std::vector<Color>& pixels)
			: width(width), height(height), channels(3)
		{
			image.resize(size_t(width * height * channels));

			for (int j = 0; j < height; ++j)
				for (int i = 0; i < width; ++i)
				{
					const auto& color = pixels[size_t(i + j * width)];

					image[(i + width * j) * channels + 0] = uint8_t(color.x * 255);
					image[(i + width * j) * channels + 1] = uint8_t(color.y * 255);
					image[(i + width * j) * channels + 2] = uint8_t(color.z * 255);
				}
		}

		//index를 통해 색상 값을 반환
		Color GetClamped(int i, int j)
		{
			//가로 세로 최대치나 최소치를 넘어가면 넘어간 쪽의 마지막 색상 값이 반복된다.
			i = std::clamp(j, 0, width - 1);
			j = std::clamp(j, 0, height - 1);

			const float r = image[(i + width * j) * channels + 0] / 255.0f;
			const float g = image[(i + width * j) * channels + 1] / 255.0f;
			const float b = image[(i + width * j) * channels + 2] / 255.0f;

			return Color(r, g, b);
		}

		//index를 통해 색상 값을 반환
		Color GetWrapped(int i, int j)
		{
			//가로 세로 최소나 최대 범위를 넘어가면 다시 첫 색상 값(텍셀)부터 샘플링
			i %= width; // i가 width면 0으로 바뀜
			j %= height;

			if (i < 0)
				i += width; // i가 -1이면 (width - 1)로 바뀜
			if (j < 0)
				j += height;

			const float r = image[(i + width * j) * channels + 0] / 255.0f;
			const float g = image[(i + width * j) * channels + 1] / 255.0f;
			const float b = image[(i + width * j) * channels + 2] / 255.0f;

			return Color(r, g, b);
		}

		Color InterpolateBilinear(const float dx, const float dy,
			const Color& c00, const Color& c10, const Color& c01, const Color& c11)
		{
			// ...
			const Color a = c00 * (1.0f - dx) + c10 * dx;
			const Color b = c01 * (1.0f - dx) + c11 * dx;

			return a * (1.0f - dy) + b * dy;
		}

		//DX에서는 PointSampling, OpenGL에서는 NearestSampling
		//반올림해서 샘플링 포인트에서 가장 가까운 하나의 픽셀에 대한 색상 값을 가져온다
		Color SamplePoint(const Vector2& uv)
		{
			// 텍스쳐 좌표의 범위 uv [0.0, 1.0] x [0.0, 1.0]
			// 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
			// 배열 인덱스의 정수 범위 ij [0, width - 1] x [0, height - 1]
			
			// Vector2 xy = ...;
			// int i = ...;
			// int j = ...;
			const Vector2 xy = uv * Vector2(float(width), float(height)) - Vector2(0.5f);
			const int i = int(round(xy.x));
			const int j = int(round(xy.y));

			//return GetClamped(0, 0);
			return GetClamped(i, j);
		}

		//DX, OpenGL에서 LinearSampling
		//샘플링 포인트에서 가장 가까운 4개의 샘플로 Linear Interpolation을 해서 색상 값을 구한다.
		//샘플링 포인트에서 버림을 통해 가장 낮은 인덱스 값(i, j)을 구한다.
		//자연스레 (i, j) (i + 1, j) (i, j + 1) (i + 1, j + 1) 4가지 샘플 인덱스를 구할 수 있고,
		//이를 통해 4개의 색상 값을 가져올 수 있다.
		//이 4가지 색상을 Linear Interpolation한다.
		//Linear Interpolation은 1차원 상에서였다.
		//00 -> 10을 보간하여 a값을 구하고, 01 -> 11을 보간하여 b값을 구한다.
		//a -> b를 다시 보간하여 샘플링 포인트의 보간 값을 도출해낸다.
		//dx(00부터 샘플링 포인트까지 x좌표 차이)
		//||00->a|| = dx
		//||a->10|| = 1 - dx
		//||a->x|| = dy
		//2차원 공간에서 1차원의 Linear Interpolation을 2번 반복해서 2차원 Linear Interpolation을 구현하는 것을,
		//Bilinear Interpolation 이라고 한다.
		Color SampleLinear(const Vector2& uv)
		{
			// 텍스쳐 좌표의 범위 uv [0.0, 1.0] x [0.0, 1.0]
			// 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
			// cout << floor(-0.3f) << " " << int(-0.3f) << '\n'; // -1 0

			// const Vector2 xy = ...;
			// const int i = ...;
			// const int j = ...;
			// const float dx = ...;
			// const float dy = ...;
			const Vector2 xy = uv * Vector2(float(width), float(height)) - Vector2(0.5f);
			const int i = int(floor(xy.x));
			const int j = int(floor(xy.y));
			const float dx = xy.x - i;
			const float dy = xy.y - j;

			//return Color(1.0f);
			//return InterpolateBilinear(dx, dy, GetClamped(i, j), GetClamped(i + 1, j), GetClamped(i, j + 1), GetClamped(i + 1, j + 1));
			return InterpolateBilinear(dx, dy, GetWrapped(i, j), GetWrapped(i + 1, j), GetWrapped(i, j + 1), GetWrapped(i + 1, j + 1));
		}
	};
}