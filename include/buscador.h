#ifndef _BUSCADOR_H_
#define _BUSCADOR_H_
#include "indexadorHash.h"
#include <queue>

class ResultadoRI {
    friend ostream& operator<<(ostream& s, const ResultadoRI& res);
private:
    double vSimilitud;
    long int idDoc;
    int numPregunta;
public:
    ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np);
    double Vsimilitud() const { return vSimilitud; }
    long int IdDoc() const { return idDoc; }
    bool operator< (const ResultadoRI& lhs) const;
};

class Buscador: public IndexadorHash {
    friend ostream& operator<<(ostream& s, const Buscador& p);
private:
    Buscador();
    /*Contendrá los resultados de la última búsqueda realizada en orden
    decreciente según la relevancia sobre la pregunta. El tipo “priority_queue” 
    podrá modificarse por cuestiones de eficiencia. */
    priority_queue< ResultadoRI > docsOrdenados;
    // 0: DFR, 1: BM25
    int formSimilitud;
    // Constante del modelo DFR
    double c;
    // Constante modelo BM25
    double k1;
    // Constante modelo BM25
    double b;
public:

};

#endif