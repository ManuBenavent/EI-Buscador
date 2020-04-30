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
    decreciente según la relevancia sobre la pregunta.*/
    priority_queue< ResultadoRI > docsOrdenados;
    //set<ResultadoRI> docsOrdenados;
    // 0: DFR, 1: BM25
    int formSimilitud;
    // Constante del modelo DFR
    double c;
    // Constante modelo BM25
    double k1;
    // Constante modelo BM25
    double b;
public:
    Buscador(const string& directorioIndexacion, const int& f);

    Buscador(const Buscador &);

    ~Buscador(){}

    Buscador& operator=(const Buscador&);

    // TODO
    bool Buscar(const int& numDocumentos = 99999);

    // TODO
    bool Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio);

    // TODO
    void ImprimirResultadoBusqueda(const int& numDocumentos = 99999) const;

    // TODO
    bool ImprimirResultadoBusqueda(const int& numDocumentos, const string& nomFichero) const;

    int DevolverFormulaSimilitud() { return formSimilitud; }

    bool CambiarFormulaSimilitud(const int& f);

    void CambiarParametrosDFR(const double& kc) { c=kc; }

    double DevolverParametrosDFR() const { return c; }

    void CambiarParametrosBM35(const double& kk1, const double& kb) { k1 = kk1; b = kb; }

    void DevolverParametrosBM35(double& kk1, double& kb) const { kk1 = k1; kb = b; }
};

#endif