#include <iostream>
#include <vector>
#include <fstream>

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

Image RidlerAndCalvard(const Image& input){

    Image binario = input;
    int sum = 0;

    int tol = 0.0005;
    for(int y = 0; y < input.height; ++y){
        for(int x = 0; x < input.width; ++x){
            sum += input.pixels[y][x]; 
        }
    }

    int mu = sum / (input.height*input.width);
    cout << "promedio = " << mu << endl;

    int new_thresh = mu;
    int prev_thresh = 0;
    int threshold = 0;
    int numN1 = 0, denN1 = 0, numB1 = 0, denB1 = 0, mN = 0, mB = 0, k = 0;

    while(1){

    numN1 = 0, denN1 = 0, numB1 = 0, denB1 = 0, mN = 0, mB = 0, k = 0;

        prev_thresh = new_thresh;
        threshold = prev_thresh;

        for(int y = 0; y < input.height; ++y){
            for(int x = 0; x < input.width; ++x){
                if(input.pixels[y][x] <= threshold){
                    numN1 += input.pixels[y][x];
                    denN1 += 1;
                }
                else{
                    numB1 += input.pixels[y][x];
                    denB1 += 1;
                }
            }
        }

        mN = numN1/denN1;
        mB = numB1/denB1;

        new_thresh = (mB+mN)/(2);

        if(abs(new_thresh - prev_thresh) <= tol){
            break;
        }
        cout << "threshold = " << threshold << endl;
    }

    for(int y = 0; y < binario.height; ++y){
        for(int x = 0; x < binario.width; ++x){
            if(binario.pixels[y][x] <= threshold){
                    binario.pixels[y][x] = 0;
            }
            else{
                binario.pixels[y][x] = 255;
            }
        }
    }

    return binario;

}

int main(){
    string inputFile = "/ImagenesBio/Proyecto1/tophat_1.pgm";
    string outputFile = "/ImagenesBio/Proyecto1/binario_1.pgm";

    Image img = loadPGM(inputFile);
    Image img_bin = RidlerAndCalvard(img);

    savePGM(img_bin, outputFile);

    cout << "La imagen se ha umbralizado con éxito." << endl;

    return 0;
}