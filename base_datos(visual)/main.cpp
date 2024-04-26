#include <iostream>

int main() {
  std::string cadena_original = "10#1#2#Nasser, Mrs. Nicholas (Adele Achem)#female#14#1#0#237736#300.708##C";
  int posicion_hashtag = 0;
  int contador_hashtag = 0;

  while (cadena_original[posicion_hashtag] != '\0' && contador_hashtag < 5) {
    if (cadena_original[posicion_hashtag] == '#') {
      contador_hashtag++;
    }
    posicion_hashtag++;
  }

  if (contador_hashtag == 5) {
    posicion_hashtag++; // Salta el quinto '#'
    std::string palabra_extraida;
    while (cadena_original[posicion_hashtag] != '\0' && cadena_original[posicion_hashtag] != '#') {
      palabra_extraida += cadena_original[posicion_hashtag];
      posicion_hashtag++;
    }
    std::cout << "Palabra extraída: " << palabra_extraida << std::endl;
  } else {
    std::cout << "Error: No se encontró el quinto '#'" << std::endl;
  }

  return 0;
}