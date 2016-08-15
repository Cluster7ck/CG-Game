//
//  color.cpp
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/color.h"

Color::Color() {
    R = 0;
    G = 0;
    B = 0;
}

Color::Color(float r, float g, float b) {
    R = r;
    G = g;
    B = b;
}

Color Color::operator*(const Color& c) const {
    Color nc;
    
    nc.R = c.R*this->R;
    nc.G = c.G*this->G;
    nc.B = c.B*this->B;
    
    return nc;
}

Color Color::operator*(const float Factor) const {
    Color nc;
    
    nc.R = this->R*Factor;
    nc.G = this->G*Factor;
    nc.B = this->B*Factor;
    
    return nc;
}

Color Color::operator+(const Color& c) const {
    Color nc;
    
    nc.R = (this->R+c.R);
    nc.G = (this->G+c.G);
    nc.B = (this->B+c.B);
    
    return nc;
} 

Color& Color::operator+=(const Color& c) {
    this->R += c.R;
    this->G += c.G;
    this->B += c.B;
    
    return *this;
}

void Color::convert(float* arr) const{
	arr[0] = this->R;
	arr[1] = this->G;
	arr[2] = this->B;
	arr[3] = 1;
}
