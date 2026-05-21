/*
 * Practica 4 - Informatica 2
 * Facultad de Ingenieria - Universidad de Antioquia
 * Simulacion de red de enrutadores con POO y STL
 */

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include <climits>
#include <algorithm>

using namespace std;


// ============================================================
//  CLASE: Enlace (representa una conexion entre dos enrutadores)
// ============================================================
class Enlace {
public:
    string destino;
    int costo;

    Enlace(string d, int c) : destino(d), costo(c) {}
};

// ============================================================
//  CLASE: Enrutador
// ============================================================
class Enrutador {
public:
    string nombre;

    // Vecinos directamente conectados: nombre vecino -> costo
    map<string, int> vecinos;

    // Tabla de costos: destino -> costo minimo
    map<string, int> tablaCostos;

    // Tabla de caminos: destino -> lista de nodos del camino
    map<string, list<string>> tablaCaminos;

    Enrutador(string n) : nombre(n) {}

    // Agrega un vecino directo
    void agregarVecino(string vecino, int costo) {
        vecinos[vecino] = costo;
    }

    // Elimina un vecino
    void eliminarVecino(string vecino) {
        vecinos.erase(vecino);
    }


int main()
{
    cout << "Hello World!" << endl;
    return 0;
}
