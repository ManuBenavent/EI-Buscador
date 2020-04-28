#include "buscador.h"

/*
* Clase ResultadoRI
*/
ResultadoRI::ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np){
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
    numPregunta = np;
}

bool ResultadoRI::operator< (const ResultadoRI& lhs) const{
    if(numPregunta == lhs.numPregunta)
        return vSimilitud < lhs.vSimilitud;
    else
        return numPregunta > lhs.numPregunta;
}

ostream& operator<< (ostream& os, const ResultadoRI &res){
    os << res.vSimilitud << "\t\t" << res.idDoc << "\tt" << res.numPregunta << endl;
    return os;
}

/*
* Clase Buscador
*/
Buscador::Buscador(){
    formSimilitud = 0;
    c = 2;
    k1 = 1.2;
    b = 0.25;
}

/*ostream& operator<<(ostream& s, const Buscador& p) {
    string preg;
    s << “Buscador: “ << endl;
    if(DevuelvePregunta(preg))
    s << “\tPregunta indexada: “ << preg << endl;
    else
    s << “\tNo hay ninguna pregunta indexada” << endl;
    s << “\tDatos del indexador: “ << endl << (IndexadorHash) p;
    // Invoca a la sobrecarga de la salida del Indexador
    return s;
}*/