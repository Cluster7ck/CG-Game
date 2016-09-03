//
//  vector.cpp
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/vector.h"

float sinus(int degree);
float cosinus(int degree);

Vector::Vector(float x, float y, float z) {
    X = x;
    Y = y;
    Z = z;
} 

Vector::Vector() {}

float Vector::dot(const Vector& v) const {
    return this->X * v.X + this->Y * v.Y + this->Z * v.Z;
}

Vector Vector::cross(const Vector& v) const {
	Vector nv;

    nv.X = this->Y*v.Z - this->Z*v.Y;
    nv.Y = this->Z*v.X - this->X*v.Z;
    nv.Z = this->X*v.Y - this->Y*v.X;

    return nv;

}

bool Vector::operator==(const Vector& v) const {
	if (this->X == v.X && this->Y == v.Y && this->Z == v.Z)
		return true;
	return false;
}

bool Vector::operator!=(const Vector& v) const {
	if (this->X == v.X && this->Y == v.Y && this->Z == v.Z)
		return false;
	return true;
}

Vector Vector::operator+(const Vector& v) const {
    Vector nv;

    nv.X = this->X + v.X;
    nv.Y = this->Y + v.Y;
    nv.Z = this->Z + v.Z;

    return nv;
}

Vector Vector::operator-(const Vector& v) const {
    Vector nv;

    nv.X = this->X - v.X;
    nv.Y = this->Y - v.Y;
    nv.Z = this->Z - v.Z;

    return nv;
}

Vector& Vector::operator+=(const Vector& v) {
    this->X += v.X;
    this->Y += v.Y;
    this->Z += v.Z;

    return *this;
}

Vector Vector::operator*(float c) const {
    Vector nv;

    nv.X = this->X * c;
    nv.Y = this->Y * c;
    nv.Z = this->Z * c;

    return nv;
}

Vector Vector::operator-() const {
    Vector nv;

    nv.X = -this->X;
    nv.Y = -this->Y;
    nv.Z = -this->Z;

    return nv;
}

Vector& Vector::normalize() {
    if(this->length() == 0) {
        //throw;
    }
	//Vector normal;
	(*this) = (*this) * (1/this->length());

    return *this;
}

float Vector::length() {
    return sqrt(pow(this->X,2)+pow(this->Y,2)+pow(this->Z,2));
}

float Vector::lengthSquared() {
    return pow(this->X,2)+pow(this->Y,2)+pow(this->Z,2);
}

Vector Vector::reflection(const Vector& normal) const {
    return *this - normal*2*(this->dot(normal));
}

bool Vector::triangleIntersection(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const {
	float epsilon = 0.000001f;
	//this == o
	//Triangle normal
	Vector n = (b - a).cross(c - a).normalize();

	s = ( a.dot(n) - (*this).dot(n) ) / d.dot(n);

	//Negativ abfangen, da Strahl vom Dreieck weg zeigt
    if(s < 0 ){
        return false;
    }

	//Intersection with plane
	Vector p = (*this)+d*s;

	//Main triangle
    float abcTri = (b - a).cross(c - a).length() / 2;


	//Partial triangles
    float abpTri = (b - a).cross(p - a).length() / 2;
    float acpTri = (c - a).cross(p - a).length() / 2;
    float bcpTri = (c - b).cross(p - b).length() / 2;

    if( abcTri + epsilon >= (abpTri + acpTri + bcpTri) ) {
        return true;
    }
    
	return false;
}

Vector Vector::rotationX(int degree) const {
	Vector nv;

	nv.X = this->X;
	nv.Y = (cosinus(degree) * this->Y) + (-sinus(degree) * this->Z);
	nv.Z = (sinus(degree) * this->Y) + (cosinus(degree) * this->Z);

	return nv;
}

Vector Vector::rotationY(int degree) const {
	Vector nv;

	nv.X = (cosinus(degree) * this->X) + (-sinus(degree) * this->Z);
	nv.Y = this->Y;
	nv.Z = (sinus(degree) * this->X) + (cosinus(degree) * this->Z);

	return nv;
}

Vector Vector::rotationZ(int degree) const {
	Vector nv;

	nv.X = (cosinus(degree) * this->X) + (-sinus(degree) * this->Y);
	nv.Y = (sinus(degree) * this->X) + (cosinus(degree) * this->Y);
	nv.Z = this->Z;

	return nv;
}

Vector Vector::rotateAxis(Vector Axis, int degree) const {
	/*Vector nv;
	Vector v = (*this);
	Vector prllComp = Axis * v.dot(Axis);
	Vector perpComp = v - prllComp;
	Vector perpendicularCompRot = perpComp * cosinus(degree) + Axis.cross(perpComp) * cosinus(degree);
	Vector vRot = prllComp + perpendicularCompRot;

	return vRot;
	*/
	Vector v = (*this);
	float c = cosinus(degree);
	float s = sinus(degree);
	float C = 1.0 - c;

	float Q[3][3];
	Q[0][0] = Axis.X * Axis.X * C + c;
	Q[0][1] = Axis.Y * Axis.X * C + Axis.Z * s;
	Q[0][2] = Axis.Z * Axis.X * C - Axis.Y * s;

	Q[1][0] = Axis.Y * Axis.X * C - Axis.Z * s;
	Q[1][1] = Axis.Y * Axis.Y * C + c;
	Q[1][2] = Axis.Z * Axis.Y * C + Axis.X * s;

	Q[2][0] = Axis.X * Axis.Z * C + Axis.Y * s;
	Q[2][1] = Axis.Z * Axis.Y * C - Axis.X * s;
	Q[2][2] = Axis.Z * Axis.Z * C + c;

	v.X = v.X * Q[0][0] + v.X * Q[0][1] + v.X * Q[0][2];
	v.Y = v.Y * Q[1][0] + v.Y * Q[1][1] + v.Y * Q[1][2];
	v.Z = v.Z * Q[2][0] + v.Z * Q[2][1] + v.Z * Q[2][2];
	return v;
}



float cosinus(int degree) {
	float radian = degree * M_PI / 180.0f;
	float cosinus = cos(radian);

	if (degree == 90 || degree == -90) {
		cosinus = 0.0f;
	}

	return cosinus;
}

float sinus(int degree) {
	float radian = degree * M_PI / 180.0f;
	float sinus = sin(radian);

	if (degree == 180 || degree == -180) {
		sinus = 0.0f;
	}

	return sinus;
}