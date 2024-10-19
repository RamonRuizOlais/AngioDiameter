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

Image sumarImagen(const Image& input, const Image& input2) {

    Image output = input;
    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            output.pixels[y][x] = input.pixels[y][x] + input2.pixels[y][x];
            // Asegurarse de que los valores estén en el rango [0, 255]
            if (output.pixels[y][x] > 255) {
                output.pixels[y][x] = 255;
            }
        }
    }

    return output;
}

int main(){

    string inputFile1 = "/ImagenesBio/Proyecto1/bordes_102.pgm";
    string inputFile2 = "/ImagenesBio/Proyecto1/esqueleto_102.pgm";
    string outputFile = "/ImagenesBio/Proyecto1/suma_102.pgm";

    Image bordes = loadPGM(inputFile1);
    Image esqueleto = loadPGM(inputFile2);

    Image suma = sumarImagen(bordes, esqueleto);

    savePGM(suma, outputFile);

    cout << "Terminado" << endl;

    return 0;

}