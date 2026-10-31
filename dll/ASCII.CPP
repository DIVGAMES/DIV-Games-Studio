
#include <stdio.h>
#include <string.h>
#define GLOBALS
#include "div.h"

#define VERSION_ASCII 2

#define VERSION_INPUT 1

#define MARK_STRING_TIZO_ASCII 111333
#define MARK_STRING_TIZO_INPUT 333111

// Old routines are kept with 0 at the end....
// Multiline load and save still missing.....
// And add a check that it is a dll string (tizo's)

 int *newasciipointer,mitxt;
 FILE *diskfile;
 int name;
 char *inname;
 FILE *logfile;

void ascii_version(){

       retval(VERSION_ASCII);
}

void ascii_load(){

        char* character;
        int filestartpos;
        int newascii;
        int *realasciipointer;

        int numtexts=getparm();
        int linestart=getparm();
        int savetable=getparm();
        name=getparm();

        if ((diskfile=div_fopen(inname,"rb"))!=NULL)
        {
                int linecounter=0;

                // Advance to the given line
                while (linecounter<linestart && feof(diskfile)==NULL)
                {

                        while (*character!=13 && feof(diskfile)==NULL)
                                {
                                fread(character,1,1,diskfile);
                                }
                        if (feof(diskfile)!=NULL)
                        {
                                div_fclose(diskfile);
                                retval(-1);
                        }else
                        {
                                // ASCII code 10....
                                fread(character,1,1,diskfile);
                                linecounter++;
                        }
                }
                if (feof(diskfile)!=NULL){
                        div_fclose(diskfile);
                        retval(-1);
                }else
                {
                        // We have positioned the file.....
                        // Now to grab the memory...
                        // and then the variable....
                        int linestoread=0;
                        while (linestoread<numtexts && feof(diskfile)==NULL)                        
                        {
                                // save the file position....
                                filestartpos=ftell(diskfile);

                                // Count the letters
                                int charcount=0;

                                while (*character!=13 && feof(diskfile)==NULL)
                                {
                                        fread(character,1,1,diskfile);                                       
                                        charcount++;
                                }
                                
                                // Allocate memory....
                                newasciipointer=(int *)div_malloc(charcount+9);
                                mitxt=(int)newasciipointer[0];

                                int *textstart;
                                //and compute the offset of the new pointer
                                textstart=&mem[text_offset];
                                int firsttxt;
                                firsttxt=(int)textstart[0];
                                mitxt+=3;
                                mitxt&=-4;
                                // Save internal values....
                                realasciipointer=(int *) mitxt;

                                // Set the mark
                                *realasciipointer=MARK_STRING_TIZO_ASCII;
                                realasciipointer++;
                                mitxt=(int)realasciipointer[0];
                                char * ptr;
                                ptr=(char *)mitxt;      
                
                                newascii=(mitxt-firsttxt)/4;

                                // Save the value into the structure field...                                
                                mem[savetable+linestoread]=newascii;

                                // Put the file back in place, once counted...
                                fseek(diskfile,filestartpos,SEEK_SET);

                                // And grab the whole line...
                                fread(ptr,1,charcount,diskfile);                                       

                                // put a zero at the end....
                                ptr[charcount-1]=0;

                                // And another line....
                                linestoread++; 
                                // Read the enter and the 10....
                                fread(character,1,1,diskfile);                                
                        }
                        if (feof(diskfile)!=NULL){        
                                div_fclose(diskfile);
                                retval(1);
                        }else
                        {
                                div_fclose(diskfile);       
                                retval(-1);
                        }
               }
        }else{
                retval(-1);
        }
}

void ascii_free(){

   int passvalue=getparm();

   newasciipointer=&mem[text_offset+passvalue];
   newasciipointer--;
   if ( *newasciipointer!=MARK_STRING_TIZO_ASCII &&
        *newasciipointer!=MARK_STRING_TIZO_INPUT ) {
        retval(-1);
   }else{ 
        if (*newasciipointer!=MARK_STRING_TIZO_INPUT) {
                newasciipointer--;
                div_free(newasciipointer);
                retval(1);
        }else{
                div_free(newasciipointer);
                retval(1);
        }
   }
}

void ascii_save() {
  char *variable2;
  char *enter_code;
  int savemode=getparm();
  int variable=getparm();
  int name=getparm();
  inname=(char *)&mem[name+text_offset];
  if (savemode==0)
  {
        // Create the new file....
        // we already have the name....
        variable2=(char *)&mem[variable+text_offset];
        // Now write the file.....
        if ((diskfile=div_fopen(inname,"ab"))!=NULL)
        {                
                fwrite(variable2,1,strlen(variable2),diskfile);
                // An enter code must be entered 
                *enter_code=13;
                fwrite(enter_code,1,1,diskfile);
                *enter_code=10;
                fwrite(enter_code,1,1,diskfile);
                div_fclose(diskfile);
                retval(1);
        }else{ retval(-1);}
  }else{
        // Create the new file....
        // we already have the name....
        variable2=(char *)&mem[variable+text_offset];
        // Now write the file.....
        if ((diskfile=div_fopen(inname,"wb"))!=NULL)
        {                
                fwrite(variable2,1,strlen(variable2),diskfile);
                // An enter code must be entered 
                *enter_code=13;
                fwrite(enter_code,1,1,diskfile);
                *enter_code=10;
                fwrite(enter_code,1,1,diskfile);
                div_fclose(diskfile);
                retval(1);
        }else{ retval(-1);}

  }
}

void ascii_load0() { // Load file in ascii format
  int counter;
  int newascii;
  int *textstart;

  name=getparm();
  // we already have the file name....
  inname=(char *)&mem[name+text_offset];
           

   // Now load the file.....
   if ((diskfile=div_fopen(inname,"rb"))!=NULL)
        {
               
                int filelength;
                int firsttxt;
                // Move it to the end
                fseek(diskfile,0,SEEK_END);
                // measure it
                filelength=ftell(diskfile);
                // and move it back to the start
                fseek(diskfile,0,SEEK_SET);

                // Allocate memory....
                newasciipointer=(int *)div_malloc(filelength+4);
                mitxt=(int)newasciipointer[0];

                //and compute the offset of the new pointer
                textstart=&mem[text_offset];
                firsttxt=(int)textstart[0];
                mitxt+=3;
                mitxt&=-4;
                char * ptr;
                ptr=(char*)mitxt;      
                newascii=(mitxt-firsttxt)/4;
                fread(ptr,1,filelength,diskfile);
                div_fclose(diskfile);
                ptr[filelength]=0;
                retval(newascii);
        }
   else
        {
         retval(-1);
        }
}

void ascii_dup(){
        int firsttxt;
        int newascii;
        int *textstart;
        int *realasciipointer;

        char *text1;
        int textamount=getparm();
        int chosenchar=getparm();


        // Allocate memory....
        newasciipointer=(int *)div_malloc(textamount+8);
        mitxt=(int)newasciipointer[0];
        //and compute the offset of the new pointer
        textstart=&mem[text_offset];
        firsttxt=(int)textstart[0];
        mitxt+=3;
        mitxt&=-4;

        // pointer positioned....
        // Save internal values....
        realasciipointer=(int *) mitxt;

        *realasciipointer=MARK_STRING_TIZO_ASCII ;
        realasciipointer++;
        mitxt=(int)realasciipointer[0];

        newascii=(mitxt-firsttxt)/4;
        char * ptr2;
        ptr2=(char*)mitxt;
        int counter;
        for (counter=0;counter<textamount;counter++){
                ptr2[counter]=(char)chosenchar;
        }
        ptr2[textamount]=0;
        retval(newascii);
}

void ascii_add()
{
        char *text1;
        char *text2;
        int firsttxt;
        int newascii;
        int *textstart;
        int *realasciipointer;

        int variable2=getparm();
        int variable1=getparm();
        // we already have the name....
        text1=(char *)&mem[variable1+text_offset];
        text2=(char *)&mem[variable2+text_offset];
        // Allocate memory....
        newasciipointer=(int *)div_malloc(strlen(text1)+strlen(text2)+4);
        mitxt=(int)newasciipointer;
        //and compute the offset of the new pointer
        textstart=&mem[text_offset];
        firsttxt=(int)textstart;
        mitxt+=3;
        mitxt&=-4;

        // pointer positioned....
        // Save internal values....
        realasciipointer=(int *) mitxt;

        *realasciipointer=MARK_STRING_TIZO_ASCII;
        realasciipointer++;
        mitxt=(int)realasciipointer;


        newascii=(mitxt-firsttxt)/4;
        char * ptr2;
        ptr2=(char*)mitxt;
        strcpy(ptr2,text1);
        strcat(ptr2,text2);
        retval(newascii);
}
void ascii_paste()
{
        char *text1;
        char *text2;

        int textposition=getparm();
        int variable2=getparm();
        int variable1=getparm();
        // we already have the name....
        text1=(char *)&mem[variable1+text_offset];
        text2=(char *)&mem[variable2+text_offset];

        if (textposition<0) retval(-1);
        if (textposition>strlen(text1)) retval(-1);
        if (strlen(text1)<strlen(text2)) retval(-1);
        if (strlen(text2)+textposition>strlen(text1)) retval(-1);
        int counter;
        for (counter=0;counter<strlen(text2);counter++){
        text1[textposition+counter]=text2[counter];}
        
        retval(1);
}
void ascii_setchar()
{
        char *text1;



        int variable2=getparm();
        int textposition=getparm();
        int variable1=getparm();
        // we already have the name....
        text1=(char *)&mem[variable1+text_offset];


        if (textposition<0) retval(-1);
        if (textposition>strlen(text1)) retval(-1);
        
        text1[textposition]=(char)variable2;

        retval(1);
}

void ascii_getchar()
{
        char character;
        int finalvalue;
        char *text1;
        int position=getparm();
        int variable1=getparm();
        // we already have the name....
        text1=(char *)&mem[variable1+text_offset];
        if (position<0)retval(-1);
        if (position>strlen(text1))retval(-1);
        character=text1[position];
        finalvalue=(int)character;
        retval(finalvalue);
}
void ascii_len()
{
 
        int finalvalue;
        char *text1;
        int variable1=getparm();
        // we already have the name....
        text1=(char *)&mem[variable1+text_offset];
        finalvalue=strlen(text1);
        retval(finalvalue);
}

void ascii_copy()
{
        int firsttxt;
        int newascii;
        int *textstart;
        int *realasciipointer;

        char *text1;
        int textlength=getparm();
        int firsttext=getparm();
        int variable1=getparm();
        // we already have the name....
        text1=(char *)&mem[variable1+text_offset];

        if (textlength<0)retval(-1);
        if (firsttext<0)retval(-1);
        if (firsttext>strlen(text1))retval(-1);
        if (textlength>strlen(text1))retval(-1);
        if (firsttext+textlength>strlen(text1))retval(-1);

        // Allocate memory....
        newasciipointer=(int *)div_malloc(textlength+8);
        mitxt=(int)newasciipointer;
        //and compute the offset of the new pointer
        textstart=&mem[text_offset];
        firsttxt=(int)textstart;
        mitxt+=3;
        mitxt&=-4;

        // pointer positioned....
        // Save internal values....
        realasciipointer=(int *) mitxt;

        *realasciipointer=MARK_STRING_TIZO_ASCII;
        realasciipointer++;
        mitxt=(int)realasciipointer;

        newascii=(mitxt-firsttxt)/4;
        char * ptr2;
        ptr2=(char*)mitxt;
        int counter;
        for (counter=0;counter<textlength;counter++){
                ptr2[counter]=text1[firsttext+counter];
        }
        ptr2[textlength]=0;
        retval(newascii);

}

// Second batch of functions..........

void input_version(){
        retval(VERSION_INPUT);
}


void input_new()
{
        int firsttxt;
        int newascii;
        int *textstart;
        char *texttoinsert;

        int *realasciipointer;

        int variable1=getparm();

        // Allocate memory....
        newasciipointer=(int *)div_malloc(variable1+12);
        mitxt=(int)newasciipointer;
        //and compute the offset of the new pointer
        textstart=&mem[text_offset];
        firsttxt=(int)textstart;
        mitxt+=3;
        mitxt&=-4;

        // pointer positioned....
        // Save internal values....
        realasciipointer=(int *) mitxt;

        *realasciipointer=variable1;
        realasciipointer++;
        *realasciipointer=MARK_STRING_TIZO_INPUT;
        realasciipointer++;
        mitxt=(int)realasciipointer;
        texttoinsert=(char *)mitxt;
        texttoinsert[0]=0;
        newascii=(mitxt-firsttxt)/4;
        retval(newascii);

}
void input_free()
{

       int variable1=getparm();

       newasciipointer=&mem[text_offset+variable1];
       newasciipointer--;
       if ( *newasciipointer!= MARK_STRING_TIZO_INPUT ){
                retval(-1);
       }else{ 
                newasciipointer--;       
                div_free(newasciipointer);
                retval(1);
       }
}
void input_add()
{
       int reservedmemory;
       char *inputtext;
       int textlength;

       int variable2=getparm();
       int variable1=getparm();

       newasciipointer=&mem[text_offset+variable1];
       newasciipointer--;
       if ( *newasciipointer!= MARK_STRING_TIZO_INPUT ){
                retval(-1);
       }else{
                newasciipointer--;
                reservedmemory=*newasciipointer;
                newasciipointer++;
                newasciipointer++;
                inputtext= (char *) newasciipointer;
                textlength=strlen(inputtext);
                if (textlength+1>reservedmemory){
                    retval(-1);
                }else{
                    inputtext[textlength]=(char) variable2;
                    inputtext[textlength+1]=0;
                    retval(1);
                }
                
       }
}
void input_delete()
{
       char *texttouse;
       int usedtextlength;
       int variable1=getparm();

       newasciipointer=&mem[text_offset+variable1];
       newasciipointer--;
       if ( *newasciipointer!= MARK_STRING_TIZO_INPUT ){
                retval(-1);
       }else{
              newasciipointer++;
              texttouse=(char *) newasciipointer;
              usedtextlength=strlen(texttouse);
              if (usedtextlength>0){
                        texttouse[usedtextlength]=0;
                        texttouse[usedtextlength-1]=0;
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

