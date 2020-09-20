#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include "filters.h"
#include <thread>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>
#include <cstring>
#include <experimental/filesystem>
#include <unistd.h>
#include <sys/stat.h>


#define ONE_OVER_BILLION 1E-9
using namespace std;

// Un esqueleto como base, modificar a gusto pero respetando la consigna
// convert string to back to lower case

string filter_check(vector<string> filter){//verifica que, en caso de haberse pasado la cantidad correcta de parametros(1) existe el filtro escrito por consola
	vector<string> filters{"blackwhite","contrast","merge","boxblur","frame","zoom","edgedetection","chanel","pixeled","nuevo","invers","inverschanel","blurpotente"};
	if(filter.size() == 1){
		string filtroabuscar = filter[0];
		for_each(filtroabuscar.begin(), filtroabuscar.end(), [](char & c){
    	c = tolower(c);
		});
		for(int i = 0;i<filters.size();i++){
			if(filtroabuscar == filters[i]){
				return filtroabuscar;
			} 
		}
	}else{
		return "Error: debe haber un solo parámetro como filtro";
	}
	return "Error: filtro no encontrado";
}	

bool dir_check(string dir){
	struct stat buffer;
	return(stat(dir.c_str(),&buffer) == 0);
}

bool img_check(vector<string> files, vector<string> &fileslaanalizar){
	int counter = 0;
	for(int i =0; i < files.size();i++){
		ifstream file(files[i]);
		if(files[i].size() > 4){
			if(file.good() == true and files[i].substr(files[i].size() - 4,4) == ".ppm")
				counter++;
		}
		if(files[i].size() > 2){
			if(dir_check(files[i].substr(0,files[i].find(".d"))) == true and files[i].substr(files[i].size() - 2,2) == ".d")
				counter++;
		}
	}
	string archivodedir;
	string barrita = "/";
	if(counter == files.size()){
		for(int n =0; n < files.size();n++){
			if(files[n].substr(files[n].size() - 2,2) != ".d")
				fileslaanalizar.push_back(files[n]);
			else{
				struct dirent *entry = nullptr;
			    DIR *dp = nullptr;
			    dp = opendir(files[n].substr(0,files[n].find(".d")).c_str());
			    if (dp != nullptr) {
			        while ((entry = readdir(dp))){
			            archivodedir = entry->d_name;
			            if(archivodedir.size() > 4){
				            if(archivodedir.substr(archivodedir.size() -4,4) == ".ppm")
				            	fileslaanalizar.push_back(files[n].substr(0,files[n].find(".d")) + barrita + archivodedir);
				        }
			        }
			    }
			    closedir(dp);
			}
		}
	}
	return (counter == files.size());
}

bool is_valid_chanle(vector<string> chanelavalidar){
	if(chanelavalidar.size() != 1){
			printf("Error: Solo puede haber un indicador como parámetro (cantidad de parámetros: %i)\n",chanelavalidar.size());
			return false;
		}	
	return (chanelavalidar[0] == "r") + (chanelavalidar[0] == "g") + (chanelavalidar[0] == "b");
}
bool is_valid_int(vector<string> intavalidar, int delimitador,int delimitadormenor){
	if(intavalidar.size() != 1){
		printf("Error: Solo puede haber un indicador como parámetro (cantidad de parámetros: %i)\n",intavalidar.size());
		return false;
	}
	char *p;
	int converted = strtol(intavalidar[0].c_str(),&p,10);
	if(*p){
		printf("Error: el parametro debe ser un número (parametro: '%s') \n", intavalidar[0]);
		return false;
	}
	else{
		if(converted > delimitador or converted < delimitadormenor){
			printf("Error: el parametro debe ser un número menor o igual a %i y mayor o igual a %i \n", delimitador,delimitadormenor);
			return false;
		}
		return true;
	}
}

bool is_valid_float(vector<string> floatavalidar, float delimitador,float delimitadormenor){
	if(floatavalidar.size() != 1){
		printf("Error: Solo puede haber un indicador como parámetro (cantidad de parámetros: %i)\n",floatavalidar.size());
		return false;
	}
	char *p;
	float converted = strtod(floatavalidar[0].c_str(),&p);
	if(*p){
		printf("Error: el parametro debe ser un número (parametro: '%s') \n", floatavalidar[0]);
		return false;
		
	}
	else{
		if(converted > delimitador or converted < delimitadormenor){
			printf("Error: el parametro debe ser un número menor o igual a %f y mayor o igual a %f \n", delimitador,delimitadormenor);
			return false;
		}
		return true;
	}
}
int main(int argc , char* argv[]){
	char tokenclave[3] = "SS";
	vector<vector<string>> grupodeparams;
	int cantgrupparams = 0;
	if(argc > 1){
		vector<string> params;
		for(int i = 1;i < argc;i++){
			if(string(argv[i]) == "SS"){
				if(params.size() != 0){
					grupodeparams.push_back(params);
					cantgrupparams++;
				}
				params = {};
				continue;
			}else{
				params.push_back(argv[i]);
				if(i == argc - 1 and string(argv[i]) != "SS")
					grupodeparams.push_back(params);
			}
		}
	}
	else{
		printf("Ingresar parámetros...\n");
		exit(EXIT_FAILURE);
	}
	if(grupodeparams[0][0] == "--help" and grupodeparams.size() == 1){
		printf("Manual: para poder compilar debe ingresar el comando ./tp, luego el filtro deseado,y en base al que eligió, los parametros correctos. Recuerde que para poder identificar los parametros se usa el token especial SS.\n    - Ejemplo de ejecución correcta(filtro blackWhite):\n     ./tp blackWhite SS imgs/imagen1.ppm undirectorio.d SS 1\n     (El último grupo de parametros indica los threads a ejecutar)\n    - Ejemplo de ejecución incorrecta(filtro blackWhite):\n     ./tp blackWhite SS imgs/imagen1 imagen1deotrodir.ppm ss 2dos\n    (Error 1: la imagen 1 no tiene extensión, Error 2: el separador de parámetros está en minúscula, Error 3: el parámetro que indica los threads contiene letras)");
		printf("\n\nFiltros, ejecución correcta (cuando hay tres puntos es que puede haber de uno a cuantas imagenes o directorios es deseado), recordar que los nombres de los filtros se pueden escribir en mayúscula:\n     -blackwhite: ./tp blackwhite SS <imagen1.ppm/dir.d> <imagen2.ppm> ... SS <threads = int>\n     -contrast: ./tp contrast SS <imagen1.ppm/dir.d>  ... SS <contraste = float perteneciente a [-225.1,225.1]>\n     -merge: ./tp merge SS <imagen1.ppm/dir.d>  ... SS <imagendeconmbinacion.ppm> SS<porcentaje = float perteneciente a [0.1,0.9]>\n     -boxblur: ./tp boxblur SS <imagen1.ppm/dir.d>  ... SS <threads = int>\n     -frame: ./tp frame SS <imagen1.ppm/dir.d>  ... SS <color pixel = perteneciente a [0,225]> SS <frame = perteneciente a [0,700]>\n     -zoom: ./tp zoom SS <imagen1.ppm/dir.d>  ... SS <zoom={2,4,8}> (Considerar que solo pueden ser estas alternativas)\n     -edgedetection: ./tp edgedetection SS <imagen1.ppm/dir.d>  ...\n     -chanel: ./tp chanel SS <imagen1.ppm/dir.d>  ... SS <chanel = 'r', 'g' o 'b'>\n     -pixeled: ./tp pixeled SS <imagen1.ppm/dir.d> ...");
		printf("\n\nWarnings:\n     -Error de filtro Merge: por temas de simplicidad, en el 3er parámetro (véase filtro merge, en el apartado de filtros) del filtro mencionado solo se puede       ingresar una imagen(tampoco se puede ingresar dirs con mas de una imagen) y además todas las imagenes que se combinar\n     -SS múltimples: no hay inconveniente al escribir dos o mas separador de parametros('SS'). Ejemplo: ./tp blackwhite SS SS <imagen1.ppm> <imagen2.ppm> ...        SS SS <threads = int> (No debería ser un error)\n     -Funciones con multithreading: hay error de ejecución al ejecutar varias imagenes(de siete u ocho en adelante) de manera multithreading\n     - Directorios(lotes):indicar la ruta del rirectorio y agregar .d, esto sirve para que el programa entienda que es un dir\n     -Error con los threads: no ingresar mas threads del ancho o alto de la imagen, no suele pasar pero hay imagenes muy pequeñas...\n");
		exit(EXIT_SUCCESS);
	}
	string chaelelgido;
	int zoom; // Var perteneciente a Zoom
	int numthreads = 1; // Var perteneciente a Black&Whit y boxBlur.
	int numeframe; // Var perteneciente a Frame.
	int colorframe; // Var perteneciente a Frame.
	float numerocontrast; // Var perteneciente Contrast.
	float numeroporc; // Var perteneciente Merge.
	vector<string> imagenes; // Se le agregan los nombres de las imágenes (que existan)
	string probablefiltro = filter_check(grupodeparams[0]); // Variable para consultar filtro
	string filtrofinal = "undefined"; // Variable para, en líneas mas adelante, definir si hay filtro y en caso de haberlo cuál
	if(probablefiltro.substr(0,5) == "Error"){} //No hay filtro
	else{//Aca comienza la validación filtro, estructura: Primero se pregunta si existe tal filtro. Segundo si la cantidad de GRUPOS de parámetros son correctos. Tercero, se prgunta a las funciones (check_file, is_valid_int,is_valid_float) si los parámetros son correctos y tienen la cantidad indicada y/o necesario. Alguna de estas funciones, declaran desde si mismo, errores de parametrización y si hay errores se hacen EXIT_FAILURE.
		if(probablefiltro == "blackwhite"){ // tambien se setea el filtrofinal y los parametros de manera correcta
			if(grupodeparams.size() == 3){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true and is_valid_int(grupodeparams[2],151,0) == true){
					numthreads = atoi(grupodeparams[2][0].c_str());
					filtrofinal = "blackwhite";
				}
			}
		}
		else if(probablefiltro == "contrast"){
			if(grupodeparams.size() == 3){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true and is_valid_float(grupodeparams[2],225.0,-225.0) == true){
					numerocontrast = strtod(grupodeparams[2][0].c_str(),NULL);
					filtrofinal = "contrast";
				}
			}
		}
		else if(probablefiltro == "merge"){
			if(grupodeparams.size() == 4){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true and img_check(grupodeparams[2],std::ref(imagenes)) == true and is_valid_float(grupodeparams[3],1.0,0.0) == true){
					numeroporc = strtod(grupodeparams[3][0].c_str(),NULL);
					filtrofinal = "merge";
				}
			}
		}
		else if(probablefiltro == "boxblur"){
			if(grupodeparams.size() == 3){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true  and is_valid_int(grupodeparams[2],150,0) == true){
					numthreads = atoi(grupodeparams[2][0].c_str());
					filtrofinal = "boxblur";
				}
			}
		}
		else if(probablefiltro == "frame"){
			if(grupodeparams.size() == 4){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true and is_valid_int(grupodeparams[3],700,0) == true and is_valid_int(grupodeparams[2],256,-1) == true){
					printf("Warning: si ingresa un número de farme (4to parametro) mayor al de la mitad de la imagen del lado mas corto, esta se verá del color que pasó como 3er parametro \n");
					numeframe = atoi(grupodeparams[3][0].c_str());
					colorframe = atoi(grupodeparams[2][0].c_str());
					filtrofinal = "frame";
				}
			}
		}else if(probablefiltro == "zoom"){
			if(grupodeparams.size() == 3){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true and  is_valid_int(grupodeparams[2],9,1) == true){
					zoom = atoi(grupodeparams[2][0].c_str());
					filtrofinal = "zoom";
				}
			}
		}else if(probablefiltro == "edgedetection"){
			if(grupodeparams.size() == 2){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true){
					filtrofinal = "edgedetection";
				}
			}
		}else if(probablefiltro == "chanel"){
			if(grupodeparams.size() == 3){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true and is_valid_chanle(grupodeparams[2]) == true){
					chaelelgido = grupodeparams[2][0];
					filtrofinal = "chanel";
				}
			}
		}
		else if(probablefiltro == "pixeled"){
			if(grupodeparams.size() == 2){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true){
					filtrofinal = "pixeled";
				}
			}
		}
		else if(probablefiltro == "invers"){
			if(grupodeparams.size() == 2){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true){
					filtrofinal = "invers";
				}
			}
		}else if(probablefiltro == "inverschanel"){
			if(grupodeparams.size() == 2){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true){
					filtrofinal = "inverschanel";
				}
			}
		}else if(probablefiltro == "blurpotente"){
			if(grupodeparams.size() == 2){
				if(img_check(grupodeparams[1],std::ref(imagenes)) == true){
					filtrofinal = "blurpotente";
				}
			}
		}
	}

	vector<ppm> objetosppm;
	int restadealerta = 0; //sirve para restarle al for que mas adelante se usara para escribir las imagenes, en caso de que haya habido una falla
	struct timespec start, stop;
    double accum;
	clock_gettime(CLOCK_REALTIME, &start);
	
	if(filtrofinal == "undefined"){
		printf("Execuction Error: filter, parameters or images are incorrets, consult manual with '--help'\n");
		exit(EXIT_FAILURE);
	} // Se consulta  a la variable filtro para saber el filtro a aplicar al conjunto de imagenes, cada filtro almacena al vector objetosppm las imagenes modificadas, y luego la lista se utiliza para escribir las imagenes en los archivos de salida.
	if(filtrofinal =="blackwhite"){
		printf("Applying Black&White... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			blackWhite(img,numthreads);
			objetosppm.push_back(img);
		}	
	}else if(filtrofinal=="contrast"){
		printf("Applying Contrast... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			contrast(img,numerocontrast);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="merge"){
		printf("Applying Merge... \n");
		ppm imgaconvi(imagenes[imagenes.size() - 1]);
		for(int ni = 0; ni < imagenes.size() - 1;ni++){
			ppm img(imagenes[ni]);
			if(img.height == imgaconvi.height and img.width == imgaconvi.width){
				merge(img,imgaconvi,numeroporc);
				objetosppm.push_back(img);
			}else{
				restadealerta++;
				printf("Couldn't apply filter, width or height do not match image (image: %i)\n",ni + 1);
			}
		}
	}else if(filtrofinal=="boxblur"){
		printf("Applying BoxBlur... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			boxBlur(img,0,img.height,numthreads);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="frame"){
		printf("Applying Frame... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			frame(img,pixel(colorframe,colorframe,colorframe),numeframe);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="zoom"){
		printf("Applying Zoom... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			ppm img_zoomed(img.width * zoom,img.height * zoom);
			Zoom(img,img_zoomed,zoom);
			objetosppm.push_back(img_zoomed);
		}
	}else if(filtrofinal=="edgedetection"){
		printf("Applying EdgeDetection... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			blackWhite(img,1);
			ppm img_target(img.width,img.height);
			edgeDetection(img,img_target);
			objetosppm.push_back(img_target);
		}
	}else if(filtrofinal=="chanel"){
		printf("Applying Chanel... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			chanel(img,chaelelgido);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="pixeled"){
		printf("Applying Pixeled... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			pixelart(img);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="invers"){
		printf("Applying Invers... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			ppm imgaux(imagenes[ni]);
			invers(img,imgaux);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="inverschanel"){
		printf("Applying Inverschanel... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			inverschanel(img);
			objetosppm.push_back(img);
		}
	}else if(filtrofinal=="blurpotente"){
		printf("Applying Blurpotente... \n");
		for(int ni = 0; ni < imagenes.size();ni++){
			ppm img(imagenes[ni]);
			blackWhite(img,1);
			blurpotente(img);
			objetosppm.push_back(img);
		}
	}
	clock_gettime(CLOCK_REALTIME, &stop);
	accum = ( stop.tv_sec - start.tv_sec )
	        + ( stop.tv_nsec - start.tv_nsec ) * ONE_OVER_BILLION;
	printf( "Filter duration: %lf s\n", accum);
	
	for(int nidos = 0; nidos < objetosppm.size();nidos++){
		if(filtrofinal == "merge" and nidos == imagenes.size() - 1){
			break;
		}
		char salida[26] = "out/outputimage";
		char id_imagen_string[4];
	    ifstream conteo("conteodeimagenes");
		conteo >> id_imagen_string;
		int id_imagen_int = atoi(id_imagen_string) + 1;
	 	conteo.close();
		ofstream conteosalida("conteodeimagenes");
		conteosalida << id_imagen_int;
		conteosalida.close();
		strcat(salida,id_imagen_string);
		strcat(salida,".ppm"); //este proceso es para que las imaganes puedan salir con id especifico
		printf("Image: %s \n", salida);
		objetosppm[nidos].write(salida);
	}
	return 0;
}

