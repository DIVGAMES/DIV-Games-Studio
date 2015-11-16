
#include <stdio.h>
#include <string.h>
#define GLOBALS
#include "div.h"

#define VERSION_ASCII 2

#define VERSION_INPUT 1

#define MARCA_CADENA_TIZO_ASCII 111333
#define MARCA_CADENA_TIZO_INPUT 333111

// Se guardan rutinas antiguas con 0 al final....
// Falta load y save multilinea.....
// Y poner comprobante de que es cadena de dll(de tizo)

 int *nuevopunteroascii,mitxt;
 FILE *ficherodisco;
 int nombre;
 char *irnombre;
 FILE *ficherolog;

void ascii_version(){

       retval(VERSION_ASCII);
}

void ascii_load(){

        char* caracter;
        int sitioiniciofichero;
        int nuevoascii;
        int *realpunteroascii;

        int numerotextos=getparm();
        int iniciolinea=getparm();
        int tablaguarda=getparm();
        nombre=getparm();

        if ((ficherodisco=div_fopen(irnombre,"rb"))!=NULL)
        {
                int contadorlinea=0;

                // Avanzamos a la linea indicada
                while (contadorlinea<iniciolinea && feof(ficherodisco)==NULL)
                {

                        while (*caracter!=13 && feof(ficherodisco)==NULL)
                                {
                                fread(caracter,1,1,ficherodisco);
                                }
                        if (feof(ficherodisco)!=NULL)
                        {
                                div_fclose(ficherodisco);
                                retval(-1);
                        }else
                        {
                                // Codigo 10 de ascii....
                                fread(caracter,1,1,ficherodisco);
                                contadorlinea++;
                        }
                }
                if (feof(ficherodisco)!=NULL){
                        div_fclose(ficherodisco);
                        retval(-1);
                }else
                {
                        // Hemos situado el fichero.....
                        // Ahora a coger la memoria...
                        // y despues la variable....
                        int lineasporleer=0;
                        while (lineasporleer<numerotextos && feof(ficherodisco)==NULL)                        
                        {
                                // guardamos la posicion del fichero....
                                sitioiniciofichero=ftell(ficherodisco);

                                // Contamos las letras
                                int cuentacaracteres=0;

                                while (*caracter!=13 && feof(ficherodisco)==NULL)
                                {
                                        fread(caracter,1,1,ficherodisco);                                       
                                        cuentacaracteres++;
                                }
                                
                                // A reservar memoria....
                                nuevopunteroascii=(int *)div_malloc(cuentacaracteres+9);
                                mitxt=(int)nuevopunteroascii[0];

                                int *empiezatexto;
                                //y calculamos el offset del nuevo puntero
                                empiezatexto=&mem[text_offset];
                                int primertxt;
                                primertxt=(int)empiezatexto[0];
                                mitxt+=3;
                                mitxt&=-4;
                                // Guarda valores internos....
                                realpunteroascii=(int *) mitxt;

                                // Ponemos la marca
                                *realpunteroascii=MARCA_CADENA_TIZO_ASCII;
                                realpunteroascii++;
                                mitxt=(int)realpunteroascii[0];
                                char * ptr;
                                ptr=(char *)mitxt;      
                
                                nuevoascii=(mitxt-primertxt)/4;

                                // Guardamos el valor, en el campo de la estructura...                                
                                mem[tablaguarda+lineasporleer]=nuevoascii;

                                // Colocamos el fichero en su sitio, una vez contado...
                                fseek(ficherodisco,sitioiniciofichero,SEEK_SET);

                                // Y cogemos toda la linea...
                                fread(ptr,1,cuentacaracteres,ficherodisco);                                       

                                // pone un cero al final....
                                ptr[cuentacaracteres-1]=0;

                                // Y otra linea....
                                lineasporleer++; 
                                // Leemos el enter y el 10....
                                fread(caracter,1,1,ficherodisco);                                
                        }
                        if (feof(ficherodisco)!=NULL){        
                                div_fclose(ficherodisco);
                                retval(1);
                        }else
                        {
                                div_fclose(ficherodisco);       
                                retval(-1);
                        }
               }
        }else{
                retval(-1);
        }
}

void ascii_free(){

   int pasavalor=getparm();

   nuevopunteroascii=&mem[text_offset+pasavalor];
   nuevopunteroascii--;
   if ( *nuevopunteroascii!=MARCA_CADENA_TIZO_ASCII &&
        *nuevopunteroascii!=MARCA_CADENA_TIZO_INPUT ) {
        retval(-1);
   }else{ 
        if (*nuevopunteroascii!=MARCA_CADENA_TIZO_INPUT) {
                nuevopunteroascii--;
                div_free(nuevopunteroascii);
                retval(1);
        }else{
                div_free(nuevopunteroascii);
                retval(1);
        }
   }
}

void ascii_save() {
  char *variable2;
  char *codigo_enter;
  int modosave=getparm();
  int variable=getparm();
  int nombre=getparm();
  irnombre=(char *)&mem[nombre+text_offset];
  if (modosave==0)
  {
        // Se crea el fichero nuevo....
        // ya se tiene el nombre....
        variable2=(char *)&mem[variable+text_offset];
        // Ahora grabamos el fichero.....
        if ((ficherodisco=div_fopen(irnombre,"ab"))!=NULL)
        {                
                fwrite(variable2,1,strlen(variable2),ficherodisco);
                // Se debe introducir un codigo enter 
                *codigo_enter=13;
                fwrite(codigo_enter,1,1,ficherodisco);
                *codigo_enter=10;
                fwrite(codigo_enter,1,1,ficherodisco);
                div_fclose(ficherodisco);
                retval(1);
        }else{ retval(-1);}
  }else{
        // Se crea el fichero nuevo....
        // ya se tiene el nombre....
        variable2=(char *)&mem[variable+text_offset];
        // Ahora grabamos el fichero.....
        if ((ficherodisco=div_fopen(irnombre,"wb"))!=NULL)
        {                
                fwrite(variable2,1,strlen(variable2),ficherodisco);
                // Se debe introducir un codigo enter 
                *codigo_enter=13;
                fwrite(codigo_enter,1,1,ficherodisco);
                *codigo_enter=10;
                fwrite(codigo_enter,1,1,ficherodisco);
                div_fclose(ficherodisco);
                retval(1);
        }else{ retval(-1);}

  }
}

void ascii_load0() { // Carga fichero formato ascii
  int contador;
  int nuevoascii;
  int *empiezatexto;

  nombre=getparm();
  // ya se tiene el nombre del archivo....
  irnombre=(char *)&mem[nombre+text_offset];
           

   // Ahora cargaremos el fichero.....
   if ((ficherodisco=div_fopen(irnombre,"rb"))!=NULL)
        {
               
                int longitudfichero;
                int primertxt;
                // Lo llevamos al final
                fseek(ficherodisco,0,SEEK_END);
                // lo medimos
                longitudfichero=ftell(ficherodisco);
                // y lo llevamos al principio
                fseek(ficherodisco,0,SEEK_SET);

                // A reservar memoria....
                nuevopunteroascii=(int *)div_malloc(longitudfichero+4);
                mitxt=(int)nuevopunteroascii[0];

                //y calculamos el offset del nuevo puntero
                empiezatexto=&mem[text_offset];
                primertxt=(int)empiezatexto[0];
                mitxt+=3;
                mitxt&=-4;
                char * ptr;
                ptr=(char*)mitxt;      
                nuevoascii=(mitxt-primertxt)/4;
                fread(ptr,1,longitudfichero,ficherodisco);
                div_fclose(ficherodisco);
                ptr[longitudfichero]=0;
                retval(nuevoascii);
        }
   else
        {
         retval(-1);
        }
}

void ascii_dup(){
        int primertxt;
        int nuevoascii;
        int *empiezatexto;
        int *realpunteroascii;

        char *texto1;
        int cantidaddetexto=getparm();
        int caracterelegido=getparm();


        // A reservar memoria....
        nuevopunteroascii=(int *)div_malloc(cantidaddetexto+8);
        mitxt=(int)nuevopunteroascii[0];
        //y calculamos el offset del nuevo puntero
        empiezatexto=&mem[text_offset];
        primertxt=(int)empiezatexto[0];
        mitxt+=3;
        mitxt&=-4;

        // puntero colocado....
        // Guarda valores internos....
        realpunteroascii=(int *) mitxt;

        *realpunteroascii=MARCA_CADENA_TIZO_ASCII ;
        realpunteroascii++;
        mitxt=(int)realpunteroascii[0];

        nuevoascii=(mitxt-primertxt)/4;
        char * ptr2;
        ptr2=(char*)mitxt;
        int contador;
        for (contador=0;contador<cantidaddetexto;contador++){
                ptr2[contador]=(char)caracterelegido;
        }
        ptr2[cantidaddetexto]=0;
        retval(nuevoascii);
}

void ascii_add()
{
        char *texto1;
        char *texto2;
        int primertxt;
        int nuevoascii;
        int *empiezatexto;
        int *realpunteroascii;

        int variable2=getparm();
        int variable1=getparm();
        // ya se tiene el nombre....
        texto1=(char *)&mem[variable1+text_offset];
        texto2=(char *)&mem[variable2+text_offset];
        // A reservar memoria....
        nuevopunteroascii=(int *)div_malloc(strlen(texto1)+strlen(texto2)+4);
        mitxt=(int)nuevopunteroascii;
        //y calculamos el offset del nuevo puntero
        empiezatexto=&mem[text_offset];
        primertxt=(int)empiezatexto;
        mitxt+=3;
        mitxt&=-4;

        // puntero colocado....
        // Guarda valores internos....
        realpunteroascii=(int *) mitxt;

        *realpunteroascii=MARCA_CADENA_TIZO_ASCII;
        realpunteroascii++;
        mitxt=(int)realpunteroascii;


        nuevoascii=(mitxt-primertxt)/4;
        char * ptr2;
        ptr2=(char*)mitxt;
        strcpy(ptr2,texto1);
        strcat(ptr2,texto2);
        retval(nuevoascii);
}
void ascii_paste()
{
        char *texto1;
        char *texto2;

        int posiciondeltexto=getparm();
        int variable2=getparm();
        int variable1=getparm();
        // ya se tiene el nombre....
        texto1=(char *)&mem[variable1+text_offset];
        texto2=(char *)&mem[variable2+text_offset];

        if (posiciondeltexto<0) retval(-1);
        if (posiciondeltexto>strlen(texto1)) retval(-1);
        if (strlen(texto1)<strlen(texto2)) retval(-1);
        if (strlen(texto2)+posiciondeltexto>strlen(texto1)) retval(-1);
        int contador;
        for (contador=0;contador<strlen(texto2);contador++){
        texto1[posiciondeltexto+contador]=texto2[contador];}
        
        retval(1);
}
void ascii_setchar()
{
        char *texto1;



        int variable2=getparm();
        int posiciondeltexto=getparm();
        int variable1=getparm();
        // ya se tiene el nombre....
        texto1=(char *)&mem[variable1+text_offset];


        if (posiciondeltexto<0) retval(-1);
        if (posiciondeltexto>strlen(texto1)) retval(-1);
        
        texto1[posiciondeltexto]=(char)variable2;

        retval(1);
}

void ascii_getchar()
{
        char caracter;
        int valorfinal;
        char *texto1;
        int posicion=getparm();
        int variable1=getparm();
        // ya se tiene el nombre....
        texto1=(char *)&mem[variable1+text_offset];
        if (posicion<0)retval(-1);
        if (posicion>strlen(texto1))retval(-1);
        caracter=texto1[posicion];
        valorfinal=(int)caracter;
        retval(valorfinal);
}
void ascii_len()
{
 
        int valorfinal;
        char *texto1;
        int variable1=getparm();
        // ya se tiene el nombre....
        texto1=(char *)&mem[variable1+text_offset];
        valorfinal=strlen(texto1);
        retval(valorfinal);
}

void ascii_copy()
{
        int primertxt;
        int nuevoascii;
        int *empiezatexto;
        int *realpunteroascii;

        char *texto1;
        int longitexto=getparm();
        int iniciotexto=getparm();
        int variable1=getparm();
        // ya se tiene el nombre....
        texto1=(char *)&mem[variable1+text_offset];

        if (longitexto<0)retval(-1);
        if (iniciotexto<0)retval(-1);
        if (iniciotexto>strlen(texto1))retval(-1);
        if (longitexto>strlen(texto1))retval(-1);
        if (iniciotexto+longitexto>strlen(texto1))retval(-1);

        // A reservar memoria....
        nuevopunteroascii=(int *)div_malloc(longitexto+8);
        mitxt=(int)nuevopunteroascii;
        //y calculamos el offset del nuevo puntero
        empiezatexto=&mem[text_offset];
        primertxt=(int)empiezatexto;
        mitxt+=3;
        mitxt&=-4;

        // puntero colocado....
        // Guarda valores internos....
        realpunteroascii=(int *) mitxt;

        *realpunteroascii=MARCA_CADENA_TIZO_ASCII;
        realpunteroascii++;
        mitxt=(int)realpunteroascii;

        nuevoascii=(mitxt-primertxt)/4;
        char * ptr2;
        ptr2=(char*)mitxt;
        int contador;
        for (contador=0;contador<longitexto;contador++){
                ptr2[contador]=texto1[iniciotexto+contador];
        }
        ptr2[longitexto]=0;
        retval(nuevoascii);

}

// Segunda tanda de funciones..........

void input_version(){
        retval(VERSION_INPUT);
}


void input_new()
{
        int primertxt;
        int nuevoascii;
        int *empiezatexto;
        char *textoameter;

        int *realpunteroascii;

        int variable1=getparm();

        // A reservar memoria....
        nuevopunteroascii=(int *)div_malloc(variable1+12);
        mitxt=(int)nuevopunteroascii;
        //y calculamos el offset del nuevo puntero
        empiezatexto=&mem[text_offset];
        primertxt=(int)empiezatexto;
        mitxt+=3;
        mitxt&=-4;

        // puntero colocado....
        // Guarda valores internos....
        realpunteroascii=(int *) mitxt;

        *realpunteroascii=variable1;
        realpunteroascii++;
        *realpunteroascii=MARCA_CADENA_TIZO_INPUT;
        realpunteroascii++;
        mitxt=(int)realpunteroascii;
        textoameter=(char *)mitxt;
        textoameter[0]=0;
        nuevoascii=(mitxt-primertxt)/4;
        retval(nuevoascii);

}
void input_free()
{

       int variable1=getparm();

       nuevopunteroascii=&mem[text_offset+variable1];
       nuevopunteroascii--;
       if ( *nuevopunteroascii!= MARCA_CADENA_TIZO_INPUT ){
                retval(-1);
       }else{ 
                nuevopunteroascii--;       
                div_free(nuevopunteroascii);
                retval(1);
       }
}
void input_add()
{
       int memoriareservada;
       char *textodeinput;
       int longitudtexto;

       int variable2=getparm();
       int variable1=getparm();

       nuevopunteroascii=&mem[text_offset+variable1];
       nuevopunteroascii--;
       if ( *nuevopunteroascii!= MARCA_CADENA_TIZO_INPUT ){
                retval(-1);
       }else{
                nuevopunteroascii--;
                memoriareservada=*nuevopunteroascii;
                nuevopunteroascii++;
                nuevopunteroascii++;
                textodeinput= (char *) nuevopunteroascii;
                longitudtexto=strlen(textodeinput);
                if (longitudtexto+1>memoriareservada){
                    retval(-1);
                }else{
                    textodeinput[longitudtexto]=(char) variable2;
                    textodeinput[longitudtexto+1]=0;
                    retval(1);
                }
                
       }
}
void input_delete()
{
       char *textoausar;
       int longitudtextousado;
       int variable1=getparm();

       nuevopunteroascii=&mem[text_offset+variable1];
       nuevopunteroascii--;
       if ( *nuevopunteroascii!= MARCA_CADENA_TIZO_INPUT ){
                retval(-1);
       }else{
              nuevopunteroascii++;
              textoausar=(char *) nuevopunteroascii;
              longitudtextousado=strlen(textoausar);
              if (longitudtextousado>0){
                        textoausar[longitudtextousado]=0;
                        textoausar[longitudtextousado-1]=0;
                        retval(1);
              }else{
                        retval(-1);
              }
      } 
}

void __export divlibrary(LIBRARY_PARAMS) {

  COM_export("ASCII_VERSION",ascii_version,0);  
  COM_export("ASCII_LOAD",ascii_load,4);
  COM_export("ASCII_SAVE",ascii_save,3);

  COM_export("ASCII_LOAD0",ascii_load0,1);

  COM_export("ASCII_FREE",ascii_free,1);
  COM_export("ASCII_DUP",ascii_dup,2);  
  COM_export("ASCII_ADD",ascii_add,2);
  COM_export("ASCII_PASTE",ascii_paste,3); 
  COM_export("ASCII_SETCHAR",ascii_setchar,3);
  COM_export("ASCII_GETCHAR",ascii_getchar,2);  
  COM_export("ASCII_COPY",ascii_copy,3);  
  COM_export("ASCII_LEN",ascii_len,1);  

  COM_export("INPUT_VERSION",input_version,0);
  COM_export("INPUT_NEW",input_new,1);
  COM_export("INPUT_FREE",input_free,1);
  COM_export("INPUT_ADD",input_add,2);
  COM_export("INPUT_DELETE",input_delete,1);                                        

}

void __export divmain(COMMON_PARAMS) {
  GLOBAL_IMPORT();
}

