#pragma once

namespace JYKim
{
	class Light
	{
	public:
		Vector3 pos; // 아주 단순화된 형태의 위치만 있는 점 조명

	public:
		void Test() { pos = Vector3(1.0f, 1.0f, 1.0f); }
	};
}