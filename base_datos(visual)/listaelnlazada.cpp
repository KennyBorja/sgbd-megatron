#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Nodo para la lista enlazada que representa una columna
struct NodoColumna {
    string nombre;
    string tipo;
    NodoColumna* siguiente;

    NodoColumna(const string& nombreColumna, const string& tipoDato) 
        : nombre(nombreColumna), tipo(tipoDato), siguiente(nullptr) {}
};

// Clase para representar una tabla con su estructura de columnas utilizando una lista enlazada
class Tabla {
private:
    string nombre;
    NodoColumna* primeraColumna;

public:
    Tabla(const string& nombreTabla) : nombre(nombreTabla), primeraColumna(nullptr) {}

    void agregarColumna(const string& nombreColumna, const string& tipoDato) {
        NodoColumna* nuevaColumna = new NodoColumna(nombreColumna, tipoDato);
        if (!primeraColumna) {
            primeraColumna = nuevaColumna;
        } else {
            NodoColumna* temp = primeraColumna;
            while (temp->siguiente) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevaColumna;
        }
    }

    template<typename... Args>
    void agregarColumnas(const string& nombreColumna, const string& tipoDato, Args... args) {
        agregarColumna(nombreColumna, tipoDato);
        agregarColumnas(args...);
    }

    void agregarColumnas() {
        guardarEnArchivo();
    } // Caso base de la recursión

  void guardarEnArchivo() const {
    ifstream archivoExistente("esquemas.txt");
    string linea;
    bool tablaExistente = false;

    // Verificar si el nombre de la tabla ya existe en el archivo
    while (getline(archivoExistente, linea)) {
        size_t indice = linea.find('#');
        if (indice != string::npos) {
            string nombreTablaExistente = linea.substr(0, indice);
            if (nombreTablaExistente == nombre) {
                tablaExistente = true;
                break;
            }
        }
    }
    archivoExistente.close();

    // Si la tabla no existe en el archivo, guardarla
    if (!tablaExistente) {
        ofstream archivo("esquemas.txt", ios::app);
        if (archivo.is_open()) {
            archivo << nombre;
            NodoColumna* temp = primeraColumna;
            while (temp) {
                archivo<< "#" << temp->nombre<< "#"  << temp->tipo ;
                temp = temp->siguiente;
            }
            archivo << endl;
            archivo.close();
            cout << "Tabla '" << nombre << "' guardada en 'esquemas.txt'" << endl;
        } else {
            cerr << "Error al abrir o crear el archivo 'esquemas.txt'" << endl;
        }
    } else {
        cout << "La tabla '" << nombre << "' ya existe en 'esquemas.txt'. No se ha guardado." << endl;
    }
}

    bool buscarYEliminarEsquema(const string& nombreEsquema, const string& nombreArchivo) {
        ifstream archivo(nombreArchivo);
        ofstream archivoTemporal("temporal.txt");
        bool encontrado = false;
        
        if (archivo.is_open() && archivoTemporal.is_open()) {
            string linea;
            while (getline(archivo, linea)) {
                size_t indice = linea.find('#');
                if (indice != string::npos) {
                    string primeraPalabra = linea.substr(0, indice);
                    if (primeraPalabra == nombreEsquema) {
                        encontrado = true;
                        continue;
                    }
                }
                archivoTemporal << linea << endl;
            }
            
            archivo.close();
            archivoTemporal.close();
            
            remove(nombreArchivo.c_str());
            rename("temporal.txt", nombreArchivo.c_str());
        } else {
            cerr << "Error al abrir el archivo '" << nombreArchivo << "'" << endl;
        }

        return encontrado;
    }

    // Destructor para liberar la memoria de los nodos de la lista enlazada
    ~Tabla() {
        NodoColumna* temp;
        while (primeraColumna) {
            temp = primeraColumna;
            primeraColumna = primeraColumna->siguiente;
            delete temp;
        }
    }
};



vector<string> extraerTiposDatos(const string& nombreTabla) {
    vector<string> tiposDatos;

    ifstream archivoExistente("esquemas.txt");
    if (!archivoExistente.is_open()) {
        cerr << "Error al abrir el archivo 'esquemas.txt'." << endl;
        return tiposDatos;
    }

    string linea;
    while (getline(archivoExistente, linea)) {
        size_t indice = linea.find('#');
        if (indice != string::npos) {
            string nombreTablaExistente = linea.substr(0, indice);
            if (nombreTablaExistente == nombreTabla) {
                // Extraer los tipos de datos de la línea
                stringstream ss(linea.substr(indice + 1)); // Saltar el nombre de la tabla
                string tipoDato;
                while (getline(ss, tipoDato, '#')) {
                    tiposDatos.push_back(tipoDato);
                }
                break; // No necesitamos seguir buscando después de encontrar la tabla
            }
        }
    }

    archivoExistente.close();
    return tiposDatos;
}


void llenarArchivo(const string& nombreEsquema, const vector<vector<string>>& conjuntosValores) {
    ofstream archivo(nombreEsquema + ".txt", ios::app); // Abre el archivo en modo de añadir al final
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo '" << nombreEsquema << ".txt'." << endl;
        return;
    }

    // Recorrer cada conjunto de valores y escribirlos en el archivo
    for (const auto& valores : conjuntosValores) {
        vector<string> tiposDatos = extraerTiposDatos(nombreEsquema);
        int Numcolumnas = tiposDatos.size() / 2;

        // Verificar que haya el número correcto de valores en el conjunto
        if (valores.size() != Numcolumnas) {
            cerr << "El esquema '" << nombreEsquema << "' requiere al menos " << Numcolumnas << " valores." << endl;
            continue; // Pasar al siguiente conjunto de valores
        }

        // Escribir los valores en el archivo separados por '#'
        for (size_t i = 0; i < Numcolumnas; ++i) {
            if(i!=Numcolumnas-1)
                archivo<<valores[i]<<"#";
            else
                archivo << valores[i]; // Escribir cada valor seguido de '#'
        }
        archivo << endl; // Agregar una nueva línea al final
    }

    cout << "Datos agregados al archivo '" << nombreEsquema << ".txt'." << endl;

    archivo.close(); // Cerrar el archivo
}


vector<vector<string>> leerLlenarCSV(const string& nombreArchivo) {
    vector<vector<string>> datos; // Vector de vectores de strings para almacenar los datos del CSV

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo '" << nombreArchivo << "'." << endl;
        return datos; // Devolver un vector vacío si no se puede abrir el archivo
    }

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string valor;
        vector<string> fila; // Vector para almacenar los valores de una fila del CSV
        while (getline(ss, valor, ',')) {
            fila.push_back(valor); // Agregar cada valor a la fila
        }
        datos.push_back(fila); // Agregar la fila al vector de datos
    }

    archivo.close();

    return datos;
}

vector<vector<string>> leerdatoTabla(const string& nombreArchivo) {
    vector<vector<string>> datos; // Vector de vectores de strings para almacenar los datos del CSV

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo '" << nombreArchivo << "'." << endl;
        return datos; // Devolver un vector vacío si no se puede abrir el archivo
    }

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string valor;
        vector<string> fila; // Vector para almacenar los valores de una fila del CSV
        while (getline(ss, valor, '#')) {
            fila.push_back(valor); // Agregar cada valor a la fila
        }
        datos.push_back(fila); // Agregar la fila al vector de datos
    }

    archivo.close();

    return datos;
}


void MostrarDatos(const string& nombreTabla) {
    vector<string> tiposDatos;

    ifstream archivoExistente("esquemas.txt");
    if (!archivoExistente.is_open()) {
        cerr << "Error al abrir el archivo 'esquemas.txt'." << endl;
    }

    string linea;
    while (getline(archivoExistente, linea)) {
        size_t indice = linea.find('#');
        if (indice != string::npos) {
            string nombreTablaExistente = linea.substr(0, indice);
            if (nombreTablaExistente == nombreTabla) {
                // Extraer los tipos de datos de la línea
                stringstream ss(linea.substr(indice + 1)); // Saltar el nombre de la tabla
                string tipoDato;
                while (getline(ss, tipoDato, '#')) {
                    tiposDatos.push_back(tipoDato);
                }
                break; // No necesitamos seguir buscando después de encontrar la tabla
            }
        }
    }

    archivoExistente.close();
cout << "Tipos de datos impares de la tabla '" << nombreTabla << "': " << endl;;
    for (size_t i = 0; i < tiposDatos.size(); ++i) {
        if (i % 2 == 0) { // Si el índice es par, imprimir el tipo de dato
            cout << tiposDatos[i] << "\t";
        }
    }
    cout << endl;
    string nombreCompleto = nombreTabla + ".txt";

    vector<vector<string>> datos = leerdatoTabla(nombreCompleto);
      for (const auto& fila : datos) {
        for (const auto& elemento : fila) {
            cout << elemento << "\t";
        }
        cout << endl;
    } 

    
}


// Función para realizar una consulta SELECT
vector<vector<string>> consultaSelect(const string& nombreTabla, const vector<string>& condiciones) {
    vector<vector<string>> resultado;
    vector<string> tiposDatos = extraerTiposDatos(nombreTabla);
    int numColumnas = tiposDatos.size() / 2;

    string nombreArchivo = nombreTabla + ".txt";
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo '" << nombreArchivo << "'." << endl;
        return resultado;
    }

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string valor;
        vector<string> fila;
        for (int i = 0; i < numColumnas; ++i) {
            getline(ss, valor, '#');
            fila.push_back(valor);
        }

        // Verificar si la fila cumple con las condiciones
        bool cumpleCondiciones = true;
        for (const auto& condicion : condiciones) {
            size_t posicionOperador = condicion.find_first_of("=<>");
            string columna = condicion.substr(0, posicionOperador);
            string operador = condicion.substr(posicionOperador, 1);
            string valor = condicion.substr(posicionOperador + 1);

            int indiceColumna = -1;
            for (int i = 0; i < numColumnas; ++i) {
                if (tiposDatos[i * 2] == columna) {
                    indiceColumna = i;
                    break;
                }
            }

            if (indiceColumna == -1) {
                cerr << "Error: La columna '" << columna << "' no existe en la tabla '" << nombreTabla << "'." << endl;
                return resultado;
            }

            if (operador == "=") {
                if (fila[indiceColumna] != valor) {
                    cumpleCondiciones = false;
                    break;
                }
            } else if (operador == "<") {
                if (fila[indiceColumna] >= valor) {
                    cumpleCondiciones = false;
                    break;
                }
            } else if (operador == ">") {
                if (fila[indiceColumna] <= valor) {
                    cumpleCondiciones = false;
                    break;
                }
            }
        }

        if (cumpleCondiciones) {
            resultado.push_back(fila);
        }
    }

    archivo.close();
    return resultado;
}

// Función para guardar los resultados en un nuevo archivo
void guardarResultado(const string& nombreResultado, const vector<vector<string>>& resultado) {
    ofstream archivo(nombreResultado + ".txt");
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo '" << nombreResultado << ".txt'." << endl;
        return;
    }

    for (const auto& fila : resultado) {
        for (size_t i = 0; i < fila.size(); ++i) {
            archivo << fila[i];
            if (i != fila.size() - 1) {
                archivo << "#";
            }
        }
        archivo << endl;
    }

    archivo.close();
    cout << "Resultados guardados en '" << nombreResultado << ".txt'." << endl;
}

int main() {


    /* 
     Tabla tabla1("empleado");
    tabla1.agregarColumnas("nombre", "string", "edad", "int", "apellidos", "string");
   

    Tabla tabla2("product");
    tabla2.agregarColumnas("nombre", "string", "precio", "double", "cantidad", "int");

    Tabla tabla2("NIÑO");
    tabla2.agregarColumnas("nombre", "string", "APELLIDO", "double", "EDAD", "int");

    Tabla tabla3("titanic");
    tabla3.agregarColumnas("PassengerId","string","	Survived","string","Pclass","string","Name","string","apel","string","Sex","string","Age","string","SibSp","string",
    "Parch","string","Ticket","string","Fare","string","Cabin","string","Embarked","string");

    vector<vector<string>> datos = leerLlenarCSV("titanic.csv");

    llenarArchivo("product", {{"Juan", "30", "Pérez"}, {"maria", "30", "Pérez"}, {"berta", "30", "Pérez"}});
    //llenarArchivo("titanic", datos);

    //MostrarDatos("titanic");
    MostrarDatos("product");

    vector<string> condiciones = {"Pclass=3", "Age>30"};
    vector<vector<string>> resultado = consultaSelect("titanic", condiciones);

    // Guardar los resultados en un nuevo archivo
    guardarResultado("resultadoConsulta", resultado);

 */
    std::string nombreTabla;
    std::cout << "Ingrese el nombre de la tabla: ";
    std::cin >> nombreTabla;

    Tabla tabla(nombreTabla);

    std::cout << "Ingrese los nombres y tipos de datos de las columnas (nombre tipo nombre tipo ...), finalice con una palabra única (ej. 'fin'): ";
    std::string nombreColumna, tipoDato;
    while (true) {
        std::cin >> nombreColumna;
        if (nombreColumna == "fin") {
            break;
        }
        std::cin >> tipoDato;
        tabla.agregarColumna(nombreColumna, tipoDato);
    }

    return 0;

    return 0;
}
