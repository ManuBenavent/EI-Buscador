#ifndef _INDEXADORINFORMACION_H_
#define _INDEXADORINFORMACION_H_
#include <iostream>
#include <string>
#include <list>
#include <unordered_map>
using namespace std;

class InfDoc;

class Fecha {
    friend ostream& operator<<(ostream& s, const Fecha& p);
private:
    int d;
    int m;
    int y;
    int h;
    int min;
public:
    // Clase
    Fecha();
    Fecha (int d, int m, int y, int h, int min);
    ~Fecha();
    Fecha(const Fecha &f);
    Fecha & operator=(const Fecha &);
    //Otros
    string ToFile() const;
    bool OlderThan(const Fecha &);
};

class InfTermDoc {
    friend ostream& operator<<(ostream& s, const InfTermDoc& p);
private:
    int ft;
    list<int> posTerm;
    double DFR;
    double BM25;
public:
    // Clase
    InfTermDoc (const InfTermDoc &p);
    InfTermDoc () { ft = 0; DFR = 0; BM25 = 0; }
    ~InfTermDoc ();
    InfTermDoc & operator= (const InfTermDoc &p);
    InfTermDoc & operator+ (const InfTermDoc &p);
    // Setters
    void set_ft(int x) { ft = x; }
    void setPesos(const double& dfr, const double &bm25) { DFR = dfr; BM25 = bm25; }
    // Add
    void add_ft() { this->ft++; }
    void add_posTerm(int pos) { posTerm.push_back(pos); }
    // Getters
    int get_ft() const { return ft; }
    list<int> getList() const { return posTerm; }
    double PesoDFR() const { return DFR; }
    double PesoBM25() const { return BM25; }
    // Otros
    void Vaciar() { posTerm.clear(); ft = 0;}
    string ToFile() const;
};

class InformacionTermino {
    friend ostream& operator<<(ostream& s, const InformacionTermino& p);
private:
    int ftc; 
    unordered_map<long int, InfTermDoc> l_docs;
public:
    // Clase
    InformacionTermino (const InformacionTermino &);
    InformacionTermino () { ftc = 0; }
    ~InformacionTermino ();
    InformacionTermino & operator= (const InformacionTermino &);
    InformacionTermino & operator+ (const InformacionTermino &);
    // Add
    void add_ftc() { this->ftc++; }
    void add_ftc(int x) { this->ftc+=x; }
    void add_l_docs(long int id, InfTermDoc term) { l_docs[id] = term; }
    // Setters
    void set_ftc(int x) { ftc = x; }
    // Getters
    size_t GetNumDocs() const { return l_docs.size(); }
    bool GetInfTermDoc(long int id, InfTermDoc&) const;
    unordered_map<long int, InfTermDoc> getMap() const { return l_docs; }
    int get_ftc() const { return ftc; }
    // Otros
    void update(InfDoc& infDoc, bool almacenarPosTerm);
    void Vaciar() { l_docs.clear(); ftc=0; }
    bool CompruebaDocumento(long int id) const;
    bool BorraDoc(long int id);
    string ToFile() const;
    void ActualizaPesos(const long int & id, const double& dfr, const double& bm25) { l_docs[id].setPesos(dfr, bm25); }
};


class InfDoc {
    friend ostream& operator<<(ostream& s, const InfDoc& p);
private:
    long int idDoc;
    int numPal;
    int numPalSinParada;
    int numPalDiferentes;
    int tamBytes;
    Fecha fechaModificacion;
public:
    // Clase
    InfDoc (int idDoc, int numPal, int numPalSinParada, int numPalDiferentes, int tamBytes, Fecha fecha);
    InfDoc (const InfDoc &);
    InfDoc ();
    ~InfDoc ();
    InfDoc & operator= (const InfDoc &);
    // Setters
    void setSize(int tamBytes) { this->tamBytes = tamBytes; }
    void setId(long int idDoc) { this->idDoc = idDoc;}
    void setFecha(const time_t &time);
    // Add
    void addWord() { this->numPal++; }
    void addWord(int x) { this->numPal+=x; }
    void addWordNoStop() { this->numPalSinParada++; }
    void addWordDif() { this->numPalDiferentes++; }
    // Getters
    long int getId() const { return this->idDoc; }
    int getNumPal() const { return this->numPal; }
    int getNumPalNoStop() const{ return this->numPalSinParada; }
    int getNumPalDif() const { return this->numPalDiferentes; }
    int getTamBytes() const { return this->tamBytes; }
    Fecha getFecha() const { return this->fechaModificacion; }
    // Otros
    void deleteWordDif() { this->numPalDiferentes--; }
    void ActualizaBorradoPalabra(const InformacionTermino &infTerm);
    string ToFile() const;
    
};


class InfColeccionDocs {
    friend ostream& operator<<(ostream& s, const InfColeccionDocs& p);
private:
    long int numDocs;
    long int numTotalPal;
    long int numTotalPalSinParada;
    long int numTotalPalDiferentes;
    long int tamBytes;
public:
    // Clase
    InfColeccionDocs (const InfColeccionDocs &);
    InfColeccionDocs ();
    ~InfColeccionDocs ();
    InfColeccionDocs & operator= (const InfColeccionDocs &);
    // Setters
    void setNumDocs(long int x) { numDocs = x; }
    void setNumTotalPal(long int x) { numTotalPal = x; }
    void setNumTotalPalSinParada(long int x) { numTotalPalSinParada = x; }
    void setNumTotalPalDiferentes(long int x) { numTotalPalDiferentes = x; }
    void setTamBytes(long int x) { tamBytes = x; }
    // Add
    void addNumTotalPal(long int x) { numTotalPal+=x; }
    void addNumTotalDif() { numTotalPalDiferentes++; }
    void addNumTotalNoStop() { numTotalPalSinParada++; }
    // Getters
    long int getNumDocs() const { return numDocs; }
    long int getNumTotalPal() const { return numTotalPal; }
    long int getNumTotalPalSinParada() const { return numTotalPalSinParada; }
    long int getNumTotalPalDiferentes() const { return numTotalPalDiferentes; }
    long int getTamBytes() const { return tamBytes; }
    // Otros
    void ReduceNumDocs() { numDocs--; }
    void update(const InfDoc& infDoc, size_t);
    void BorraDoc(const InfDoc &d);
    void BorraPal(long int x) { numTotalPal-=x; numTotalPalSinParada-=x; numTotalPalDiferentes--; }
    string ToFile() const;
};


class InformacionTerminoPregunta {
    friend ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p);
private:
    int ft;
    float IDF;
    list<int> posTerm;
public:
    // Clase
    InformacionTerminoPregunta (const InformacionTerminoPregunta &);
    InformacionTerminoPregunta ();
    ~InformacionTerminoPregunta ();
    InformacionTerminoPregunta & operator= (const InformacionTerminoPregunta &);
    // Add
    void add_ft() { ft++; }
    void addPosTermItem(int x) { posTerm.push_back(x); }
    // Setter
    void set_ft (int x) { ft=x; }
    void setIDF (const float & x)  {IDF = x;}
    // Getter
    int get_ft() const { return ft; }
    float getIDF() const { return IDF; }
    // Otros
    string ToFile() const;
};

class InformacionPregunta {
    friend ostream& operator<<(ostream& s, const InformacionPregunta& p);
private:
    long int numTotalPal;
    long int numTotalPalSinParada;
    long int numTotalPalDiferentes;
public:
    // Clase
    InformacionPregunta (const InformacionPregunta &);
    InformacionPregunta ();
    ~InformacionPregunta ();
    InformacionPregunta & operator= (const InformacionPregunta &);
    // Setters
    void setNumTotalPal(long int x) { numTotalPal = x; }
    void setNumTotalPalSinParada(long int x) { numTotalPalSinParada = x; }
    void setNumTotalPalDiferentes(long int x) { numTotalPalDiferentes = x; }
    // Add
    void addPal() { numTotalPal++; }
    void addPalNoStop() { numTotalPalSinParada++; }
    void addPalDif() { numTotalPalDiferentes++; }
    // Getters
    int getNumTotalPal() const { return numTotalPal; }
    int getNumTotalPalSinParada() const { return numTotalPalSinParada; }
    // Otros
    string ToFile() const;
};
#endif