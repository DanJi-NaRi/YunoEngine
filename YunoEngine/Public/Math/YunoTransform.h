#pragma once
#include <DirectXMath.h>
#include <cmath> // std::fmod
using namespace DirectX;

struct YunoTransform
{
    XMFLOAT3 position{ 0,0,0 };
    XMFLOAT3 rotation{ 0,0,0 };
    XMFLOAT3 scale{ 1,1,1 };

    XMMATRIX ToMatrix() const
    {

        const XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
        const XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
        const XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);
        return S * R * T;
    }
};

struct Float2;
struct Float3;
struct Float4;

struct Float2
{
    float x{}, y{};

    // same-dim compound
    Float2& operator+=(const Float2& o) { x += o.x; y += o.y; return *this; }
    Float2& operator-=(const Float2& o) { x -= o.x; y -= o.y; return *this; }
    Float2& operator*=(const Float2& o) { x *= o.x; y *= o.y; return *this; }
    Float2& operator/=(const Float2& o) { x /= o.x; y /= o.y; return *this; }

    Float2& operator+=(float s) { x += s; y += s; return *this; }
    Float2& operator-=(float s) { x -= s; y -= s; return *this; }
    Float2& operator*=(float s) { x *= s; y *= s; return *this; }
    Float2& operator/=(float s) { x /= s; y /= s; return *this; }

    // modulus
    Float2& operator%=(float m) { x = std::fmod(x, m); y = std::fmod(y, m); return *this; }
    Float2& operator%=(int   m) { x = float(int(x) % m); y = float(int(y) % m); return *this; }
};

struct Float3
{
    float x{}, y{}, z{};

    // same-dim compound
    Float3& operator+=(const Float3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Float3& operator-=(const Float3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Float3& operator*=(const Float3& o) { x *= o.x; y *= o.y; z *= o.z; return *this; }
    Float3& operator/=(const Float3& o) { x /= o.x; y /= o.y; z /= o.z; return *this; }

    // allow smaller-dim compound (promote rhs)
    Float3& operator+=(const Float2& o) { x += o.x; y += o.y; /*z += 0*/ return *this; }
    Float3& operator-=(const Float2& o) { x -= o.x; y -= o.y; return *this; }
    Float3& operator*=(const Float2& o) { x *= o.x; y *= o.y; return *this; }
    Float3& operator/=(const Float2& o) { x /= o.x; y /= o.y; return *this; }

    Float3& operator+=(float s) { x += s; y += s; z += s; return *this; }
    Float3& operator-=(float s) { x -= s; y -= s; z -= s; return *this; }
    Float3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Float3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    // modulus
    Float3& operator%=(float m) { x = std::fmod(x, m); y = std::fmod(y, m); z = std::fmod(z, m); return *this; }
    Float3& operator%=(int   m) { x = float(int(x) % m); y = float(int(y) % m); z = float(int(z) % m); return *this; }
};

struct Float4
{
    float x{}, y{}, z{}, w{};

    // same-dim compound
    Float4& operator+=(const Float4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
    Float4& operator-=(const Float4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
    Float4& operator*=(const Float4& o) { x *= o.x; y *= o.y; z *= o.z; w *= o.w; return *this; }
    Float4& operator/=(const Float4& o) { x /= o.x; y /= o.y; z /= o.z; w /= o.w; return *this; }

    // allow smaller-dim compound (promote rhs)
    Float4& operator+=(const Float3& o) { x += o.x; y += o.y; z += o.z; /*w += 0*/ return *this; }
    Float4& operator-=(const Float3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Float4& operator*=(const Float3& o) { x *= o.x; y *= o.y; z *= o.z; return *this; }
    Float4& operator/=(const Float3& o) { x /= o.x; y /= o.y; z /= o.z; return *this; }

    Float4& operator+=(const Float2& o) { x += o.x; y += o.y; return *this; } // z,w += 0
    Float4& operator-=(const Float2& o) { x -= o.x; y -= o.y; return *this; }
    Float4& operator*=(const Float2& o) { x *= o.x; y *= o.y; return *this; }
    Float4& operator/=(const Float2& o) { x /= o.x; y /= o.y; return *this; }

    Float4& operator+=(float s) { x += s; y += s; z += s; w += s; return *this; }
    Float4& operator-=(float s) { x -= s; y -= s; z -= s; w -= s; return *this; }
    Float4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
    Float4& operator/=(float s) { x /= s; y /= s; z /= s; w /= s; return *this; }

    // modulus
    Float4& operator%=(float m) { x = std::fmod(x, m); y = std::fmod(y, m); z = std::fmod(z, m); w = std::fmod(w, m); return *this; }
    Float4& operator%=(int   m) { x = float(int(x) % m); y = float(int(y) % m); z = float(int(z) % m); w = float(int(w) % m); return *this; }
};

////////////////////
// Promote helpers
////////////////////
inline Float3 Promote3(const Float2& v, float z = 0.0f) { return { v.x, v.y, z }; }
inline Float4 Promote4(const Float2& v, float z = 0.0f, float w = 0.0f) { return { v.x, v.y, z, w }; }
inline Float4 Promote4(const Float3& v, float w = 0.0f) { return { v.x, v.y, v.z, w }; }

////////////////////
// Same-dimension free ops
////////////////////
inline Float2 operator+(Float2 a, const Float2& b) { return a += b; }
inline Float2 operator-(Float2 a, const Float2& b) { return a -= b; }
inline Float2 operator*(Float2 a, const Float2& b) { return a *= b; }
inline Float2 operator/(Float2 a, const Float2& b) { return a /= b; }

inline Float3 operator+(Float3 a, const Float3& b) { return a += b; }
inline Float3 operator-(Float3 a, const Float3& b) { return a -= b; }
inline Float3 operator*(Float3 a, const Float3& b) { return a *= b; }
inline Float3 operator/(Float3 a, const Float3& b) { return a /= b; }

inline Float4 operator+(Float4 a, const Float4& b) { return a += b; }
inline Float4 operator-(Float4 a, const Float4& b) { return a -= b; }
inline Float4 operator*(Float4 a, const Float4& b) { return a *= b; }
inline Float4 operator/(Float4 a, const Float4& b) { return a /= b; }

////////////////////
// Cross-dimension ops (PROMOTE result)
////////////////////

// 2 <op> 3 -> 3
inline Float3 operator+(const Float2& a, const Float3& b) { return Promote3(a) + b; }
inline Float3 operator+(const Float3& a, const Float2& b) { return a + Promote3(b); }
inline Float3 operator-(const Float2& a, const Float3& b) { return Promote3(a) - b; }
inline Float3 operator-(const Float3& a, const Float2& b) { return a - Promote3(b); }
inline Float3 operator*(const Float2& a, const Float3& b) { return Promote3(a) * b; }
inline Float3 operator*(const Float3& a, const Float2& b) { return a * Promote3(b); }
inline Float3 operator/(const Float2& a, const Float3& b) { return Promote3(a) / b; }
inline Float3 operator/(const Float3& a, const Float2& b) { return a / Promote3(b); }

// 2 <op> 4 -> 4
inline Float4 operator+(const Float2& a, const Float4& b) { return Promote4(a) + b; }
inline Float4 operator+(const Float4& a, const Float2& b) { return a + Promote4(b); }
inline Float4 operator-(const Float2& a, const Float4& b) { return Promote4(a) - b; }
inline Float4 operator-(const Float4& a, const Float2& b) { return a - Promote4(b); }
inline Float4 operator*(const Float2& a, const Float4& b) { return Promote4(a) * b; }
inline Float4 operator*(const Float4& a, const Float2& b) { return a * Promote4(b); }
inline Float4 operator/(const Float2& a, const Float4& b) { return Promote4(a) / b; }
inline Float4 operator/(const Float4& a, const Float2& b) { return a / Promote4(b); }

// 3 <op> 4 -> 4
inline Float4 operator+(const Float3& a, const Float4& b) { return Promote4(a) + b; }
inline Float4 operator+(const Float4& a, const Float3& b) { return a + Promote4(b); }
inline Float4 operator-(const Float3& a, const Float4& b) { return Promote4(a) - b; }
inline Float4 operator-(const Float4& a, const Float3& b) { return a - Promote4(b); }
inline Float4 operator*(const Float3& a, const Float4& b) { return Promote4(a) * b; }
inline Float4 operator*(const Float4& a, const Float3& b) { return a * Promote4(b); }
inline Float4 operator/(const Float3& a, const Float4& b) { return Promote4(a) / b; }
inline Float4 operator/(const Float4& a, const Float3& b) { return a / Promote4(b); }

////////////////////
// Modulus (PROMOTE result)
// - float modulus uses fmod per component
// - int modulus casts each component to int then %
////////////////////
inline Float2 operator%(Float2 a, float m) { a %= m; return a; }
inline Float2 operator%(Float2 a, int   m) { a %= m; return a; }

inline Float3 operator%(Float3 a, float m) { a %= m; return a; }
inline Float3 operator%(Float3 a, int   m) { a %= m; return a; }

inline Float4 operator%(Float4 a, float m) { a %= m; return a; }
inline Float4 operator%(Float4 a, int   m) { a %= m; return a; }

// vector % vector (same-dim) - float fmod
inline Float2 operator%(const Float2& a, const Float2& b) { return { std::fmod(a.x, b.x), std::fmod(a.y, b.y) }; }
inline Float3 operator%(const Float3& a, const Float3& b) { return { std::fmod(a.x, b.x), std::fmod(a.y, b.y), std::fmod(a.z, b.z) }; }
inline Float4 operator%(const Float4& a, const Float4& b) { return { std::fmod(a.x, b.x), std::fmod(a.y, b.y), std::fmod(a.z, b.z), std::fmod(a.w, b.w) }; }

// cross-dim vector % vector -> promoted dim (float fmod)
inline Float3 operator%(const Float2& a, const Float3& b) { return Promote3(a) % b; }
inline Float3 operator%(const Float3& a, const Float2& b) { return a % Promote3(b); }

inline Float4 operator%(const Float2& a, const Float4& b) { return Promote4(a) % b; }
inline Float4 operator%(const Float4& a, const Float2& b) { return a % Promote4(b); }

inline Float4 operator%(const Float3& a, const Float4& b) { return Promote4(a) % b; }
inline Float4 operator%(const Float4& a, const Float3& b) { return a % Promote4(b); }
