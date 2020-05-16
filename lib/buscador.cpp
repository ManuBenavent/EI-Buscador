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
    for(auto it = indiceDocs.begin(); it != indiceDocs.end(); it++){
        pos = it->first.find_last_of('/');
        pos = pos==string::npos?0:pos + 1;
        nombreFicheroPuro.push_back(it->first.substr(pos,it->first.find('.')-pos));
        PalSinParadaDocs.push_back(it->second.getNumPalNoStop());
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
    if(indicePregunta.empty()){ // No hay ninguna pregunta indexada con terminos validos
        return false;
    }
    docsOrdenados.clear();
    // Almacena la información de la búsqueda para que sea accesible por id
    map<long int, ResultadoRI> mapa;
    // Solo recorro los términos 'útiles' de la pregunta indexada
    for(unordered_map<string, InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin(); it != indicePregunta.end(); it++){
        // Obtengo InformacionTermino (si existe) para el termino
        unordered_map<string, InformacionTermino>::const_iterator infIterator = indice.find(it->first);
        if(infIterator == indice.end())
            continue;

        // Recorro los documentos que contienen ese termino
        unordered_map<long int, InfTermDoc> l_docs = infIterator->second.getMap();
        for(unordered_map<long int, InfTermDoc>::const_iterator term = l_docs.begin(); term != l_docs.end(); term++){
            if(term->first == 48)
                cout << "el gorrino";
            double res;
            if(formSimilitud == 0){
                res = ((double)it->second.get_ft()/(double)infPregunta.getNumTotalPalSinParada()) * term->second.PesoDFR(); // TODO optimizar más
            }
            else
                res = (double)it->second.getIDF()*term->second.PesoBM25();
            
            // Si existe actualizo valor sino inserto nuevo par en el mapa
            map<long int, ResultadoRI>::iterator pos = mapa.find(term->first);
            if(pos != mapa.end())
                pos->second.vSimilitud+=res;
            else
                mapa[term->first] = ResultadoRI(res, term->first, 0);
        }
    }
    int i = 0;
    // TODO insertar el ultimo primero es mas eficiente?
    for(auto it = mapa.begin(); it != mapa.end() && i < numDocumentos; it++){
        docsOrdenados.insert(it->second);
        i++;
    }
    return true;
}

// TODO
bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio){
    /*
    Realizará la búsqueda entre el número de pregunta “numPregInicio” y
“numPregFin”, ambas preguntas incluidas. El corpus de preguntas estará
en el directorio “dirPreguntas”, y tendrá la estructura de cada pregunta
en un fichero independiente, de nombre el número de pregunta, y
extensión “.txt” (p.ej. 1.txt 2.txt 3.txt ... 83.txt). Esto significa
que habrá que indexar cada pregunta por separado y ejecutar una búsqueda
por cada pregunta añadiendo los resultados de cada pregunta (junto con
su número de pregunta) en la variable privada “docsOrdenados”. Asimismo,
se supone que previamente se mantendrá la indexación del corpus.
// Se guardarán los primeros “numDocumentos” documentos más relevantes
para cada pregunta en la variable privada “docsOrdenados”
// La búsqueda se realiza con la fórmula de similitud indicada en la
variable privada “formSimilitud”.
Devuelve falso si no finaliza la búsqueda (p.ej. por falta de
memoria), mostrando el mensaje de error correspondiente, indicando el
documento, pregunta y término en el que se ha quedado.
    */
}

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const{
    string pregunta="";
    this->DevuelvePregunta(pregunta);
    int i = numDocumentos;
    for(set<ResultadoRI>::reverse_iterator it = docsOrdenados.rbegin(); it != docsOrdenados.rend() && i > 0; it++){
        cout << (*it).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << nombreFicheroPuro[(*it).idDoc - 1] << " " << (numDocumentos - i) << " " 
            << (*it).vSimilitud << " " << ((((*it).numPregunta)==0)?pregunta:"ConjuntoDePreguntas") << endl;
        i--;
    }
}

bool Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, const string& nomFichero) const{
    string pregunta = "";
    this->DevuelvePregunta(pregunta);
    int i = numDocumentos;
    ofstream file;
    file.open(nomFichero.c_str());
    if(!file){
        cerr << "ERROR: No se pudo abrir el archivo nomFichero" << endl;
        return false;
    }
    for(set<ResultadoRI>::reverse_iterator it = docsOrdenados.rbegin(); it != docsOrdenados.rend() && i > 0; it++){
        file << (*it).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << nombreFicheroPuro[(*it).idDoc - 1] << " " << (numDocumentos - i) << " " 
            << (*it).vSimilitud << " " << ((((*it).numPregunta)==0)?pregunta:"ConjuntoDePreguntas") << "\n";
    }
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