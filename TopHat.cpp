#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>

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
    file << " " << endl;
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
Image erosion(const Image& input, int n) {
    Image output = input;
    int size = 2*n + 1;
    std::vector<std::vector<int>> kernel;
    kernel.resize(size, std::vector<int>(size, 0));
    Diamante(size, kernel);
    //Disk(n, kernel);

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

// Función para aplicar la operación de dilatación a una imagen binaria
Image dilation(const Image& input, int n) {
    Image output = input;
    int size = 2*n + 1;
    std::vector<std::vector<int>> kernel;
    kernel.resize(size, std::vector<int>(size, 0));
    Diamante(size, kernel);
    //Disk(n, kernel);

    for (int y = n/2; y < input.height - n/2; ++y) {
        for (int x = n/2; x < input.width - n/2; ++x) {
            int max_val = 0; // Inicializamos con un valor pequeño
            // Recorremos el kernel
            for (int ky = -n/2; ky <= n/2; ++ky) {
                for (int kx = -n/2; kx <= n/2; ++kx) {
                    if (kernel[ky + n/2][kx + n/2]==1) {
                        max_val = max(max_val, input.pixels[y + ky][x + kx]);
                    }
                }
            }
            output.pixels[y][x] = max_val;
        }
    }

    return output;
}

// Función para aplicar la operación de apertura a una imagen binaria
Image opening(const Image& input, int n) {
    // Aplicamos erosión seguida de dilatación
    Image output = erosion(input, n);
    output = dilation(output, n);
    return output;
}

// Función para aplicar la operación de cerradura a una imagen binaria
Image closing(const Image& input, int n) {
    // Aplicamos dilatación seguida de erosión
    Image output = dilation(input, n);
    output = erosion(output, n);
    return output;
}

// Función para aplicar el operador Tophat White a una imagen binaria
Image tophatWhite(const Image& input, int n) {
    // Aplicar apertura a la imagen original
    Image openedImage = opening(input, n);

    // Restar la imagen original del resultado de la apertura
    Image output = input;
    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            output.pixels[y][x] = input.pixels[y][x] - openedImage.pixels[y][x];
            // Asegurarse de que los valores estén en el rango [0, 255]
            if (output.pixels[y][x] < 0) {
                output.pixels[y][x] = 0;
            }
        }
    }

    return output;
}

// Función para aplicar un ajuste lineal a una imagen
Image applyLinearAdjustment(const Image& input, int y1, int y2, int x1, int x2) {
    Image output = input;

    // Iterar sobre cada píxel de la imagen de salida
    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            // Aplicar la fórmula de ajuste lineal a cada píxel
            output.pixels[y][x] = int(double(y2 - y1) / double(x2 - x1)) * (output.pixels[y][x] - x1) + y1;

            // Asegurarse de que los valores estén en el rango [0, 255]
            if (output.pixels[y][x] < 0) {
                output.pixels[y][x] = 0;
            } else if (output.pixels[y][x] > 255) {
                output.pixels[y][x] = 255;
            }
        }
    }

    return output;
}


int maxElement(const Image& img) {
    int largestElement = img.pixels[0][0]; // Suponemos que el primer elemento es el más grande

    // Iteramos sobre la matriz de píxeles para encontrar el elemento más grande
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            if (img.pixels[y][x] > largestElement) {
                largestElement = img.pixels[y][x];
            }
        }
    }

    return largestElement;
}

int minElement(const Image& img){
    int smallestElement = img.pixels[0][0];  // Suponemos que el primer elemento es el más pequeño

    // Iteramos sobre la matriz para encontrar el elemento más pequeño
    for (int y = 0; y < img.height; ++y){
        for (int x = 0; x < img.width; ++x){
            if(img.pixels[y][x] < smallestElement){
                smallestElement = img.pixels[y][x];
            }
        }
    }

    return smallestElement;
}

// Función para obtener el negativo de una imagen
Image obtenerNegativo(const Image& input) {
    Image negativo = input;

    // Iterar sobre cada píxel de la imagen de entrada
    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            // Calcular el negativo de cada píxel
            negativo.pixels[y][x] = 255 - input.pixels[y][x];
        }
    }

    return negativo;
}

int main() {
    string folderPath = "ImagenesBio/Database_134_Angiograms/"; // Modifica la ruta de acuerdo a tu carpeta
    string outputFilename = "/ImagenesBio/Proyecto1/";


    // Tamaño del elemento estructurante
    int n = 19;

    int numImages = 123; // Número total de imágenes que deseas cargar

    // Cargar las imágenes de la carpeta y aplicar Tophat a cada una
    for (int i = 122; i <= numImages; ++i) {
        stringstream ss;
        ss << folderPath << i << ".pgm";
        string imagePath = ss.str();

        // Cargar la imagen
        Image img = loadPGM(imagePath);
        
        Image negativos = obtenerNegativo(img);

        // Aplicar Tophat a la imagen
        Image tophatImage = tophatWhite(negativos, n);

        int minValue = minElement(tophatImage);
        int maxValue = maxElement(tophatImage);
        //cout << "maxvalue de imagen " << i << ": " << maxValue << endl;

        Image outputImage = applyLinearAdjustment(tophatImage, 0, 255, minValue, maxValue);

        // Guardar la imagen transformada en la carpeta TopHat
        string tophatFilename = outputFilename + "tophat_" + to_string(i) + ".pgm";
        savePGM(outputImage, tophatFilename);
    }

    cout << "Operación Tophat aplicada a las imágenes. Imagen guardada como " << outputFilename << endl;

    return 0;
}