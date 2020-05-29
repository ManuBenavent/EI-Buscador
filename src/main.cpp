#include <iostream>
#include <string>
#include <sys/resource.h>
#include "buscador.h"
using namespace std;

double getcputime(void) {
    struct timeval tim;
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tim=ru.ru_utime;
    double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    tim=ru.ru_stime;
    t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    return t;
}

int main() {
    // Indexacion,;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@
    IndexadorHash b("./StopWordsIngles.txt", ",;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@", false, true, "./indicePruebaEspanyol", 2, false, false);
    b.Indexar("ficherosTimes.txt");
    b.GuardarIndexacion();
    // Creo el buscador
    Buscador a("./indicePruebaEspanyol", 0);
    // Primera consulta
    time_t inicioA, finA;
    time(&inicioA);
    double aa = getcputime();

    a.IndexarPregunta("KENNEDY ADMINISTRATION PRESSURE ON NGO DINH DIEM TO STOP SUPPRESSING THE BUDDHISTS . ");
    a.Buscar(423);
    a.ImprimirResultadoBusqueda(423, "DFR_simple.txt");

    double AA = getcputime()-aa;
    cout << "\nHa tardado " << AA << " segundos\n\n";
    
    time_t inicioB, finB;
    time(&inicioB);
    double aaB=getcputime();

    a.Buscar("./CorpusTime/Preguntas/", 423, 1, 83);
    a.ImprimirResultadoBusqueda(423, "fich_salida_buscador_alumno_DFR_Stemming.txt");
    /*a.CambiarFormulaSimilitud(1);
    a.Buscar("./CorpusTime/Preguntas/", 423, 1, 83);
    a.ImprimirResultadoBusqueda(423, "fich_salida_buscador_alumno_BM25_Stemming.txt");*/
    double bbB=getcputime()-aaB;
    cout << "\nHa tardado " << bbB << " segundos\n\n";
    return 0;
}