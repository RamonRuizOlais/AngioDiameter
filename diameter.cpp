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

vector<double> diameter(const Image& inputOG, const Image& inputSke){

    int D = 0, interruptor = 0;
    vector<double> distancias;
    for(int y = 0; y < inputSke.height; ++y){
        for(int x = 0; x < inputSke.width; ++x){
            if(inputSke.pixels[y][x] == 255){
                D = 1;
                interruptor = 0;
                while(interruptor == 0){
                    for(int k = 0; k < D; k++){
                        if(inputOG.pixels[y + k][x - (D-k)] == 0){
                            interruptor = 1;
                            break;
                        }
                        if(inputOG.pixels[y - k][x - (D-k)] == 0){
                            interruptor = 1;
                            break;
                        }
                        if(inputOG.pixels[y + k][x + (D-k)] == 0){
                            interruptor = 1;
                            break;
                        }
                        if(inputOG.pixels[y - k][x + (D-k)] == 0){
                            interruptor = 1;
                        }
                    }
                    D++;
                }
                distancias.push_back(D-1);
            }
        }
    }

    return distancias;
}

void saveDistances(const vector<double>& distances, const string& filename) {
    ofstream outputFile(filename);

    if (!outputFile) {
        cerr << "No se pudo abrir el archivo " << filename << " para escribir." << endl;
        exit(1);
    }

    for (size_t i = 0; i < distances.size(); ++i) {
        outputFile << distances[i] << endl;
    }

    outputFile.close();

    cout << "Distancias guardadas en " << filename << endl;
}

int main(){

    string inputFile1 = "/ImagenesBio/Proyecto1/esqueleto_102.pgm";
    string inputFile2 = "/ImagenesBio/Proyecto1/binario_102.pgm";
    string outputFile = "/ImagenesBio/Proyecto1/distancias_102.txt";

    Image img_esqueleto = loadPGM(inputFile1);
    Image img_bin = loadPGM(inputFile2);

    vector<double> distancias = diameter(img_bin, img_esqueleto);

    saveDistances(distancias, outputFile);

    cout << "Distancias guardadas" << endl;

    return 0;


}