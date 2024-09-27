#pragma once

#include <iostream>
#include <directxtk/SimpleMath.h>

namespace JYKim
{
    using namespace DirectX;
    using namespace SimpleMath;

	class Object;

    class Hit
    {
    public:
        float distance;						// 광선의 시작부터 충돌 지점까지의 거리
        Vector3 point;						// 충돌한 위치
        Vector3 normal;						// 충돌한 위치에서 표면의 수직 벡터
		Vector2 uv;							// 텍스쳐 좌표

        std::shared_ptr<Object> obj;		// 나중에 물체의 재질 등을 가져오기 위한 포인터
    };
}
