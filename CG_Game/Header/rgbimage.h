//
//  rgbimage.hpp
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef rgbimage_hpp
#define rgbimage_hpp

#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "color.h"

class RGBImage{
public:
    RGBImage(unsigned int Width, unsigned Height);
    ~RGBImage();
    void setPixelColor(unsigned int x, unsigned int y, const Color& c);
    const Color& getPixelColor(unsigned int x, unsigned int y) const;
    bool saveToDisk(const char* Filename);
    unsigned int width() const;
    unsigned int height() const;
    
    static unsigned char convertColorChannel(float f){
		unsigned char b;
		unsigned int i;
        f = std::max<float>(0.0, std::min<float>(1.0, f));
        i = (unsigned int)floor(f * 255);
		b = (unsigned char)i;
		
		b = (unsigned char)(f * 255);
        return b;
    }
    
protected:
    Color* m_Image;
    unsigned int m_Height;
    unsigned int m_Width;
};

#endif /* rgbimage_hpp */
