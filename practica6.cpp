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

