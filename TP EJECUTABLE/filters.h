#ifndef FILTERS_H
#define FILTERS_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include "ppm.h"
#include <atomic>

void blackWhite(ppm& img,short int threads);
void contrast(ppm& img, float contrast);
void merge(ppm& img1, ppm& img2, float alpha);
void boxBlur(ppm &img,short int comienzo,short int final, short int threads);
void frame(ppm& img, pixel color, int x);
void Zoom(ppm &img, ppm &img_zoomed, int n);
void edgeDetection(ppm &img, ppm &img_target);
void chanel(ppm& img, string chanel);
void pixelart(ppm& img);
void invers(ppm& img,ppm& imgaux);
void inverschanel(ppm& img);
void blurpotente(ppm& img);

#endif