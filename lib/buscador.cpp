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

bool Buscador::Buscar(const int& numDocumentos){
/*
Devuelve true si en IndexadorHash.pregunta hay indexada una pregunta
no vacía con algún término con contenido, y si sobre esa pregunta se
finaliza la búsqueda correctamente con la fórmula de similitud indicada
en la variable privada “formSimilitud”.
Por ejemplo, devuelve falso si no finaliza la búsqueda por falta de
memoria, mostrando el mensaje de error correspondiente, e indicando el
documento y término en el que se ha quedado.
// Se guardarán los primeros “numDocumentos” documentos más relevantes
en la variable privada “docsOrdenados” en orden decreciente según la
relevancia sobre la pregunta (se vaciará previamente el contenido de
esta variable antes de realizar la búsqueda). Como número de pregunta en
“ResultadoRI.numPregunta” se almacenará el valor 0. En caso de que no se
introduzca el parámetro “numDocumentos”, entonces dicho parámetro se
inicializará a 99999)
*/
}

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
        cout << (*it).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << (*it).idDoc << " " << (numDocumentos - i) << " " 
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
        file << (*it).numPregunta << " " << (this->formSimilitud==0?"DFR":"BM25") << " " << (*it).idDoc << " " << (numDocumentos - i) << " " 
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