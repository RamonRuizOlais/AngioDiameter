# AngioDiameter
Determinar el diámetro arterial a través de métodos computacionales requiere realizar múltiples pasos de procesamiento de imágenes para obtener un resultado preciso. También se requieren varias fotografías que nos permitan utilizar la mayor parte de ellas como un conjunto de entrenamiento, para determinar los parámetros más efectivos en la mejora de las imágenes.

Las fotografías deben ser convertidas a escala de grises, y deben ser convertidas a negativos, esto para aplicar un método de mejora de imágenes, en el presente trabajo se utilizó TopHat White para esto, que consiste en restar a la imagen en negativo su apertura (la apertura consiste en erosionar y después dilatar la imagen de entrada).

Una vez se hayan mejorado las imágenes se segmentan para que queden en blanco y negro únicamente, de este modo buscamos distinguir el objeto de interés del fondo (la arteria). Se realiza un calculo de la mayor componente conexa en la imagen para eliminar el ruido de fondo y calculamos los pixeles que pasan por en medio de la arteria (a esto le llamamos esqueleto). Finalmente podemos utilizar el esqueleto y la imagen umbralizada para determinar cuál es el ancho arterial en distancia de pixeles, calculando cuál es la distancia del esqueleto a el fondo negro de la imagen.

# Referencias

Cervantes-Sanchez, Fernando; Cruz-Aceves, Ivan; Hernandez-Aguirre, Arturo; Hernandez-Gonzalez, Martha A.; Solorio-Meza, Sergio E. 2019. "Automatic Segmentation of Coronary Arteries in X-ray Angiograms using Multiscale Analysis and Artificial Neural Networks." Applied Sciences 9, no. 24: 5507. https://doi.org/10.3390/app9245507
