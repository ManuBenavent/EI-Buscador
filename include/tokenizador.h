#ifndef _TOKENIZADOR_H_
#define _TOKENIZADOR_H_
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <string.h>

using namespace std;

class Tokenizador{

friend ostream& operator<<(ostream&, const Tokenizador&);

private:

    bool stop[256];
    char newCharValue[256];
    /* Delimitadores de términos. Aunque se modifique la forma de almacenamiento
    interna para mejorar la eficiencia, este campo debe permanecer para indicar
    el orden en que se introdujeron los delimitadores*/
    string delimiters;

    /* Si true detectará palabras compuestas y casos especiales. Sino, trabajará al igual
    que el algoritmo propuesto en la sección “Versión del tokenizador vista en clase”*/
    bool casosEspeciales;

    // Si true pasará el token a minúsculas y quitará acentos, antes de realizar la tokenización
    bool pasarAminuscSinAcentos;

    // Elimina los delimiters que estén en str y devuelve la nueva cadena en una string (no modifica atributos)
    size_t EliminaDelimiters(const string& str, const size_t& inicio, const string& del) const;

    // Comprueba si en la siguiente parte a comprobar de la linea hay una URL
    bool CheckURL(const string& url, string::size_type& pos, const string::size_type& lastPos) const;

    bool CheckDecimal(const string& decimal, string::size_type& pos, string::size_type& lastPos, bool& initial_zero) const;

    bool CheckEmail(const string& email, string::size_type& pos) const;

    bool CheckAcronimoMultipalabra(const string& str, string::size_type& pos, const string& delimiter) const;

    size_t FindFirstNoDelimiter(const string& str, const size_t& inicio) const;
    
    size_t FindFirstDelimiter(const string& str, const size_t& inicio) const;

    void set_up_newValue();

    void TokenizarInterno(string& , string&) const;

public:
    // Constructor
    Tokenizador(const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos);

    // Constructor copia
    Tokenizador(const Tokenizador&);

    // Constructor vacío
    Tokenizador();

    // Destructor
    ~Tokenizador();

    // Operador asignación
    Tokenizador& operator= (const Tokenizador&);

    // Tokeniza str, resultado en tokens
    void Tokenizar(string , list<string>&) const;

    // Tokeniza fichero i, resultado en fichero f
    bool Tokenizar(const string&, const string&) const;

    // Tokeniza fichero y resultado en fichero.tk
    bool Tokenizar(const string &) const;

    // Tokeniza ficheros nombres en fichero pasado como parametro
    bool TokenizarListaFicheros(const string&) const;

    // Tokeniza directorio
    bool TokenizarDirectorio(const string&) const;

    // Setter delimiters (comprueba repetidos)
    void DelimitadoresPalabra(const string& nuevoDelimiters);

    // Añade delimiters al final de los actuales si no existian
    void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters);

    // Getter delimiters
    string DelimitadoresPalabra() const { return delimiters; }

    // Setter casosEspeciales
    void CasosEspeciales (const bool& nuevoCasosEspeciales);

    // Getter casosEspeciales
    bool CasosEspeciales ()  const{ return casosEspeciales; }

    // Setter pasarAminuscSinAcentos
    void PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos) { pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos; }

    // Getter pasarAminuscSinAcentos
    bool PasarAminuscSinAcentos () const { return pasarAminuscSinAcentos; }

    // Quitar mayus y acentos
    void EliminarMayusAcentos(string& str) const;
};
#endif
