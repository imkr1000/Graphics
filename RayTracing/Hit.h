#pragma once

#include <iostream>
#include <directxtk/SimpleMath.h>

namespace JYKim
{
    using namespace DirectX;
    using namespace SimpleMath;

    class Hit
    {
    public:
        float distance;                         // ������ ���ۺ��� �浹 ���������� �Ÿ�
        Vector3 point;     // �浹�� ��ġ
        Vector3 normal;    // �浹�� ��ġ���� ǥ���� ���� ����

        //std::shared_ptr<Object> obj;          // ���߿� ��ü�� ���� ���� �������� ���� ������
    };
}
