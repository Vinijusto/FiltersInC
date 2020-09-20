#ifndef PPM_H
#define PPM_H

using namespace std;


struct pixel{

    short int  r;
    short int  g;
    short int  b;

    // Constructor 
    pixel(short int r2=0, short int g2=0, short int b2 = 0) : r(r2), g(g2), b(b2) {}

	pixel& add(int k){ r += k; g += k; b += k; return *this; }

    pixel& sub(int k){ r -= k; g -= k; b -= k; return *this; }

    pixel& mult(float k){ r *= k; g *= k; b *= k; return *this; }

    pixel& addp(pixel p){ r += p.r; g += p.g; b += p.b; return *this; }

    short int cumsum(){ return r + g + b; } 

	pixel& truncate(){ 
	   r = (r > 255) ? 255 : ((r < 0) ? 0: (unsigned char)r); 
	   g = (g > 255) ? 255 : ((g < 0) ? 0: (unsigned char)g); 
	   b = (b > 255) ? 255 : ((b < 0) ? 0: (unsigned char)b);
	   return *this;
	}
};


class ppm {

private:

    void init(int _width, int _height);

    unsigned int nr_lines;
    unsigned int nr_columns;
    vector<vector<pixel>> bitmap;

public:
    
    unsigned int height; //altura
    unsigned int width;
    unsigned int max_col_val;
    unsigned int size;

    ppm();

    //create a PPM object and fill it with data stored in fname 
    ppm(const std::string &fname);

    //create an "epmty" PPM image with a given width and height;the R,G,B arrays are filled with zeros
    ppm(int _width, int _height);

    //read the PPM image from fname
    void read(const std::string &fname);

    //write the PPM image in fname
    void write(const std::string &fname);

    void setPixel(int i, int j, pixel p){ bitmap[i][j] = p; }
  
    pixel getPixel(int i, int j){ return bitmap[i][j]; }

};

#endif