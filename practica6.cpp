#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <omp.h>

using namespace std;

//Espacios en blanco

int espaciosEnBlanco(const string& cadena){
    int count = 0;
    #pragma omp parallel for reduction(+:count) //reduction(op:list)
    for (int i = 0; i < cadena.size(); i++) {
        if (cadena[i] == ' ') {
            count++;
        }
    }
    return count;
}

//Palabras y caracteres en archivo

void contarEnArchivo(string nombre){
    ifstream archivo (nombre);
    string linea;
    int palabras = 0, caracteres = 0;

    if (!archivo) {
        cout << "No se pudo abrir el archivo.\n";
        return;
    }

    vector<string> lineas;

    while (getline(archivo, linea)) {
        lineas.push_back(linea);
    }

    #pragma omp parallel for reduction(+:palabras,caracteres)
    for (int i = 0; i < (int)lineas.size(); i++) {
        caracteres += lineas[i].size();

        stringstream ss(lineas[i]);
        string palabra;

        while (ss >> palabra)
            palabras++;
    }

    cout << "Palabras: " << palabras << endl;
    cout << "Caracteres: " << caracteres << endl;
}

//Sumar dos matrices cuadradas de números enteros

void sumarMatrices() {

    const int n = 3;

    //Read data

    int A[n][n] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    int B[n][n] = {
        {9, 8, 7},
        {6, 5, 4},
        {3, 2, 1}
    };

    int C[n][n];

    #pragma omp parallel for collapse(2) //merges the iterations of i and j (nested loops)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }

    cout << "\nMatriz A:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << A[i][j] << "\t";
        cout << endl;
    }

    cout << "\nMatriz B:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << B[i][j] << "\t";
        cout << endl;
    }

    cout << "\nMatriz resultante C = A + B:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << C[i][j] << "\t";
        cout << endl;
    }
}

//Cadenas iguales

bool cadenasIguales(const string& a, const string& b){
    if (a.size() != b.size())
        return false;
    
    bool iguales = true;

    #pragma omp parallel for reduction(&:iguales)
    for (int i = 0; i < a.size(); i++){
        if (a[i] != b[i])
            iguales = false;
    }
    return iguales;
}

//Ordenar numeros enteros (odd-even transposition sort)

void ordenarNumeros(vector<int>& v){
    int n = v.size();

    //en cada fase se comparan pares independientes, por eso se pueden repartir entre hilos
    for (int fase = 0; fase < n; fase++) {
        #pragma omp parallel for //cada hilo trabaja con pares distintos (i, i+1)
        for (int i = fase % 2; i < n - 1; i += 2) {
            if (v[i] > v[i + 1])
                swap(v[i], v[i + 1]);
        }
    }
}

//Buscar palabra en texto

bool buscarPalabra(const string& texto, const string& palabra){
    vector<string> palabras;
    stringstream ss(texto);
    string w;

    //separa el texto en palabras
    while (ss >> w)
        palabras.push_back(w);

    int coincidencias = 0;

    #pragma omp parallel for reduction(+:coincidencias)
    for (int i = 0; i < (int)palabras.size(); i++) {
        if (palabras[i] == palabra) {
            coincidencias++;
        }
    }
    return coincidencias > 0;
}

//Interseccion de dos conjuntos de enteros

vector<int> interseccionConjuntos(const vector<int>& A, const vector<int>& B){
    vector<int> resultado;

    #pragma omp parallel for
    for (int i = 0; i < (int)A.size(); i++) {
        bool encontrado = false;

        for (int j = 0; j < (int)B.size(); j++) {
            if (A[i] == B[j]) {
                encontrado = true;
                break;
            }
        }

        if (encontrado) {
            #pragma omp critical //solo un hilo a la vez agrega al resultado
            resultado.push_back(A[i]);
        }
    }

    sort(resultado.begin(), resultado.end()); //los hilos terminan en distinto orden
    return resultado;
}

//Raices reales de una ecuacion cuadratica (ax^2 + bx + c = 0)
//Regresa: -1 si no es cuadratica (a = 0), 0 si no hay raices reales, 1 si hay una raiz doble, 2 si hay dos raices

int raicesCuadratica(double a, double b, double c, double& x1, double& x2){
    if (a == 0)
        return -1;

    double discriminante = b * b - 4 * a * c;

    if (discriminante < 0)
        return 0;

    double raiz = sqrt(discriminante);

    #pragma omp parallel sections //cada raiz se calcula en una seccion distinta
    {
        #pragma omp section
        x1 = (-b + raiz) / (2 * a);

        #pragma omp section
        x2 = (-b - raiz) / (2 * a);
    }

    return (discriminante == 0) ? 1 : 2;
}

//Contar apariciones de una palabra en un texto

int contarApariciones(const string& texto, const string& palabra){
    vector<string> palabras;
    stringstream ss(texto);
    string w;

    //separa el texto en palabras
    while (ss >> w)
        palabras.push_back(w);

    int coincidencias = 0;

    #pragma omp parallel for reduction(+:coincidencias)
    for (int i = 0; i < (int)palabras.size(); i++) {
        if (palabras[i] == palabra) {
            coincidencias++;
        }
    }
    return coincidencias;
}

//Sumatoria de n numeros enteros

long long sumatoria(const vector<int>& numeros){
    long long suma = 0;

    #pragma omp parallel for reduction(+:suma)
    for (int i = 0; i < (int)numeros.size(); i++) {
        suma += numeros[i];
    }
    return suma;
}


int main() {
    int opc;

    do{

        cout << "\n===== MENU OPENMP =====\n";
        cout << "1. Contar espacios en blanco\n";
        cout << "2. Contar palabras y caracteres de archivo\n";
        cout << "3. Sumar matrices\n";
        cout << "4. Comparar cadenas\n";
        cout << "5. Ordenar numeros\n";
        cout << "6. Buscar palabra en texto\n";
        cout << "7. Interseccion de conjuntos\n";
        cout << "8. Raices de ecuacion cuadratica\n";
        cout << "9. Contar apariciones de palabra\n";
        cout << "10. Sumatoria de n numeros\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opc;

        //validate opc as an int

        switch (opc) {
        case 1:{
            string cadena;
            cin.ignore(); //clean buffer
            cout << "Ingrese el texto: ";
            getline(cin, cadena);
            cout << "Numero de espacios: " << espaciosEnBlanco(cadena) << endl;
            break;}

        case 2: {
            string nombre;
            cin.ignore();
            cout << "Ingrese el nombre del archivo: ";
            getline(cin, nombre);
            contarEnArchivo(nombre);
            break;
        }

        case 3: {
            sumarMatrices();
            break;
        }

        case 4: {
            string a, b;
            cin.ignore();
            cout << "Cadena A: ";
            getline(cin, a);
            cout << "Cadena B: ";
            getline(cin, b);

            cout << (cadenasIguales(a,b) ? "Las cadenas son iguales\n" : "Las cadenas son diferentes\n");
            break;
        }

        case 5: {
            int n;
            cout << "Cuantos numeros va a ordenar: ";
            cin >> n;
            vector<int> numeros(n);
            cout << "Ingrese los numeros: ";
            for (int i = 0; i < n; i++)
                cin >> numeros[i];

            ordenarNumeros(numeros);

            cout << "Numeros ordenados: ";
            for (int x : numeros)
                cout << x << " ";
            cout << endl;
            break;
        }
        
        case 6: {
            string texto, palabra;
            cin.ignore();
            cout << "Ingrese el texto: ";
            getline(cin, texto);
            cout << "Palabra a buscar: ";
            getline(cin, palabra);

            cout << (buscarPalabra(texto, palabra) ? "La palabra SI se encuentra en el texto\n" : "La palabra NO se encuentra en el texto\n");
            break;
        }

        case 7: {
            int n1, n2;

            cout << "Tamano del conjunto A: ";
            cin >> n1;
            vector<int> A(n1);
            cout << "Elementos de A: ";
            for (int i = 0; i < n1; i++)
                cin >> A[i];

            cout << "Tamano del conjunto B: ";
            cin >> n2;
            vector<int> B(n2);
            cout << "Elementos de B: ";
            for (int i = 0; i < n2; i++)
                cin >> B[i];

            vector<int> resultado = interseccionConjuntos(A, B);

            cout << "Interseccion: { ";
            for (int x : resultado)
                cout << x << " ";
            cout << "}\n";
            break;
        }

        case 8: {
            double a, b, c, x1, x2;
            cout << "Ecuacion ax^2 + bx + c = 0\n";
            cout << "Ingrese a: ";
            cin >> a;
            cout << "Ingrese b: ";
            cin >> b;
            cout << "Ingrese c: ";
            cin >> c;

            int tipo = raicesCuadratica(a, b, c, x1, x2);

            if (tipo == -1)
                cout << "No es una ecuacion cuadratica (a no puede ser 0)\n";
            else if (tipo == 0)
                cout << "La ecuacion no tiene raices reales\n";
            else if (tipo == 1)
                cout << "Raiz doble: x = " << x1 << endl;
            else {
                cout << "x1 = " << x1 << endl;
                cout << "x2 = " << x2 << endl;
            }
            break;
        }

        case 9: {
            string texto, palabra;
            cin.ignore();
            cout << "Ingrese el texto: ";
            getline(cin, texto);
            cout << "Palabra a contar: ";
            getline(cin, palabra);

            cout << "La palabra aparece " << contarApariciones(texto, palabra) << " veces\n";
            break;
        }

        case 10: {
            int n;
            cout << "Cuantos numeros va a sumar: ";
            cin >> n;
            vector<int> numeros(n);
            cout << "Ingrese los numeros: ";
            for (int i = 0; i < n; i++)
                cin >> numeros[i];

            cout << "La sumatoria es: " << sumatoria(numeros) << endl;
            break;
        }

        case 0:
            cout << "Programa finalizado.\n";
            break;
        
        default:
            cout << "Opcion no valida.\n";
            break;
        }

    } while(opc != 0);
    return 0;
}
