#ifndef _INDEXADORHASH_H_
#define _INDEXADORHASH_H_
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include "indexadorInformacion.h"
#include "tokenizador.h"
#include "stemmer.h"
using namespace std;

typedef struct {
    int ftc;
    streampos inicio;
    size_t numDocs;
} InformacionTerminoBinario;

typedef struct {
    long int idDoc;
    int ft;
    int numPosTerm;
    streampos posLista;
} InfTermDocBinario;

class IndexadorHash {
    friend ostream& operator<<(ostream& s, const IndexadorHash& p);
private:
    IndexadorHash();

    unordered_map<string, InformacionTermino> indice;

    unordered_map<string, InformacionTermino> indiceUpdates;

    unordered_map<string, streampos> indiceMinimo;

    unordered_map<string, InfDoc> indiceDocs;

    unordered_map<string, streampos> indiceDocsMinimo;

    InfColeccionDocs informacionColeccionDocs;

    string pregunta;

    

    InformacionPregunta infPregunta;

    unordered_set<string> stopWords;

    string ficheroStopWords;
    
    Tokenizador tok;
    
    string directorioIndice;
    
    int tipoStemmer;
    
    bool almacenarEnDisco;
    
    bool almacenarPosTerm;

    void GuardarInfDocEnDisco(const InfDoc &infdoc, bool update, const string& );

    void GuardarIndiceEnDisco();

    void DesfragmentarArchivos();

    void DevuelveDoc(streampos, InfDoc &infDoc) const;

    // Nuevo

    vector<long int> PalSinParadaDocs;

    double MediaDocsSinParada;

    vector<string> nombreFicheroPuro;

public:   

    string getNombreFichero(int x) const {return nombreFicheroPuro[x-1];}

    double getMediaDocsSinparada() const { return MediaDocsSinParada; }

    long int getDocSinParada(int x) const { return PalSinParadaDocs[x]; }

    unordered_map<string, InformacionTerminoPregunta> indicePregunta; // TODO devolver a parte privada

    IndexadorHash(const string& ficheroStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, 
                const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm);
    
    IndexadorHash(const string& directorioIndexacion);

    IndexadorHash(const IndexadorHash&);

    ~IndexadorHash();

    IndexadorHash& operator= (const IndexadorHash&);

    bool Indexar(const string& ficheroDocumentos);

    bool IndexarDirectorio(const string& dirAIndexar);
    
    bool GuardarIndexacion() const;

    bool RecuperarIndexacion (const string& directorioIndexacion);
    
    void ImprimirIndexacion() const;
    
    bool IndexarPregunta(const string& preg);

    bool DevuelvePregunta(string& preg) const;    

    bool DevuelvePregunta(string word, InformacionTerminoPregunta& inf) const;

    bool DevuelvePregunta(InformacionPregunta& inf) const;
    
    void ImprimirIndexacionPregunta() const; 

    void ImprimirPregunta() const;

    bool Devuelve(string word, InformacionTermino& inf) const;

    bool Devuelve(string word, const string& nomDoc, InfTermDoc& infTermDoc) const;

    bool Existe(string word) const;

    bool Borra(string word);

    bool BorraDoc(const string& nomDoc);

    void VaciarIndiceDocs() { almacenarEnDisco?indiceDocsMinimo.clear():indiceDocs.clear(); }

    void VaciarIndicePreg() { this->indicePregunta.clear(); }
    
    bool Actualiza(string word, const InformacionTermino& inf);
    
    bool Inserta(string word, const InformacionTermino& inf);
    
    int NumPalIndexadas() const { return almacenarEnDisco?indiceMinimo.size():indice.size(); }

    int NumDocsIndexados() const { return indiceDocs.size(); }
    
    string DevolverFichPalParada () const { return this->ficheroStopWords; }
    
    void ListarPalParada() const;
    
    int NumPalParada() const { return this->stopWords.size(); }
    
    string DevolverDelimitadores () const { return this->tok.DelimitadoresPalabra(); }
    
    bool DevolverCasosEspeciales () const { return tok.CasosEspeciales(); }
    
    bool DevolverPasarAminuscSinAcentos () const { return this->tok.PasarAminuscSinAcentos(); }
    
    bool DevolverAlmacenarPosTerm () const { return this->almacenarPosTerm; }

    string DevolverDirIndice () const { return this->directorioIndice; }

    int DevolverTipoStemming () const { return this->tipoStemmer; }
    
    bool DevolverAlmEnDisco () const { return this->almacenarEnDisco; }
    
    void ListarInfColeccDocs() const;
    
    void ListarTerminos() const;
    
    bool ListarTerminos(const string& nomDoc) const;
    
    void ListarDocs() const;
    
    bool ListarDocs(const string& nomDoc) const;
};
#endif