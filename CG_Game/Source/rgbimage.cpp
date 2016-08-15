//
//  rgbimage.cpp
//  CG_P1
//
//  Created by  on 17.03.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/rgbimage.h"

RGBImage::RGBImage(unsigned int Width, unsigned Height){
    m_Width = Width;
    m_Height = Height;
    m_Image = new Color[Width*Height];
}

RGBImage::~RGBImage(){
    delete[] m_Image;
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c){
    if(x >= m_Width || y >= m_Height){
       throw "x or y out of bounds!";
    }

	m_Image[x + y*m_Width] = c;
}


const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const{
    if(x >= m_Width || y >= m_Height){
        throw "x or y out of bounds!";
    }
    
    return m_Image[x + y*m_Width];
}

bool RGBImage::saveToDisk(const char* Filename){
    int k = m_Width * m_Height;
	FILE *f;

	try {
		f = fopen(Filename, "wb");

		unsigned int filesize = (m_Width * m_Height * 3) + 54;
		unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
		unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0,
											0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
											0,0,0,0, 0,0,0,0};
		
		bmpfileheader[2] = (unsigned char)(filesize);
		bmpfileheader[3] = (unsigned char)(filesize >> 8);
		bmpfileheader[4] = (unsigned char)(filesize >> 16);
		bmpfileheader[5] = (unsigned char)(filesize >> 24);
		
		bmpinfoheader[4] = (unsigned char)(m_Width);
		bmpinfoheader[5] = (unsigned char)(m_Width >> 8);
		bmpinfoheader[6] = (unsigned char)(m_Width >> 16);
		bmpinfoheader[7] = (unsigned char)(m_Width >> 24);

		bmpinfoheader[8] = (unsigned char)(m_Height);
		bmpinfoheader[9] = (unsigned char)(m_Height >> 8);
		bmpinfoheader[10] = (unsigned char)(m_Height >> 16);
		bmpinfoheader[11] = (unsigned char)(m_Height >> 24);
		
		fwrite(&bmpfileheader, 14, 1, f);
		fwrite(&bmpinfoheader, 40, 1, f);
		
		unsigned char rgb[3];
		Color color;
		
        for (int i = 0; i<k; i++) {
            color = m_Image[i];

			//needs to be BGR in file
			rgb[2] = convertColorChannel(color.R);
			rgb[1] = convertColorChannel(color.G);
			rgb[0] = convertColorChannel(color.B);
			
			fwrite(&rgb, 1, 3, f);
        }

		fclose(f);
        return true;
    }
    catch(std::exception &e){
        std::cout << "exception !! " << e.what() << std::endl ;
		return false;
    }
    return false;
}

unsigned int RGBImage::width() const{
    return m_Width;
}
unsigned int RGBImage::height() const{
    return m_Height;
}
