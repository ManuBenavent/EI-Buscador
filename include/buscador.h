#ifndef _BUSCADOR_H_
#define _BUSCADOR_H_
#include "indexadorHash.h"
#include <queue>
#include <set>
#include <map>
#include <math.h>

class ResultadoRI {
    friend class IndexadorHash;
    friend class Buscador;
    friend ostream& operator<<(ostream& s, const ResultadoRI& res);
private:
    double vSimilitud;
    long int idDoc;
    int numPregunta;
public:
    ResultadoRI(){}
    ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np);
    ResultadoRI& operator=(const ResultadoRI& p){vSimilitud = p.vSimilitud; idDoc = p.idDoc; numPregunta = p.numPregunta; return *this;}
    ResultadoRI(const ResultadoRI& p){vSimilitud = p.vSimilitud; idDoc = p.idDoc; numPregunta = p.numPregunta; }
    double Vsimilitud() const { return vSimilitud; }
    long int IdDoc() const { return idDoc; }
    bool operator< (const ResultadoRI& lhs) const;
};

class Buscador: public IndexadorHash {
    friend ostream& operator<<(ostream& s, const Buscador& p);
private:
    Buscador();
    
    //priority_queue< ResultadoRI > docsOrdenados;
    set<ResultadoRI> docsOrdenados;

    // 0: DFR, 1: BM25
    int formSimilitud;

    // Constante del modelo DFR
    double c;

    // Constante modelo BM25
    double k1;

    // Constante modelo BM25
    double b;
    
    //Valores para agilizar el acceso a ciertos campos

    //Num pal sin parada de un documento (se accede por id doc - 1)
    vector<long int> PalSinParadaDocs;

    // Media de todos los valores anteriores
    double MediaDocsSinParada;

    // Nombres de los ficheros ajustandose a la salida correcta, acceso mediante id - 1
    vector<string> nombreFicheroPuro;

    // Actualiza los valores del almacenados para DFR y BM25
    void ActualizaPesos();

    bool BuscarInterno(const int& numDocumentos, const int& numPreg);

    int NumeroDocumentosPorPregunta;
public:
    Buscador(const string& directorioIndexacion, const int& f);

    Buscador(const Buscador &);

    ~Buscador(){}

    Buscador& operator=(const Buscador&);

    // TODO
    bool Buscar(const int& numDocumentos = 99999);

    // TODO
    bool Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin);

    void ImprimirResultadoBusqueda(const int& numDocumentos = 99999) const;

    bool ImprimirResultadoBusqueda(const int& numDocumentos, const string& nomFichero) const;

    int DevolverFormulaSimilitud() { return formSimilitud; }

    bool CambiarFormulaSimilitud(const int& f);

    void CambiarParametrosDFR(const double& kc) { c=kc; }

    double DevolverParametrosDFR() const { return c; }

    void CambiarParametrosBM35(const double& kk1, const double& kb) { k1 = kk1; b = kb; ActualizaPesos(); }

    void DevolverParametrosBM25(double& kk1, double& kb) { kk1 = k1; kb = b; ActualizaPesos(); }
};

#endif