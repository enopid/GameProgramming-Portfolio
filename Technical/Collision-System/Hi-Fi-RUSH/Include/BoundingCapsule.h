#pragma once
#include "Engine_Defines.h"

struct BoundingCapsule
{
    
    XMFLOAT3    Center;         // Center       of the capsule.
    float       Radius;         // Radius       of the capsule.
    float       HalfHeight;     // HalfHeight   of the capsule.
    XMFLOAT4    Orientation;    // Unit quaternion representing rotation (box -> world).

    // Creators
    BoundingCapsule() noexcept : Center(0, 0, 0), Radius(1.f), HalfHeight(1.f), Orientation(0, 0, 0, 1.f) {}

    BoundingCapsule(const BoundingCapsule&)                 = default;
    BoundingCapsule& operator=(const BoundingCapsule&)      = default;

    BoundingCapsule(BoundingCapsule&&)                      = default;
    BoundingCapsule& operator=(BoundingCapsule&&)           = default;

    constexpr BoundingCapsule(_In_ const XMFLOAT3& center, _In_ float radius, _In_ float halfHeight, _In_ XMFLOAT4 orientation) noexcept
        : Center(center), Radius(radius), HalfHeight(halfHeight), Orientation(orientation){}

    void    XM_CALLCONV     Transform(_Out_ BoundingCapsule& Out, _In_ FXMMATRIX M) const noexcept;

    bool Intersects(_In_ const BoundingCapsule&     cs)     const noexcept;
    bool Intersects(_In_ const BoundingSphere&      sh)     const noexcept;
    bool Intersects(_In_ const BoundingBox&         box)    const noexcept;
    bool Intersects(_In_ const BoundingOrientedBox& box)    const noexcept;

    bool    XM_CALLCONV     Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _Out_ float& Dist) const noexcept;

    RAY     GetRay() const;

    static void CreateFromPoints(_Out_ BoundingCapsule& Out, _In_ size_t Count,
        _In_reads_bytes_(sizeof(XMFLOAT3) + Stride * (Count - 1)) const XMFLOAT3* pPoints, _In_ size_t Stride) noexcept;
};