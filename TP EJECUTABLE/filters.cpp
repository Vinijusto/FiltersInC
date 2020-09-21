#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "filters.h"
#include <thread>
#include <atomic>
#include <sys/sysinfo.h>  

#define BLACK 0

using namespace std;

void blackWhite_thread(ppm& img, int starth,int endh)
{
	short int r,g,b,gcalc;
	for(int h = starth; h < endh; h++){
		for(int w = 0; w < img.width; w++){
			r = img.getPixel(h,w).r;
			g = img.getPixel(h,w).g; 
			b = img.getPixel(h,w).b;
			gcalc = (r + g + b) / 3;
			img.setPixel(h,w,pixel(gcalc,gcalc,gcalc));
		}
	}
	return;
}
void blackWhite(ppm& img, short int threads){
	/* CONVERTS IMAGES TO GRAY SCALE */
	if(threads == 1){
		short int r,g,b,gcalc; // gcalc auxiliary var to apply grey averege between channels
		for(int h = 0; h < img.height; h++){
			for(int w = 0; w < img.width; w++){
				r = img.getPixel(h,w).r;
				g = img.getPixel(h,w).g;
				b = img.getPixel(h,w).b;
				gcalc = (r + g + b) / 3;
				img.setPixel(h,w,pixel(gcalc,gcalc,gcalc));
			}
		}
	}
	else{
		vector<thread> threadsvect;
		vector<vector<int>> limites;
		vector<int> aux;
		int hinit = 0;
		for(int d = 0; d < threads; d++)
		{
			aux = {hinit,hinit+img.height/threads};
			limites.push_back(aux);
			hinit += img.height/threads;
		}
		for(int i = 0; i < threads; i++){
			threadsvect.push_back(thread(blackWhite_thread,std::ref(img),limites[i][0],limites[i][1]));
		}
		for(int l = 0; l < threads; l++)
		{
    		threadsvect[l].join();
		}
	}
	return;
}


void contrast(ppm& img, float contrast){
	/* APPLIES CONTRAST TO IMAGE */
	short int r,g,b,ccalc; // ccalc auxiliary var to apply every channel	
	ccalc = (259 * (contrast + 255)) / (255 * (259 - contrast));
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			r = img.getPixel(h,w).r;
			g = img.getPixel(h,w).g; 
			b = img.getPixel(h,w).b;
			r = ccalc * (r - 128) + 128;
			g = ccalc * (g - 128) + 128;
			b = ccalc * (b - 128) + 128;
			img.setPixel(h,w,pixel(r,g,b).truncate());
		}
	}
	return;
}

void merge(ppm& img1, ppm& img2, float alpha){
	/* FUSES TWO IMAGES */
	short int rimg1,gimg1,bimg1,rimg2,gimg2,bimg2;
	float alpha2 = 1 - alpha; // alpha and alpha2 determin the impact porcentage in the final image.
	for(int h = 0; h < img1.height; h++){
		for(int w = 0; w < img1.width; w++){
			rimg1 = img1.getPixel(h,w).r;
			gimg1 = img1.getPixel(h,w).g;
			bimg1 = img1.getPixel(h,w).b;
			rimg2 = img2.getPixel(h,w).r;
			gimg2 = img2.getPixel(h,w).g;
			bimg2 = img2.getPixel(h,w).b;
			img1.setPixel(h,w,pixel(rimg1*alpha+rimg2*alpha2,gimg1*alpha+gimg2*alpha2,bimg1*alpha+bimg2*alpha2));
		}
	}
	return;
}

void boxBlur(ppm &img,short int comienzo,short int final, short int threads){
    /* APPLIES BLUR TO IMAGES */
    vector<int> pos{0,1,2};
    short int r,g,b;
	if(threads == 1){
		for(int h1 = comienzo; h1 < final - 1; h1++){
			for(int w1 = 0; w1 < img.width; w1++){
				r = 0;
				g = 0;
				b = 0;
				for(int h2 = 0; h2 < 3; h2++){
					for(int w2 = 0; w2 < 3; w2++){
						if(h1 < img.height - 2 and w1 < img.width -2){
							r += img.getPixel((h1 + pos[h2]),(w1 + pos[w2])).r * 1;
							g += img.getPixel((h1 + pos[h2]),(w1 + pos[w2])).g * 1;
							b += img.getPixel((h1 + pos[h2]),(w1 + pos[w2])).b * 1;
						} 
					}
				}
				r = r/9;
				g = g/9;
				b = b/9;
				img.setPixel(h1,w1,pixel(r,g,b));
			}
		}
	}
	else{ 
		vector<thread> threadsvect;
		vector<vector<int>> limites;
		vector<int> aux;
		short int hinit = 0;
		for(int d = 0; d < threads; d++)
		{
			aux = {hinit,hinit+img.height/threads};
			limites.push_back(aux);
			hinit += img.height/threads;
		}
		for(int i = 0; i < threads; i++){
			threadsvect.push_back(thread(boxBlur,std::ref(img),limites[i][0],limites[i][1],1));
		}
		for(int l = 0; l < threads; l++)
		{
    		threadsvect[l].join();
		}
	}
	return;
}

void Zoom(ppm &img, ppm &img_zoomed, int n){
	/* ZOOMS IN */
	short int r,g,b;
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			r = img.getPixel(h,w).r;
			g = img.getPixel(h,w).g; 
			b = img.getPixel(h,w).b;
			for(int h2 = 0; h2 < n; h2++){
				for(int w2 = 0; w2 < n; w2++)
					img_zoomed.setPixel(h * n + h2,w * n + w2,pixel(r,g,b));

			}
		}
	}
	return;
}


void frame(ppm& img, pixel color, int x){
	/* MAKES FRAME ARROUND */
	short int r,g,b;
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			if(h < img.height - x and h > x and w < img.width - x and w > x){
				r = img.getPixel(h,w).r;
				g = img.getPixel(h,w).g; 
				b = img.getPixel(h,w).b;
				img.setPixel(h,w,pixel(r,g,b));
			}else
				img.setPixel(h,w,color);
		}
	}
	return;
}



void edgeDetection(ppm& img, ppm& img_target){
	/* DETECTS EDGES AND HIGHLIGHTS THEM */
	vector<vector<int>> multiplic{{1,0,-1},{2,0,-2},{1,0,-1}}; // convolution kernel (matrix)
	vector<vector<int>> multiplicinv{{1,2,1},{0,0,0},{-1,-2,-1}};
	short int r,g,b,rinv,ginv,binv;
	int height = img_target.height - 2;
	int width = img_target.width - 2;
	for(int h1 = 0; h1 < height; h1++){
		for(int w1 = 0; w1 < width; w1++){
			r = 0, g = 0, b = 0, rinv = 0, ginv = 0, binv = 0;
			for(int h2 = 0; h2 < 3; h2++){
				for(int w2 = 0; w2 < 3; w2++){
					r += img.getPixel((h1 + h2),(w1 +w2)).r * multiplic[h2][w2];
					g += img.getPixel((h1 + h2),(w1 +w2)).g * multiplic[h2][w2];
					b += img.getPixel((h1 + h2),(w1 +w2)).b * multiplic[h2][w2];
					rinv += img.getPixel((h1 + h2),(w1 +w2)).r * multiplicinv[h2][w2];
					ginv += img.getPixel((h1 + h2),(w1 +w2)).g * multiplicinv[h2][w2];
					binv += img.getPixel((h1 + h2),(w1 +w2)).b * multiplicinv[h2][w2];
				}
			}
			img_target.setPixel(h1,w1,(pixel(sqrt(pow(r,2) + pow(rinv,2)),sqrt(pow(g,2) + pow(ginv,2)),sqrt(pow(b,2) + pow(binv,2)))).truncate());
		}
	}
	return;
}

void blurpotente(ppm& img){
	/* IN PROGRESS */
	vector<vector<int>> multiplic{{0,1,0},{1,-4,1},{0,1,0}};
	short int r,g,b;
	int height = img.height - 2, width = img.width - 2;
	for(int h1 = 0; h1 < height; h1++){
		for(int w1 = 0; w1 < width; w1++){
			r = 0,g = 0,b = 0;
			for(int h2 = 0; h2 < 3; h2++){ 
				for(int w2 = 0; w2 < 3; w2++){
					r += img.getPixel((h1 + h2),(w1 +w2)).r * multiplic[h2][w2];
					g += img.getPixel((h1 + h2),(w1 +w2)).g * multiplic[h2][w2];
					b += img.getPixel((h1 + h2),(w1 +w2)).b * multiplic[h2][w2];
				}
			}
			img.setPixel(h1,w1,(pixel(r,g,b).truncate()));
		}
	}
	return;
}

void chanel(ppm& img, string chanel){
	/* SWITCHES OFF TWO CHANNELS (R,G,B) */
	short int r,g,b;
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			r = img.getPixel(h,w).r * (chanel == "r");
			g = img.getPixel(h,w).g * (chanel == "g"); 
			b = img.getPixel(h,w).b * (chanel == "b");
			img.setPixel(h,w,pixel(r,g,b));
		}
	}
	return;
}

void pixelart(ppm& img){
	/* PIXELATES THE IMAGE */
	short int r,g,b,divisorias = 0;
	for(int h = 0; h < img.height; h = h + 7){
		for(int w = 0; w < img.width; w = w + 7){
			r = 0,divisorias = 0,g = 0,b = 0;
			for(int h2 = 0; h2 < 7; h2++){ 
				for(int w2 = 0; w2 < 7; w2++){
					if(h + h2 < img.height and w + w2 < img.width){
						divisorias++;
						r += img.getPixel((h + h2),(w + w2)).r;
						g += img.getPixel((h + h2),(w + w2)).g;
						b += img.getPixel((h + h2),(w + w2)).b;
					}
				}
			}
			r = r/divisorias;
			g = g/divisorias;
			b = b/divisorias;
			for(int h3 = 0; h3 < 7; h3++){
				for(int w3 = 0; w3 < 7; w3++){
					if(h + h3 < img.height and w + w3 < img.width)
						img.setPixel(h + h3,w + w3,pixel(r,g,b));
				}
			}
		}
	}
	return;
}

void invers(ppm& img,ppm& imgaux){
	/* INVERTS THE IMAGE */
	short int r,g,b;
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			r = imgaux.getPixel(img.height - h- 1,img.width - w - 1).r;
			g = imgaux.getPixel(img.height - h- 1,img.width - w - 1).g; 
			b = imgaux.getPixel(img.height - h- 1,img.width - w - 1).b;
			img.setPixel(h,w,pixel(r,g,b).truncate());
		}
	}
}

void inverschanel(ppm& img){
	/* INVERTS THE CHANNELS */
	short int r,g,b;
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			r = img.getPixel(h,w).r;
			g = img.getPixel(h,w).g; 
			b = img.getPixel(h,w).b;
			img.setPixel(h,w,pixel(b,g,r).truncate());
		}
	}
	return;
}

void cuadraditos(ppm& img){
	/* INVERTS THE CHANNELS */
	short int r,g,b,aux,aux2;
	vector<vector<int>> multiplic;
	vector<vector<int>> colorporgrupo;
	vector<vector<int>> colors{{250, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 250, 0}, {255, 0, 255}, {0, 255, 255}, {240, 50, 230}, {210, 245, 60}, {250, 190, 212}, {0, 128, 128}, {220, 190, 255}, {170, 110, 40}, {255, 250, 200}, {128, 0, 0}, {170, 255, 195}, {128, 128, 0}, {255, 215, 180}, {0, 0, 128}, {128, 128, 128}, {255, 255, 255}};
	vector<vector<int>> colorsdescartados;
	vector<int> colorelegido;
	srand(time(NULL));
	colorporgrupo.push_back({0,0,0}); 
	multiplic.push_back({5,5,5});
	for(int h = 0; h < img.height; h++){
		for(int w = 0; w < img.width; w++){
			if(colors.size() <= 1){
				for(int n = 0; n < colorsdescartados.size(); n++){
					colors.push_back(colorsdescartados[n]);
				}
				colorsdescartados.clear();
			}
			r = img.getPixel(h,w).r;
			g = img.getPixel(h,w).g; 
			b = img.getPixel(h,w).b;
			aux = 0;
			for(int i = 0; i < multiplic.size(); i++){
				if(r > multiplic[i][0] - 40 and r < multiplic[i][0] + 40 and g > multiplic[i][1] - 40 and g < multiplic[i][1] + 40 and b > multiplic[i][2] - 40 and b < multiplic[i][2] + 40){
					img.setPixel(h,w,pixel(colorporgrupo[i][0],colorporgrupo[i][1],colorporgrupo[i][2]).truncate());
					aux++;
					break;
				}
			}
			if(aux == 0){
				multiplic.push_back({r,g,b});
				aux2 = rand() % colors.size();
				colorelegido = colors[aux2];
				colors.erase(colors.begin() + aux2);
				colorsdescartados.push_back(colorelegido);
				colorporgrupo.push_back(colorelegido);
				img.setPixel(h,w,pixel(colorelegido[0],colorelegido[1],colorelegido[2]).truncate());
			}
		}
	}
	return;
}