#include "Vec2.h" 
#include <math.h>


Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{

}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x+rhs.x, y+rhs.y);
}

Vec2 Vec2::operator - (const Vec2 & rhs) const
{
	return Vec2(x-rhs.x, y-rhs.y);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x/val, y/val);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x*val, y*val);
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	float dx = x - rhs.x;
	float dy = y - rhs.y;
	return sqrt(dx * dx + dy * dy);
}

float Vec2::length() const {
	return sqrt(x * x + y * y); // Compute magnitude using Pythagoras' theorem
}

void Vec2::normalize() {
	float len = length();
	if (len != 0) { // Avoid division by zero
		x /= len;
		y /= len;
	}
}

Vec2 Vec2::normalized() const {
	float len = length();
	if (len != 0) {
		return *this / len; // Use operator/ to return a new normalized Vec2
	}
	else {
		return Vec2(0, 0); // Return zero vector if length is zero
	}
}



