#include "BoundingCapsule.h"

void XM_CALLCONV BoundingCapsule::Transform(BoundingCapsule& Out, FXMMATRIX M) const noexcept
{
    auto _result = BoundingCapsule(*this);
    
    XMVECTOR _vScale, _vRotation, _vTranslation;
    XMMatrixDecompose(&_vScale, &_vRotation, &_vTranslation, M);
    
    /*XMStoreFloat3(
        &_result.Center,
        XMVector3TransformCoord(XMLoadFloat3(&_result.Center), XMMatrixRotationQuaternion(_vRotation)) + _vTranslation
    );*/
    XMStoreFloat3(
        &_result.Center,
        XMVector3TransformCoord(
            XMLoadFloat3(&Center),
            M
        )
    );

    XMStoreFloat4(&_result.Orientation,
        XMQuaternionMultiply(XMLoadFloat4(&_result.Orientation), _vRotation)
    );

    _result.HalfHeight  *= XMVectorGetY(_vScale);
    _result.Radius      *= max(XMVectorGetX(_vScale), XMVectorGetZ(_vScale));

    Out = _result;
}

bool BoundingCapsule::Intersects(_In_ const BoundingCapsule& cs) const noexcept
{
    //직선사이의 최근접점
    auto _srcRay = GetRay();
    auto _dstRay = cs.GetRay();

    _vector u = XMLoadFloat4(&_srcRay.vRayDir);                                        //u
    _vector v = XMLoadFloat4(&_dstRay.vRayDir);                                        //v
    _vector w = XMLoadFloat4(&_srcRay.vRayOrigin) - XMLoadFloat4(&_dstRay.vRayOrigin); //w

    auto a = XMVector3Dot(u, u); // A의 길이
    auto b = XMVector3Dot(u, v); // A B 내적
    auto c = XMVector3Dot(v, v); // B의 길이
    auto d = XMVector3Dot(u, w); //시작점 A 내적
    auto e = XMVector3Dot(v, w); //시작점 B 내적

    float a_ = XMVectorGetX(a);
    float b_ = XMVectorGetX(b);
    float c_ = XMVectorGetX(c);
    float d_ = XMVectorGetX(d);
    float e_ = XMVectorGetX(e);
    float D_ = a_ * c_ - b_ * b_;

    float sN, sD = D_;
    float tN, tD = D_;

    if (D_ < 1e-6f) // 거의 평행
    {
        sN = 0.0f;
        sD = 1.0f;
        tN = e_;
        tD = c_;
    }
    else
    {
        sN = (b_ * e_ - c_ * d_);
        tN = (a_ * e_ - b_ * d_);

        if (sN < 0.f)
        {
            sN = 0.f;
            tN = e_;
            tD = c_;
        }
        else if (sN > sD)
        {
            sN = sD;
            tN = e_ + b_;
            tD = c_;
        }
    }

    if (tN < 0.f)
    {
        tN = 0.f;

        if (-d_ < 0.f)
            sN = 0.f;
        else if (-d_ > a_)
            sN = sD;
        else
        {
            sN = -d_;
            sD = a_;
        }
    }
    else if (tN > tD)
    {
        tN = tD;

        if ((-d_ + b_) < 0.f)
            sN = 0;
        else if ((-d_ + b_) > a_)
            sN = sD;
        else
        {
            sN = (-d_ + b_);
            sD = a_;
        }
    }

    float s = (fabs(sN) < 1e-6f ? 0.f : sN / sD);
    float t = (fabs(tN) < 1e-6f ? 0.f : tN / tD);
    _vector dP = w + s * u - t * v;
    float distSq = XMVectorGetX(XMVector3LengthSq(dP));

    return distSq <= (Radius + cs.Radius) * (Radius + cs.Radius);
}

bool BoundingCapsule::Intersects(_In_ const BoundingSphere& sh) const noexcept
{
    auto _vAxisDir = XMVector3TransformNormal(
        XMVectorSet(0.f, 1.f, 0.f, 0.f),
        XMMatrixRotationQuaternion(XMLoadFloat4(&Orientation))
    );
    _vAxisDir = XMVector3Normalize(_vAxisDir);
    _vector _vSrt       = XMLoadFloat3(&Center) - _vAxisDir * HalfHeight;
    _vector _vSHCenter  = XMLoadFloat3(&sh.Center);
    
    _float fRatio = XMVectorGetX(XMVector3Dot(_vAxisDir, _vSHCenter - _vSrt));
    fRatio = clamp(fRatio, 0.f, HalfHeight*2);
    _vector _vProjCenter = _vSrt + fRatio * _vAxisDir;
    if (XMVectorGetX(XMVector3LengthSq(_vSHCenter - _vProjCenter)) <= (Radius + sh.Radius) * (Radius + sh.Radius)) return true;
    return false;
}

bool BoundingCapsule::Intersects(_In_ const BoundingBox& box) const noexcept
{
    auto _srcRay = GetRay();
    
    float t = 0.f;
    auto _vMin = XMVectorSet(
        box.Center.x - box.Extents.x,
        box.Center.y - box.Extents.y,
        box.Center.z - box.Extents.z,
        1.f
    );auto _vMax = XMVectorSet(
        box.Center.x + box.Extents.x,
        box.Center.y + box.Extents.y,
        box.Center.z + box.Extents.z,
        1.f
    );

    //X Axis
    if (_srcRay.vRayOrigin.x < XMVectorGetX(_vMin)      && _srcRay.vRayDir.x > 0.00001f)
        t = max(t, (XMVectorGetX(_vMin) - _srcRay.vRayOrigin.x) / _srcRay.vRayDir.x);
    else if (_srcRay.vRayOrigin.x > XMVectorGetX(_vMax) && _srcRay.vRayDir.x < -0.00001f)
        t = max(t, (XMVectorGetX(_vMax) - _srcRay.vRayOrigin.x) / _srcRay.vRayDir.x);

    if (_srcRay.vRayOrigin.y < XMVectorGetY(_vMin) && _srcRay.vRayDir.y > 0.00001f)
        t = max(t, (XMVectorGetY(_vMin) - _srcRay.vRayOrigin.y) / _srcRay.vRayDir.y);
    else if (_srcRay.vRayOrigin.y > XMVectorGetY(_vMax) && _srcRay.vRayDir.y < -0.00001f)
        t = max(t, (XMVectorGetY(_vMax) - _srcRay.vRayOrigin.y) / _srcRay.vRayDir.y);

    if (_srcRay.vRayOrigin.z < XMVectorGetZ(_vMin) && _srcRay.vRayDir.z > 0.00001f)
        t = max(t, (XMVectorGetZ(_vMin) - _srcRay.vRayOrigin.z) / _srcRay.vRayDir.z);
    else if (_srcRay.vRayOrigin.z > XMVectorGetZ(_vMax) && _srcRay.vRayDir.z < -0.00001f)
        t = max(t, (XMVectorGetZ(_vMax) - _srcRay.vRayOrigin.z) / _srcRay.vRayDir.z);

    t = clamp(t, 0.0f, 1.0f);
    _vector _vPoint     = XMLoadFloat4(&_srcRay.vRayOrigin)  + XMLoadFloat4(&_srcRay.vRayDir) * t;

    _vector _vClosest = XMVectorClamp(_vPoint, _vMin, _vMax);
    if (XMVectorGetX(XMVector3LengthSq(_vPoint - _vClosest)) <= Radius * Radius) return true;

    return false;
}

bool BoundingCapsule::Intersects(_In_ const BoundingOrientedBox& box) const noexcept
{
    auto _srcRay    = GetRay();

    auto _rotM      = XMMatrixInverse(nullptr, XMMatrixRotationQuaternion(XMLoadFloat4(&box.Orientation)));
    
    XMStoreFloat4(
        &_srcRay.vRayDir,
        XMVector3TransformNormal(XMLoadFloat4(&_srcRay.vRayDir), _rotM)
    );
    XMStoreFloat4(
        &_srcRay.vRayOrigin,
        XMVectorSetW(
            XMVector3TransformCoord(XMLoadFloat4(&_srcRay.vRayOrigin) - XMLoadFloat3(&box.Center), _rotM),
            1.f
        )        
    );

    float t = 0.f;
    auto _vMin = XMVectorSet(
        - box.Extents.x,
        - box.Extents.y,
        - box.Extents.z,
        1.f
    ); auto _vMax = XMVectorSet(
        + box.Extents.x,
        + box.Extents.y,
        + box.Extents.z,
        1.f
    );

    //X Axis
    if (_srcRay.vRayOrigin.x < XMVectorGetX(_vMin) && _srcRay.vRayDir.x > 0.00001f)
        t = max(t, (XMVectorGetX(_vMin) - _srcRay.vRayOrigin.x) / _srcRay.vRayDir.x);
    else if (_srcRay.vRayOrigin.x > XMVectorGetX(_vMax) && _srcRay.vRayDir.x < -0.00001f)
        t = max(t, (XMVectorGetX(_vMax) - _srcRay.vRayOrigin.x) / _srcRay.vRayDir.x);

    if (_srcRay.vRayOrigin.y < XMVectorGetY(_vMin) && _srcRay.vRayDir.y > 0.00001f)
        t = max(t, (XMVectorGetY(_vMin) - _srcRay.vRayOrigin.y) / _srcRay.vRayDir.y);
    else if (_srcRay.vRayOrigin.y > XMVectorGetY(_vMax) && _srcRay.vRayDir.y < -0.00001f)
        t = max(t, (XMVectorGetY(_vMax) - _srcRay.vRayOrigin.y) / _srcRay.vRayDir.y);

    if (_srcRay.vRayOrigin.z < XMVectorGetZ(_vMin) && _srcRay.vRayDir.z > 0.00001f)
        t = max(t, (XMVectorGetZ(_vMin) - _srcRay.vRayOrigin.z) / _srcRay.vRayDir.z);
    else if (_srcRay.vRayOrigin.z > XMVectorGetZ(_vMax) && _srcRay.vRayDir.z < -0.00001f)
        t = max(t, (XMVectorGetZ(_vMax) - _srcRay.vRayOrigin.z) / _srcRay.vRayDir.z);

    t = clamp(t, 0.0f, 1.0f);
    _vector _vPoint = XMLoadFloat4(&_srcRay.vRayOrigin) + XMLoadFloat4(&_srcRay.vRayDir) * t;

    _vector _vClosest = XMVectorClamp(_vPoint, _vMin, _vMax);
    if (XMVectorGetX(XMVector3LengthSq(_vPoint - _vClosest)) <= Radius * Radius) 
        return true;
    return false;
}

bool XM_CALLCONV BoundingCapsule::Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _Out_ float& Dist) const noexcept
{
    Dist = 0;

    auto _srcRay = GetRay();
    auto _dstRay = RAY{};
    XMStoreFloat4(&_dstRay.vRayOrigin,  Origin);
    XMStoreFloat4(&_dstRay.vRayDir,     XMVector3Normalize(Direction));

    _vector u = XMLoadFloat4(&_srcRay.vRayDir);                                        //u
    _vector v = XMLoadFloat4(&_dstRay.vRayDir);                                        //v
    _vector w = XMLoadFloat4(&_srcRay.vRayOrigin) - XMLoadFloat4(&_dstRay.vRayOrigin); //w

    auto a = XMVector3Dot(u, u); // A의 길이
    auto b = XMVector3Dot(u, v); // A B 내적
    auto c = XMVector3Dot(v, v); // B의 길이
    auto d = XMVector3Dot(u, w); //시작점 A 내적
    auto e = XMVector3Dot(v, w); //시작점 B 내적

    auto D = a * c - b * b;     //A^2 B^2 sin^2 / 0일시 세타가 0이라 평행함
    float s, t;
    if (abs(XMVectorGetX(D)) < 0.00001f) {
        s = 0;
        t = XMVectorGetX(e / c);
    }
    else {
        s = XMVectorGetX((b * e - c * d) / D);
        t = XMVectorGetX((a * e - b * d) / D);
    }

    s = clamp(s, 0.f, 1.f);

    Dist = t;
    if (t < 0.f) return false;
    if (XMVectorGetX(XMVector3LengthSq(w + s * u - v * t)) <= Radius * Radius) return true;

    return false;
}

RAY BoundingCapsule::GetRay() const
{
    auto _vAxisDir = XMVector3TransformNormal(
        XMVectorSet(0.f, 1.f, 0.f, 0.f),
        XMMatrixRotationQuaternion(XMLoadFloat4(&Orientation))
    );
    _vAxisDir = XMVector3Normalize(_vAxisDir);
    _vector _vSrt = XMLoadFloat3(&Center) - _vAxisDir * HalfHeight;
    RAY _ray;

    XMStoreFloat4(&_ray.vRayDir,    _vAxisDir * HalfHeight * 2.f);
    XMStoreFloat4(&_ray.vRayOrigin, _vSrt);
    _ray.vRayDir.w      = 0.f;
    _ray.vRayOrigin.w   = 1.f;

    return _ray;
}

//줏ㄱ
void BoundingCapsule::CreateFromPoints(_Out_ BoundingCapsule& Out, _In_ size_t Count,
    _In_reads_bytes_(sizeof(XMFLOAT3) + Stride * (Count - 1)) const XMFLOAT3* pPoints, _In_ size_t Stride) noexcept
{
    BoundingCapsule _CapsuleDesc = BoundingCapsule();

    BoundingOrientedBox _OBBDesc;
    BoundingOrientedBox::CreateFromPoints(_OBBDesc, Count, pPoints, Stride);

    _CapsuleDesc.Center         = _OBBDesc.Center;
    _CapsuleDesc.Orientation    = _OBBDesc.Orientation;
    _CapsuleDesc.Radius         = max(_OBBDesc.Extents.x, _OBBDesc.Extents.z);
    _CapsuleDesc.HalfHeight     = max(0.f, _OBBDesc.Extents.y - _CapsuleDesc.Radius);

    Out = _CapsuleDesc;
}
