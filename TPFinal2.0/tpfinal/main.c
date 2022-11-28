#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <windows.h>
#include <locale.h>
#include <ctype.h>
#define TAM_MAX 6000

///Estructuras
typedef struct {
  char palabra[20];
  int idDOC;
  int pos; //incrementa palabra por palabra, y no letra por letra
}termino;

typedef struct nodoT
{
    int idDOC;
    int pos;
    struct nodoT* sig;
}nodoT;

typedef struct nodoA
{
   char palabra[20];
   int frecuencia; //representa la cantidad de nodos de la lista
   nodoT* ocurrencias; //ordenada por idDOC, luego por pos
   struct nodoA* der;
   struct nodoA* izq;
}nodoA;

typedef struct{
char palabra[20];
int frecuencia;
}datoPalabra;

///Funciones diccionario
int compararSimbolos(char);
int compararSimbolos(char letra){
    char simbolos[] =",. :;])!?-}([{\n\t";
    for (int i = 0; i < 15; i++)
    {
        if (letra == simbolos[i])
        {
            return 0;///Termina la palabra
        }
    }
    return 1;///No termina la palabra
}

void muestraDiccionario();
void muestraDiccionario()
{
    termino arch;
    FILE* fp = fopen("diccionario.bin","rb");
    if(fp)
    {
    while(fread(&arch,sizeof(termino),1,fp) > 0)
    {
        printf("ID: %i. POS: %i %s\n",arch.idDOC,arch.pos,arch.palabra);
    }
    fclose(fp);
    }

}

int deteccionError(char[]);
int deteccionError(char palabra[]){
    termino error;
    FILE*fp=fopen("error.bin","rb");
    if(fp){
        fread(&error,sizeof(termino),1,fp);
        fclose(fp);
    }
    if(strcmp(error.palabra,palabra)==0){
        return 1;
    }
    return 0;
}

void agregarAlDiccionario(termino diccionario[],int* validosD,char archivo[], int validosA, int idDoc)
{
    int i = 0, j = 0, pos=1;
    char palabra[20];
    while (i < validosA)
    {
        while (i < validosA && compararSimbolos(archivo[i]) != 0)
        {
            palabra[j] = archivo[i];
            i++;
            j++;
        }
        palabra[j] = '\0';
        if(deteccionError(palabra)==0){
            strcpy(diccionario[*validosD].palabra,palabra);
            diccionario[*validosD].pos = pos;
            diccionario[*validosD].idDOC = idDoc;
            pos++;
            (*validosD)++;
        }
        j = 0;
        while(i<validosA && compararSimbolos(archivo[i]) == 0)
        {
            i++;
        }
    }
    //muestraArray(diccionario,*validosD);
        ///printf("%s",diccionario[(*validosD)-10].palabra);
}
void persistenciaDiccionario(termino[],int);
void persistenciaDiccionario(termino diccionario[],int validosD){
    FILE*fp=fopen("diccionario.bin","a+b");
    if(fp){
        fwrite(diccionario,sizeof(termino),validosD,fp);
        fclose(fp);
    }
}

int cargaString(termino diccionario[], int* validosD, char archivo[], int idDoc) ///Funcion para separar las palabras del texto
{
        FILE* fp = fopen(archivo,"rb");
        char textoentero[TAM_MAX];
        int validosA= 0;
        if (fp)
        {
            while(fread(&textoentero[validosA],sizeof(char),1,fp) > 0)
            {
                    validosA++;
            }
                agregarAlDiccionario(diccionario,validosD,textoentero,validosA,idDoc);
                return 1;
        }
        else
        {
                printf("ERROR: EL ARCHIVO NO SE ENCONTRO O NO SE ABRIO CORRECTAMENTE. INTENTE NUEVAMENTE\n");
                return 0;
        }
}

int ultimoIdDoc();
int ultimoIdDoc(){
    FILE*fp=fopen("registroArchivos.bin","rb");
    if(fp){
        int i;
        fread(&i,sizeof(int),1,fp);
        fclose(fp);
        return i+1;
    }else{
        return 0;
    }
}
void cargaUltIdDoc(int);
void cargaUltIdDoc(int idDoc){
    FILE*fp=fopen("registroArchivos.bin","wb");
    if(fp){
        fwrite(&idDoc,sizeof(int),1,fp);
        fclose(fp);
    }
}

void Consulta_Archivo(termino diccionario[], int* validos_d)///Funcion para agregar palabras de un archivo de texto al diccionario
{
    char archivo[TAM_MAX];
    char seguir;
    int idDoc = 0,flag;
    do
    {
        printf("INGRESE UN TEXTO VALIDO PARA CARGAR AL DICCIONARIO\n");
        fflush(stdin);
        gets(archivo);
        idDoc=ultimoIdDoc();
        flag=cargaString(diccionario,validos_d,archivo,idDoc);
        if(flag==1){
            cargaUltIdDoc(idDoc);
        }
        printf("INGRESE 'S' PARA CARGAR OTRO ARCHIVO\n");
        fflush(stdin);
        scanf("%c",&seguir);

    }while(seguir == 's' || seguir=='S');
}
///MOTOR DE BUSQUEDA

nodoA* crearNodoA(char[]);
nodoA* crearNodoA(char palabra[])
{
    nodoA* nuevo = (nodoA*)malloc(sizeof(nodoA));
    nuevo->der = NULL;
    nuevo->izq = NULL;
    nuevo->frecuencia = 1;
    strcpy(nuevo->palabra,palabra);
    nuevo->ocurrencias = NULL;
    return nuevo;
}

nodoT* crearNodoT(int,int);
nodoT* crearNodoT(int idDoc,int pos)
{
    nodoT* nuevo = (nodoT*)malloc(sizeof(nodoT));
    nuevo->idDOC = idDoc;
    nuevo->pos = pos;
    nuevo->sig = NULL;
    return nuevo;
}

void insercionOcurrencia(nodoT**,int,int);
void insercionOcurrencia(nodoT**ocurrencias,int idDoc,int pos){
    nodoT*nuevo=crearNodoT(idDoc,pos);
    if((*ocurrencias)==NULL){
        (*ocurrencias)=nuevo;
    }else{
        if((*ocurrencias)->idDOC>idDoc){
            nuevo->sig=(*ocurrencias);
            (*ocurrencias)=nuevo;
        }else{
            nodoT*ante,*seg;
            ante=(*ocurrencias);
            seg=(*ocurrencias)->sig;
            while(seg!=NULL && seg->idDOC<idDoc){
                ante=seg;
                seg=seg->sig;
            }
            if(seg==NULL){
                ante->sig=nuevo;
            }else{
                if(seg->idDOC!=idDoc){
                    ante->sig=nuevo;
                    nuevo->sig=seg;
                }else{
                    while(seg!=NULL && seg->idDOC==idDoc && seg->pos<pos){
                        ante=seg;
                        seg=seg->sig;
                    }
                    ante->sig=nuevo;
                    nuevo->sig=seg;
                }

            }
        }
    }
}



nodoA*existeNodo(nodoA*,char[]);
nodoA*existeNodo(nodoA* arbol,char dato[])
{
    nodoA* found = NULL;
    if (arbol != NULL)
    {
        if (strcmp(arbol->palabra,dato) == 0)
        {
            found = arbol;
        }
        else
        {
            found = existeNodo(arbol->izq,dato);
            if (!found)
            {
                found = existeNodo(arbol->der,dato);
            }
        }
    }

    return found;
}


void insertarABB(nodoA**arbol,char palabra[],int i, int j)
{
    if (*arbol != NULL)
    {

        if(palabra[i]!='\0' && (*arbol)->palabra[j]!='\0'){
            if(palabra[i]!=(*arbol)->palabra[j]){
                if (palabra[i]<(*arbol)->palabra[j]) //si el valor de la raiz es mas grande, me muevo a izquierda
                {
                    insertarABB(&((*arbol)->izq),palabra,0,0);
                }
                else //si el valor de la raiz es menor, me muevo a derecha
                {
                    if ((*arbol)->palabra[j] < palabra[i])
                    {
                        insertarABB(&((*arbol)->der),palabra,0,0);
                    }
                }
            }else{
                insertarABB(arbol,palabra,i+1,j+1);
            }
        }else{
            if(palabra[i]=='\0'){
                insertarABB(&((*arbol)->izq),palabra,0,0);
            }else{
                insertarABB(&((*arbol)->der),palabra,0,0);
            }
        }
    }else{
        (*arbol)=crearNodoA(palabra);
    }
}

void minusculas(char*);
void minusculas(char*str){
    int lenght;
    lenght=strlen(str);
    for(int i=0;i<lenght;i++){
        str[i]=tolower(str[i]);
    }
}

void ordenLexicografico(nodoA**,termino);
void ordenLexicografico(nodoA**arbol,termino dato){
    minusculas(dato.palabra);
    nodoA*buscado=existeNodo((*arbol),dato.palabra);
    if(buscado!=NULL){
        insercionOcurrencia(&(buscado->ocurrencias),dato.idDOC,dato.pos);
        (buscado)->frecuencia++;
    }else{

        minusculas(dato.palabra);
        insertarABB(arbol,dato.palabra,0,0);
        buscado=existeNodo((*arbol),dato.palabra);
        insercionOcurrencia(&(buscado->ocurrencias),dato.idDOC,dato.pos);

    }
}


void despersistenciaDiccionario(nodoA**);
void despersistenciaDiccionario(nodoA** arbol)
{
    FILE* fp = fopen("diccionario.bin","rb");
    termino datoArch;
    if (fp)
    {
        while(fread(&datoArch,sizeof(termino),1,fp)> 0)
        {
            ordenLexicografico(arbol,datoArch);
        }
        fclose(fp);
    }else{
        printf("ERROR: DICCIONARIO NO ENCONTRADO\n");
    }

}


void inorder(nodoA* A){
    if (A){
        inorder(A->izq);
        printf("WORD: %s FRECUENCIA: %i\n",A->palabra,A->frecuencia);
        inorder(A->der);
    }
}

void recorrerLista(nodoA*,nodoT*);
void recorrerLista(nodoA*arbol,nodoT*ocurrenciass){
    printf("PALABRA: %s\n",arbol->palabra);
    while(ocurrenciass!=NULL){
        printf("ID DOC: %i, POS: %i \n",ocurrenciass->idDOC,ocurrenciass->pos);
        ocurrenciass=ocurrenciass->sig;
    }

}

///USER FUNCTIONS
    /**PUNTO 1**/
void buscarOcurrencia(nodoA* Arbol, char palabra[])
{
    nodoA* aux = existeNodo(Arbol,palabra);
    if(aux != NULL)
    {
        nodoT* lista = aux->ocurrencias;
        while(lista!= NULL)
        {
            printf("LA PALABRA %s APARECE EN EL DOC NRO.: %i, POSICION: %i\n",palabra,lista->idDOC,lista->pos);
            lista=lista->sig;
        }
    }
    else
    {
        printf("ERROR: PALABRA NO ENCONTRADA\n");
    }
}

void consultaPalabra(nodoA* Arbol)
{
    char palabra[27];
    printf("INGRESE UNA PALABRA\n");
    fflush(stdin);
    gets(palabra);
    buscarOcurrencia(Arbol,palabra);
}

    /**PUNTO 2**/
void comparacionOcurrencias(nodoA* arbol,char palabra[],int id1, int id2)
{
    nodoA* encontrado = existeNodo(arbol,palabra);
    int flag = 0,auxPos;
    nodoT*seg;
    if (encontrado)
    {
        nodoT* lista = encontrado->ocurrencias;
        seg = lista;
        while (seg != NULL && seg->idDOC != id1)
        {
            seg = seg->sig;
        }
        if ( seg != NULL && seg->idDOC == id1)
        {
            auxPos=seg->pos;
            seg = lista;
            while(seg != NULL && seg->idDOC != id2)
            {
                seg = seg->sig;
            }
            if(seg != NULL && seg->idDOC == id2)
            {
                flag = 1;
            }


        }
    }
    if (flag == 1)
    {
        printf("LA PALABRA %s SE ENCONTRO EN:\nARCHIVO: %i, POS: %i\nARCHIVO: %i, POS: %i\n",palabra,id1,auxPos,id2,seg->pos);
    }
    else
    {
        printf("ERROR: NO SE ENCONTRO LA PALABRA EN AMBOS ARCHIVOS\n");
    }

}
void interseccionArchivos(nodoA* arbol)
{
    FILE* fp = fopen("registroArchivos.bin","rb");
    int tam;
    int arch1;
    int arch2;
    char palabra[TAM_MAX];
    if(fp)
    {
        fread(&tam,sizeof(int),1,fp);
        if(tam>0){
            printf("INGRESE UNA PALABRA A BUSCAR\n");
            fflush(stdin);
            gets(palabra);
            printf("ELIJA UN ARCHIVO ENTRE 0 Y %i\n",tam);
            scanf("%i",&arch1);
            printf("ELIJA UN ARCHIVO ENTRE 0 Y %i\n",tam);
            scanf("%i",&arch2);
            comparacionOcurrencias(arbol,palabra,arch1,arch2);
        }else{
            printf("ERROR: NO SE PUEDEN COMPARAR ARCHIVOS PORQUE HAY UNO SOLO CARGADO.\nPOR FAVOR CARGUE MAS ARCHIVOS.\n");
        }
    }else{
        printf("ERROR: NO HAY ARCHIVOS CARGADOS.\nPOR FAVOR CARGUE MAS ARCHIVOS.\n");
    }
}
    /**PUNTO 3**/
int lecturaIdMax();
int lecturaIdMax(){
    FILE* fp = fopen("registroArchivos.bin","rb");
    if(fp)
    {
        int idMax;
        fread(&idMax,sizeof(int),1,fp);
        return idMax;
    }
    else
    {
        return -1;
    }
}

void ocurrenciasEnArchivo(nodoT*,int);
void ocurrenciasEnArchivo(nodoT*lista,int id){
    if(lista!=NULL){
        if(lista->idDOC==id){
            printf("POS: %i\n",lista->pos);
        }
        ocurrenciasEnArchivo(lista->sig,id);
    }
}

void busquedasMasTerminosArbol(nodoA*,char[],int);
void busquedasMasTerminosArbol(nodoA* Arbol,char palabra[], int id)
{
    if (Arbol != NULL)///Como la palabra no se repite, recorro el arbol hasta encontrarlo, luego printeo las ocurrencias que coinciden con el id
    {
        if(strcmp(Arbol->palabra,palabra) == 0)
        {
            printf("MUESTRA DE LA PALABRA %s EN EL ARCHIVO %i.\n",palabra,id);
            nodoT* lista = Arbol->ocurrencias;
            ocurrenciasEnArchivo(lista,id);
        }
        else
        {
            busquedasMasTerminosArbol(Arbol->izq,palabra,id);
            busquedasMasTerminosArbol(Arbol->der,palabra,id);
        }
    }
}

void busquedasMasTerminos(nodoA*);
void busquedasMasTerminos(nodoA* Arbol)///No la llegue a testear del todo srry
{
    int idmax = lecturaIdMax();///Leo id max de la despersistencia del archivo, le pregunto al usuario el id que quiera buscar y la palabra, luego mando esas variables x parametro a la otra funcion
    if(idmax==-1){
        printf("ERROR: NO HAY ARCHIVOS CARGADOS\n");
    }else{
        int id;
        char palabra[TAM_MAX];
        char seguir;
        printf("INGRESE ID A BUSCAR ENTRE 0 Y %i: \n",idmax);
        scanf("%i",&id);
        if(id<=idmax){
            do
            {
                printf("INGRESE LA PALABRA A BUSCAR\n");
                fflush(stdin);
                gets(palabra);
                if(existeNodo(Arbol,palabra)){
                    busquedasMasTerminosArbol(Arbol,palabra,id);
                }else{
                    printf("ERROR: LA PALABRA %s NO EXISTE\n",palabra);
                }
                printf("PRESIONE 'S' PARA CONTINUAR, 'N' PARA CANCELAR\n");
                fflush(stdin);
                scanf("%c",&seguir);
            }while (seguir == 's' || seguir == 'S');
        } else{
            printf("ERROR: ARCHIVO INVALIDO\n");
        }
    }
}

/**PUNTO 4**/

int cmpS(char,char);
int cmpS(char letra,char letra2){
    char simbolos[] =",.:;])!?-}([{\n\t";
    for (int i = 0; i < 14; i++)
    {
        if (letra == simbolos[i])
        {
            return 0;///Termina la palabra
        }
        if(letra==' '){
            if(letra2==' '){
                return 0;
            }
        }
    }
    return 1;///No termina la palabra
}

void fromFToW(char[],char[],int*);
void fromFToW(char word[],char frase[],int*i){
    int c=0;
    while(compararSimbolos(frase[*i])==1 && frase[*i]!='\0'){
        word[c]=frase[*i];
        c++;
        (*i)++;
    }
    word[c]='\0';
    (*i)++;
}

void eliminarSimbolosF(char[]);
void eliminarSimbolosF(char frase[]){
    int len=strlen(frase);
    len--;
    while(len>-1 && compararSimbolos(frase[len])!=1){
        if(compararSimbolos(frase[len])==0){
            frase[len]='\0';
        }
        len--;
    }
}

int cantPalabrasFrase(char[],int);
int cantPalabrasFrase(char frase[],int val){
    int contador=0;
    val++;
    for(int i=0;i<val;i++){
        if(compararSimbolos(frase[i])==0){
            contador++;
        }
    }
    contador++;
    if(compararSimbolos(frase[strlen(frase)-1])==0){
        contador--;
    }
    return contador;
}

int compOcurrencias(nodoT*,int,int);
int compOcurrencias(nodoT*ocurrStr2,int idDoc,int pos){
    while(ocurrStr2!=NULL){
        if(ocurrStr2->idDOC==idDoc && ocurrStr2->pos==pos){
            return 1;
        }
        ocurrStr2=ocurrStr2->sig;
    }
    return 0;
}

void idYPos(nodoT*,int*,int*);
void idYPos(nodoT*lista,int*id,int*pos){
        (*id)=lista->idDOC;
        (*pos)=lista->pos;
}

int especiales(char[]);
int especiales(char frase[]){
    int len=strlen(frase),i=0;
    while(i<len){
        if(cmpS(frase[i],frase[i+1])!=1){
            return 0;
        }
        i++;
    }
    return 1;
}

void buscarFrase(nodoA*arbol);
void buscarFrase(nodoA*arbol){
    char frase[TAM_MAX],word[TAM_MAX];
    int cantPalabras,c=0,flag=1,flag2=1,encontrado=0,j=0,id,pos;
    nodoA*str1,*str2;
    printf("INGRESE LA FRASE A BUSCAR\n");
    fflush(stdin);
    gets(frase);
    minusculas(frase);
    eliminarSimbolosF(frase);
    if(especiales(frase)==0){
        printf("POR FAVOR, REINGRESE LA FRASE SIN SIMBOLOS NI ESPACIOS DOBLES\n");
    }else{
        cantPalabras=cantPalabrasFrase(frase,strlen(frase));
        if(cantPalabras<2){
            fromFToW(word,frase,&c);
            str1=existeNodo(arbol,word);
            if(str1){
                nodoT*lista=str1->ocurrencias;
                while(lista!=NULL){
                    printf("LA FRASE SE HA ENCONTRADO EN EL DOCUMENTO %i, A PARTIR DE LA POSICION %i\n",lista->idDOC,lista->pos);
                    lista=lista->sig;
                }
            }else{
                if(compararSimbolos(word[0])==0 || word[0]=='\0'){
                    printf("ERROR: POR FAVOR INGRESE UNA FRASE\n");
                }else{
                    printf("ERROR: NO SE ENCONTRO LA FRASE\nLA PALABRA: %s NO EXISTE\n",word);
                }
            }
        }else{
            fromFToW(word,frase,&c);
            str1=existeNodo(arbol,word);
            nodoT*lista=NULL;
            if(str1){
                lista=str1->ocurrencias;
                idYPos(lista,&id,&pos);
                c=0;
            }else{
                printf("ERROR: NO SE ENCONTRO LA FRASE\nLA PALABRA: %s NO EXISTE\n",word);
                encontrado=1;
            }
            while(lista!=NULL && flag2==1){
                while(flag==1 && j<(cantPalabras-1)){
                    fromFToW(word,frase,&c);
                    str1=existeNodo(arbol,word);
                    if(str1){
                        fromFToW(word,frase,&c);
                        str2=existeNodo(arbol,word);
                        if(str2){
                            flag=compOcurrencias(str2->ocurrencias,id,pos+1);
                            if(flag==1){
                                c-=(strlen(str2->palabra)+1);
                                pos++;
                            }else{
                                flag=0;
                            }
                        }else{
                            printf("ERROR: NO SE ENCONTRO LA FRASE\nLA PALABRA: %s NO EXISTE\n",word);
                            flag=0;
                            flag2=0;
                            encontrado=1;
                        }
                    }else{
                        printf("ERROR: NO SE ENCONTRO LA FRASE\nLA PALABRA: %s NO EXISTE\n",word);
                        flag=0;
                        flag2=0;
                        encontrado=1;
                    }
                    j++;
                }
                if(flag==1 && j>0){
                    encontrado=1;
                    printf("LA FRASE SE HA ENCONTRADO EN EL DOCUMENTO %i, A PARTIR DE LA POSICION %i\n",id,pos-(cantPalabras-1));
                }
                flag=1;
                j=0;
                c=0;
                lista=lista->sig;
                if(lista){
                    id=lista->idDOC;
                    pos=lista->pos;
                }
            }
            if(encontrado==0 && flag2==1){
                printf("LA FRASE INGRESADA NO EXISTE\n");
            }

        }
    }
}


/**PUNTO 5**/
int contarOcurrencias(nodoT* lista, int idDoc)
{
    int i = 0;
    while(lista != NULL)
    {
        if (lista->idDOC == idDoc)
        {
            i++;
        }
        lista = lista->sig;
    }
    return i;
}

datoPalabra palabraMasFrecuente(nodoA* arbol, int id){
    datoPalabra aux;
    datoPalabra maxder;
    datoPalabra maxizq;
    int num;
    strcpy(aux.palabra,"");
    aux.frecuencia = 0;
    if(arbol != NULL)
    {
        num = contarOcurrencias(arbol->ocurrencias,id);
        if(num != 0)
        {
            strcpy(aux.palabra,arbol->palabra);
            aux.frecuencia = num;
            if (arbol->der != NULL)
            {
                maxder = palabraMasFrecuente(arbol->der,id);
                if(aux.frecuencia> maxder.frecuencia)
                {
                    maxder = aux;
                }
            }
            else
            {
                maxder = aux;
            }
            if(arbol->izq != NULL)
            {
                maxizq = palabraMasFrecuente(arbol->izq,id);
                if(aux.frecuencia > maxizq.frecuencia)
                {
                    maxizq = aux;
                }
            }
            else
            {
                maxizq = aux;
            }
            if(maxder.frecuencia > maxizq.frecuencia)
            {
                return maxder;
            }
            else
            {
                return maxizq;
            }

        }
        else
        {
            maxder=palabraMasFrecuente(arbol->der,id);
            maxizq=palabraMasFrecuente(arbol->izq,id);
            if(maxder.frecuencia > maxizq.frecuencia)
            {
                return maxder;
            }
            else
            {
                return maxizq;
            }
        }


    }
    else
    {
        return aux;
    }
}

void consultaMasBuscado(nodoA* arbol)
{
    int id;
    printf("INGRESE UN ID\n");
    scanf("%i",&id);
    if(id<ultimoIdDoc()){
        datoPalabra max = palabraMasFrecuente(arbol,id);
        printf("LA PALABRA %s SE REPITE %i VECES EN EL ARCHIVO NRO: %i\n",max.palabra,max.frecuencia,id);
    }else{
        printf("ERROR: INGRESE UN ID VALIDO\n");
    }
}

/**PUNTO 6**/
int Minimo(int a,int b)
{
   if (a < b )
       return a;
   else
      return b;
}

int Levenshtein(char *s1,char *s2)
{
    int t1,t2,i,j,*m,costo,res,ancho;

// Calcula tamanios strings
    t1=strlen(s1);
    t2=strlen(s2);

// Verifica que exista algo que comparar
    if (t1==0) return(t2);
    if (t2==0) return(t1);
    ancho=t1+1;

// Reserva matriz con malloc                     m[i,j] = m[j*ancho+i] !!
    m=(int*)malloc(sizeof(int)*(t1+1)*(t2+1));
    if (m==NULL) return(-1); // ERROR!!

// Rellena primera fila y primera columna
    for (i=0; i<=t1; i++) m[i]=i;
    for (j=0; j<=t2; j++) m[j*ancho]=j;

// Recorremos resto de la matriz llenando pesos
    for (i=1; i<=t1; i++) for (j=1; j<=t2; j++)
        {
            if (s1[i-1]==s2[j-1]) costo=0;
            else costo=1;
            m[j*ancho+i]=Minimo(Minimo(m[j*ancho+i-1]+1,     // Eliminacion
                                       m[(j-1)*ancho+i]+1),              // Insercion
                                m[(j-1)*ancho+i-1]+costo);
        }      // Sustitucion

// Devolvemos esquina final de la matriz
    res=m[t2*ancho+t1];
    free(m);
    return(res);
}
void Coincidencia(nodoA* Arbol, char palabra[])///P6 Comparo las palabras de los arboles con la palabra mal escrita para encontrar semejanzas
{
    if (Arbol != NULL)
    {
        Coincidencia(Arbol->izq,palabra);
        if(Levenshtein(Arbol->palabra,palabra) <=3)///Una vez encuentro la semejanza la printeo
                                                   ///no estoy seguro si puede haber mas de 1 asi que recorro todo el arbol independientemente si la encuentro o no
        {
                printf("QUIZAS QUISISTE DECIR: %s\n",Arbol->palabra);
        }
        Coincidencia(Arbol->der,palabra);
    }
}
void llamadaCoincidencia(nodoA* Arbol)
{
    char palabra[27];
    printf("INGRESE PALABRA\n");
    fflush(stdin);
    gets(palabra);
    minusculas(palabra);
    if(existeNodo(Arbol,palabra)){
        printf("PALABRA: %s ENCONTRADA\n",palabra);
    }else{
        Coincidencia(Arbol,palabra);
    }
}


///AAAAAAAAAAAAAAAAAAAAAAAAA
int main(){
    termino diccionario[TAM_MAX];
    int validosD=0,existe,opcion=0;
    nodoA* MotorBusq = NULL;
    do
    {
        printf("-----------Motor de busqueda-----------\n");
        printf("[1] CARGAR ARCHIVO DE TEXTO\n"); ///CONSULTA_ARCHIVO Y PERSISTENCIA DICCIONARIO
        printf("[2] MUESTRA DICCIONARIO\n"); ///PRINTEA EL DICCIONARIO
        printf("[3] CARGAR MOTOR\n"); ///CARGA EL MOTOR. ACTIVA FLAG PARA EL RESTO DE OPCIONES
        printf("[4] BUSCAR TODAS LAS APARICIONES DE UN TERMINO EN ALGUN DOCUMENTO\n");
        printf("[5] BUSCAR TODAS LAS APARICIONES DE UN TERMINO EN UN DOCUMENTO Y OTROS\n");
        printf("[6] BUSCAR LA APARICION DE MAS DE UN TERMINO EN EL MISMO DOCUMENTO\n");
        printf("[7] BUSCAR UNA FRASE COMPLETA\n");
        printf("[8] BUSCAR LA PALABRA DE MAS FRECUENCIA QUE APARECE EN UN DOC\n");
        printf("[9] INGRESAR UNA PALABRA Y BUSCAR SIMILARES\n");
        printf("\n[0] SALIR DEL MENU\n");
        scanf("%i",&opcion);
        switch (opcion)
        {
            case 1:
                system("cls");
                if(validosD>0){
                    validosD=0;
                }
                Consulta_Archivo(diccionario,&validosD);
                persistenciaDiccionario(diccionario,validosD);
                printf("Presione una tecla para continuar\n>");
                getch();
                system("cls");
            break;

            case 2:
     //           consultaPalabra(MotorBusq); ///Busq palabra
                system("cls");
                existe=ultimoIdDoc();
                if(existe!=0){
                    muestraDiccionario();
                }else{
                    printf("ERROR: CARGUE ARCHIVOS AL DICCIONARIO. OPCION [1] DEL MENU\n");
                }
                printf("PRESIONE UNA TECLA PARA CONTINUAR\n");
                getch();
                system("cls");
            break;

            case 3:
                //interseccionArchivos(MotorBusq);///Interseccion entre 2 archivos elegidos por el usuario
                system("cls");
                if(MotorBusq){
                    MotorBusq=NULL;
                    despersistenciaDiccionario(&MotorBusq);
                }else{
                    despersistenciaDiccionario(&MotorBusq);
                }
                //flagM=1;
                inorder(MotorBusq);
                printf("\nMOTOR CARGADO CON EXITO!\n");
                printf("PRESIONE UNA TECLA PARA CONTINUAR\n");
                getch();
                system("cls");
            break;

            case 4:
                system("cls");
                if(MotorBusq){
                    consultaPalabra(MotorBusq);
                }else{
                    printf("ERROR: CARGUE EL MOTOR DE BUSQUEDA. OPCION [3] DEL MENU\n");
                }
                printf("Presione una tecla para continuar\n");
                getch();
                system("cls");
            break;

            case 5:
                system("cls");
                if(MotorBusq){
                    interseccionArchivos(MotorBusq);
                }else{
                    printf("ERROR: CARGUE EL MOTOR DE BUSQUEDA. OPCION [3] DEL MENU\n");
                }
                printf("Presione una tecla para continuar\n");
                getch();
                system("cls");
                ///p4
            break;

            case 6:
                system("cls");
                if(MotorBusq){
                    busquedasMasTerminos(MotorBusq);
                }else{
                    printf("ERROR: CARGUE EL MOTOR DE BUSQUEDA. OPCION [3] DEL MENU\n");
                }
                printf("Presione una tecla para continuar\n");
                getch();
                system("cls");
            break;
            case 7:
                system("cls");
                if(MotorBusq){
                    buscarFrase(MotorBusq);
                }else{
                    printf("ERROR: CARGUE EL MOTOR DE BUSQUEDA. OPCION [3] DEL MENU\n");
                }
                printf("Presione una tecla para continuar\n");
                getch();
                system("cls");
            break;
            case 8:
                system("cls");
                if(MotorBusq){
                    consultaMasBuscado(MotorBusq);
                }else{
                    printf("ERROR: CARGUE EL MOTOR DE BUSQUEDA. OPCION [3] DEL MENU\n");
                }
                printf("Presione una tecla para continuar\n");
                getch();
                system("cls");

            break;
            case 9:
                system("cls");
                if(MotorBusq){
                    llamadaCoincidencia(MotorBusq);
                }else{
                    printf("ERROR: CARGUE EL MOTOR DE BUSQUEDA. OPCION [3] DEL MENU\n");
                }
                printf("Presione una tecla para continuar\n");
                getch();
                system("cls");

            break;

        }
    }while(opcion!=0);


//    /**PUNTO 1**/
//    printf("\nPUNTO 1:\n");
//    consultaPalabra(MotorBusq);
//
//    /**PUNTO 2**/
//    printf("\nPUNTO 2:\n");
//    interseccionArchivos(MotorBusq);
//
//    /**PUNTO 3**/
//    printf("\nPUNTO 3:\n");
//    busquedasMasTerminos(MotorBusq);
//
//    /**PUNTO 4**/
//    printf("\nPUNTO 4:\n");
//    buscarFrase(MotorBusq);
//
//    /**PUNTO 5**/
//    printf("\nPUNTO 5:\n");
//    consultaMasBuscado(MotorBusq);
//
//    /**PUNTO 6**/
//    printf("\nPUNTO 6:\n");
//    llamadaCoincidencia(MotorBusq);

    return 0;
}

