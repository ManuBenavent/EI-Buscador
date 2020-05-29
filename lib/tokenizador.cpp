#include "tokenizador.h"

void Tokenizador::set_up_newValue(){
    int pos;
    for(unsigned i=0; i<256; i++){
            pos = i;
            if(i<128){
                if(i>=65 && i<=90)
                    pos+=32;
                newCharValue[i] = (char)pos;
                continue;
            }

            if(i>=192 && i<215 || (i>=216 && i<223))
                pos += 32;

            switch(pos){
                case 224:
                case 225:
                    newCharValue[i] = 'a';
                    break;
                case 232:
                case 233:
                    newCharValue[i] = 'e';
                    break;
                case 236:
                case 237:
                    newCharValue[i] = 'i';
                    break;
                case 242:
                case 243:
                    newCharValue[i] = 'o';
                    break;
                case 249:
                case 250:
                    newCharValue[i] = 'u';
                    break;
                default:
                    newCharValue[i] = (char)pos;
            }
    }
}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){
    memset(stop, false, sizeof(stop));
    this->delimiters = "";
    int value = 0;
    for (size_t i=0; i < delimitadoresPalabra.size(); i++){
        value = (int)delimitadoresPalabra[i];
        if(value<0){
            value += 256;
        }
        if(value == 191)
            stop[191] = true;
        else if(value == 161)
            stop[161] = true;
        else if(!stop[value]){
            this->delimiters += delimitadoresPalabra[i];
            stop[value] = true;
        }
    }
    this->casosEspeciales = kcasosEspeciales;
    this->pasarAminuscSinAcentos = minuscSinAcentos;
    if(kcasosEspeciales)
        stop[(int)' '] = true;
    set_up_newValue();
}

Tokenizador::Tokenizador (const Tokenizador& t){
    this->delimiters = t.delimiters;
    this->casosEspeciales = t.casosEspeciales;
    this->pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
    memcpy(this->stop, t.stop, sizeof(t.stop));
    memcpy(this->newCharValue, t.newCharValue, sizeof(t.newCharValue));
}

Tokenizador::Tokenizador (){
    this->delimiters = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
    this->casosEspeciales = true;
    this->pasarAminuscSinAcentos = false;
    memset(stop, false, sizeof(stop));
    stop[191] = true;
    stop[161] = true;
    for(size_t i=0; i<delimiters.size(); i++){
        if((int)delimiters[i]>=0 && (int)delimiters[i]<128)
            stop[(int)delimiters[i]] = true;        
    }
    set_up_newValue();
}

Tokenizador::~Tokenizador (){
    this->delimiters="";
    memset(stop, false, sizeof(stop));
}

Tokenizador& Tokenizador::operator= (const Tokenizador& t){
    this->delimiters = t.delimiters;
    this->casosEspeciales = t.casosEspeciales;
    this->pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
    memcpy(this->stop, t.stop, sizeof(t.stop));
    memcpy(this->newCharValue, t.newCharValue, sizeof(t.newCharValue));
    return (*this);
}

bool Tokenizador::CheckURL(const string& url, string::size_type& pos, const string::size_type& lastPos) const{
    if(url.substr(lastPos,5)=="http:" || url.substr(lastPos,6)=="https:" || url.substr(lastPos,4)=="ftp:"){
        string::size_type dosPuntos = url.find_first_of(":", lastPos);
        string::size_type aux_pos = EliminaDelimiters(url, dosPuntos, "_:/.?&-=#@");
        if(aux_pos==string::npos)
            aux_pos=url.length();
        if((aux_pos-dosPuntos)>1){
            pos = aux_pos;
            return true;
        }
    }
    return false;
}

size_t Tokenizador::EliminaDelimiters(const string& str, const size_t& inicio, const string& del) const{
    size_t r = inicio;
    do{
        r = FindFirstDelimiter(str, r + 1);
    }while(r!=string::npos && del.find(str[r],0)!=string::npos);
    return r;
}

bool Tokenizador::CheckDecimal(const string& decimal, string::size_type& pos, string::size_type& lastPos, bool& initial_zero) const{   
    string::size_type i = lastPos;
    string::size_type limit = pos;
    bool final = false;
    while(i < decimal.size()){
        limit = (limit==string::npos?decimal.size():limit);
        while(i<limit){
            if(decimal[i]<48 || decimal[i]>57){
                if(i!=lastPos && (decimal[i] == '$' || decimal[i]=='%') 
                    && ((i + 2<decimal.size() && decimal[i+1]==' ') || i==decimal.size()-1)){
                    final = true;
                    break;
                }
                return false;                    
            }
            i++;
        }
        if(final || (decimal[limit]!='.' && decimal[limit]!=','))
            break;
        i = limit + 1;
        limit = FindFirstDelimiter(decimal, i);
        if(i==limit)
            return false;
    }
    pos = (limit==string::npos?decimal.size():limit);

    // Si lo anterior era un punto, añadimos el 0
    if(lastPos>0 && (decimal[lastPos - 1] == '.' || decimal[lastPos - 1]==',')){
            lastPos--;
            initial_zero=true;
    }
    // Quitamos posibles $ o % al final
    if (decimal[pos-1]<48 || decimal[pos-1]>57){
        pos--;
    }
    return true;
}

bool Tokenizador::CheckEmail(const string& email, string::size_type& pos) const{
    size_t aux_pos = EliminaDelimiters(email, pos, ".-_");
    if(email[aux_pos]!='@'){
        pos = aux_pos;
        return true;
    }
    return false;
}

bool Tokenizador::CheckAcronimoMultipalabra(const string& str, string::size_type& pos, const string& delimiter) const{
    size_t i=pos;
    pos = EliminaDelimiters(str, pos, delimiter);
    pos = (pos==string::npos?str.length() : pos);
    while(i<pos){
        if(str[i]==delimiter[0] && str[i-1]==delimiter[0]){
            pos = i - 1;
            return false;
        }
        i++;
    }
    if(str[pos-1]==delimiter[0])
        pos--;
    return true;
}

size_t Tokenizador::FindFirstNoDelimiter(const string& str, const size_t& inicio) const{
    for(size_t i=inicio; i<str.size(); i++){
        if((int)str[i]<0 || !stop[(int)str[i]])
            return i;
    }
    return string::npos;
}

size_t Tokenizador::FindFirstDelimiter(const string& str, const size_t& inicio) const{
    if(inicio==string::npos)
        return inicio;
    for(size_t i=inicio; i<str.size(); i++){
            if((int)str[i]<0)
                continue;
            if(stop[(int)str[i]])
                return i;
        }
    return string::npos;
}

void Tokenizador::Tokenizar(string str, list<string>& tokens) const{
    tokens.clear();
    string output;
    this->TokenizarInterno(str, output);
    int pos = 0;
    for(int i=0; i<output.size(); i++){
        if(output[i]=='\n'){
            tokens.push_back(output.substr(pos, i-pos));
            pos = i + 1;
        }
    }
}

void Tokenizador::Tokenizar(string str, deque<string>& v)  const{
    // Corregir acentos y mayusculas si es necesario
    if(this->pasarAminuscSinAcentos){
        EliminarMayusAcentos(str);
    }
    string::size_type lastPos = FindFirstNoDelimiter(str, 0);
    string::size_type pos = FindFirstDelimiter(str, lastPos);

    while(lastPos != string::npos){
        pos = (pos==string::npos?str.length():pos);
        v.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = FindFirstNoDelimiter(str, pos);
        pos = FindFirstDelimiter(str, lastPos);
    }
}


void Tokenizador::TokenizarInterno(string &str, string& output) const{
    bool initial_zero = false;
        
    // Corregir acentos y mayusculas si es necesario
    if(this->pasarAminuscSinAcentos){
        EliminarMayusAcentos(str);
    }

    /*bool decimales = stop[(int)'.'] && stop[(int)','];
    bool found = false;*/
    string::size_type lastPos = FindFirstNoDelimiter(str, 0);
    string::size_type pos = FindFirstDelimiter(str, lastPos);

    while(lastPos != string::npos){
        pos = (pos==string::npos?str.length():pos);
        /*if(casosEspeciales){
            if(CheckURL(str, pos, lastPos)){
                output += str.substr(lastPos, pos - lastPos);
                output+="\n";
                lastPos = FindFirstNoDelimiter(str, pos);
                pos = FindFirstDelimiter(str, lastPos);
                continue;
            }
            if(decimales)
                found = CheckDecimal(str, pos, lastPos, initial_zero);
            if(!found && stop[(int)'@'] && str[pos]=='@')
                found = CheckEmail(str, pos);
            if(!found && stop[(int)'.'] && str[pos]=='.')
                found = CheckAcronimoMultipalabra(str, pos, ".");
            if(!found && stop[(int)'-'] && str[pos]=='-')
                CheckAcronimoMultipalabra(str, pos, "-");
        }
        if(initial_zero)
            output+="0";*/
        output+=str.substr(lastPos, pos - lastPos);
        output+="\n";
        lastPos = FindFirstNoDelimiter(str, pos);
        /*initial_zero = false;
        found = false;*/
        pos = FindFirstDelimiter(str, lastPos);
    }
}

bool Tokenizador::Tokenizar (const string& NomFichEntr, const string& NomFichSal) const{
    ifstream i;
    ofstream f;
    string cadena, output;

    output="";
    i.open(NomFichEntr.c_str());
    if(!i) {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << "\n";
        return false;
    }
    while(getline(i,cadena)){
        if(cadena.length()!=0)
        {
            TokenizarInterno(cadena, output);
        }
    }

    i.close();

    f.open(NomFichSal.c_str());
    f << output;
    f.close();
    return true;
}

bool Tokenizador::Tokenizar (const string & NomFichEntr) const{
    ifstream i;
    ofstream f;
    string output="";
    string cadena;

    i.open(NomFichEntr.c_str());
    if(!i) {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << "\n";
        return false;
    }
    while(getline(i,cadena)){
        if(cadena.length()!=0)
        {
            TokenizarInterno(cadena, output);
        }
    }

    i.close();

    f.open((NomFichEntr + ".tk").c_str());
    f << output;
    f.close();
    return true;
}

bool Tokenizador::TokenizarListaFicheros (const string& NomFichEntr) const{
    ifstream i;
    string cadena;
    bool result=true;

    i.open(NomFichEntr.c_str());
    if(!i) {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << "\n";
        return false;
    }
    while(getline(i, cadena)){
        result = Tokenizar(cadena) && result;
    }
    i.close();

    return result;
}

bool Tokenizador::TokenizarDirectorio (const string& dirAIndexar) const{
    struct stat dir;
    // Compruebo la existencia del directorio
    int err=stat(dirAIndexar.c_str(), &dir);
    if(err==-1 || !S_ISDIR(dir.st_mode))
        return false;
    else {
        // Hago una lista en un fichero con find>fich
        string cmd="find "+dirAIndexar+" -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    this->delimiters = "";
    memset(stop, false, sizeof(stop));
    for (size_t i=0; i < nuevoDelimiters.size(); i++){
        if(!stop[(int)nuevoDelimiters[i]]){
            this->delimiters += nuevoDelimiters[i];
            stop[(int)nuevoDelimiters[i]] = true;
        }
    }
    if(casosEspeciales)
        stop[(int)' '] = true;
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){
    for (size_t i=0; i < nuevoDelimiters.size(); i++){
        if(casosEspeciales && nuevoDelimiters[i]==' ' && (delimiters.find(' ', 0)==string::npos))
            delimiters += " ";
        if(!stop[(int)nuevoDelimiters[i]]){
            this->delimiters += nuevoDelimiters[i];
            stop[(int)nuevoDelimiters[i]] = true;
        }
    }  
}

void Tokenizador::CasosEspeciales (const bool& nuevoCasosEspeciales) {
    if(casosEspeciales && !nuevoCasosEspeciales && delimiters.find(' ')==string::npos)
        stop[(int)' '] = false;
    casosEspeciales = nuevoCasosEspeciales; 
}

void Tokenizador::EliminarMayusAcentos(string& str) const{
    if(pasarAminuscSinAcentos){
        for(size_t i = 0; i<str.size(); i++){
            str[i] = newCharValue[((int)str[i]<0?(int)str[i]+256:(int)str[i])];
        }
    }
}

ostream& operator<<(ostream& os, const Tokenizador& t){
    os << "DELIMITADORES: " << t.delimiters << " TRATA CASOS ESPECIALES: " << t.casosEspeciales
        << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.pasarAminuscSinAcentos;
    return os;
}
