#include "buscador.h"

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
}

Buscador::Buscador(const Buscador &p):IndexadorHash(p){
    this->docsOrdenados = p.docsOrdenados;
    this->formSimilitud = p.formSimilitud;
    this->c = p.c;
    this->k1 = p.k1;
    this->b = p.b;
}

Buscador& Buscador::operator=(const Buscador& p){
    this->docsOrdenados = p.docsOrdenados;
    this->formSimilitud = p.formSimilitud;
    this->c = p.c;
    this->k1 = p.k1;
    this->b = p.b;
    return (*this);
}

// TODO
bool Buscador::Buscar(const int& numDocumentos){
    if(indicePregunta.empty()){ // No hay ninguna pregunta indexada con terminos validos
        return false;
    }
    InformacionPregunta infPreg;
    DevuelvePregunta(infPreg);
    docsOrdenados.clear();

    map<long int, ResultadoRI> mapa;
    for(unordered_map<string, InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin(); it != indicePregunta.end(); it++){
        InformacionTermino inf;
        Devuelve(it->first, inf);
        auto l_docs = inf.getMap();

        for(auto term = l_docs.begin(); term != l_docs.end(); term++){
            double res;
            // TODO comprobar logs de negativos
            if(this->formSimilitud == 0){
                double ftd = term->second.get_ft() * log2(1 + ( (c * getMediaDocsSinparada()) /PalSinParadaDocs[term->first-1]) );
                double lambdat = (double)inf.get_ftc()/NumDocsIndexados();
                double aux = (log2(1 + lambdat) + ftd*log2((1+lambdat)/lambdat)) * ((inf.get_ftc() + 1) / (l_docs.size()*(ftd + 1)));
                res = ((double)it->second.get_ft()/infPreg.getNumTotalPalSinParada()) * aux;
            }
            else
                res = (it->second.getIDF()*term->second.get_ft()*(k1 + 1)) / (term->second.get_ft() + (k1 * (1 - b + ((b*PalSinParadaDocs[term->first-1])/getMediaDocsSinparada()))));
            
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
    for(set<ResultadoRI>::const_iterator it = docsOrdenados.begin(); it != docsOrdenados.end() && i > 0; it++){
        cout << (*it).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << getNombreFichero((*it).idDoc) << " " << (numDocumentos - i) << " " 
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
    for(set<ResultadoRI>::const_iterator it = docsOrdenados.begin(); it != docsOrdenados.end() && i > 0; it++){
        file << (*it).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << getNombreFichero((*it).idDoc) << " " << (numDocumentos - i) << " " 
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