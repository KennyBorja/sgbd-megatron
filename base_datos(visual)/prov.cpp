#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

class Tabla {
private:
    string nombre;
    string nombreColumna;
    string tipoDato;
    

    bool continua = true;

public:
    Tabla(const std::string& nombreTabla) : nombre(nombreTabla) {}
    

    void setColumna(const string& nombreColumna) {
        this->nombreColumna = nombreColumna;
    }

    void settipoDato(const string& tipoDato) {
        this->tipoDato = tipoDato;
    }

    string getColumna() const {
        return nombreColumna;
    }

    string getTipoDato() const {
        return tipoDato;
    }

    void guardaparametros() {

        std::ofstream archivo("esquemas.txt", std::ios::app); // Abre en modo append

        if (archivo.is_open()) {
            // Crea una cadena para almacenar los datos de la columna
            std::string datosColumna;
            datosColumna = nombre;

            while (continua == 1) {
                string nColumna, tDato;

                cout << "Ingrese el nombre de la columna: ";
                cin >> nColumna;

                cout << "Ingrese el tipo de dato de la columna: ";
                cin >> tDato;

                // Concatenar los datos de la columna en una cadena
                datosColumna +=  "#" + nColumna + "#" + tDato;

                cout << "¿Desea agregar otra columna? (1 = sí, 0 = no): ";
                cin >> continua;

                while (cin.fail() || (continua != 0 && continua != 1)) {
                    cout << "Por favor, ingrese 1 si desea continuar o 0 si desea salir: ";
                    cin.clear(); // Restablecer el estado de error de cin
                   cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpiar el búfer de entrada
                    cin >> continua;
                }

                // Validación de entrada para continua
                
            }

            // Escribir la cadena completa en el archivo
            archivo << datosColumna << std::endl;

            archivo.close();
        } else {
            std::cerr << "Error al abrir el archivo 'esquema.txt'" << std::endl;
        }
    }

   string determinarTipoDato(const string& valor) {
    stringstream ss(valor);
    char c;
    int numInt;
    double numDouble;
    
    // Intenta convertir el valor a diferentes tipos de datos
    if ((ss >> numInt) && ss.eof()) {
        return "int"; // Si se puede convertir a int, es un número entero
    } else if ((ss.clear(), ss.seekg(0), ss >> numDouble) && ss.eof()) {
        return "double"; // Si se puede convertir a double, es un número de punto flotante
    } else if ((ss.clear(), ss.seekg(0), ss >> c) && ss.eof()||valor.size() == 1) {
        return "char"; // Si se puede convertir a char, es un carácter
    } else {
        return "string"; // Si no se puede convertir a ninguno de los anteriores, es una cadena
    }
}

  void guardaparametrosdesdearchivo() {
    string resultado = nombre.substr(0, nombre.size() - 4);
    resultado += "#";

    ifstream archivo(nombre);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo '" << nombre << "'." << endl;
        return; // Devolver un string vacío si no se puede abrir el archivo
    }

    // Leer la primera línea para obtener los nombres de las columnas
    string encabezadosLinea;
    getline(archivo, encabezadosLinea);
    istringstream ssEncabezados(encabezadosLinea);
    string encabezado;

    // Leer la segunda línea para obtener los valores de cada columna
    string datosLinea;
    getline(archivo, datosLinea);
    istringstream ssDatos(datosLinea);
    string dato;
    bool primerElemento = true; // Variable para controlar si es el primer elemento
    while (getline(ssEncabezados, encabezado, ',') && getline(ssDatos, dato, ',')) {
        // Si el dato comienza con comillas dobles
        if (dato.front() == '"') {
            string valorCompleto = dato;
            // Concatenar los siguientes datos hasta encontrar la siguiente comilla doble
            while (dato.back() != '"') {
                getline(ssDatos, dato, ',');
                valorCompleto += "," + dato;
            }
            // Concatenar el nombre de la columna con el tipo de dato para la cadena completa
            resultado += (primerElemento ? "" : "#") + encabezado + "#string";
        } else {
            // Concatenar el nombre de la columna con el tipo de dato del valor correspondiente
            resultado += (primerElemento ? "" : "#") + encabezado + "#" + determinarTipoDato(dato);
        }
        primerElemento = false; // Ya no es el primer elemento después del primer par
    }

    archivo.close();

    ofstream archivoSalida("esquemas.txt", ios::app);

    archivoSalida<<resultado<< '\n';

    archivoSalida.close();
}


void guardarDesdeArchivo() {
    
    string nombreEsq = nombre.substr(0, nombre.size() - 4);
    string fila = extraerEsquema(nombreEsq);
    if (fila.empty()) {

        cout<<"esquema '"<< nombreEsq <<" no encontrado."<<endl;
        return;
    } 

    ofstream archivoSalida(nombreEsq+".txt", ios::app);
    ifstream archivo(nombre);
    string linea;
     
    getline(archivo, linea);

    while (getline(archivo, linea)) {
        istringstream ss(linea);
        string campo;
        bool primerCampo = true; 
        
        while (getline(ss, campo, ',')) {
            
            if (campo.front() == '"') {
                string campoCompleto = campo;
                while (campo.back() != '"' && getline(ss, campo, ',')) {
                    campoCompleto += "," + campo;
                }
                archivoSalida << (primerCampo ? "" : "#") << campoCompleto;
            } else {
                archivoSalida << (primerCampo ? "" : "#") << campo;
            }
            primerCampo = false;
        }

        
        archivoSalida << endl;
    }

    archivo.close();
    archivoSalida.close();

    cout << "Archivo guardado exitosamente como '" << nombre << "'." << endl;
    
     
    }


    string extraerEsquema(const string& nombreTabla) {
    string fila;
    ifstream archivoExistente("esquemas.txt");

    if (!archivoExistente.is_open()) {
        cerr << "Error al abrir el archivo 'esquemas.txt'." << endl;
        return fila;
    }

    string linea;
    while (getline(archivoExistente, linea)) {
        size_t indice = linea.find('#');
        if (indice != string::npos) {
            string nombreTablaExistente = linea.substr(0, indice);
            if (nombreTablaExistente == nombreTabla) {
                // Extraer los tipos de datos de la línea
                stringstream ss(linea.substr(indice + 1)); // Saltar el nombre de la tabla
                getline(ss, fila); // Leer toda la línea desde el stringstream
                break; // Salir del bucle una vez que se ha encontrado la línea
            }
        }
    }

    archivoExistente.close();
    return fila;
}


   void llenarDatos() {
    string fila = extraerEsquema(nombre);

    if (fila.empty()) {
        cout << "El esquema '" << nombre << "' no fue encontrado." << endl;
    } else {
        ofstream archivo(nombre + ".txt", ios::app);
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo" << nombre << ".txt" << endl;
            return;
        } else {
            istringstream ss(fila);
            string parte;
            string guardar;
            string dato;
            bool primeraColumna = true; 

            while (getline(ss, parte, '#')) {
                
                if (!primeraColumna) {
                    guardar += "#";
                } else {
                    primeraColumna = false; 
                }

                setColumna(parte);

                getline(ss, parte, '#');
                settipoDato(parte);

                cout << "Ingrese el nombre de la columna: ";
                cin >> dato;
                guardar += dato;
            }

            archivo << guardar << endl;
        }
        archivo.close();
    }
}


void mostrarTabla() {
    ifstream archivo(nombre + ".txt");
    string esquema=extraerEsquema(nombre);


    if (esquema.empty()) {
        cout << "El archivo '" << nombre << ".txt' no se pudo abrir." << endl;
        cout<< esquema << endl;
        return;
    } 
    else {
        

        string token;
       
        size_t pos = 0;
         int contador = 0;

        while ((pos = esquema.find('#', pos)) != string::npos) {
            if (contador % 2 == 0) {
                token = esquema.substr(0, pos);
                cout << token << "\t"; 
                
            }
            esquema.erase(0, pos+1); 
            contador++; 
            pos=0; 
        }
        cout<<endl;
       

        string linea;
        while (getline(archivo, linea)) {
            
            for (size_t i = 0; i < linea.size(); ++i) {
                if (linea[i] == '#') {
                    linea[i] = '\t'; 
                }
            }
            cout << linea << endl;
        }
    }
    archivo.close();
}


void consultaSelect() {
    string resultado;
    string condiciones;

    string esquema = extraerEsquema(nombre);
    string copiaesquema=esquema;

    if (esquema.empty()) {
        cout << "El archivo '" << nombre << ".txt' no se pudo abrir." << endl;
        return;
    }


    size_t pos = 0;
    int contador = 0;
    string token;
    cout<<"ingrese la condicion"<<endl;
    string condicion;
    cin>>condicion;

    size_t posicionOperador = condicion.find_first_of("=<>");
    string columna = condicion.substr(0, posicionOperador);
    string operador = condicion.substr(posicionOperador, 1);
    string valor = condicion.substr(posicionOperador + 1);


    bool encontrado = false;

    while ((pos = esquema.find('#', pos)) != string::npos) {
        
        token = esquema.substr(0, pos);
        cout << token << "\t"; 
        esquema.erase(0, pos+1); 
        contador++; 
        pos=0; 
        if (token == columna) {
            encontrado=true;
        
            break;
        }

    }

    if(esquema==columna){
        contador++;
        encontrado=true;
        cout<<"ultimontrado";
    }
    
    if (encontrado==false){
        cout<<"no se encontro";
        return;
    }
       

    contador=(contador+1)/2;
    cout<<contador;
    

    string nombreArchivo = nombre + ".txt";
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo '" << nombreArchivo << "'." << endl;
        return;
    }

    string linea;
    string token2;
    ofstream archivo2(columna + ".txt", ios::app);
    bool Cumple = false;
    while (getline(archivo, linea)) {
         size_t poss = 0;
        string verificar=linea;

        for(int i = 0; i < contador; i++) {
            poss = verificar.find('#', poss);
            token2 = verificar.substr(0, poss);
            verificar.erase(0, poss+1);
            poss=0;
        }

        cout<<token2<<endl;
       
        bool cumpleCondiciones = true;
        
            

            if (operador == "=") {
                if (token2 != valor) {
                    cumpleCondiciones = false;
                    
                   
                }
            } else if (operador == "<") {
                if (token2 >= valor) {
                    cumpleCondiciones = false;
                    
                }
            } else if (operador == ">") {
                if (token2 <= valor) {
                    cumpleCondiciones = false;
                    
                }
            }
        
        

        if (cumpleCondiciones) {
            archivo2 << linea << endl;
            Cumple = true;

            
        } 
    }

    if (Cumple == true) {

        
        bool esta=verificaresquema(columna);
        if(esta==false){
            ofstream archivo3("esquemas.txt", ios::app);
            archivo3 << columna<<'#' <<copiaesquema <<endl;
        }
    }

    archivo.close();
    
}

bool verificaresquema(string esquema) {
    ifstream archivo("esquemas.txt");
    string linea;
    size_t pos = 0;
    string token;
    while (getline(archivo, linea)) {
        
            pos = linea.find('#', pos); 
            token = linea.substr(0, pos);
            pos = 0;
            cout << endl;
            if (token == esquema) {
                return true;
            }
    }
    return false;
}




};



int main() {
    
    

    // Menú de ejecución
    int opcion;
    do {
        cout << "\n--- Menú ---" << endl;
        cout << "1. Agregar esquema" << endl;
        cout << "2. agregar datos por teclado a esquema" << endl;
        cout << "3. agregar datos desde archivo" << endl;
        cout << "4. Mostrar tabla" << endl;
        cout << "5. Buscar segun y guardar en archivo" << endl;
        cout << "6. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1:{
                system("cls");
                string nombreTabla;
                cout << "Ingrese el nombre de la tabla: ";
                cin >> nombreTabla;
                Tabla tabla(nombreTabla); // Declaración dentro del case
                tabla.guardaparametros();
                break;
            }
            case 2:{
                
                string nombreTabla;
                cout << "Ingrese el nombre de la tabla: ";
                cin >> nombreTabla;
                Tabla tabla(nombreTabla); // Declaración dentro del case
                tabla.llenarDatos();
                

                break;
            }
            case 3:{
                cout<<"Ingrese el nombre de la tabla: ";
                int opc;
    
                do {
                        cout << "Selecciona una opción:" << endl;
                        cout << "1. llenar esquema" << endl;
                        cout << "2. llenar desde csv" << endl;
                        cout << "3. Salir" << endl;
                        cin >> opc;
                        
                        switch (opc) {
                            case 1:{
                                string nombreTabla;
                                cout << "Ingrese el nombre de la tabla: ";
                                cin >> nombreTabla;
                                Tabla tabla(nombreTabla);
                                tabla.guardaparametrosdesdearchivo();
                                break;
                             }
                            case 2:
                            {
                                string nombreTabla;
                                cout << "Ingrese el nombre de la tabla: ";
                                cin >> nombreTabla;
                                Tabla tabla(nombreTabla);
                                tabla.guardarDesdeArchivo();
                                break;
                            }
                                
                            case 3:
                                cout << "Saliendo del programa..." << endl;
                                break;
                            default:
                                cout << "Opción inválida. Intenta de nuevo." << endl;
                                break;
                        }
                    } while (opc != 3);



                break;
            }
               
            case 4:{
                system("cls");
                string nombreTabla;
                cout << "Ingrese el nombre de la tabla: ";
                cin >> nombreTabla;
                Tabla tabla(nombreTabla);
                tabla.mostrarTabla();
                break;
            }
                

            case 5:{
                system("cls");
                string nombreTabla;
                cout << "Ingrese el nombre de la tabla: ";
                cin >> nombreTabla;
                Tabla tabla(nombreTabla);
                tabla.consultaSelect();
                break;
            }
               
               
            case 6:
                std::cout << "\nSaliendo del programa..." << std::endl;
                break;

            default:
                std::cout << "\nOpción inválida. Intente nuevamente." << std::endl;
        }
    } while (opcion != 6);

    return 0;
}
