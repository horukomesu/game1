#pragma once
#include "raylib.h"
#include "raymath.h"

class AABB {
public:
    Vector3 m_min;
    Vector3 m_max;

    AABB(const Vector3& min, const Vector3& max);

    AABB expand(const Vector3& size) const;
    AABB grow(const Vector3& size) const;

    float clipXCollide(AABB& other, float xa);
    float clipYCollide(AABB& other, float ya);
    float clipZCollide(AABB& other, float za);

    bool intersects(AABB& other) const;
    void move(const Vector3& other);
};
