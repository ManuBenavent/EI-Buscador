#include "indexadorInformacion.h"

/**
 * Clase InformacionTermino
**/
 
InformacionTermino::InformacionTermino (const InformacionTermino &p){
    this->ftc = p.ftc;
    this->l_docs = p.l_docs;
}

InformacionTermino::~InformacionTermino (){
    this->ftc = 0;
    this->l_docs.clear();
}

InformacionTermino& InformacionTermino::operator= (const InformacionTermino &p){
    this->ftc = p.ftc;
    this->l_docs = p.l_docs;
    return *this;
}

void InformacionTermino::update(InfDoc& infDoc, bool almacenarPosTerm){
    this->ftc++;
    if(l_docs.find(infDoc.getId()) == l_docs.end()){
        infDoc.addWordDif();
        InfTermDoc infTermDoc;
        infTermDoc.add_ft();
        if(almacenarPosTerm)
            infTermDoc.add_posTerm(infDoc.getNumPal()-1);
        this->l_docs[infDoc.getId()] = infTermDoc;
    }
    else{
        this->l_docs[infDoc.getId()].add_ft();
        this->l_docs[infDoc.getId()].add_posTerm(infDoc.getNumPal()-1);
    }
}

bool InformacionTermino::CompruebaDocumento(long int id) const{
    return l_docs.find(id) != l_docs.end();
}

bool InformacionTermino::GetInfTermDoc(long int id, InfTermDoc& doc) const{
    if(CompruebaDocumento(id)){
        doc = l_docs.at(id);
        return true;
    }
    return false;
}

bool InformacionTermino::BorraDoc(long int id){
    if(l_docs.erase(id)!=0){
        this->ftc--;
        return true;
    }
    return false;
}

string InformacionTermino::ToFile() const{
    string info = to_string(ftc) + " " + to_string(l_docs.size());
    for(unordered_map<long int, InfTermDoc>::const_iterator it = l_docs.begin(); it != l_docs.end(); ++it){
        info += "\n";
        info += to_string(it->first);
        info += " ";
        info += it->second.ToFile();
    }
    return info;
}

InformacionTermino& InformacionTermino::operator+(const InformacionTermino& p){
    this->ftc += p.ftc;
    for(unordered_map<long int, InfTermDoc>::const_iterator it = p.l_docs.begin(); it != p.l_docs.end(); it++){
        if(this->l_docs.find(it->first) == this->l_docs.end())
            this->l_docs[it->first] = it->second;
        else
            this->l_docs[it->first] = this->l_docs[it->first] + it->second;
    }
    return (*this);
}

ostream& operator<<(ostream& s, const InformacionTermino& p) {
    s << "Frecuencia total: " << p.ftc << "\tfd: " << p.l_docs.size();
    for(unordered_map<long int, InfTermDoc>::const_iterator it = p.l_docs.begin(); it != p.l_docs.end(); it++)
        s << "\tId.Doc: " << it->first << "\t" << it->second;
    return s;
}

/**
 * Clase InfTermDoc
**/

InfTermDoc::InfTermDoc(const InfTermDoc &p){
    this->ft = p.ft;
    this->posTerm = p.posTerm;
    this->DFR = p.DFR;
    this->BM25 = p.BM25;
}

InfTermDoc& InfTermDoc::operator= (const InfTermDoc &p){
    this->ft = p.ft;
    this->posTerm = p.posTerm;
    this->DFR = p.DFR;
    this->BM25 = p.BM25;
    return *this;
}

InfTermDoc & InfTermDoc::operator+ (const InfTermDoc &p){
    this->ft += p.ft;
    this->DFR += p.DFR;
    this->BM25 += p.BM25;
    for(list<int>::const_iterator it = p.posTerm.begin(); it != p.posTerm.end(); it++)
        this->posTerm.push_back(*it);
    return (*this);
}

InfTermDoc::~InfTermDoc(){
    this->ft = 0;
    this->DFR = 0;
    this->BM25 = 0;
    this->posTerm.clear();
}

string InfTermDoc::ToFile() const{
    string info = to_string(ft) + " " + to_string(posTerm.size()) + " ";
    for (list<int>::const_iterator it = posTerm.begin(); it != posTerm.end(); it++){
        info += to_string(*it);
        info += " ";
    }
    return info;
}

ostream& operator<<(ostream& s, const InfTermDoc& p) {
    s << "ft: " << p.ft;
    for(list<int>::const_iterator it = p.posTerm.begin(); it != p.posTerm.end(); it++)
        s << "\t" << (*it);
    return s;
}

/**
 * Clase InfDoc
**/

InfDoc::InfDoc():fechaModificacion(){
    idDoc = 0;
    numPal = 0;
    numPalSinParada = 0;
    numPalDiferentes = 0;
    tamBytes = 0;
}

InfDoc::InfDoc (int idDoc, int numPal, int numPalSinParada, int numPalDiferentes, int tamBytes, Fecha fecha):fechaModificacion(fecha){
    this->idDoc = idDoc;
    this->numPal = numPal;
    this->numPalSinParada = numPalSinParada;
    this->numPalDiferentes = numPalDiferentes;
    this->tamBytes = tamBytes;
}

InfDoc::InfDoc(const InfDoc &doc):fechaModificacion(doc.fechaModificacion){
    idDoc = doc.idDoc;
    numPal = doc.numPal;
    numPalSinParada = doc.numPalSinParada;
    numPalDiferentes = doc.numPalDiferentes;
    tamBytes = doc.tamBytes;
}

InfDoc & InfDoc::operator=(const InfDoc &doc){
    idDoc = doc.idDoc;
    numPal = doc.numPal;
    numPalSinParada = doc.numPalSinParada;
    numPalDiferentes = doc.numPalDiferentes;
    tamBytes = doc.tamBytes;
    fechaModificacion = doc.fechaModificacion;
    return (*this);
}

InfDoc::~InfDoc(){
    idDoc = 0;
    numPal = 0;
    numPalSinParada = 0;
    numPalDiferentes = 0;
    tamBytes = 0;
}

void InfDoc::setFecha(const time_t &time){
    struct tm * timeinfo = localtime(&time);
    this->fechaModificacion = Fecha(timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
}

string InfDoc::ToFile() const{
    return to_string(idDoc) + " " + to_string(numPal) + " " + to_string(numPalSinParada) + " " + to_string(numPalDiferentes) + " " + to_string(tamBytes) + " " + fechaModificacion.ToFile();
}

void InfDoc::ActualizaBorradoPalabra(const InformacionTermino &infTerm){
    numPalDiferentes--;
    InfTermDoc infTermDoc;
    if(infTerm.GetInfTermDoc(idDoc, infTermDoc)){
        numPal-=infTermDoc.get_ft();
        numPalSinParada-=infTermDoc.get_ft();
    } 
}

ostream& operator<<(ostream& s, const InfDoc& p) {
    s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal << "\tnumPalSinParada: " << p.numPalSinParada << "\tnumPalDiferentes: " << p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

/**
 * Clase InfColeccionDocs
**/

InfColeccionDocs::InfColeccionDocs (const InfColeccionDocs &p){
    this->numDocs = p.numDocs;
    this->numTotalPal = p.numTotalPal;
    this->numTotalPalSinParada = p.numTotalPalSinParada;
    this->numTotalPalDiferentes = p.numTotalPalDiferentes;
    this->tamBytes = p.tamBytes;
}

InfColeccionDocs::InfColeccionDocs(){
    this->numDocs = 0;
    this->numTotalPal = 0;
    this->numTotalPalSinParada = 0;
    this->numTotalPalDiferentes = 0;
    this->tamBytes = 0;
}

InfColeccionDocs::~InfColeccionDocs (){
    this->numDocs = 0;
    this->numTotalPal = 0;
    this->numTotalPalSinParada = 0;
    this->numTotalPalDiferentes = 0;
    this->tamBytes = 0;
}

InfColeccionDocs & InfColeccionDocs::operator= (const InfColeccionDocs &p){
    this->numDocs = p.numDocs;
    this->numTotalPal = p.numTotalPal;
    this->numTotalPalSinParada = p.numTotalPalSinParada;
    this->numTotalPalDiferentes = p.numTotalPalDiferentes;
    this->tamBytes = p.tamBytes;
    return (*this);
}

void InfColeccionDocs::update(const InfDoc& infDoc, size_t pal_dif){
    this->numDocs++;
    this->numTotalPal += infDoc.getNumPal();
    this->numTotalPalSinParada += infDoc.getNumPalNoStop();
    this->numTotalPalDiferentes = pal_dif;
    this->tamBytes += infDoc.getTamBytes();
}

void InfColeccionDocs::BorraDoc(const InfDoc &d){
    numDocs--;
    numTotalPal -= d.getNumPal();
    numTotalPalSinParada -= d.getNumPalNoStop();
    numTotalPalDiferentes -= d.getNumPalDif();
    tamBytes -= d.getTamBytes();
}

string InfColeccionDocs::ToFile() const{
    return to_string(numDocs) + " " + to_string(numTotalPal) + " " + to_string(numTotalPalSinParada) + " " + to_string(numTotalPalDiferentes) + " " + to_string(tamBytes);
}

ostream& operator<<(ostream& s, const InfColeccionDocs& p){
    s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada 
        << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

/**
 * Clase InformacionTerminoPregunta
**/
InformacionTerminoPregunta::InformacionTerminoPregunta(){
    ft = 0;
    IDF = 0;
}

InformacionTerminoPregunta::InformacionTerminoPregunta(const InformacionTerminoPregunta &p){
    ft = p.ft;
    posTerm = p.posTerm;
    IDF = p.IDF;
}

InformacionTerminoPregunta::~InformacionTerminoPregunta(){
    ft = 0;
    IDF = 0;
}

InformacionTerminoPregunta & InformacionTerminoPregunta::operator=(const InformacionTerminoPregunta &p){
    ft = p.ft;
    posTerm = p.posTerm;
    IDF = p.IDF;
    return (*this);
}

string InformacionTerminoPregunta::ToFile() const{
    string info = to_string(ft) + " " + to_string(posTerm.size()) + " ";
    for(list<int>::const_iterator it = posTerm.begin(); it != posTerm.end(); it++){
        info += to_string(*it);
        info += " ";
    }
    return info;
}

ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p) {
    s << "ft: " << p.ft;
    for(list<int>::const_iterator it = p.posTerm.begin(); it != p.posTerm.end(); it++)
        s << "\t" << (*it);
    return s;
}

/**
 * Clase InformacionPregunta
**/

InformacionPregunta::InformacionPregunta(){
    numTotalPal = 0;
    numTotalPalSinParada = 0;
    numTotalPalDiferentes = 0;
}

InformacionPregunta::~InformacionPregunta(){
    numTotalPal = 0;
    numTotalPalSinParada = 0;
    numTotalPalDiferentes = 0;
}

InformacionPregunta::InformacionPregunta(const InformacionPregunta &p){
    numTotalPal = p.numTotalPal;
    numTotalPalSinParada = p.numTotalPalSinParada;
    numTotalPalDiferentes = p.numTotalPalDiferentes;
}

InformacionPregunta & InformacionPregunta::operator=(const InformacionPregunta &p){
    numTotalPal = p.numTotalPal;
    numTotalPalSinParada = p.numTotalPalSinParada;
    numTotalPalDiferentes = p.numTotalPalDiferentes;
    return (*this);
}

string InformacionPregunta::ToFile() const{
    return to_string(numTotalPal) + " " + to_string(numTotalPalSinParada) + " " + to_string(numTotalPalDiferentes);
}

ostream& operator<<(ostream& s, const InformacionPregunta& p){
    s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
    return s;
}


/**
 * Clase Fecha
**/

Fecha::Fecha(){
    d = 0;
    m = 0;
    y = 0;
    h = 0;
    min = 0;
}

Fecha::~Fecha(){
    d = 0;
    m = 0;
    y = 0;
    h = 0;
    min = 0;
}

Fecha::Fecha(const Fecha &f){
    this->d = f.d;
    this->m = f.m;
    this->y = f.y;
    this->h = f.h;
    this->min = f.min;
}

Fecha & Fecha::operator=(const Fecha &f){
    this->d = f.d;
    this->m = f.m;
    this->y = f.y;
    this->h = f.h;
    this->min = f.min;
    return (*this);
}

Fecha::Fecha(int d, int m, int y, int h, int min){
    this->d = d;
    this->m = m;
    this->y = y;
    this->h = h;
    this->min = min;
}

bool Fecha::OlderThan(const Fecha &nueva){
    return this->y < nueva.y || this->m < nueva.m || this->d < nueva.d || this->h < nueva.h || this->min < nueva.min;
}

string Fecha::ToFile() const{
    return to_string(d) + " " + to_string(m) + " " + to_string(y) + " " + to_string(h) + " " + to_string(min);
}

ostream& operator<<(ostream& s, const Fecha& p){
    s << p.d << "/" << p.m << "/" << p.y << "-" << p.h << ":" << p.min;
    return s;
}