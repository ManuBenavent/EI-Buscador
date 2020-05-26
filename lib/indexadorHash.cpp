#include "indexadorHash.h"

IndexadorHash::IndexadorHash(){
    this->ficheroStopWords = "";
    this->directorioIndice = "";
    this->tipoStemmer = 0;
    this->almacenarEnDisco = false;
    this->almacenarPosTerm = false;
}

IndexadorHash::IndexadorHash(const string& ficheroStopWords, const string& delimitadores, const bool& detectComp, 
const bool& minuscSinAcentos, const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm):tok(delimitadores, detectComp, minuscSinAcentos){
    this->ficheroStopWords = ficheroStopWords;
    this->directorioIndice = dirIndice;
    if(tStemmer<0 || tStemmer>2){
        cerr << "ERROR: Tipo stemmer invalido" << endl;
        throw "ERROR: Tipo stemmer invalido";
    }
    this->tipoStemmer = tStemmer;
    this->almacenarEnDisco = almEnDisco;
    this->almacenarPosTerm = almPosTerm;

    ifstream f;
    string line;
    f.open(this->ficheroStopWords.c_str());

    if(!f){
        cerr << "ERROR: No se pudo abrir ficheroStopWords" << endl;
        throw "ERROR: No se pudo abrir ficheroStopWords";
    }

    while(getline(f, line)){
        if(minuscSinAcentos)
            tok.EliminarMayusAcentos(line);
        this->stopWords.insert(line);
    }
        
        
    f.close();
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion){
    if(!RecuperarIndexacion(directorioIndexacion)){
        cerr << "ERROR: No se pudo recuperar la indexacion";
    }
}

IndexadorHash::IndexadorHash(const IndexadorHash& i){
    this->ficheroStopWords = i.ficheroStopWords;
    this->tok = i.tok;
    this->directorioIndice = i.directorioIndice;
    this->tipoStemmer = i.tipoStemmer;
    this->almacenarEnDisco = i.almacenarEnDisco;
    this->almacenarPosTerm = i.almacenarPosTerm;
    this->indice = i.indice;
    this->indiceDocs = i.indiceDocs;
    this->informacionColeccionDocs = i.informacionColeccionDocs;
    this->pregunta = i.pregunta;
    this->indicePregunta = i.indicePregunta;
    this->infPregunta = i.infPregunta;
    this->stopWords = i.stopWords;
    this->indiceMinimo = i.indiceMinimo;
    this->indiceDocsMinimo = i.indiceDocsMinimo;
}

IndexadorHash::~IndexadorHash(){
    this->ficheroStopWords = "";
    this->directorioIndice = "";
    this->indice.clear();
    this->indiceDocs.clear();
    this->pregunta = "";
    this->indicePregunta.clear();
    this->stopWords.clear();
    system("rm -f *.data");
}

IndexadorHash& IndexadorHash::operator= (const IndexadorHash& i){
    this->ficheroStopWords = i.ficheroStopWords;
    this->tok = i.tok;
    this->directorioIndice = i.directorioIndice;
    this->tipoStemmer = i.tipoStemmer;
    this->almacenarEnDisco = i.almacenarEnDisco;
    this->almacenarPosTerm = i.almacenarPosTerm;
    this->indice = i.indice;
    this->indiceDocs = i.indiceDocs;
    this->informacionColeccionDocs = i.informacionColeccionDocs;
    this->pregunta = i.pregunta;
    this->indicePregunta = i.indicePregunta;
    this->infPregunta = i.infPregunta;
    this->stopWords = i.stopWords;
    this->indiceMinimo = i.indiceMinimo;
    this->indiceDocsMinimo = i.indiceDocsMinimo;
    return (*this);
}

bool IndexadorHash::Indexar(const string& ficheroDocumentos){
    ifstream iDocuments, iFile;
    string file, token;
    stemmerPorter stemmer;
    struct stat t_stat;
    // Abrimos archivo proporcionado
    iDocuments.open(ficheroDocumentos.c_str());
    if(!iDocuments){
        cerr << "ERROR: No se pudo abrir el archivo ficheroDocumentos" << endl;
        return false;
    }
    // Tokenizamos todos los documentos
    tok.TokenizarListaFicheros(ficheroDocumentos);
    // Indexamos cada uno de los documentos
    while(getline(iDocuments, file)){
        // Obtenemos información sobre el documento
        int err = stat(file.c_str(), &t_stat);
        if(err == -1 || S_ISDIR(t_stat.st_mode)){
            continue;
        }
        // Info sobre documento
        InfDoc infdoc;
        infdoc.setSize(t_stat.st_size);
        infdoc.setId(informacionColeccionDocs.getNumDocs() + 1);
        infdoc.setFecha(t_stat.st_mtime);
        iFile.open((file + ".tk").c_str());
        if(!iFile){
            cerr << "ERROR: No se pudo leer el documento " << file << endl;
            continue;
        }
        if(indiceDocs.find(file) != indiceDocs.end()){
            InfDoc infDocOld;
            if(!infDocOld.getFecha().OlderThan(infdoc.getFecha())){ // No se cumple: Fecha actual < Fecha nueva
                cerr << "ERROR: El documento " << file << " ya habia sido indexado" << endl;
                continue;
            }
            infdoc.setId(indiceDocs.at(file).getId());
            // Borro el documento indexado para reindexarlo de nuevo
            BorraDoc(file);
            // La indexacion continua de la manera habitual
        }
        while(getline(iFile, token)){
            // Stopwords
            infdoc.addWord();
            if(stopWords.find(token) != stopWords.end())
                continue;
            infdoc.addWordNoStop();
            // stemming
            stemmer.stemmer(token, tipoStemmer);
            // indexar
            if(indice.find(token) == indice.end() && indiceMinimo.find(token) == indiceMinimo.end() ){
                infdoc.addWordDif();
                InformacionTermino infTerm;
                infTerm.add_ftc();
                InfTermDoc infTermDoc;
                infTermDoc.add_ft();
                if(almacenarPosTerm)
                    infTermDoc.add_posTerm(infdoc.getNumPal()-1);
                infTerm.add_l_docs(infdoc.getId(), infTermDoc);
                indice[token] = infTerm;
            }
            else if(almacenarEnDisco && indiceMinimo.find(token) != indiceMinimo.end()){
                if(indiceUpdates.find(token) != indiceUpdates.end())
                    indiceUpdates[token].update(infdoc, almacenarPosTerm);
                else{
                    infdoc.addWordDif();
                    InformacionTermino infTerm;
                    infTerm.add_ftc();
                    InfTermDoc infTermDoc;
                    infTermDoc.add_ft();
                    if(almacenarPosTerm)
                        infTermDoc.add_posTerm(infdoc.getNumPal()-1);
                    infTerm.add_l_docs(infdoc.getId(), infTermDoc);
                    indiceUpdates[token] = infTerm;
                }
            }
            else
                indice[token].update(infdoc, almacenarPosTerm);
        }
        iFile.close();
        this->informacionColeccionDocs.update(infdoc, almacenarEnDisco?indice.size()+indiceMinimo.size():indice.size()); 
        
        if(almacenarEnDisco){
            GuardarIndiceEnDisco();
            GuardarInfDocEnDisco(infdoc, false, file);
        }
        else
            indiceDocs[file] = infdoc;
    }
    iDocuments.close();
    if(almacenarEnDisco)
        DesfragmentarArchivos();
    return true;
}

void IndexadorHash::DesfragmentarArchivos(){
    ofstream indiceFile, l_docsFile, posTermFile;
    ifstream oldIndice, oldl_docs, oldPosTerm;
    InformacionTerminoBinario infTerm;
    InfTermDocBinario infTermDoc;
    system("cp indiceDisco.data oldIndice.data 2>/dev/null || :");
    system("cp l_docsDisco.data oldl_docs.data 2>/dev/null || :");
    system("cp posTermDisco.data oldPosTerm.data 2>/dev/null || :");
    system("rm -f indiceDisco.data");
    system("rm -f l_docsDisco.data");
    system("rm -f posTermDisco.data");
    oldIndice.open("oldIndice.data", ios::binary);
    oldl_docs.open("oldl_docs.data", ios::binary);
    oldPosTerm.open("oldPosTerm.data", ios::binary);
    indiceFile.open("indiceDisco.data", ios::binary);
    l_docsFile.open("l_docsDisco.data", ios::binary);
    posTermFile.open("posTermDisco.data", ios::binary);
    for(unordered_map<string, streampos>::iterator it = indiceMinimo.begin(); it!=indiceMinimo.end(); it++){
        // Recupero informacion almacenada
        InformacionTermino inf;
        oldIndice.seekg(it->second);
        oldIndice.read((char *)&infTerm, sizeof(InformacionTerminoBinario));
        inf.set_ftc(infTerm.ftc);
        oldl_docs.seekg(infTerm.inicio);
        
        for (int i = 0; i < infTerm.numDocs; i++){
            InfTermDoc aux;
            oldl_docs.read((char *)&infTermDoc, sizeof(InfTermDocBinario));
            aux.set_ft(infTermDoc.ft);
            oldPosTerm.seekg(infTermDoc.posLista);
            int pos;
            for (int j = 0; j < infTermDoc.numPosTerm; j++){
                oldPosTerm.read((char *)&pos, sizeof(int));
                aux.add_posTerm(pos);
            }
            inf.add_l_docs(infTermDoc.idDoc, aux);
        }

        // GUARDO la informacion
        infTerm.ftc = inf.get_ftc();
        infTerm.numDocs = inf.GetNumDocs();
        infTerm.inicio = l_docsFile.tellp();
        it->second = indiceFile.tellp();
        indiceFile.write((char *)&infTerm, sizeof(infTerm));

        unordered_map<long int, InfTermDoc> l_docs = inf.getMap();
        for(unordered_map<long int, InfTermDoc>::iterator itDoc = l_docs.begin(); itDoc != l_docs.end(); itDoc++){
            list<int> posTerm = itDoc->second.getList();
            infTermDoc.idDoc = itDoc->first;
            infTermDoc.ft = itDoc->second.get_ft();
            infTermDoc.numPosTerm = posTerm.size();
            infTermDoc.posLista = posTermFile.tellp();
            l_docsFile.write((char *)&infTermDoc, sizeof(infTermDoc));

            for(list<int>::iterator pos = posTerm.begin(); pos != posTerm.end(); pos++)
                posTermFile.write((char *)&(*pos), sizeof(int));
        }
    }
    oldIndice.close();
    oldl_docs.close();
    oldPosTerm.close();
    indiceFile.close();
    l_docsFile.close();
    posTermFile.close();
    system("rm -f oldIndice.data");
    system("rm -f oldl_docs.data");
    system("rm -f oldPosTerm.data");
}

void IndexadorHash::GuardarIndiceEnDisco(){
    fstream indiceFile, l_docsFile, posTermFile;
    InformacionTerminoBinario infTerm = {};
    InfTermDocBinario infTermDoc = {};
    indiceFile.open("indiceDisco.data", ios::app | ios::in | ios::out | ios::ate);
    l_docsFile.open("l_docsDisco.data", ios::app | ios::in | ios::out| ios::ate);
    posTermFile.open("posTermDisco.data", ios::app | ios::in | ios::out| ios::ate);
    if(!indiceFile || !l_docsFile || !posTermFile){
    cerr << "ERROR: No se pudo guardar en disco" << endl;
        return;
    }
    // ACTUALIZAR PALABRAS YA ALMACENADAS
    for(unordered_map<string, InformacionTermino>::iterator it = indiceUpdates.begin(); it!=indiceUpdates.end(); ){
        // Recupero informacion almacenada
        InformacionTermino inf;
        indiceFile.seekg(indiceMinimo.at(it->first));
        indiceFile.read((char *)&infTerm, sizeof(InformacionTerminoBinario));
        inf.set_ftc(infTerm.ftc);

        l_docsFile.seekg(infTerm.inicio);
        for (int i = 0; i < infTerm.numDocs; i++){
            InfTermDoc aux;
            l_docsFile.read((char *)&infTermDoc, sizeof(InfTermDocBinario));
            aux.set_ft(infTermDoc.ft);
            posTermFile.seekg(infTermDoc.posLista);
            int pos;
            for (int j = 0; j < infTermDoc.numPosTerm; j++){
                posTermFile.read((char *)&pos, sizeof(int));
                aux.add_posTerm(pos);
            }
            inf.add_l_docs(infTermDoc.idDoc, aux);
        }

        //ACTUALIZO
        inf = inf + it->second;
        // GUARDO la informacion
        infTerm.ftc = inf.get_ftc();
        infTerm.numDocs = inf.GetNumDocs();
        infTerm.inicio = l_docsFile.tellp();
        unordered_map<string, streampos>::iterator indexIterator = indiceMinimo.find(it->first);
        indexIterator->second = indiceFile.tellp();
        indiceFile.write((char *)&infTerm, sizeof(infTerm));

        unordered_map<long int, InfTermDoc> l_docs = inf.getMap();
        for(unordered_map<long int, InfTermDoc>::iterator itDoc = l_docs.begin(); itDoc != l_docs.end(); itDoc++){
            list<int> posTerm = itDoc->second.getList();
            infTermDoc.idDoc = itDoc->first;
            infTermDoc.ft = itDoc->second.get_ft();
            infTermDoc.numPosTerm = posTerm.size();
            infTermDoc.posLista = posTermFile.tellp();
            l_docsFile.write((char *)&infTermDoc, sizeof(infTermDoc));

            for(list<int>::iterator pos = posTerm.begin(); pos != posTerm.end(); pos++)
                posTermFile.write((char *)&(*pos), sizeof(int));
        }
        indiceUpdates.erase(it++);
    }
    // ESCRIBIR NUEVOS DATOS
    for(unordered_map<string, InformacionTermino>::iterator it = indice.begin(); it!=indice.end(); ){
        infTerm.ftc = it->second.get_ftc();
        infTerm.inicio = l_docsFile.tellp();
        infTerm.numDocs = it->second.GetNumDocs();
        indiceMinimo[it->first] = indiceFile.tellp();
        indiceFile.write((char *)&infTerm, sizeof(infTerm));
        
        unordered_map<long int, InfTermDoc> l_docs = it->second.getMap();
        for(unordered_map<long int, InfTermDoc>::iterator itDoc = l_docs.begin(); itDoc != l_docs.end(); itDoc++){
            list<int> posTerm = itDoc->second.getList();
            infTermDoc.posLista = posTermFile.tellp();
            infTermDoc.idDoc = itDoc->first;
            infTermDoc.ft = itDoc->second.get_ft();
            infTermDoc.numPosTerm = posTerm.size();
            l_docsFile.write((char *)&infTermDoc, sizeof(infTermDoc));

            for(list<int>::iterator pos = posTerm.begin(); pos != posTerm.end(); pos++){
                int aux = *pos;
                posTermFile.write((char *)&aux, sizeof(int));
            }
        }
        indice.erase(it++);
    }
    indiceFile.close();
    l_docsFile.close();
    posTermFile.close();
}

void IndexadorHash::GuardarInfDocEnDisco(const InfDoc& infdoc, bool update, const string& doc){  
    ofstream binFile;
    if(update){
        system("cp indiceDocsDisco.data oldIndiceDocs.data 2>/dev/null || :");
        system("rm -f indiceDocs.data");
        ifstream old;
        old.open("oldIndiceDocs.data", ios::binary);
        binFile.open("indiceDocsDisco.data", ios::binary);
        InfDoc doc;
        for(unordered_map<string, streampos>::iterator it = indiceDocsMinimo.begin(); it != indiceDocsMinimo.end(); it++){
            if(it->second==-1){
                it->second = binFile.tellp();
                binFile.write((char *)&indiceDocs.at(it->first), sizeof(InfDoc));
            }
            else{
                old.seekg(it->second);
                old.read((char *)&doc, sizeof(InfDoc));
                it->second = binFile.tellp();
                binFile.write((char *)&doc, sizeof(InfDoc));
            }
        }
        binFile.close();
        old.close();
    }
    else{
        binFile.open("indiceDocsDisco.data", ios::app);
        if(!binFile)
            cerr << "ERROR: No se pudo guardar en disco" << endl;
        indiceDocsMinimo[doc] = binFile.tellp();
        binFile.write((char *)&infdoc, sizeof(InfDoc)); 
        binFile.close();
    }
}

bool IndexadorHash::IndexarDirectorio(const string& dirAIndexar){
    struct stat dir;
    // Compruebo la existencia del directorio
    int err=stat(dirAIndexar.c_str(), &dir);
    if(err==-1 || !S_ISDIR(dir.st_mode)){
        cerr << "ERROR: El directorio no existe" << endl;
        return false;
    }
        
    // Hago una lista en un fichero con find>fich (todos los ficheros incluyendo directorios)
    string cmd="find "+dirAIndexar+" -follow |sort > .lista_fich";
    system(cmd.c_str());
    // A partir de la lista generada indexo cada uno de los ficheros almacenados
    return Indexar(".lista_fich");
}

bool IndexadorHash::GuardarIndexacion() const{
    // Compruebo directorio
    if(directorioIndice!=""){
        string cmd = "mkdir -p " + directorioIndice;
        system(cmd.c_str());
    }

    ofstream out;
    // Datos generales
    out.open(directorioIndice==""?".":directorioIndice + "/general.data");
    if(!out){
        system(("rm " + directorioIndice + "/*.data").c_str());
        cerr << "ERROR: Se ha producido un error al guardar la informacion" << endl;
        return false;
    }
    out << pregunta << "\n";
    out << infPregunta.ToFile() << "\n";
    out << ficheroStopWords << "\n"; 
    out << directorioIndice << "\n";
    out << informacionColeccionDocs.ToFile() << "\n";
    out << tipoStemmer << " " << almacenarEnDisco << " " << almacenarPosTerm << "\n";
    out << tok.CasosEspeciales() << " " << tok.PasarAminuscSinAcentos() << " " << (tok.DelimitadoresPalabra().find('\n')!=string::npos?"y":"n") << " " << tok.DelimitadoresPalabra();
    out.close();

    // Indice docs
    out.open(directorioIndice==""?".":directorioIndice + "/indiceDocs.data");
    if(!out){
        system(("rm " + directorioIndice + "/*.data").c_str());
        cerr << "ERROR: Se ha producido un error al guardar la informacion" << endl;
        return false;
    }
    out << indiceDocs.size() << "\n";
    for(unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin(); it != indiceDocs.end(); it++){
        out << it->first << " " << it->second.ToFile() << "\n";
    }
    out.close();

    // Indice pregunta
    out.open(directorioIndice==""?".":directorioIndice + "/indicePregunta.data");
    if(!out){
        system(("rm " + directorioIndice + "/*.data").c_str());
        cerr << "ERROR: Se ha producido un error al guardar la informacion" << endl;
        return false;
    }
    out << indicePregunta.size() << "\n";
    for(unordered_map<string, InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin(); it != indicePregunta.end(); it++){
        out << it->first << " " << it->second.ToFile() << "\n";
    }
    out.close();

    // Indice
    out.open(directorioIndice==""?".":directorioIndice + "/indice.data");
    if(!out){
        system(("rm " + directorioIndice + "/*.data").c_str());
        cerr << "ERROR: Se ha producido un error al guardar la informacion" << endl;
        return false;
    }
    out << indice.size() << "\n";
    for(unordered_map<string, InformacionTermino>::const_iterator it = indice.begin(); it != indice.end(); ++it){
        out << it->first << " " << it->second.ToFile() << "\n";
    }
    out.close();

    // StopWords
    out.open(directorioIndice==""?".":directorioIndice + "/stopWords.data");
    if(!out){
        system(("rm " + directorioIndice + "/*.data").c_str());
        cerr << "ERROR: Se ha producido un error al guardar la informacion" << endl;
        return false;
    }
    out << stopWords.size() << "\n";
    for(unordered_set<string>::const_iterator it = stopWords.begin(); it != stopWords.end(); it++){
        out << (*it) << "\n";
    }
    out.close();
    return true;
}

bool IndexadorHash::RecuperarIndexacion (const string& directorioIndexacion){
    struct stat dir;
    // Compruebo la existencia del directorio
    if(directorioIndexacion!=""){
        int err=stat(directorioIndexacion.c_str(), &dir);
        if(err==-1 || !S_ISDIR(dir.st_mode)){
            cerr << "ERROR: El directorio no existe" << endl; 
            return false;
        }
    }

    indice.clear();
    indiceDocs.clear();
    indicePregunta.clear();

    ifstream info;
    string aux_string;
    long int aux_long;
    bool aux_bool;
    // Datos generales
    info.open(directorioIndexacion==""?".":directorioIndexacion + "/general.data");
    if(!info){
        cerr << "ERROR: Se ha producido un error al recuperar la informacion" << endl;
        return false;
    }
    // Pregunta
    getline(info, pregunta);
    // InformacionPregunta
    info >> aux_long;
    infPregunta.setNumTotalPal(aux_long);
    info >> aux_long;
    infPregunta.setNumTotalPalSinParada(aux_long);
    info >> aux_long;
    infPregunta.setNumTotalPalDiferentes(aux_long);
    info.ignore();
    // Fichero stop words
    getline(info, ficheroStopWords);
    // DirectorioIndice
    getline(info, directorioIndice);
    // InformacionColeccionDocs
    info >> aux_long;
    informacionColeccionDocs.setNumDocs(aux_long);
    info >> aux_long;
    informacionColeccionDocs.setNumTotalPal(aux_long);
    info >> aux_long;
    informacionColeccionDocs.setNumTotalPalSinParada(aux_long);
    info >> aux_long;
    informacionColeccionDocs.setNumTotalPalDiferentes(aux_long);
    info >> aux_long;
    informacionColeccionDocs.setTamBytes(aux_long);
    info.ignore();
    // TipoStemmer
    info >> tipoStemmer;
    // almacenarEnDisco
    info >> almacenarEnDisco;
    // almacenarPosTerm
    info >> almacenarPosTerm;
    // Tokenizador
    info >> aux_bool;
    tok.CasosEspeciales(aux_bool);
    info >> aux_bool;
    tok.PasarAminuscSinAcentos(aux_bool);
    
    char saltoLinea;
    info >> saltoLinea;
    info.ignore();
    getline(info, aux_string);
    tok.DelimitadoresPalabra(aux_string);
    if(saltoLinea=='y'){
        if(getline(info, aux_string))
            tok.AnyadirDelimitadoresPalabra("\n" + aux_string);    
        else
            tok.AnyadirDelimitadoresPalabra("\n");
    }
    info.close();

    // Indice
    info.open(directorioIndexacion==""?".":directorioIndexacion + "/indice.data");
    if(!info){
        cerr << "ERROR: Se ha producido un error al recuperar la informacion" << endl;
        return false;
    }
    string palabra;
    int ftc, l_docs_size, docId, ft, posTerm_size, posTermItem;
    // Numero elementos indice
    info >> aux_long;
    info.ignore();
    for(int i = 0; i < aux_long; i++){
        InformacionTermino infTerm;
        // Key
        info >> palabra;
        // Value
        info >> ftc;
        infTerm.set_ftc(ftc);
        // l_docs
        info >> l_docs_size;
        info.ignore();
        for(int j = 0; j < l_docs_size; j++){
            InfTermDoc infTermDoc;
            // key
            info >> docId;
            info >> ft;
            // value
            infTermDoc.set_ft(ft);
            info >> posTerm_size;
            for(int k = 0; k < posTerm_size; k++){
                info >> posTermItem;
                infTermDoc.add_posTerm(posTermItem);
            }
            infTerm.add_l_docs(docId, infTermDoc);
            info.ignore();
        }
        
        // Inserto en el indice
        indice[palabra] = infTerm;            
    }

    info.close();

    // Indice docs
    info.open(directorioIndexacion==""?".":directorioIndexacion + "/indiceDocs.data");
    if(!info){
        cerr << "ERROR: Se ha producido un error al recuperar la informacion" << endl;
        return false;
    }
    info >> aux_long;
    info.ignore();
    for(int i = 0; i < aux_long; i++){
        int idDoc, numPal, numPalSinParada, numPalDiferentes, tamBytes, d, m, y, h, min;
        info >> aux_string;
        info >> idDoc;
        info >> numPal;
        info >> numPalSinParada;
        info >> numPalDiferentes;
        info >> tamBytes;
        info >> d;
        info >> m;
        info >> y;
        info >> h;
        info >> min;
        Fecha fecha (d, m, y, h, min);
        InfDoc doc(idDoc, numPal, numPalSinParada, numPalDiferentes, tamBytes, fecha);
        indiceDocs[aux_string] = doc;
        info.ignore();
    }
    info.close();

    // Indice pregunta
    info.open(directorioIndexacion==""?".":directorioIndexacion + "/indicePregunta.data");
    if(!info){
        cerr << "ERROR: Se ha producido un error al recuperar la informacion" << endl;
        return false;
    }
    info >> aux_long;
    info.ignore();
    for(int i = 0; i < aux_long; i++){
        InformacionTerminoPregunta infTermPreg;
        int ft, posTermSize, posTermItem;
        info >> aux_string;
        info >> ft;
        infTermPreg.set_ft(ft);
        info >> posTermSize;
        for(int i = 0; i < posTermSize; i++){
            info >> posTermItem;
            infTermPreg.addPosTermItem(posTermItem);
        }
        indicePregunta[aux_string] = infTermPreg;
        info.ignore();
    }
    info.close();

    // StopWords
    info.open(directorioIndexacion==""?".":directorioIndexacion + "/stopWords.data");
    if(!info){
        cerr << "ERROR: Se ha producido un error al recuperar la informacion" << endl;
        return false;
    }
    // Numero de terminos
    info >> aux_long;
    info.ignore();
    // Terminos
    stopWords.clear();
    for(int i = 0; i < aux_long; i++){
        getline(info, aux_string);
        stopWords.insert(aux_string);
    }
    info.close();
    return true;
}

void IndexadorHash::ImprimirIndexacion() const {
    if(almacenarEnDisco){
        string word;
        InformacionTermino inf;
        cout << "Terminos indexados: \n";
        for(unordered_map<string, streampos>::const_iterator it = indiceMinimo.begin(); it != indiceMinimo.end(); it++){
            word = it->first;
            Devuelve(word, inf);
            cout << it->first << "\t" << inf << endl;
        }
        cout << "Documentos indexados: \n";
        ListarDocs();
    }
    else{
        cout << "Terminos indexados: \n";
        for(unordered_map<string, InformacionTermino>::const_iterator it = indice.begin(); it != indice.end(); it++)
            cout << it->first << "\t" << it->second << endl;
        cout << "Documentos indexados: " << endl;
        for(unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin(); it != indiceDocs.end(); it++)
            cout << it->first << "\t" << it->second << endl;
    }
    
}

bool IndexadorHash::IndexarPregunta(const string& preg){
    pregunta = preg;
    indicePregunta.clear();
    infPregunta = InformacionPregunta();
    //list<string> tokens;
    //string tokens;
    deque<string> tokens;
    stemmerPorter stemmer;
    tok.Tokenizar(preg, tokens);
    for(deque<string>::iterator it = tokens.begin(); it != tokens.end(); it++){
        infPregunta.addPal();
        if(stopWords.find(*it) == stopWords.end()){
            stemmer.stemmer(*it, tipoStemmer);
            infPregunta.addPalNoStop();
            unordered_map<string, InformacionTerminoPregunta>::iterator termino = indicePregunta.find(*it);
            if(termino != indicePregunta.end()){
                // Existe actualizar InformacionTerminoPregunta
                termino->second.add_ft();
                if(almacenarPosTerm)
                    termino->second.addPosTermItem(infPregunta.getNumTotalPal()-1);
            }
            else{
                // No existe crear nuevo InformacionTerminoPregunta
                infPregunta.addPalDif();
                InformacionTerminoPregunta infTerm;
                infTerm.add_ft();
                if(almacenarPosTerm)
                    infTerm.addPosTermItem(infPregunta.getNumTotalPal()-1);

                // Si el termino esta indexado almaceno el IDF
                auto infoTermino = indice.find(*it);
                if(infoTermino != indice.end()){
                    double nqi = (double)infoTermino->second.GetNumDocs();
                    double aux = ((double)informacionColeccionDocs.getNumDocs() - nqi + 0.5) / (nqi + 0.5);
                    if(aux > 0)
                        infTerm.setIDF(log2(aux));
                }
                
                indicePregunta[*it] = infTerm;
            }
        }
    }
    if(indicePregunta.empty()){
        cerr << "ERROR: No se ha indexado ningun termino." << endl;
        return false;
    }
    return true;
}

bool IndexadorHash::DevuelvePregunta(string &preg) const{
    if(indicePregunta.empty())
        return false;
    preg = pregunta;
    return true;
}

bool IndexadorHash::DevuelvePregunta(string word, InformacionTerminoPregunta& inf) const{
    tok.EliminarMayusAcentos(word);
    stemmerPorter stemmer;
    stemmer.stemmer(word, tipoStemmer);
    if(indicePregunta.find(word) == indicePregunta.end()){
        inf.~InformacionTerminoPregunta();
        return false;
    }
    inf = indicePregunta.at(word);
    return true;
}

bool IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) const{
    if(indicePregunta.empty()){
        inf.~InformacionPregunta();
        return false;
    }
    inf = infPregunta;
    return true;
}

void IndexadorHash::ImprimirIndexacionPregunta() const{
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Terminos indexados en la pregunta: " << endl;
    for(unordered_map<string, InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin(); it != indicePregunta.end(); it++)
        cout << it->first << "\t" << it->second << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

void IndexadorHash::ImprimirPregunta() const{
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

bool IndexadorHash::Devuelve(string word, InformacionTermino& inf) const{
    tok.EliminarMayusAcentos(word);
    stemmerPorter stemmer;
    stemmer.stemmer(word, tipoStemmer);
    if(almacenarEnDisco){
        inf.Vaciar();
        if(indiceMinimo.find(word) == indiceMinimo.end()){
            return false;
        }
        ifstream data, posTerm;
        InformacionTerminoBinario infTerm;
        // General
        data.open("indiceDisco.data", ios::binary);
        if(!data){
            inf.Vaciar();
            return false;
        }
        data.seekg(indiceMinimo.at(word));
        data.read((char *)&infTerm, sizeof(InformacionTerminoBinario));
        data.close();
        inf.set_ftc(infTerm.ftc);
        // l_docs
        data.open("l_docsDisco.data", ios::binary);
        posTerm.open("posTermDisco.data", ios::binary);
        if(!data || !posTerm){
            inf.Vaciar();
            return false;
        }
        data.seekg(infTerm.inicio);
        InfTermDocBinario infTermDocBin;        
        int pos;
        for (int i = 0; i < infTerm.numDocs; i++){
            InfTermDoc infTermDoc;
            data.read((char *)&infTermDocBin, sizeof(InfTermDocBinario));
            infTermDoc.set_ft(infTermDocBin.ft);
            posTerm.seekg(infTermDocBin.posLista);
            for (int j = 0; j < infTermDocBin.numPosTerm; j++){
                posTerm.read((char *)&pos, sizeof(int));
                infTermDoc.add_posTerm(pos);
            }
            inf.add_l_docs(infTermDocBin.idDoc, infTermDoc);
        }
        data.close();
        posTerm.close();

    }
    else{
        if(indice.find(word) == indice.end()){
            inf.Vaciar();
            return false;
        }
    inf = indice.at(word);
    }
    return true;
}

bool IndexadorHash::Devuelve(string word, const string& nomDoc, InfTermDoc& infTermDoc) const{
    tok.EliminarMayusAcentos(word);
    stemmerPorter stemmer;
    stemmer.stemmer(word, tipoStemmer);

    if(almacenarEnDisco){
        if(indiceMinimo.find(word)==indiceMinimo.end() || indiceDocsMinimo.find(nomDoc)==indiceDocsMinimo.end()){
            infTermDoc.Vaciar();
            return false;
        }
        InformacionTermino infTerm;
        Devuelve(word, infTerm);
        InfDoc doc;
        DevuelveDoc(indiceDocsMinimo.at(nomDoc), doc);
        infTerm.GetInfTermDoc(doc.getId(), infTermDoc);
    }
    else{
        if(indice.find(word)==indice.end() || indiceDocs.find(nomDoc)==indiceDocs.end() || !indice.at(word).GetInfTermDoc(indiceDocs.at(nomDoc).getId(), infTermDoc)){
            infTermDoc.Vaciar();
            return false;
        }
    }
    return true;
}

bool IndexadorHash::Existe(string word) const{
    tok.EliminarMayusAcentos(word);
    stemmerPorter stemmer;
    stemmer.stemmer(word, tipoStemmer);
    if(almacenarEnDisco)
        return (indiceMinimo.find(word) != indiceMinimo.end());
    else
        return (indice.find(word) != indice.end());
}

void IndexadorHash::DevuelveDoc(streampos pos, InfDoc &infDoc) const{
    ifstream bin;
    bin.open("indiceDocsDisco.data", ios::binary);
    bin.seekg(pos);
    bin.read((char *)&infDoc, sizeof(infDoc));
    bin.close();
}

bool IndexadorHash::Borra(string word){
    tok.EliminarMayusAcentos(word);
    stemmerPorter stemmer;
    stemmer.stemmer(word, tipoStemmer);
    if(almacenarEnDisco){
        if(indiceMinimo.find(word) == indiceMinimo.end())
            return false;
        InformacionTermino infTerm;
        Devuelve(word, infTerm);
        for(unordered_map<string, streampos>::iterator it = indiceDocsMinimo.begin(); it != indiceDocsMinimo.end(); it++){
            InfDoc doc;
            DevuelveDoc(it->second, doc);
            if(infTerm.CompruebaDocumento(doc.getId())){
                doc.ActualizaBorradoPalabra(infTerm);
                indiceDocs[it->first] = doc;
                indiceDocsMinimo[it->first] = -1;
                GuardarInfDocEnDisco(doc, true, it->first);
            }
        }
        informacionColeccionDocs.BorraPal(infTerm.get_ftc());
        indiceMinimo.erase(word);
        DesfragmentarArchivos();
        return true;
    }
    else{
        if(indice.find(word) == indice.end())
            return false;
        InformacionTermino infTerm = indice.at(word);
        for(unordered_map<string, InfDoc>::iterator it = indiceDocs.begin(); it!=indiceDocs.end(); it++){
            if(infTerm.CompruebaDocumento(it->second.getId()))
                it->second.ActualizaBorradoPalabra(infTerm);
        }
        informacionColeccionDocs.BorraPal(infTerm.get_ftc());
        return indice.erase(word) != 0;
    }
}

bool IndexadorHash::BorraDoc(const string& nomDoc){
    if(almacenarEnDisco){
        if(indiceDocsMinimo.find(nomDoc) == indiceDocsMinimo.end())
            return false;
        InfDoc doc;
        DevuelveDoc(indiceDocsMinimo.at(nomDoc), doc);
        for(unordered_map<string, streampos>::iterator it = indiceMinimo.begin(); it!=indiceMinimo.end();){
            InformacionTermino infTerm;
            Devuelve(it->first, infTerm);
            if(infTerm.BorraDoc(doc.getId())){
                if(infTerm.GetNumDocs()==0)
                    indiceMinimo.erase(it++);
                else{
                    doc.deleteWordDif();
                    indice[it->first] = infTerm;
                    it++;
                }
            }
            else
                it++;
        }
        informacionColeccionDocs.BorraDoc(doc);
        indiceDocsMinimo.erase(nomDoc);
        GuardarIndiceEnDisco();
        DesfragmentarArchivos();
        return true;
    }
    else{
        if(indiceDocs.find(nomDoc) == indiceDocs.end())
            return false;
        InfDoc doc = indiceDocs.at(nomDoc);
        for(unordered_map<string, InformacionTermino>::iterator it = indice.begin(); it!=indice.end();){
            if(it->second.BorraDoc(doc.getId())){
                if(it->second.GetNumDocs()==0)
                    indice.erase(it++);
                else{
                    doc.deleteWordDif();
                    it++;
                }
            }
            else
                it++;
        }
        informacionColeccionDocs.BorraDoc(doc);
        return indiceDocs.erase(nomDoc) != 0;
    }
    
}

bool IndexadorHash::Actualiza(string word, const InformacionTermino& inf){
    return Borra(word) && Inserta(word, inf);
}

bool IndexadorHash::Inserta(string word, const InformacionTermino& inf){
    tok.EliminarMayusAcentos(word);
    stemmerPorter stemmer;
    stemmer.stemmer(word, tipoStemmer);
    
    if(almacenarEnDisco){
        if(indiceMinimo.find(word) != indiceMinimo.end() && stopWords.find(word) != stopWords.end())
            return false;
        indice[word] = inf;
        GuardarIndiceEnDisco();
        DesfragmentarArchivos();
        ifstream doc;
        doc.open("indiceDocsDisco.data", ios::binary);
        unordered_map<long int, InfTermDoc> l_docs = inf.getMap();
        for(unordered_map<long int, InfTermDoc>::const_iterator it = l_docs.begin(); it != l_docs.end(); it++){
            for(unordered_map<string, streampos>::iterator itDoc = indiceDocsMinimo.begin(); itDoc != indiceDocsMinimo.end(); itDoc++){
                InfDoc infDoc;
                doc.seekg(itDoc->second);
                doc.read((char *)&infDoc, sizeof(infDoc));
                if(infDoc.getId() == it->first){
                    DevuelveDoc(itDoc->second, infDoc);
                    infDoc.addWord(it->second.get_ft());
                    infDoc.addWordNoStop();
                    infDoc.addWordDif();
                    indiceDocs[itDoc->first] = infDoc;
                    indiceDocsMinimo[itDoc->first] = -1;
                    GuardarInfDocEnDisco(infDoc, true, itDoc->first);
                    indiceDocs.clear();
                }
            }
        }
        doc.close();
    }
    else{
        if(indice.find(word) != indice.end() && stopWords.find(word) != stopWords.end())
            return false;
        indice[word] = inf;
        unordered_map<long int, InfTermDoc> l_docs = inf.getMap();
        for(unordered_map<long int, InfTermDoc>::const_iterator it = l_docs.begin(); it != l_docs.end(); it++){
            for(unordered_map<string, InfDoc>::iterator itDoc = indiceDocs.begin(); itDoc != indiceDocs.end(); itDoc++){
                if(itDoc->second.getId() == it->first){
                    itDoc->second.addWord(it->second.get_ft());
                    itDoc->second.addWordNoStop();
                    itDoc->second.addWordDif();
                }
        }
    }
    }
    informacionColeccionDocs.addNumTotalPal(inf.get_ftc());
    informacionColeccionDocs.addNumTotalNoStop();
    informacionColeccionDocs.addNumTotalDif();
    return true;
}

void IndexadorHash::ListarPalParada() const{
    ifstream file;
    file.open(ficheroStopWords.c_str());
    if(!file){
        cerr << "ERROR: No se encuentra fichero stopwords" << endl;
        return;
    }
    string str;
    while(getline(file, str)){
        cout << str << endl;
    }
    file.close();
}

void IndexadorHash::IndexadorHash::ListarInfColeccDocs() const{
    cout << informacionColeccionDocs << endl;
}

void IndexadorHash::ListarTerminos() const{
    if(almacenarEnDisco){
        string word;
        InformacionTermino inf;
        for(unordered_map<string, streampos>::const_iterator it = indiceMinimo.begin(); it != indiceMinimo.end(); it++){
            word = it->first;
            Devuelve(word, inf);
            cout << it->first << "\t" << inf << endl;
        }
    }
    else
        for(unordered_map<string, InformacionTermino>::const_iterator it = (this->indice).begin(); it != this->indice.end(); it++)
            cout << it->first << "\t" << it->second << endl;
}

bool IndexadorHash::ListarTerminos(const string& nomDoc) const{
    if(almacenarEnDisco && indiceDocsMinimo.find(nomDoc) != indiceDocsMinimo.end()){
        InfDoc doc;
        DevuelveDoc(indiceDocsMinimo.at(nomDoc), doc);
        string word;
        InformacionTermino inf;
        for(unordered_map<string, streampos>::const_iterator it = indiceMinimo.begin(); it != indiceMinimo.end(); it++){
            word = it->first;
            Devuelve(word, inf);
            if(inf.CompruebaDocumento(doc.getId()))
                cout << it->first << "\t" << inf << endl;
        }
    }
    else if(indiceDocs.find(nomDoc) != indiceDocs.end()){
        long int id = indiceDocs.at(nomDoc).getId();
        for(unordered_map<string, InformacionTermino>::const_iterator it = (this->indice).begin(); it != this->indice.end(); it++){
            if((it->second).CompruebaDocumento(id))
                cout << it->first << "\t" << it->second << endl;
        }
        return true;
    }
    return false;
}

void IndexadorHash::ListarDocs() const{
    if(almacenarEnDisco){
        InfDoc inf;
        for(unordered_map<string, streampos>::const_iterator it = (this->indiceDocsMinimo).begin(); it != this->indiceDocsMinimo.end(); it++){
            DevuelveDoc(it->second, inf);
            cout << it->first << "\t" << inf << endl;
        }
    }   
    else{
        for(unordered_map<string, InfDoc>::const_iterator it = (this->indiceDocs).begin(); it != this->indiceDocs.end(); it++)
            cout << it->first << "\t" << it->second << endl;
    }
    
}

bool IndexadorHash::ListarDocs(const string& nomDoc) const{
    if(almacenarEnDisco && this->indiceDocsMinimo.find(nomDoc) != this->indiceDocsMinimo.end()){
        InfDoc doc;
        DevuelveDoc(indiceDocsMinimo.at(nomDoc), doc);
        cout << nomDoc << "\t" << doc << endl;
        return true;
    }
    else if(this->indiceDocs.find(nomDoc) != this->indiceDocs.end()){
        cout << nomDoc << "\t" << this->indiceDocs.at(nomDoc) << endl;
        return true;
    }
    return false;
}

ostream& operator<<(ostream& s, const IndexadorHash& p) {
    s << "Fichero con el listado de palabras de parada: " << p.ficheroStopWords << endl;
    s << "Tokenizador: " << p.tok << endl;
    s << "Directorio donde se almacenara el indice generado: " << p.directorioIndice << endl;
    s << "Stemmer utilizado: " << p.tipoStemmer << endl;
    s << "Informacion de la  coleccion indexada: " << p.informacionColeccionDocs << endl;
    s << "Se almacenara parte del indice en disco duro: " << p.almacenarEnDisco << endl;
    s << "Se almacenaran las posiciones de los terminos: " << p.almacenarPosTerm;
    return s;
}