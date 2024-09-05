#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace JYKim
{
	// Bilinear interpolation reference
	// https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/bilinear-filtering

	class Texture
	{
	public:
		int width, height, channels;
		vector<uint8_t> image;

		Texture(const string& fileName)
		{
			unsigned char* img = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

			image.resize(size_t(width * height * channels));
			memcpy(image.data(), img, image.size() * sizeof(image[0]));

			delete img;
		}
		Texture(const int width, const int height, const vector<Color>& pixels)
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

		//index�� ���� ���� ���� ��ȯ
		Color GetClamped(int i, int j)
		{
			//���� ���� �ִ�ġ�� �ּ�ġ�� �Ѿ�� �Ѿ ���� ������ ���� ���� �ݺ��ȴ�.
			i = clamp(j, 0, width - 1);
			j = clamp(j, 0, height - 1);

			const float r = image[(i + width * j) * channels + 0] / 255.0f;
			const float g = image[(i + width * j) * channels + 1] / 255.0f;
			const float b = image[(i + width * j) * channels + 2] / 255.0f;

			return Color(r, g, b);
		}

		//index�� ���� ���� ���� ��ȯ
		Color GetWrapped(int i, int j)
		{
			//���� ���� �ּҳ� �ִ� ������ �Ѿ�� �ٽ� ù ���� ��(�ؼ�)���� ���ø�
			i %= width; // i�� width�� 0���� �ٲ�
			j %= height;

			if (i < 0)
				i += width; // i�� -1�̸� (width - 1)�� �ٲ�
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

		//DX������ PointSampling, OpenGL������ NearestSampling
		//�ݿø��ؼ� ���ø� ����Ʈ���� ���� ����� �ϳ��� �ȼ��� ���� ���� ���� �����´�
		Color SamplePoint(const Vector2& uv)
		{
			// �ؽ��� ��ǥ�� ���� uv [0.0, 1.0] x [0.0, 1.0]
			// �̹��� ��ǥ�� ���� xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
			// �迭 �ε����� ���� ���� ij [0, width - 1] x [0, height - 1]
			
			// Vector2 xy = ...;
			// int i = ...;
			// int j = ...;
			const Vector2 xy = uv * Vector2(width, height) - Vector2(0.5f);
			const int i = int(round(xy.x));
			const int j = int(round(xy.y));

			//return GetClamped(0, 0);
			return GetClamped(i, j);
		}

		//DX, OpenGL���� LinearSampling
		//���ø� ����Ʈ���� ���� ����� 4���� ���÷� Linear Interpolation�� �ؼ� ���� ���� ���Ѵ�.
		//���ø� ����Ʈ���� ������ ���� ���� ���� �ε��� ��(i, j)�� ���Ѵ�.
		//�ڿ����� (i, j) (i + 1, j) (i, j + 1) (i + 1, j + 1) 4���� ���� �ε����� ���� �� �ְ�,
		//�̸� ���� 4���� ���� ���� ������ �� �ִ�.
		//�� 4���� ������ Linear Interpolation�Ѵ�.
		//Linear Interpolation�� 1���� �󿡼�����.
		//00 -> 10�� �����Ͽ� a���� ���ϰ�, 01 -> 11�� �����Ͽ� b���� ���Ѵ�.
		//a -> b�� �ٽ� �����Ͽ� ���ø� ����Ʈ�� ���� ���� �����س���.
		//dx(00���� ���ø� ����Ʈ���� x��ǥ ����)
		//||00->a|| = dx
		//||a->10|| = 1 - dx
		//||a->x|| = dy
		//2���� �������� 1������ Linear Interpolation�� 2�� �ݺ��ؼ� 2���� Linear Interpolation�� �����ϴ� ����,
		//Bilinear Interpolation �̶�� �Ѵ�.
		Color SampleLinear(const Vector2& uv)
		{
			// �ؽ��� ��ǥ�� ���� uv [0.0, 1.0] x [0.0, 1.0]
			// �̹��� ��ǥ�� ���� xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
			// cout << floor(-0.3f) << " " << int(-0.3f) << '\n'; // -1 0

			// const Vector2 xy = ...;
			// const int i = ...;
			// const int j = ...;
			// const float dx = ...;
			// const float dy = ...;
			const Vector2 xy = uv * Vector2(width, height) - Vector2(0.5f);
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