#include "buscador.h"

/*
* ----------------------------------------------------------------------------------------------------------------
* Clase ResultadoRI
* ----------------------------------------------------------------------------------------------------------------
*/
ResultadoRI::ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np){
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
    numPregunta = np;
}

bool ResultadoRI::operator< (const ResultadoRI& lhs) const{
    if(idDoc == lhs.idDoc)
        return vSimilitud < lhs.vSimilitud;
    else
        return idDoc > lhs.idDoc;
}

ostream& operator<< (ostream& os, const ResultadoRI &res){
    os << res.vSimilitud << "\t\t" << res.idDoc << "\tt" << res.numPregunta << endl;
    return os;
}

/*
* ----------------------------------------------------------------------------------------------------------------
* Clase Buscador
* ----------------------------------------------------------------------------------------------------------------
*/
Buscador::Buscador():IndexadorHash("", "", false, false, "", 0, false, false){
    this->formSimilitud = 0;
    this->c = 2;
    this->k1 = 1.2;
    this->b = 0.25;
}

Buscador::Buscador(const string& directorioIndexacion, const int& f):IndexadorHash(directorioIndexacion){
    if(f!=0 && f!= 1)
        throw "ERROR: El parámetro f no es correcto";
    this->formSimilitud = f;
    this->c = 2;
    this->k1 = 1.2;
    this->b = 0.75;
    double res = 0;
    string::size_type pos; 
    nombreFicheroPuro.resize(NumDocsIndexados());
    PalSinParadaDocs.resize(NumDocsIndexados());
    for(auto it = indiceDocs.begin(); it != indiceDocs.end(); it++){
        pos = it->first.find_last_of('/');
        pos = pos==string::npos?0:pos + 1;
        nombreFicheroPuro[it->second.getId()-1] = (it->first.substr(pos,it->first.find('.', pos)-pos));
        PalSinParadaDocs[it->second.getId()-1] = (it->second.getNumPalNoStop());
        res += it->second.getNumPalNoStop();
    }
    MediaDocsSinParada = res / PalSinParadaDocs.size();

    ActualizaPesos();
}

void Buscador::ActualizaPesos(){
    for(unordered_map<string, InformacionTermino>::iterator it = indice.begin(); it != indice.end(); it++){
        unordered_map<long int, InfTermDoc> l_docs = it->second.getMap();
        for(unordered_map<long int, InfTermDoc>::const_iterator term = l_docs.begin(); term != l_docs.end(); term++){
            // DFR
            double ftd = (double)term->second.get_ft() * log2(1 + ( (c * MediaDocsSinParada) / (double)PalSinParadaDocs[term->first-1]) );
            double lambdat = (double)it->second.get_ftc()/(double)NumDocsIndexados();
            double DFR = (log2(1.0 + lambdat) + ftd*log2((1+lambdat)/lambdat)) * (((double)it->second.get_ftc() + 1) / ((double)l_docs.size()*(ftd + 1.0)));
            // BM25
            double BM25 = ((double)term->second.get_ft()*(k1 + 1)) / ((double)term->second.get_ft() + (k1 * (1.0 - b + (((double)b*(double)PalSinParadaDocs[term->first-1])/MediaDocsSinParada))));
            it->second.ActualizaPesos(term->first, DFR, BM25);
        }
    }
}

Buscador::Buscador(const Buscador &p):IndexadorHash((IndexadorHash)p){
    this->docsOrdenados = p.docsOrdenados;
    this->formSimilitud = p.formSimilitud;
    this->c = p.c;
    this->k1 = p.k1;
    this->b = p.b;
    this->nombreFicheroPuro = p.nombreFicheroPuro;
    this->MediaDocsSinParada = p.MediaDocsSinParada;
    this->PalSinParadaDocs = p.PalSinParadaDocs;
}

Buscador& Buscador::operator=(const Buscador& p){
    this->docsOrdenados = p.docsOrdenados;
    this->formSimilitud = p.formSimilitud;
    this->c = p.c;
    this->k1 = p.k1;
    this->b = p.b;
    this->nombreFicheroPuro = p.nombreFicheroPuro;
    this->MediaDocsSinParada = p.MediaDocsSinParada;
    this->PalSinParadaDocs = p.PalSinParadaDocs;
    return (*this);
}

bool Buscador::Buscar(const int& numDocumentos){
    NumeroDocumentosPorPregunta = numDocumentos;
    if(indicePregunta.empty()){ // No hay ninguna pregunta indexada con terminos validos
        return false;
    }
    docsOrdenados.clear();

    return BuscarInterno(numDocumentos, 0);
}

bool Buscador::BuscarInterno(const int& numDocumentos, const int& numPreg){    
    // Almacena la información de la búsqueda para que sea accesible por id
    set<ResultadoRI> docs;
    pair<bool, set<ResultadoRI>::iterator> par (false, docs.end());
    vector<pair<bool, set<ResultadoRI>::iterator>> posDocs (indiceDocs.size(), par);
    // Solo recorro los términos 'útiles' de la pregunta indexada
    for(unordered_map<string, InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin(); it != indicePregunta.end(); it++){
        double wtPreg = (double)it->second.get_ft()/infPregunta.getNumTotalPalSinParada();
        /*if((formSimilitud && it->second.getIDF() < 1) || (!formSimilitud && wtPreg < 0.1))
            continue;*/
        // Obtengo InformacionTermino (si existe) para el termino
        unordered_map<string, InformacionTermino>::const_iterator infIterator = indice.find(it->first);
        if(infIterator == indice.end())
            continue;            
        // Recorro los documentos que contienen ese termino
        unordered_map<long int, InfTermDoc> l_docs = infIterator->second.getMap();
        for(unordered_map<long int, InfTermDoc>::const_iterator term = l_docs.begin(); term != l_docs.end(); term++){
            double res;
            if(formSimilitud == 0)
                res = wtPreg * term->second.PesoDFR();
            else
                res = (double)it->second.getIDF()*term->second.PesoBM25();

            if(posDocs[term->first-1].first){
                res += (*posDocs[term->first-1].second).vSimilitud;
                docs.erase(posDocs[term->first-1].second);
            }
            posDocs[term->first-1].first = true;
            posDocs[term->first-1].second = (docs.insert(ResultadoRI(res, term->first, numPreg))).first;
        }
    }

    while(docs.size() > numDocumentos)
        docs.erase(docs.begin());
    docsOrdenados.push_back(docs);
    return true; // TODO revisar que pasa si docsOrdenados sigue siendo 0
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin){
    NumeroDocumentosPorPregunta = numDocumentos;
    ifstream fich;
    string preg;
    bool res = true;
    docsOrdenados.clear();
    for (int i = numPregInicio; i <= numPregFin; i++){
        fich.open(dirPreguntas + to_string(i) + ".txt");
        if(!fich){
            cerr << "ERROR: No se pudo abrir la pregunta " << dirPreguntas + to_string(i) + ".txt" << endl;
            return false;
        }
        getline(fich, preg);
        fich.close();
        IndexarPregunta(preg);
        res = BuscarInterno(numDocumentos, i) && res;
    }
    return res;
}

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const{
    stringstream res;
    int i;
    for(auto it = docsOrdenados.begin(); it != docsOrdenados.end(); it++){
        i = 0;
        for(auto it2 = (*it).rbegin(); it2 != (*it).rend()&& i < numDocumentos; it2++, i++){
            res << (*it2).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << nombreFicheroPuro[(*it2).idDoc - 1] << " " 
            << i << " " << (*it2).vSimilitud << " " << ((((*it2).numPregunta)==0)?pregunta:"ConjuntoDePreguntas") << "\n";
        }
    }
    cout << res.str();
}

bool Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, const string& nomFichero) const{
    ofstream file;
    file.open(nomFichero);
    if(!file){
        cerr << "ERROR: No se pudo abrir el archivo nomFichero" << endl;
        return false;
    }
    stringstream res;
    int i;
    for(auto it = docsOrdenados.begin(); it != docsOrdenados.end(); it++){
        i = 0;
        for(auto it2 = (*it).rbegin(); it2 != (*it).rend()&& i < numDocumentos; it2++, i++){
            res << (*it2).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << nombreFicheroPuro[(*it2).idDoc - 1] << " " 
            << i << " " << (*it2).vSimilitud << " " << ((((*it2).numPregunta)==0)?pregunta:"ConjuntoDePreguntas") << "\n";
        }
    }
    file << res.str();
    file.close();
    return true;
}

bool Buscador::CambiarFormulaSimilitud(const int& f){
    if(f==0 || f==1){
        this->formSimilitud = f;
        return true;
    }
    return false;
}

ostream& operator<<(ostream& s, const Buscador& p) {
    string preg;
    s << "Buscador: " << endl;
    if(p.DevuelvePregunta(preg))
        s << "\tPregunta indexada: " << preg << endl;
    else
        s << "\tNo hay ninguna pregunta indexada" << endl;
    s << "\tDatos del indexador: " << endl << (IndexadorHash) p;
    return s;
}