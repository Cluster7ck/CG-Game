//
//  color.h
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef color_h
#define color_h

#include <stdio.h>

class Color {
public:
    float R;
    float G;
    float B;
    
    Color();
    Color(float r, float g, float b);
    Color operator*(const Color& c) const;
    Color operator*(const float Factor) const;
    Color operator+(const Color& c) const;
    Color& operator+=(const Color& c);
	void convert(float* arr) const;
};

#endif /* color_h */
