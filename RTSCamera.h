#pragma once
#include "raylib.h"

class RTS_Camera {
public:
    // ������� ����� (����������� � ������������ ����������)
    struct MapBounds {
        Vector2 minXZ;
        Vector2 maxXZ;
        bool enabled = false;
    };

    RTS_Camera(int screenWidth, int screenHeight);
    Vector3 cameraTarget;

    void Update();
    Camera3D GetCamera3D() const;

    void SetMapBounds(Vector2 minXZ, Vector2 maxXZ); // �������� �����������
    void DisableMapBounds();                         // ��������� �����������

    // �����������: ��������� ��������, ����������������, � �.�.
    void SetMoveSpeed(float s) { moveSpeed = s; }
    void SetZoomLimits(float min, float max) { minZoom = min; maxZoom = max; }
    void SetEdgeSize(float px) { edgeSize = px; }
    void SetRotationSensitivity(float s) { rotSensitivity = s; }
    void SetInitialTarget(Vector3 tgt) { cameraTarget = tgt; }

private:
    int screenWidth, screenHeight;
    Camera3D camera;
    float cameraDistance;
    float cameraRotation;
    float cameraTilt;
    float cameraZoom;

    // ����������
    float minZoom, maxZoom, moveSpeed, edgeSize, rotSensitivity;
    bool rotating;
    Vector2 prevMouse;

    MapBounds bounds;

    void ClampToBounds();
};
