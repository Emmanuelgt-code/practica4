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

    // Muestra la tabla de costos del enrutador
    void mostrarTabla() {
        cout << "\n  Tabla de costos del enrutador [" << nombre << "]:\n";
        cout << "  Destino  | Costo | Camino\n";
        cout << "  ---------+-------+---------------------------\n";
        for (auto& entrada : tablaCostos) {
            cout << "  " << entrada.first << "        | ";
            if (entrada.second == INT_MAX)
                cout << "INF   | (sin camino)\n";
            else {
                cout << entrada.second << "     | ";
                // Imprimir camino
                list<string>& cam = tablaCaminos[entrada.first];
                bool primero = true;
                for (string nodo : cam) {
                    if (!primero) cout << " -> ";
                    cout << nodo;
                    primero = false;
                }
                cout << "\n";
            }
        }
    }
};

// ============================================================
//  CLASE: Red
// ============================================================
class Red {
public:
    // Contenedor principal: nombre -> Enrutador
    map<string, Enrutador*> enrutadores;

    // Destructor: libera memoria
    ~Red() {
        for (auto& par : enrutadores)
            delete par.second;
    }

    // ---------- Agregar enrutador ----------
    void agregarEnrutador(string nombre) {
        if (enrutadores.count(nombre)) {
            cout << "  [!] El enrutador " << nombre << " ya existe.\n";
            return;
        }
        enrutadores[nombre] = new Enrutador(nombre);
        cout << "  [+] Enrutador " << nombre << " agregado.\n";
    }

    // ---------- Eliminar enrutador ----------
    void eliminarEnrutador(string nombre) {
        if (!enrutadores.count(nombre)) {
            cout << "  [!] El enrutador " << nombre << " no existe.\n";
            return;
        }
        // Eliminar enlaces que apuntan a este enrutador en los demas
        for (auto& par : enrutadores) {
            par.second->eliminarVecino(nombre);
        }
        delete enrutadores[nombre];
        enrutadores.erase(nombre);
        cout << "  [-] Enrutador " << nombre << " eliminado.\n";
        calcularCaminos(); // Recalcular al cambiar topologia
    }

    // ---------- Agregar enlace ----------
    void agregarEnlace(string a, string b, int costo) {
        if (!enrutadores.count(a) || !enrutadores.count(b)) {
            cout << "  [!] Uno o ambos enrutadores no existen.\n";
            return;
        }
        enrutadores[a]->agregarVecino(b, costo);
        enrutadores[b]->agregarVecino(a, costo);
        cout << "  [+] Enlace " << a << " <-> " << b << " (costo " << costo << ") agregado.\n";
        calcularCaminos(); // Recalcular al cambiar topologia
    }

    // ---------- Eliminar enlace ----------
    void eliminarEnlace(string a, string b) {
        if (!enrutadores.count(a) || !enrutadores.count(b)) {
            cout << "  [!] Uno o ambos enrutadores no existen.\n";
            return;
        }
        enrutadores[a]->eliminarVecino(b);
        enrutadores[b]->eliminarVecino(a);
        cout << "  [-] Enlace " << a << " <-> " << b << " eliminado.\n";
        calcularCaminos(); // Recalcular al cambiar topologia
    }

    // ---------- Algoritmo de Floyd-Warshall (caminos minimos) ----------
    void calcularCaminos() {
        if (enrutadores.empty()) return;

        // Recolectar todos los nombres en un vector para indexar
        vector<string> nodos;
        for (auto& par : enrutadores)
            nodos.push_back(par.first);

        int n = nodos.size();

        // Matrices de distancia y "siguiente nodo"
        // Usamos indices para facilitar Floyd-Warshall
        vector<vector<int>> dist(n, vector<int>(n, INT_MAX));
        vector<vector<int>> siguiente(n, vector<int>(n, -1));

        // Encontrar indice de un nodo
        auto idx = [&](string nombre) -> int {
            for (int i = 0; i < n; i++)
                if (nodos[i] == nombre) return i;
            return -1;
        };

        // Inicializar diagonal en 0
        for (int i = 0; i < n; i++) {
            dist[i][i] = 0;
            siguiente[i][i] = i;
        }

        // Cargar costos directos
        for (auto& par : enrutadores) {
            int i = idx(par.first);
            for (auto& vecino : par.second->vecinos) {
                int j = idx(vecino.first);
                dist[i][j] = vecino.second;
                siguiente[i][j] = j;
            }
        }

        // Floyd-Warshall
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX) {
                        if (dist[i][k] + dist[k][j] < dist[i][j]) {
                            dist[i][j] = dist[i][k] + dist[k][j];
                            siguiente[i][j] = siguiente[i][k];
                        }
                    }
                }
            }
        }

        // Guardar resultados en cada enrutador
        for (int i = 0; i < n; i++) {
            Enrutador* r = enrutadores[nodos[i]];
            r->tablaCostos.clear();
            r->tablaCaminos.clear();

            for (int j = 0; j < n; j++) {
                string destino = nodos[j];
                r->tablaCostos[destino] = dist[i][j];

                // Reconstruir camino
                list<string> camino;
                if (dist[i][j] == INT_MAX) {
                    // Sin camino
                } else {
                    int actual = i;
                    camino.push_back(nodos[actual]);
                    while (actual != j) {
                        actual = siguiente[actual][j];
                        camino.push_back(nodos[actual]);
                    }
                }
                r->tablaCaminos[destino] = camino;
            }
        }
    }

    // ---------- Mostrar toda la red ----------
    void mostrarRed() {
        cout << "\n  === Topologia de la red ===\n";
        if (enrutadores.empty()) {
            cout << "  (La red esta vacia)\n";
            return;
        }
        for (auto& par : enrutadores) {
            cout << "  Enrutador [" << par.first << "] - Vecinos: ";
            if (par.second->vecinos.empty()) {
                cout << "(ninguno)";
            } else {
                for (auto& v : par.second->vecinos)
                    cout << v.first << "(costo " << v.second << ")  ";
            }
            cout << "\n";
        }
    }

    // ---------- Mostrar todas las tablas ----------
    void mostrarTodasLasTablas() {
        calcularCaminos();
        for (auto& par : enrutadores)
            par.second->mostrarTabla();
    }

    // ---------- Encontrar camino entre dos nodos ----------
    void encontrarCamino(string origen, string destino) {
        if (!enrutadores.count(origen) || !enrutadores.count(destino)) {
            cout << "  [!] Uno o ambos enrutadores no existen.\n";
            return;
        }
        calcularCaminos();
        Enrutador* r = enrutadores[origen];
        int costo = r->tablaCostos[destino];
        list<string>& camino = r->tablaCaminos[destino];

        cout << "\n  Camino de " << origen << " a " << destino << ":\n";
        if (costo == INT_MAX || camino.empty()) {
            cout << "  (No existe camino)\n";
        } else {
            cout << "  Ruta: ";
            bool primero = true;
            for (string nodo : camino) {
                if (!primero) cout << " -> ";
                cout << nodo;
                primero = false;
            }
            cout << "\n  Costo total: " << costo << "\n";
        }
    }

    // ---------- Cargar desde archivo ----------
    // Formato del archivo:
    //   enrutador A
    //   enrutador B
    //   enlace A B 4
    void cargarDesdeArchivo(string nombreArchivo) {
        ifstream archivo(nombreArchivo);
        if (!archivo.is_open()) {
            cout << "  [!] No se pudo abrir el archivo: " << nombreArchivo << "\n";
            return;
        }

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty() || linea[0] == '#') continue; // comentarios
            istringstream ss(linea);
            string tipo;
            ss >> tipo;

            if (tipo == "enrutador") {
                string nombre;
                ss >> nombre;
                agregarEnrutador(nombre);
            } else if (tipo == "enlace") {
                string a, b;
                int costo;
                ss >> a >> b >> costo;
                agregarEnlace(a, b, costo);
            }
        }
        archivo.close();
        cout << "  [+] Red cargada desde archivo exitosamente.\n";
        calcularCaminos();
    }

    // ---------- Cargar la red del ejemplo de la practica ----------
    void cargarEjemploPractica() {
        // Limpiar red actual
        for (auto& par : enrutadores) delete par.second;
        enrutadores.clear();

        agregarEnrutador("A");
        agregarEnrutador("B");
        agregarEnrutador("C");
        agregarEnrutador("D");
        agregarEnlace("A", "B", 4);
        agregarEnlace("A", "C", 10);
        agregarEnlace("A", "D", 5);
        agregarEnlace("B", "C", 3);
        agregarEnlace("B", "D", 1);
        agregarEnlace("C", "D", 2);
        cout << "  [+] Red de ejemplo de la practica cargada.\n";
    }
};

// ============================================================
//  FUNCION: Crear archivo de topologia de ejemplo
// ============================================================
void crearArchivoEjemplo() {
    ofstream archivo("topologia.txt");
    archivo << "# Archivo de topologia de red\n";
    archivo << "# Formato:\n";
    archivo << "#   enrutador <nombre>\n";
    archivo << "#   enlace <A> <B> <costo>\n\n";
    archivo << "enrutador A\n";
    archivo << "enrutador B\n";
    archivo << "enrutador C\n";
    archivo << "enrutador D\n";
    archivo << "enlace A B 4\n";
    archivo << "enlace A C 10\n";
    archivo << "enlace A D 5\n";
    archivo << "enlace B C 3\n";
    archivo << "enlace B D 1\n";
    archivo << "enlace C D 2\n";
    archivo.close();
    cout << "  [+] Archivo 'topologia.txt' creado con el ejemplo de la practica.\n";
}

// ============================================================
//  MENU PRINCIPAL
// ============================================================
int main() {
    Red red;
    int opcion;

    cout << "\n========================================\n";
    cout << "  Practica 4 - Red de Enrutadores\n";
    cout << "  Informatica 2 - UdeA\n";
    cout << "========================================\n";

    do {
        cout << "\n--- MENU PRINCIPAL ---\n";
        cout << "1. Cargar red de ejemplo (Figura 1 de la practica)\n";
        cout << "2. Cargar red desde archivo\n";
        cout << "3. Agregar enrutador\n";
        cout << "4. Eliminar enrutador\n";
        cout << "5. Agregar enlace\n";
        cout << "6. Eliminar enlace\n";
        cout << "7. Mostrar topologia de la red\n";
        cout << "8. Mostrar tablas de enrutamiento (todos)\n";
        cout << "9. Encontrar camino entre dos enrutadores\n";
        cout << "10. Crear archivo de topologia de ejemplo\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;

        switch (opcion) {

        case 1: {
            red.cargarEjemploPractica();
            break;
        }

        case 2: {
            string nombreArchivo;
            cout << "  Nombre del archivo: ";
            cin >> nombreArchivo;
            red.cargarDesdeArchivo(nombreArchivo);
            break;
        }

        case 3: {
            string nombre;
            cout << "  Nombre del enrutador: ";
            cin >> nombre;
            red.agregarEnrutador(nombre);
            break;
        }

        case 4: {
            string nombre;
            cout << "  Nombre del enrutador a eliminar: ";
            cin >> nombre;
            red.eliminarEnrutador(nombre);
            break;
        }

        case 5: {
            string a, b;
            int costo;
            cout << "  Enrutador A: ";
            cin >> a;
            cout << "  Enrutador B: ";
            cin >> b;
            cout << "  Costo del enlace: ";
            cin >> costo;
            red.agregarEnlace(a, b, costo);
            break;
        }

        case 6: {
            string a, b;
            cout << "  Enrutador A: ";
            cin >> a;
            cout << "  Enrutador B: ";
            cin >> b;
            red.eliminarEnlace(a, b);
            break;
        }

        case 7: {
            red.mostrarRed();
            break;
        }

        case 8: {
            red.mostrarTodasLasTablas();
            break;
        }

        case 9: {
            string origen, destino;
            cout << "  Enrutador origen: ";
            cin >> origen;
            cout << "  Enrutador destino: ";
            cin >> destino;
            red.encontrarCamino(origen, destino);
            break;
        }

        case 10: {
            crearArchivoEjemplo();
            break;
        }

        case 0: {
            cout << "\n  Hasta luego!\n";
            break;
        }

        default: {
            cout << " Opcion invalida. Intente de nuevo.\n";
            break;
        }
        }

    } while (opcion != 0);

    return 0;
}
