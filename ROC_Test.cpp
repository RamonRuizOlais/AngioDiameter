#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <chrono>

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

// Función para aplicar la operación de dilatación a una imagen binaria
Image dilation(const Image& input, int n, int interruptor) {
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
Image opening(const Image& input, int n, int interruptor) {
    // Aplicamos erosión seguida de dilatación
    Image output = erosion(input, n, interruptor);
    output = dilation(output, n, interruptor);
    return output;
}

// Función para aplicar la operación de cerradura a una imagen binaria
Image closing(const Image& input, int n, int interruptor) {
    // Aplicamos dilatación seguida de erosión
    Image output = dilation(input, n, interruptor);
    output = erosion(output, n, interruptor);
    return output;
}

// Función para aplicar el operador Tophat White a una imagen binaria
Image tophatWhite(const Image& input, int n, int interruptor) {
    // Aplicar apertura a la imagen original
    Image openedImage = opening(input, n, interruptor);

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

// Función para concatenar imágenes horizontalmente
Image concatenateImagesHorizontal(const vector<Image>& images) {
    if (images.empty()) {
        cerr << "No hay imágenes para concatenar." << endl;
        exit(1);
    }

    // Verificar que todas las imágenes tengan la misma altura
    int totalHeight = images[0].height;
    for (const auto& img : images) {
        if (img.height != totalHeight) {
            cerr << "Las imágenes no tienen la misma altura." << endl;
            exit(1);
        }
    }

    // Calcular la anchura total de la imagen resultante
    int totalWidth = 0;
    for (const auto& img : images) {
        totalWidth += img.width;
    }

    // Crear la imagen resultante
    Image concatenatedImage;
    concatenatedImage.width = totalWidth;
    concatenatedImage.height = totalHeight;
    concatenatedImage.pixels.resize(totalHeight, vector<int>(totalWidth));

    // Concatenar las imágenes
    int offset = 0;
    for (const auto& img : images) {
        for (int y = 0; y < totalHeight; ++y) {
            for (int x = 0; x < img.width; ++x) {
                concatenatedImage.pixels[y][x + offset] = img.pixels[y][x];
            }
        }
        offset += img.width;
    }

    return concatenatedImage;
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

// Definición de la función
std::vector<std::pair<double, double>> calcularROC(const Image& concatenatedImage, const Image& concatenatedImage_gt) {
    std::vector<std::pair<double, double>> metrics; // Vector para almacenar los resultados

    for (int k = 0; k < 256; ++k) {
        int TP = 0, TN = 0, FP = 0, FN = 0;

        // Binarizar la imagen concatenatedImage con el umbral k
        Image binarizedImage = concatenatedImage;
        for (int y = 0; y < concatenatedImage.height; ++y) {
            for (int x = 0; x < concatenatedImage.width; ++x) {
                if (concatenatedImage.pixels[y][x] >= k) {
                    binarizedImage.pixels[y][x] = 255;
                } else {
                    binarizedImage.pixels[y][x] = 0;
                }
            }
        }

        // Comparar la imagen binarizada con concatenatedImage_gt
        for (int y = 0; y < concatenatedImage_gt.height; ++y) {
            for (int x = 0; x < concatenatedImage_gt.width; ++x) {
                if (binarizedImage.pixels[y][x] == 255 && concatenatedImage_gt.pixels[y][x] == 255) {
                    TP++;
                } else if (binarizedImage.pixels[y][x] == 0 && concatenatedImage_gt.pixels[y][x] == 0) {
                    TN++;
                } else if (binarizedImage.pixels[y][x] == 255 && concatenatedImage_gt.pixels[y][x] == 0) {
                    FP++;
                } else if (binarizedImage.pixels[y][x] == 0 && concatenatedImage_gt.pixels[y][x] == 255) {
                    FN++;
                }
            }
        }

        int suma = TP + TN + FP + FN;
        //cout << "suma = " << suma << endl;

        // Calcular sensitivity y specificity
        double sensitivity = static_cast<double>(TP) / (TP + FN);
        double specificity = static_cast<double>(TN) / (TN + FP);

        // Almacenar los valores en el vector
        metrics.push_back(std::make_pair(specificity, sensitivity));
    }

    return metrics;
}

// Función para guardar las métricas en un archivo de texto
void saveMetricsToFile(const std::vector<std::pair<double, double>>& metrics, const std::string& filename) {
    std::ofstream outFile(filename); // Abrir el archivo para escritura

    if (!outFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << filename << " para escritura." << std::endl;
        return;
    }

    // Escribir las métricas en el archivo
    for (int i = 0; i < metrics.size(); ++i) {
        double specificity = metrics[i].first;
        double sensitivity = metrics[i].second;
        outFile << 1 - specificity << " " << sensitivity << std::endl;
    }

    // Cerrar el archivo
    outFile.close();

    std::cout << "Métricas guardadas en " << filename << std::endl;
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

// Función para leer un archivo.txt y guardar su contenido en un vector de pares ordenados
vector<pair<double, double>> leerArchivo(const string& nombreArchivo) {
    vector<pair<double, double>> pares;

    // Abre el archivo
    ifstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombreArchivo << endl;
        return pares; // Retorna el vector vacío si no se puede abrir el archivo
    }

    string linea;
    while (getline(archivo, linea)) {
        istringstream ss(linea);
        double x, y;
        // Lee los valores de x e y desde la línea
        if (!(ss >> x >> y)) {
            cerr << "Error al leer la línea: " << linea << endl;
            continue; // Salta a la siguiente línea si hay un error en el formato
        }
        // Añade el par ordenado al vector
        pares.push_back(make_pair(x, y));
    }

    // Cierra el archivo
    archivo.close();

    return pares;
}

// Función para calcular la integral usando método del trapecio
double AreaAU(vector<pair<double, double>>& puntos){

    double area = 0.0;
    int n = puntos.size();

    for(int i = 0; i < n; i++){

        pair<double,double> punto1 = puntos[i];
        pair<double,double> punto2 = puntos[i + 1];
    
        double base = punto2.first - punto1.first;
        double altura_promedio = (punto1.second + punto2.second)/2.0;

        area += base*altura_promedio;
    }

    return abs(area);
}

// Función para encontrar el elemento mayor en un vector y su posición
pair<double, int> encontrarMayor(const std::vector<double>& vec) {
    // Verificar si el vector está vacío
    if (vec.empty()) {
        std::cerr << "El vector está vacío." << std::endl;
        return {0.0, -1}; // Devolver un par con valor 0.0 y posición -1
    }

    double mayor = vec[0]; // Suponemos que el primer elemento es el mayor
    int posicion = 0; // La posición del mayor elemento

    // Iterar sobre el vector para encontrar el mayor elemento y su posición
    for (int i = 1; i < vec.size(); ++i) {
        if (vec[i] > mayor) {
            mayor = vec[i];
            posicion = i;
    }
}

    return {mayor, posicion}; // Devolver un par con el mayor elemento y su posición
}


int main() {
    auto start = chrono::steady_clock::now();
    string folderPath = "/ImagenesBio/Database_134_Angiograms/"; // Modifica la ruta de acuerdo a tu carpeta

    int numImages = 100; // Número total de imágenes que deseas cargar

    for(int interruptor = 0; interruptor < 2; interruptor++){
        for(int j = 1; j < 22; j+=2){
        int n = 2*j + 1;
        vector<Image> imagesToConcatenate;
        vector<Image> imagesToConcatenate_gt;

            // Cargar las imágenes de la carpeta y aplicar Tophat a cada una
            for (int i = 1; i <= numImages; ++i) {
                stringstream ss;
                stringstream ss2;
                ss << folderPath << i << ".pgm";
                ss2 << folderPath << i << "_gt.pgm";
                string imagePath = ss.str();
                string image_gtPath = ss2.str();

                // Cargar la imagen
                Image img = loadPGM(imagePath);
                Image img_gt = loadPGM(image_gtPath);
        
                Image negativos = obtenerNegativo(img);

                // Aplicar Tophat a la imagen
                Image tophatImage = tophatWhite(negativos, n, interruptor);

                int minValue = minElement(tophatImage);
                int maxValue = maxElement(tophatImage);
                //cout << "maxvalue de imagen " << i << ": " << maxValue << endl;

                Image outputImage = applyLinearAdjustment(tophatImage, 0, 255, minValue, maxValue);

                // Agregar la imagen transformada al vector
                imagesToConcatenate.push_back(outputImage);
                imagesToConcatenate_gt.push_back(img_gt);
            }
            // Concatenar las imágenes horizontalmente
            Image concatenatedImage = concatenateImagesHorizontal(imagesToConcatenate);
            Image concatenatedImage_gt = concatenateImagesHorizontal(imagesToConcatenate_gt);
        
            // Calcular métricas
            vector<pair<double, double>> metrics = calcularROC(concatenatedImage, concatenatedImage_gt);

            // Generar el nombre del archivo de métricas basado en el valor de k
            string metricsFilename = "/ImagenesBio/Proyecto1/resultados_ROC/metrics" + to_string(interruptor) + "_" + to_string(j) + ".txt";
            // Guardar las métricas en un archivo
            saveMetricsToFile(metrics, metricsFilename);
        }
    }
    

    cout << "Operación TopHat aplicada a las imágenes y se han concatenado. Las métricas se han guardado correctamente." << endl;

    // Obtenemos la dirección de nuestros archivos txt
    string folderPath2= "/ImagenesBio/Proyecto1/resultados_ROC/";
    vector<double> resultados;

    // Leemos los archivos.txt, calculamos la integral para cada uno y guardamos los resultados en un vector
    for(int i = 0; i < 2; i++){
        for(int k = 3; k <= 25; k+=2){
            string metricsPath = folderPath2 + "metrics" + to_string(i) + "_" + to_string(k) + ".txt";
            vector<pair<double,double>> puntos= leerArchivo(metricsPath);
            double integral = AreaAU(puntos);
            resultados.push_back(integral);
        }
    }

    // Buscamos la integral con mayor valor en el vector de resultados e imprimimos su valor y sus correspondientes parámetros
    pair<double, int> Az = encontrarMayor(resultados);
    cout << "El valor mayor de Az es: " << Az.first << endl;
    switch(Az.second){
        case 0:
            cout << "Parámetros: Diamante | Tamaño 1 (3x3)" << endl;
            break;
        case 1:
            cout << "Parámetros: Diamante | Tamaño 3 (7x7)" << endl;
            break;
        case 2:
            cout << "Parámetros: Diamante | Tamaño 5 (11x11)" << endl;
            break;
        case 3:
            cout << "Parámetros: Diamante | Tamaño 7 (15x15)" << endl;
            break;
        case 4:
            cout << "Parámetros: Diamante | Tamaño 9 (19x19)" << endl;
            break;
        case 5:
            cout << "Parámetros: Diamante | Tamaño 11 (23x23)" << endl;
            break;
        case 6:
            cout << "Parámetros: Diamante | Tamaño 13 (27x27)" << endl;
            break;
        case 7:
            cout << "Parámetros: Diamante | Tamaño 15 (31x31)" << endl;
            break;
        case 8:
            cout << "Parámetros: Diamante | Tamaño 17 (35x35)" << endl;
            break;
        case 9:
            cout << "Parámetros: Diamante | Tamaño 19 (39x39)" << endl;
            break;
        case 10:
            cout << "Parámetros: Diamante | Tamaño 21 (43x43)" << endl;
            break;

        case 11:
            cout << "Parámetros: Disco | Tamaño 1 (3x3)" << endl;
            break;
        case 12:
            cout << "Parámetros: Disco | Tamaño 3 (7x7)" << endl;
            break;
        case 13:
            cout << "Parámetros: Disco | Tamaño 5 (11x11)" << endl;
            break;
        case 14:
            cout << "Parámetros: Disco | Tamaño 7 (15x15)" << endl;
            break;
        case 15:
            cout << "Parámetros: Disco | Tamaño 9 (19x19)" << endl;
            break;
        case 16:
            cout << "Parámetros: Disco | Tamaño 11 (23x23)" << endl;
            break;
        case 17:
            cout << "Parámetros: Disco | Tamaño 13 (27x27)" << endl;
            break;
        case 18:
            cout << "Parámetros: Disco | Tamaño 15 (31x31)" << endl;
            break;
        case 19:
            cout << "Parámetros: Disco | Tamaño 17 (35x35)" << endl;
            break;
        case 20:
            cout << "Parámetros: Disco | Tamaño 19 (39x39)" << endl;
            break;
        case 21:
            cout << "Parámetros: Disco | Tamaño 21 (43x43)" << endl;
            break;
    }

    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "Tiempo transcurrido: " << elapsed_seconds.count() << " segundos." << endl;

    return 0;
}