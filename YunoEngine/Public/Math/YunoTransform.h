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

struct Int2;
struct Int3;
struct Int4;

struct Float2
{
    float x{}, y{};

    constexpr Float2() = default;
    constexpr Float2(float _x, float _y) : x(_x), y(_y) {}

    // XMFLOAT2 -> Float2
    explicit Float2(const XMFLOAT2& v) : x(v.x), y(v.y) {}

    // ===== 추가: cross-dim ctor / assign =====
    // Float3/Float4 -> Float2 (z,w는 버림)
    Float2(const Float3& v);
    Float2(const Float4& v);
    Float2& operator=(const Float3& v);
    Float2& operator=(const Float4& v);

    // Float2 -> XMFLOAT2 (명시 함수 권장)
    XMFLOAT2 ToXM() const { return XMFLOAT2{ x, y }; }

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

    constexpr Float3() = default;
    constexpr Float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    explicit Float3(const XMFLOAT3& v) : x(v.x), y(v.y), z(v.z) {}

    // ===== 추가: cross-dim ctor / assign =====
    // Float2 -> Float3 (z=0), Float4 -> Float3 (w는 버림)
    Float3(const Float2& v, float _z = 0.0f) : x(v.x), y(v.y), z(_z) {}
    Float3(const Float4& v);
    Float3& operator=(const Float2& v) { x = v.x; y = v.y; z = 0.0f; return *this; }
    Float3& operator=(const Float4& v);

    XMFLOAT3 ToXM() const { return XMFLOAT3{ x, y, z }; }

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

    constexpr Float4() = default;
    constexpr Float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    explicit Float4(const XMFLOAT4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    // ===== 추가: cross-dim ctor / assign =====
    // Float2 -> Float4 (z=0,w=0), Float3 -> Float4 (w=0)
    Float4(const Float2& v, float _z = 0.0f, float _w = 0.0f) : x(v.x), y(v.y), z(_z), w(_w) {}
    Float4(const Float3& v, float _w = 0.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Float4& operator=(const Float2& v) { x = v.x; y = v.y; z = 0.0f; w = 0.0f; return *this; }
    Float4& operator=(const Float3& v) { x = v.x; y = v.y; z = v.z; w = 0.0f; return *this; }

    XMFLOAT4 ToXM() const { return XMFLOAT4{ x, y, z, w }; }

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


// promote : 차원 승격 함수. Float2에 Float3을 대입하면, Float2는 Float3가 된다.

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

////////////////////
// Cross-dimension ctor/assign definitions
// (Float3/Float4가 완전히 정의된 뒤에 구현)
////////////////////

// Float2 <- Float3/Float4
inline Float2::Float2(const Float3& v) : x(v.x), y(v.y) {}
inline Float2::Float2(const Float4& v) : x(v.x), y(v.y) {}
inline Float2& Float2::operator=(const Float3& v) { x = v.x; y = v.y; return *this; }
inline Float2& Float2::operator=(const Float4& v) { x = v.x; y = v.y; return *this; }

// Float3 <- Float4
inline Float3::Float3(const Float4& v) : x(v.x), y(v.y), z(v.z) {}
inline Float3& Float3::operator=(const Float4& v) { x = v.x; y = v.y; z = v.z; return *this; }


struct Int2 
{
    constexpr Int2() = default;
    constexpr Int2(int _x, int _y) : x(_x), y(_y) {}
    int x = 0, y = 0;
};

struct Int3
{
    constexpr Int3() = default;
    constexpr Int3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
    int x = 0, y = 0, z = 0;
};

struct Int4
{
    int x, y, z, w;
};
