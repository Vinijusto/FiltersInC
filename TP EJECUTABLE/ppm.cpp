#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <vector>
#include "ppm.h"


unsigned char truncate_pixel(float color)
{ 
    return (color > 255) ? 255 : ((color < 0) ? 0: (unsigned char)color); 
}

//init with default values
void ppm::init(int _width, int _height) {
    width = _width;
    height = _height;
    max_col_val = 255;
    size = _width*_height;
}

//create a PPM object

ppm::ppm() {
    init(0,0);
}


ppm::ppm(int _width, int _height){
    init(_width,_height);
    bitmap.resize(height);
    for(unsigned int i = 0; i<height;i++)
        bitmap[i].resize(width);

}

//create a PPM object and fill it with data stored in fname 
ppm::ppm(const  string &fname) {
    init(0,0);
    read(fname);
}


//read the PPM image from fname

void ppm::read(const  string &fname) {
     ifstream inp(fname.c_str(),  ios::in |  ios::binary);
    if (inp.is_open()) {
        string line;
        //Obtain the header
        getline(inp, line);
        if (line != "P6") {
             cout << "Error. Unrecognized file format." <<  endl;
            return;
        }
        //Skip comments
        getline(inp, line);
        while (line[0] == '#') {
             getline(inp, line);
        }
        //Obtain image dimensions
        stringstream dimensions(line);
        try {
            dimensions >> width;
            dimensions >> height;
            nr_lines = height;
            nr_columns = width;
        } catch ( exception &e) {
             cout << "Header file format error. " << e.what() <<  endl;
            return;
        }

        //Obtain max color value
        getline(inp, line);
        stringstream max_val(line);
        try {
            max_val >> max_col_val;
        } catch ( exception &e) {
             cout << "Header file format error. " << e.what() <<  endl;
            return;
        }

        size = width*height;

        //default initialize image matrix
        bitmap.resize(height);
        for(unsigned int i = 0; i<height;i++)
                bitmap[i].resize(width);
        

        //Iterate through the image filling each vector r,g,b
        char aux;
        for (unsigned int i = 0; i < size; ++i) {
            inp.read(&aux, 1);
            bitmap[i/width][i%width].r = (unsigned char)aux;
            inp.read(&aux, 1);
            bitmap[i/width][i%width].g = (unsigned char)aux;
            inp.read(&aux, 1);
            bitmap[i/width][i%width].b = (unsigned char)aux;
        }

    } else {
         cout << "Error. Unable to open " << fname <<  endl;
    }
    inp.close();
}

//write the PPM image in fname

void ppm::write(const  string &fname) {
     ofstream inp(fname.c_str(),  ios::out |  ios::binary);
    if (inp.is_open()) {

        inp << "P6\n";
        inp << width;
        inp << " ";
        inp << height << "\n";
        inp << max_col_val << "\n";

        char aux;

        for (unsigned int i = 0; i < height; ++i) {
            for(unsigned int j = 0; j < width;j++){
                aux = (char) bitmap[i][j].r;
                inp.write(&aux, 1);
                aux = (char)  bitmap[i][j].g;
                inp.write(&aux, 1);
                aux = (char)  bitmap[i][j].b;
                inp.write(&aux, 1);
            }
        }
    }
     else {
         cout << "Error. Unable to open " << fname <<  endl;
    }
    inp.close();
}
