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

void Diamante(int n, std::vector<std::vector<int>>& matriz) {

    for(int j=0; j<=n/2; j++){
        for(int i= n/2 - j; i <= n/2 + j; i++){
            matriz[i][j] = 1;
            matriz[i][n - 1 - j] = 1; // Reflejar simétricamente sobre el eje vertical
        }
   }

}

void Disk(int n, std::vector<std::vector<int>>& matriz) {
    int mitad=n/2;
    for(int j=0; j<n; j++){
        for(int i= 0; i <n; i++){
            if((i-mitad)*(i-mitad) + (j-mitad)*(j-mitad) <= mitad*mitad){
                matriz[i][j] = 1;
            }else{
                matriz[i][j] = 0;
            }
            
        }
   }

}

// Función para aplicar la operación de erosión a una imagen binaria
Image erosion(const Image& input, int n, int interruptor) {
    Image output = input;

    std::vector<std::vector<int>> kernel;
    kernel.resize(n, std::vector<int>(n, 0));
    if(interruptor == 0){
        Diamante(n, kernel);
    }
    else{
        Disk(n, kernel);
    }

    for (int y = n/2; y < input.height - n/2; ++y) {
        for (int x = n/2; x < input.width - n/2; ++x) {
            int min_val = 255; // Inicializamos con un valor grande
            // Recorremos el kernel
            for (int ky = -n/2; ky <= n/2; ++ky) {
                for (int kx = -n/2; kx <= n/2; ++kx) {
                    if (kernel[ky + (n/2)][kx + (n/2)]==1) {
                        min_val = min(min_val, input.pixels[y + ky][x + kx]);
                    }
                }
            }
            output.pixels[y][x] = min_val;
        }
    }

    return output;
}

Image restarImagen(const Image& input, const Image& input2) {

    Image output = input;
    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            output.pixels[y][x] = input.pixels[y][x] - input2.pixels[y][x];
            // Asegurarse de que los valores estén en el rango [0, 255]
            if (output.pixels[y][x] < 0) {
                output.pixels[y][x] = 0;
            }
        }
    }

    return output;
}

Image Bordes(const Image& input){

    int n = 3;

    Image img_ero = erosion(input, n, 0);

    Image resultado = restarImagen(input, img_ero);

    return resultado;

}

int main(){
    string inputFile = "/ImagenesBio/Proyecto1/conexa_102.pgm";
    string outputFile = "/ImagenesBio/Proyecto1/bordes_102.pgm";

    Image img = loadPGM(inputFile);

    Image img_bordes = Bordes(img);

    savePGM(img_bordes, outputFile);

    cout << "terminado"<< endl;

    return 0;
}