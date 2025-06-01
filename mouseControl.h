#pragma once
#include "raylib.h"
#include "World.h"
#include "RTSCamera.h"

// ��������� ��� �������� ���������� � ���������� �����
struct MouseBlockSelection {
    bool found = false;
    Vector3 blockPos = { 0, 0, 0 }; // ������� ���������� �����
    Vector3 faceNormal = { 0, 0, 0 }; // ������� �����, �� ������� ������� ������ (��� ���������)
};

// �������� ����� ���������� ����� ��� �������������/����������
class MouseControl {
public:
    MouseControl(World* world, RTS_Camera* camera);

    // �������� ��������� � ������� ������ ����
    void Update();

    // ���������� wireframe ������ ���������� �����
    void DrawSelection() const;

private:
    World* world;
    RTS_Camera* camera;

    MouseBlockSelection currentSelection;

    // Raycast �� ���� � ���������� ���� � ��������� � ����
    MouseBlockSelection RaycastBlock(float maxDistance = 6.0f) const;

    // ��������������� ��������
    void TryDestroyBlock();
    void TryPlaceBlock();
};
#pragma once
