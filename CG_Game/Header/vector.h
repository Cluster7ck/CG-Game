//
//  vector.h
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef vector_h
#define vector_h

#include <stdio.h>
#include <cmath> 

class Vector {
public:
    float X;
    float Y;
    float Z;
    
    Vector(float x, float y, float z);
    Vector();
    
    float dot(const Vector& v) const;
    Vector cross(const Vector& v) const;
    Vector operator+(const Vector& v) const;
    Vector operator-(const Vector& v) const;
    Vector& operator+=(const Vector& v) ;
    Vector operator*(float c) const;
    Vector operator-() const;
    Vector& normalize();
    float length();
    float lengthSquared();
    Vector reflection(const Vector& normal) const;
    bool triangleIntersection(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const;
};

#endif /* vector_h */
