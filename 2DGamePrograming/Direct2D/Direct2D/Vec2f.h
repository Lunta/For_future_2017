//-----------------------------------------------------------------------------
// File: Vec2d.h
//-----------------------------------------------------------------------------

#pragma once

#include <Windows.h>

template<typename Ty>
class Vector2D {
private:
	const float EffectiveEffer = 0.0001f;

public:
	Ty x;
	Ty y;
	Vector2D() : x(0), y(0) {}
	template<typename OTy>
	Vector2D(OTy X, OTy Y) : x(static_cast<Ty>(X)), y(static_cast<Ty>(Y)) {}
	Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
	Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
	Vector2D& operator*=(const float f) { x *= f; y *= f; return *this; }
	Vector2D& operator*=(const int i) { x *= i; y *= i; return *this; }
	Vector2D& operator=(const POINT& other) { x = other.x; y = other.y; return *this; }
	Vector2D& operator=(const Vector2D& other) { x = other.x; y = other.y; return *this; }

	operator POINT() const { return POINT{ static_cast<LONG>(x) ,static_cast<LONG>(y) }; }
	Vector2D Normalize() { float fLength = Length(); if (fLength != 0) { static_cast<float>(x) /= fLength; static_cast<float>(y) /= fLength; } return *this; }
	constexpr float Length() const { return(sqrt(x * x + y * y)); }

	bool operator==(const Vector2D& other) const { return(fabs(x - other.x) < EffectiveEffer && fabs(y - other.y) < EffectiveEffer); }

};

using Vec2f = Vector2D<float>;
using Vec2i = Vector2D<int>;

template<typename Ty>
inline Vector2D<Ty> operator+(const Vector2D<Ty> a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x + b.x, a.y + b.y); }
template<typename Ty>
inline Vector2D<Ty> operator-(const Vector2D<Ty> a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x - b.x, a.y - b.y); }

template<typename Ty>
inline Vector2D<Ty> operator+(const POINT a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x + b.x, a.y + b.y); }
template<typename Ty>
inline Vector2D<Ty> operator+(const Vector2D<Ty> a, const POINT b) { return Vector2D<Ty>(a.x + b.x, a.y + b.y); }
template<typename Ty>
inline Vector2D<Ty> operator-(const POINT a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x - b.x, a.y - b.y); }
template<typename Ty>
inline Vector2D<Ty> operator-(const Vector2D<Ty> a, const POINT b) { return Vector2D<Ty>(a.x - b.x, a.y - b.y); }

template<typename Ty>
inline float operator*(const Vector2D<Ty> a, const Vector2D<Ty> b) { return (a.x * b.x + a.y * b.y); }
template<typename Ty>
inline Vector2D<Ty> operator*(const float a, const Vector2D<Ty> b) { return Vector2D<Ty>(a * b.x, a * b.y); }
template<typename Ty>
inline Vector2D<Ty> operator*(const Vector2D<Ty> a, const float b) { return Vector2D<Ty>(b * a.x, b * a.y); }
