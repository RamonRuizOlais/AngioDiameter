#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

// Estructura para representar una imagen PGM
struct Image {
    int width;
    int height;
    vector<vector<int>> pixels;
};

// Función para cargar una imagen PGM desde un archivo
Image loadPGM(const string& filename) {
    Image img;
    ifstream file(filename);

    if (!file) {
        cerr << "No se pudo abrir el archivo " << filename << endl;
        exit(1);
    }

    string format;
    string comment;
    getline(file, format);
    getline(file, comment);

    if (format != "P2") {
        cout << "El formato es: " << format << endl;
        cerr << "Formato de archivo no válido. Se esperaba P2." << endl;
        exit(1);
    }

    file >> img.width >> img.height;
    int max_val;
    file >> max_val;

    img.pixels.resize(img.height, vector<int>(img.width));

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            file >> img.pixels[y][x];
        }
    }

    file.close();

    return img;
}

// Función para guardar una imagen PGM en un archivo
void savePGM(const Image& img, const string& filename) {
    ofstream file(filename);

    if (!file) {
        cerr << "No se pudo crear el archivo " << filename << endl;
        exit(1);
    }

    file << "P2" << endl;
    file << "#prueba" << endl;
    file << img.width << " " << img.height << endl;
    file << "255" << endl;

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            file << img.pixels[y][x] << " ";
        }
        file << endl;
    }

    file.close();
}

int neighbours2_6(const Image& input, int y, int x){
    int k = 0;
    
    if(input.pixels[y+1][x] == 255){
        k += 1;
    }
    if(input.pixels[y - 1][x] == 255){
        k += 1;
    }
    if(input.pixels[y][x + 1] == 255){
        k += 1;
    }
    if(input.pixels[y][x - 1] == 255){
        k += 1;
    }
    if(input.pixels[y+1][x+1] == 255){
        k += 1;
    }
    if(input.pixels[y-1][x-1] == 255){
        k += 1;
    }
    if(input.pixels[y+1][x-1] == 255){
        k += 1;
    }
    if(input.pixels[y-1][x+1] == 255){
        k += 1;
    } 

    return k;
}

int neighboursChanges(const Image& input, int y, int x){

    int neighboursValues[8];
    int changes = 0;
    neighboursValues[0] = (input.pixels[y-1][x]);
    neighboursValues[1] = (input.pixels[y-1][x+1]);
    neighboursValues[2] = (input.pixels[y][x+1]);
    neighboursValues[3] = (input.pixels[y+1][x+1]);
    neighboursValues[4] = (input.pixels[y+1][x]);
    neighboursValues[5] = (input.pixels[y+1][x-1]);
    neighboursValues[6] = (input.pixels[y][x-1]);
    neighboursValues[7] = (input.pixels[y-1][x-1]);

    for(int i = 0; i < 7; i++){
        if(neighboursValues[i] == 0 && neighboursValues[i+1] == 255){
            changes += 1;
        }
    }

    // Verificar el cambio entre el último y el primer vecino
    if (neighboursValues[7] == 0 && neighboursValues[0] == 255) {
        changes += 1;
    }

    return changes;
}

int condicionExtra(const Image&input, int y, int x){
    int sum = 0;

    sum = input.pixels[y-1][x]*pow(2,0) + input.pixels[y-1][x+1]*pow(2,1) + input.pixels[y][x+1]*pow(2,2) + input.pixels[y+1][x+1]*pow(2,3) + input.pixels[y+1][x]*pow(2,4) + input.pixels[y+1][x-1]*pow(2,5) + input.pixels[y][x-1]*pow(2,6) + input.pixels[y-1][x-1]*pow(2,7);

    return int(float(sum/255));
}

Image ZhangSuen(const Image& input){

    Image skeleton = input;
    vector<pair<int,int>> position;
    position.push_back(make_pair(1,1));

    while(position.size() > 0){

        position.clear();
        for(int y = 1; y < skeleton.height - 1; ++y){
            for(int x = 1; x < skeleton.width - 1 ; ++x){
                if(skeleton.pixels[y][x] == 255){
                    cout << "vecinos = " << neighbours2_6(skeleton, y, x) << endl;
                    if(2 <= neighbours2_6(skeleton, y, x) && neighbours2_6(skeleton, y, x) <= 6){
                        //cout << "cambios = " << neighboursChanges(skeleton, y, x) << endl;
                        if(neighboursChanges(skeleton, y, x) == 1){
                            if(skeleton.pixels[y-1][x] == 0|| skeleton.pixels[y][x+1] == 0 || skeleton.pixels[y+1][x] == 0){
                                if(skeleton.pixels[y][x+1] == 0 || skeleton.pixels[y+1][x] == 0 || skeleton.pixels[y][x-1] == 0){
                                   position.push_back(make_pair(y,x)); 
                                }
                            }
                        }
                    }
                }
            }
        }

        for (const auto& pos : position) {
            int y = pos.first;
            int x = pos.second;
            skeleton.pixels[y][x] = 0; // Establecer el píxel en negro
        }
    

        
        position.clear();

        for(int y = 1; y < skeleton.height - 1; ++y){
            for(int x = 1; x < skeleton.width - 1; ++x){
                if(skeleton.pixels[y][x] == 255){
                    if(2 <= neighbours2_6(skeleton, y, x) && neighbours2_6(skeleton, y, x) <= 6){
                        if(neighboursChanges(skeleton, y, x) == 1){
                            if(skeleton.pixels[y - 1][x] == 0 || skeleton.pixels[y][x+1] == 0 || skeleton.pixels[y][x-1] == 0){
                                if(skeleton.pixels[y -1][x] == 0 || skeleton.pixels[y+1][x] == 0 || skeleton.pixels[y][x-1] == 0){
                                    position.push_back(make_pair(y,x));    
                                }    
                            }
                        }
                    }
                }
            }
        }

        for (const auto& pos : position) {
            int y = pos.first;
            int x = pos.second;
            skeleton.pixels[y][x] = 0; // Establecer el píxel en negro
        }

    }

    return skeleton;
    
}


Image ZhangSuen2(const Image& input){

    Image skeleton = input;
    int interruptor = 1;
    while(interruptor == 1){
        interruptor = 0;
        for(int y = 1; y < skeleton.height - 1; ++y){
            for(int x = 1; x < skeleton.width - 1 ; ++x){
                if(skeleton.pixels[y][x] == 255){
                    cout << "vecinos = " << neighbours2_6(skeleton, y, x) << endl;
                    if(2 <= neighbours2_6(skeleton, y, x) && neighbours2_6(skeleton, y, x) <= 6){
                        //cout << "cambios = " << neighboursChanges(skeleton, y, x) << endl;
                        if(neighboursChanges(skeleton, y, x) == 1 || condicionExtra(skeleton, y, x) == 65 || condicionExtra(skeleton, y, x) == 5 || condicionExtra(skeleton, y, x) == 20 || condicionExtra(skeleton, y, x) == 80 || condicionExtra(skeleton, y, x) == 97 || condicionExtra (skeleton, y, x)== 133 || condicionExtra(skeleton, y, x) == 52 || condicionExtra(skeleton, y, x) == 208 || condicionExtra(skeleton, y, x) == 67 || condicionExtra(skeleton, y, x) == 13 || condicionExtra(skeleton, y, x) == 22 || condicionExtra(skeleton, y, x) == 88 || condicionExtra(skeleton, y, x) == 99 || condicionExtra(skeleton, y, x) == 141 || condicionExtra(skeleton, y, x) == 54 || condicionExtra(skeleton, y, x) ==216){
                            if(skeleton.pixels[y-1][x] == 0|| skeleton.pixels[y][x+1] == 0 || skeleton.pixels[y+1][x] == 0){
                                if(skeleton.pixels[y][x+1] == 0 || skeleton.pixels[y+1][x] == 0 || skeleton.pixels[y][x-1] == 0){
                                   skeleton.pixels[y][x] = 0;
                                   interruptor = 1; 
                                }
                            }
                        }
                    }
                }
            }
        }
    
        for(int y = 1; y < skeleton.height - 1; ++y){
            for(int x = 1; x < skeleton.width - 1; ++x){
                if(skeleton.pixels[y][x] == 255){
                    if(2 <= neighbours2_6(skeleton, y, x) && neighbours2_6(skeleton, y, x) <= 6){
                        if(neighboursChanges(skeleton, y, x) == 1 || condicionExtra(skeleton, y, x) == 65 || condicionExtra(skeleton, y, x) == 5 || condicionExtra(skeleton, y, x) == 20 || condicionExtra(skeleton, y, x) == 80 || condicionExtra(skeleton, y, x) == 97 || condicionExtra (skeleton, y, x)== 133 || condicionExtra(skeleton, y, x) == 52 || condicionExtra(skeleton, y, x) == 208 || condicionExtra(skeleton, y, x) == 67 || condicionExtra(skeleton, y, x) == 13 || condicionExtra(skeleton, y, x) == 22 || condicionExtra(skeleton, y, x) == 88 || condicionExtra(skeleton, y, x) == 99 || condicionExtra(skeleton, y, x) == 141 || condicionExtra(skeleton, y, x) == 54 || condicionExtra(skeleton, y, x) ==216){
                            if(skeleton.pixels[y - 1][x] == 0 || skeleton.pixels[y][x+1] == 0 || skeleton.pixels[y][x-1] == 0){
                                if(skeleton.pixels[y -1][x] == 0 || skeleton.pixels[y+1][x] == 0 || skeleton.pixels[y][x-1] == 0){
                                    skeleton.pixels[y][x] = 0;
                                    interruptor = 1;    
                                }    
                            }
                        }
                    }
                }
            }
        }

    }

    return skeleton;
    
}

int main(){
    string inputFile = "/ImagenesBio/Proyecto1/conexa_103.pgm";
    string outputFile = "/ImagenesBio/Proyecto1/esqueleto_103.pgm";

    Image img = loadPGM(inputFile);
    Image img_ske1 = ZhangSuen(img);
    Image img_ske2 = ZhangSuen2(img_ske1);

    savePGM(img_ske2, outputFile);

    cout << "terminado"<< endl;

    return 0;
}