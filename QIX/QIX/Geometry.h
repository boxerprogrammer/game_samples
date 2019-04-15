#pragma once


struct Vector2 {
	int x, y;
	Vector2(int ix, int iy) : x(ix), y(iy) {}
	void operator+=(float scale) {
		x *= scale;
		y *= scale;
	}
};
bool operator==(const Vector2& lval , const Vector2& rval) {
	return lval.x == rval.x && lval.y == rval.y;
}
typedef Vector2 Position2;

Vector2 operator+(const Vector2& lval, const Vector2& rval) {
	return Vector2(lval.x + rval.x, lval.y + rval.y);
}

Vector2 operator-(const Vector2& lval, const Vector2& rval) {
	return Vector2(lval.x - rval.x, lval.y - rval.y);
}

Vector2 operator*(const Vector2& v,float scale) {
	return Vector2(v.x*scale, v.y*scale);
}
