//
//  vector.cpp
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/vector.h"

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
