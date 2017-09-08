
// Strings, nuevo: p_string, cglo, cloc, p_*char, l*chr, ...
// 00 - optimizacion de código intermedio
// 00 - tablas y estructuras de hasta 3 dimensiones
// 01 - añadir strings globales
// 02 - probar en PRG, y provocar todos los nuevos errores
// 03 - añadir nuevo set de instrucciones al intérprete
// 04 - Preparar el debugger para el soporte de strings
// 05 - añadir strings locales y privadas
// 06 - comprobacion de rango en ejecución (strings con prefijo 0xdad00000)
// 07 - long_header=9, en lugar de 36
// 08 - strings globales miembros
// 09 - strings locales miembros
// 10 - inicialización de estructuras contemplando los strings miembros
// 11 - STRING p,q; define 2 cadenas (en lugar de una cadena y un int)
// 12 - Poder inicializar los registros en las estructuras globalmente (struct pepe[9] x=10; end)
// 13 - Sentencia compiler_options (_max_process,_extended_conditions,_simple_conditions,_case_sensitive,_ignore_errors,_free_sintax)
// 14 - Auto "\0" y ZERO2SPACE al poner un char en un string (strfix)
// 15 - Nuevo sistema de tipos (int/char/string) en expresiones y condiciones (tipo_expresion)
// 16 - Auto conversion de "A" a char("A") en asignaciones a char (literal/constante literal/string -> char)
// 17 - Sobrecarga de = += + en operaciones de cadenas (strcpy,strcat,stradd)
// 18 - Añadir y quitar caracteres con string++/--, string-=n, string-n (strdec,strsub,strlen)
// 19 - Poder definir literales en varias lineas del fuente (varios consecutivos se convierten en uno)
// 20 - Corregido fallo del coloreador de listados (al llegar un eof con /* sin cerrar)
// 21 - Comparaciones entre cadenas (strigu/dis/may/men/mai/mei)
// 22 - Nuevo sistema con 4 strings "en el aire" para permitir mas operaciones con cadenas (+/-)
// 23 - Descartada la idea de pasar los offset a bytes (se pretendia hacer para &cadena[n],
//      bytes y words: pero en [ptr+n] no se puede saber por cuanto multiplicar n)
// 24 - Solucionados algunos fallos relacionados con los accesos remotos
// 25 - Unidas las reservadas del modo7 con las del modo 8
// 26 - Añadidas la opciones _no_check (no comprobar rangos ni ides) y _no_strfix
// 27 - Funciones ascii: char,strcpy,strcat,strlen,strcmp,strchr,strstr,strset,upper,lower y strdel
// 28 - Habilitado un 'parche' para indicar al compilador que algunas funciones devuelven un string
// 29 - Poder recibir un parametro en un string/char global o local (cpachr, cpastr)
// 30 - Habilitado un sistema multi-par/pri y arreglados algunos temas de las llamadas a procesos
// 31 - Habilitada la idea de recibir un parámetro en un string private, incluso indicando su longitud (ej, proc(string s[2]))
// 32 - Permitir la redeclaracion del parámetro string dentro de la seccion private (con la misma longitud)
// 33 - Procesos tipo "function" (no retornan al llamador hasta haber terminado) (FCount,CallerId)
// 34 - Arreglado el tema de los retornos de function (al caller del último proceso)
// 35 - Poder hacer STEP sobre funciones (se hace un go here! a la siguiente sentencia)
// 36 - Añadir bytes globales y un nuevo, nuevamente, sistema de inicialización de datos
//      (ver p_byte, bglo y p_pointerbyte)
// 37 - Preparar el debugger para el soporte de bytes y words
// 38 - Ultimos cambios en el Debugger (en Ver datos), y conversión al Castellano.
// 39 - Añadir bytes locales y privados (bloc)
// 40 - Bytes miembros de estructuras (globales/locales y privados)
// 41 - Inicialización de estructuras contemplando los bytes miembros (frm,ifrm_max)
// 42 - Añadidas al intérprete instrucciones para el manejo de words (???wor)
// 43 - Words al igual que bytes (wglo wloc *word ???wor)
// 44 - Poder recibir un byte o word como parámetro (y redeclararlo como private)
// 45 - Fallo del coloreador (debajo de process ... poner ?????????)
// 46 - Que en las listas de procesos (F5) aparezcan también las funciones (editor y debugger)
// 47 - Fallo de que desaparezcan (completamente) los sprites en los modos 8
// 48 - Pasar los IMPORT al comienzo del programa y emitir un error adecuado para los IMPORT antiguos (y adaptar help)
// 49 - Quitado el formateador de listados (dejado todo en divfrm.cpp por si en el futuro se quiere hacer, de forma separada a divc.cpp)
// 50 - Que las funciones puedan devolver un valor con return despues de haber hecho FRAME (sistema multipila)
// 51 - Añadido el tipo INT opcional para los datos enteros convencionales en DIV 1
// 52 - Declaración de punteros a INT globales/locales/privados y miembros de estructuras (tpigl,tpilo) de hasta 3 dimensiones
// 53 - Utilización de punteros INT, generación de código para los mismos (y pruebas)
// 54 - Soporte en el debugger para punteros INT globales y locales (p, p[0], p[1], ...)
// 55 - Punteros a int como parámetros y redeclaración en sección PRIVATE
// 56 - Prohibir la redeclaración de variables private (solo si es 1 vez como parametro y otra como private)
// 57 - Punteros a byte/word, tanto global como local y private, member o no, declaración y código
// 58 - Soporte en el debugger de punteros de tipo byte y word (globales y locales)
// 59 - Eliminada la posibilidad de utilizar * para definir un puntero (y en el debugger se usa POINTER)
//      (esto es importante para no animar a utilizar *puntero después ...)
// 60 - Probar punteros int/word/byte (glo/loc/pri/mem/par)
// 61 - Implementados "string pointer" exactamente igual que los "byte pointer" (sólo varia la visualización en el debug)
// 62 - Punteros a estructuras STRUCT POINTER NOMBRESTRUCT tal; (por defecto a 0)
// 63 - Resuelto el problema del acceso a los miembros de la estructura original
// 64 - Soporte en el debugger para punteros a estructuras (!!! mas chungo de lo que yo pensaba)
// 65 - Funcion de "capar()" direcciones en DEBUG para evitar en lo posible los page fault
// 66 - Resueltos "n-mil" problemas con los putos punteros, el debugger y la madre que lo parió ...
// 67 - Hacer que int/byte/word/string pointer p1,p2; defina dos punteros
// 68 - Funcion screen_copy que puede pasar una region de pantalla a una region de un gráfico
// 69 - Argc/argv al igual que en C, con los parámetros del programa
// 70 - Nuevo sistema de carga de funciones, definiendo el retorno y el tipo de los parámetros
//      (y eliminadas las excepciones antes contempladas con las funciones de cadenas)
// 71 - Funcion de ordenación y desordenación (nombre_estructura,campo,modo)
// 72 - Minimizado el buffer circular de teclado en la ejecución de los juegos
// 73 - Comprobar accesos a "NULL pointed data" de lectura/escritura
// 74 - Opciones _no_id_check _no_range_check _no_null_check (englobadas en _no_check)
// 75 - Opcion de compilacion _no_optimize (para poder deshabiliatar la optimizacion)

//    fopen, fclose, fread, fwrite, fseek, ftell, filelen (ver flushall, fcloseall)
//    findfisrt, findnext, fullpath, splitpath, set/getcwd, set/getdrive, fprintf
//    remove (danidel), chdir, mkdir

//    - explicar los nuevos errores (strings, bytes, words y pointers) en help
//    - funciones de malloc/free
//    - escribir libro

// A - Poder hacer includes de {constantes/globales/locales/procesos}
// C - Multiasignaciones, utilizando variables privadas ocultas (x=3,45,y*10,2..4,-8;)
// D - pensar algun truquillo tipo "macros" facil de implementar y que quede bien
// E - Finalizar el autoformateador de listados
// F - Generar un NFO informativo del programa compilado (TAB+LST+EML)

// ─────────────────────────────────────────────────────────────────────────────

// SOLUCION 1: POINTERS BYTE/WORD LIMITADOS A TABLAS BYTE/WORD

// OJO! No se podrá hacer ptrbyte++ (ya que incrementará 4 bytes)
//      ni ptrword++, que incrementará 2 words

// (*)  A la hora de hacer que *ptrbyte sea ptrbyte[0] (supongo que añadiendo
//      un nuevo tipo de ¿p_pointerb?), se debería emitir un error al hacer
//      ++, --, +=, etc. sobre un p_pointerb/w ...
//    - Problema, ¿como prohibir ptrbyte=ptrbyte+1;?
//    - También se podrían permitir las operaciones, aunque los resultados fueran erróneos

//    - Aun así, ¿como explicar a la gente que ptrint++ funciona, pero que ptrbyte++ no?

//    - Ventajas: ya está (casi) hecho, no presenta incompatibilidades con
//      DIV1, hay poco tiempo disponible ..., aun así, se permite el uso
//      de memoria dinámica ... y siempre se podrá pasar a la SOLUCION 3 más
//      adelante

//    - aunque quizá esto no se debería llamar pointer ... sino algo como
//      "táblas dinámicas" (por que se pueden hacer mallocs de las mismas)

//    - Hombre, también serviría para pasar tablas y estructuras por parámetros.
//    - Quizá se debería prohibir utilizar "p_mul" (*) para diferenciarlos de
//      los punteros en C.

//    - El problema es: ¿Se prohibe operar (sumas y demás) con ptrbyte/word? -> NO

//    - ¿Poder utilizar *ptrbyte como ptrbyte[0] ? -> NO, ya que ello induciría
//      al acceso de bytes con *ptrbyte, cuando no es posible

// SOLUCION 2: POINTERS BYTE/WORD RELATIVOS A &MEM[0]

//    - OJO, quizá los punteros a byte y word deberían ser diferentes
//    - (que pointerb/w fueran un offset byte/word relativo a &memb[0])
//    - ¿Nuevo diseño para: tpwgl tpwlo tpbgl tpblo? (teniéndolos siempre multiplicados *2 o *4)
//    - a) en incializacion se debe multiplicar *2 o *4 la constante (ptrbyte=&tablabyte)
//    - b) como cojones se hace ptrbyte=&loquesea ? (como paso de parámetro, asignación, etc...)
//    - c) y que pasa con ¿ptrword=ptrbyte?
//    - d) es más, ¿y que pasa si se envia como parámetro un ptrbyte y se recibe un ptrint?

//    - Solo podría hacerse de forma que todos los accesos de lectura a ptrbyte se dividiera por 4
//      y en los de escritura se multiplicara por cuatro, y aun así:
//      ¡¡¡ fallaría ptrbyte1=ptrbyte2 !!! (o pasando y recibiendo un ptrbyte parámetro)
//      ¿[ptrbyte+n] ... y *ptrbyte?

//    - Es una gilipollez, ya que ptrbyte=ptrbyte+1 le seguiría sumando 4 !!!

//    - Ventajas: Pues que no sé si en este caso la virtud estará en el
//      término medio ...

// SOLUCION 3: PASAR TODOS LOS OFFSET A BYTE

//    - Debería haber alguna forma de tener todos los offset multiplicados por 4 (en bytes)

//    - Problemas: ["ptr"+n] (por cuanto multiplicar n), además de como resolver
//      [ptrint+ptrbyte+2*x...], acceso en el intérprete a todos los datos de mem
//      mem[id+_Graph], los propios id (multiplicarlos o no?- > mejor no, y hacer que el p_punto los multiplique,
//      ya que, además deben ser impares), programas antiguos que hicieran uso de punteros
//      (p=&kk; *p..., p++, *p... para ello se debería emitir un error de compilación al utilizar *p o [p],
//      obligando a que redeclaren p como un ptrint), que ptrint++ sea ptrint+=4 y ptrword++ sea ptrword+=2,

//    - Cambiarian tambien los sizeof(), ahora en bytes, y las operaciones con
//      ficheros, aumentando la incompatibilidad con DIV1 (ya que la gente podría
//      haber utilizado un save("pepe.dat",&tabla,10) ...

//    - Resumiendo: que los problemas gordos son el intérprete (sobre todo, ya
//      que era un rollo eso del "#define memo(x) *(int*)(memb+x)" ),
//      el de [ptr???+???] (y ptrint++), y las incompatibilidades con DIV1

//    - Ventajas: El acceso libre a cualquier tipo de dato, la simplificación
//      de strings, el poder hacer que "byte a" ocupe realmente sólo un byte, ...
//      que es la forma correcta de hacerlo, y que si no se hace ahora quizá
//      no se pueda hacer nunca.

// SOLUCION 4: PASAR LOS OFFSET A FLOAT

//    - Se que a bote pronto parace una gilipollez, pero ¿no sería lo ideal
//      poder tener como offset de bytes numeros .25, .50 y .75?

//    - Lo guay sería que ptrint++ seguiría sumando 1.0

//    - y para los accesos a bytes se sumaría .25, el problema es que todo
//      se llenaría de llamadas a _CHP e iria muy lento

//    - Además, ¿como iban a devolver offset o sizeof() un float si el
//      lenguaje no los maneja?

// OJO! EN ESTA VERSION TODAVIA NO HE HECHO NINGUNA BURRADA
//      RELATIVA A MULTIPLICAR LOS OFFSET POR 4 !!!!!!!!!!!
//      (en una ocasión lo intenté .. y lo acabé deshaciendo todo)

// ─────────────────────────────────────────────────────────────────────────────

// Optimización de codigo intermedio

// 1er paso: Crear una función de generación de mnemónicos, en lugar de un
//           acceso libre a mem[], para localizar el problema de la optimización

// no se puede generar codigo a byte, pero si utilizar el sistema de "byte"
// como compresion de los EXE, descomprimiendo en el intérprete al cargar

// ─────────────────────────────────────────────────────────────────────────────

// Tablas y estructuras multidimensionales
// revisar errores: 35, ...
// OJO en la ayuda con las tablas y las estrucuturas de SOLO una dimensión

//═════════════════════════════════════════════════════════════════════════════
//      DIV - Compilador Interno
//═════════════════════════════════════════════════════════════════════════════

//#define listados // Para generar los listados de objetos y EML (LST/TAB/EML)

#include "global.h"
#include "divdll.h"

#ifdef ZLIB
#include <zlib.h>
#endif

//extern char ExeGen[_MAX_PATH];
void l_objetos (void);
void l_ensamblador (void);

void delete_code(void);
void precarga_obj (void);
void psintactico(void);
void test_buffer(int * * buffer,int * maximo,int n);
void save_dbg(void);
void save_exec_bin(void);
void tloc_init(int tipo);
void g2(int op, int pa);
void analiza_private(void);
void g1(int op);
void expresion_cpa(void);
void tglo_init2(int tipo);
void condicion(void);
void con00(int tipo_exp);
void generar_expresion(void);
void con0(void);
void con1(void);
void con2(void);
void exp00(int tipo_exp);
void exp0(void);
void exp1(void);
void div_exp2(void);
void exp3(void);
void exp4(void);
void exp5(void);
void exp6(void);
void unario(void);
void factor(void);
void factor_struct(void);
void gen(int param, int op, int pa);
void remove_code(int i);
void add_code(int dir, int param, int op);





//═════════════════════════════════════════════════════════════════════════════
//      Constantes
//═════════════════════════════════════════════════════════════════════════════

#define version "0c88b" //Versión del compilador

#ifndef SHARE
#define max_obj 8192    //Compiler object maximum limit compiler
#define long_med_id 16  //Longitud media de los identificadores (+4+4+1)
#else
#define max_obj 700     //El malvado tiene 626 ...
#define long_med_id 20  //Longitud media de los identificadores (+4+4+1)
#endif

#define max_nodos 128   //Máximo número de nodos del léxico para símbolos
#define max_exp 512     //Máximo número de elementos en una expresión
#define long_header 9   //Longitud de la cabecera al inicio de los programas
#define long_pila 2048  //Longitud de la pila en ejecución

#define default_buffer    (16384*8)
#define security_distance (4096*8)
#define buffer_grow       (16384*8)

#define cr 13           //Retorno de carro
#define lf 10           //Salto de linea
#define tab 9           //Tabulación

//═════════════════════════════════════════════════════════════════════════════

#define l_err 0         //Valores de lex_case, si no son punteros a lex_simb
#define l_cr  1         //Fin de linea (l_err Carácter no esperado)
#define l_id  2         //Identificador o palabra reservada
#define l_spc 3         //Espacios y tabulaciones
#define l_lit 4         //Literal
#define l_num 5         //Constante numérica

//═════════════════════════════════════════════════════════════════════════════
//      Valores sintácticos o tokens (pieza)
//═════════════════════════════════════════════════════════════════════════════

#define p_ultima        0x00 //Fin de fichero <EOF>

#define p_program       0x01
#define p_const         0x02
#define p_global        0x03
#define p_local         0x04
#define p_begin         0x05
#define p_end           0x06
#define p_process       0x07
#define p_private       0x08
#define p_struct        0x09
#define p_import        0x0A
#define p_include       0x75 

#define p_setup_program 0x0B

#define p_string        0x0C
#define p_byte          0x0D
#define p_word          0x0E
#define p_int           0x0F

#define p_compiler_options 0x10
#define p_function      0x11

#define p_until         0x16
#define p_else          0x17

#define p_return        0x18

#define p_from          0x19
#define p_to            0x1a
#define p_step          0x1b
#define p_if            0x20
#define p_loop          0x21
#define p_while         0x22
#define p_repeat        0x23
#define p_for           0x24
#define p_switch        0x25
#define p_case          0x26
#define p_default       0x27

#define p_frame         0x28

#define p_break         0x29
#define p_continue      0x2A

#define p_clone         0x2B

#define p_debug         0x2C

#define p_ptocoma       0x30
#define p_coma          0x31
#define p_corab         0x32
#define p_corce         0x33

#define p_asig          0x38

#define p_dup           0x3C

#define p_abrir         0x40
#define p_cerrar        0x41

#define p_suma          0x44
#define p_resta         0x45

#define p_shr           0x46
#define p_shl           0x47

#define p_or            0x48
#define p_xor           0x49
#define p_and           0x4a

#define p_xorptr        0x4b
#define p_andofs        0x4c

#define p_neg           0x4e
#define p_pointer       0x4f

#define p_offset        0x50
#define p_not           0x51

#define p_add           0x52
#define p_sub           0x53

#define p_mul           0x54
#define p_div           0x55
#define p_mod           0x56
#define p_multi         0x57

#define p_inc           0x58
#define p_dec           0x59

#define p_igu           0x5b
#define p_dis           0x5c

#define p_may           0x5d
#define p_men           0x5e
#define p_mei           0x5f
#define p_mai           0x60

#define p_sizeof        0x66

#define p_type          0x67

#define p_whoami        0x68

#define p_punto         0x69

#define p_rango         0x6a

#define p_add_asig      0x6b
#define p_sub_asig      0x6c
#define p_mul_asig      0x6d
#define p_div_asig      0x6e
#define p_mod_asig      0x6f
#define p_and_asig      0x70
#define p_or_asig       0x71
#define p_xor_asig      0x72
#define p_shr_asig      0x73
#define p_shl_asig      0x74

#define p_ini_rem       0x7d //Inicio comentario
#define p_end_rem       0x7e //Fin comentario
#define p_rem           0x7f //Comentario de una linea

#define p_strigu        0xc0
#define p_strdis        0xc1
#define p_strmay        0xc2
#define p_strmen        0xc3
#define p_strmei        0xc4
#define p_strmai        0xc5

#define p_pointerword   0xd0
#define p_sumaword      0xd1
#define p_restaword     0xd2
#define p_incword       0xd3
#define p_decword       0xd4
#define p_asigword      0xd5
#define p_add_asigword  0xd6
#define p_sub_asigword  0xd7
#define p_mul_asigword  0xd8
#define p_div_asigword  0xd9
#define p_mod_asigword  0xda
#define p_and_asigword  0xdb
#define p_or_asigword   0xdc
#define p_xor_asigword  0xdd
#define p_shr_asigword  0xde
#define p_shl_asigword  0xdf

#define p_pointerchar   0xe0
#define p_sumachar      0xe1
#define p_restachar     0xe2
#define p_incchar       0xe3
#define p_decchar       0xe4
#define p_asigchar      0xe5
#define p_add_asigchar  0xe6
#define p_sub_asigchar  0xe7
#define p_mul_asigchar  0xe8
#define p_div_asigchar  0xe9
#define p_mod_asigchar  0xea
#define p_and_asigchar  0xeb
#define p_or_asigchar   0xec
#define p_xor_asigchar  0xed
#define p_shr_asigchar  0xee
#define p_shl_asigchar  0xef

#define p_strcpy        0xf0
#define p_strfix        0xf1
#define p_strcat        0xf2
#define p_stradd        0xf3
#define p_strdec        0xf4
#define p_strsub        0xf5
#define p_strlen        0xf6

#define p_pointerbyte   0xf7

#define p_lit           0xfc //Puntero al literal (txt) en pieza_num
#define p_id            0xfd //o es un ptr a vnom (a un ptr al objeto)
#define p_num           0xfe //Número en pieza_num

//═════════════════════════════════════════════════════════════════════════════

#define tnone   0       // Tipos de objetos en obj[]
#define tcons   1
#define tvglo   2
#define ttglo   3
#define tcglo   4
#define tvloc   5
#define ttloc   6
#define tcloc   7
#define tproc   8
#define tfunc   9
#define tsglo   10
#define tsloc   11
#define tfext   12      // Función de una librería externa

#define tbglo   13      // Byte global
#define twglo   14
#define tbloc   15
#define twloc   16

#define tpigl   17      // Puntero a ints global (un ttglo direccionable)
#define tpilo   18      // Puntero a ints local (idem)

#define tpwgl   19      // Punteros a word
#define tpwlo   20
#define tpbgl   21      // Punteros a byte
#define tpblo   22

#define tpcgl   23      // Punteros a string
#define tpclo   24
#define tpsgl   25      // Punteros a struct
#define tpslo   26

//═════════════════════════════════════════════════════════════════════════════

#define econs   0       // Constante          // Tipos de elementos en tabexp[]
#define eoper   1       // Operador
#define erango  2       // Comprobación de rango
#define ewhoami 3       // Identificador de proceso
#define ecall   4       // Creación de un proceso
#define efunc   5       // Llamada a una función interna
#define efext   6       // Llamada a una función externa
#define echeck  7       // Comprobación de validez de un identificador
#define estring 8       // Cadena de texto (su offset constante)
#define enull   9       // Comprobación de NULL pointer

//═════════════════════════════════════════════════════════════════════════════

//─Mnemónico─Códg.─Operandos (Generación de código EML, "*" ­ "aún no usado")

#define DIVBC_nop  0 // *            No operación
#define DIVBC_car  1 // valor        Carga una constante en pila
#define DIVBC_asi  2 //              Saca valor, offset y mete el valor en [offset]
#define DIVBC_ori  3 //              Or lógico
#define DIVBC_xor  4 //              Xor, or exclusivo
#define DIVBC_and  5 //              And lógico, operador sobre condiciones
#define DIVBC_igu  6 //              Igual, operador logico de comparación
#define DIVBC_dis  7 //              Distinto, true si los 2 valores son diferentes
#define DIVBC_may  8 //              Mayor, comparación con signo
#define DIVBC_men  9 //              Menor, idem
#define DIVBC_mei 10 //              Menor o igual
#define DIVBC_mai 11 //              Mayor o igual
#define DIVBC_add 12 //              Suma dos constantes
#define DIVBC_sub 13 //              Resta, operación binaria
#define DIVBC_mul 14 //              Multiplicación
#define DIVBC_div 15 //              División de enteros
#define DIVBC_mod 16 //              Módulo, resto de la división
#define DIVBC_neg 17 //              Negación, cambia de signo una constante
#define DIVBC_ptr 18 //              Pointer, saca offset y mete [offset]
#define DIVBC_not 19 //              Negación binaria, bit a bit
#define DIVBC_aid 20 //              Suma id a la constante de la pila
#define DIVBC_cid 21 //              Carga id en la pila
#define DIVBC_rng 22 // rango        Realiza una comparación de rango
#define DIVBC_jmp 23 // offset       Salta a una dirección de mem[]
#define DIVBC_jpf 24 // offset       Salta si un valor es falso a una dirección
#define DIVBC_fun 25 // código       Llamada a un proceso interno, ej. signal()
#define DIVBC_cal 26 // offset       Crea un nuevo proceso en el programa
#define DIVBC_ret 27 //              Auto-eliminación del proceso
#define DIVBC_asp 28 //              Desecha un valor apilado
#define DIVBC_frm 29 //              Detiene por este frame la ejecución del proceso
#define DIVBC_cbp 30 // num_par      Inicializa el puntero a los parámetros locales
#define DIVBC_cpa 31 //              Saca offset, lee parámetro [offset] y bp++
#define DIVBC_typ 32 // bloque       Define el tipo de proceso actual (colisiones)
#define DIVBC_pri 33 // offset       Salta a la dirección, y carga var. privadas
#define DIVBC_cse 34 // offset       Si switch <> expresión, salta al offset
#define DIVBC_csr 35 // offset       Si switch no esta en el rango, salta al offset
#define DIVBC_shr 36 //              Rotacion a la derecha (modo C, >>)
#define DIVBC_shl 37 //              Rotacion a la izquierda (modo C, <<)
#define DIVBC_ipt 38 //              Incremento y pointer
#define DIVBC_pti 39 //              Pointer e incremento
#define DIVBC_dpt 40 //              Decremento y pointer
#define DIVBC_ptd 41 //              Pointer y decremento
#define DIVBC_ada 42 //              Add-asignación
#define DIVBC_sua 43 //              Sub-asignación
#define DIVBC_mua 44 //              Mul-asignación
#define DIVBC_dia 45 //              Div-asignación
#define DIVBC_moa 46 //              Mod-asignación
#define DIVBC_ana 47 //              And-asignación
#define DIVBC_ora 48 //              Or-asignación
#define DIVBC_xoa 49 //              Xor-asignación
#define DIVBC_sra 50 //              Shr-asignación
#define DIVBC_sla 51 //              Shl-asignación
#define DIVBC_par 52 // num_par_pri  Define el número de parámetros privados
#define DIVBC_rtf 53 //              Auto-eliminación del proceso, devuelve un valor
#define DIVBC_clo 54 // offset       Crea un clon del proceso actual
#define DIVBC_frf 55 //              Pseudo-Frame (frame a un porcentaje, frame(100)==frame)
#define DIVBC_imp 56 // offset text  Importa una DLL externa
#define DIVBC_ext 57 // código       Llama a una función externa
#define DIVBC_chk 58 //              Comprueba la validez de un identificador
#define DIVBC_dbg 59 //              Invoca al debugger

// Instrucciones añadidas para la optimización (DIV 2.0)

#define DIVBC_car2 60
#define DIVBC_car3 61
#define DIVBC_car4 62
#define DIVBC_asiasp 63
#define DIVBC_caraid 64
#define DIVBC_carptr 65
#define DIVBC_aidptr 66
#define DIVBC_caraidptr 67
#define DIVBC_caraidcpa 68
#define DIVBC_addptr 69
#define DIVBC_funasp 70
#define DIVBC_caradd 71
#define DIVBC_caraddptr 72
#define DIVBC_carmul 73
#define DIVBC_carmuladd 74
#define DIVBC_carasiasp 75
#define DIVBC_carsub 76
#define DIVBC_cardiv 77

// Instrucciones añadidas para el manejo de caracteres

#define DIVBC_ptrchr 78  // Pointer, saca (index, offset) y mete [offset+byte index]
#define DIVBC_asichr 79  // Saca (valor, index, offset) y mete el valor en [offset+byte index]
#define DIVBC_iptchr 80  // Incremento y pointer
#define DIVBC_ptichr 81  // Pointer e incremento
#define DIVBC_dptchr 82  // Decremento y pointer
#define DIVBC_ptdchr 83  // Pointer y decremento
#define DIVBC_adachr 84  // Add-asignación
#define DIVBC_suachr 85  // Sub-asignación
#define DIVBC_muachr 86  // Mul-asignación
#define DIVBC_diachr 87  // Div-asignación
#define DIVBC_moachr 88  // Mod-asignación
#define DIVBC_anachr 89  // And-asignación
#define DIVBC_orachr 90  // Or-asignación
#define DIVBC_xoachr 91  // Xor-asignación
#define DIVBC_srachr 92  // Shr-asignación
#define DIVBC_slachr 93  // Shl-asignación
#define DIVBC_cpachr 94  // Saca offset, lee parámetro [offset] y bp++

// Instrucciones añadidas para el manejo de cadenas

#define DIVBC_strcpy 95  // Saca si, di, y hace strcpy(mem[di],[si]) (deja di en pila)
#define DIVBC_strfix 96  // Amplia una cadena antes de meter un char en ella
#define DIVBC_strcat 97  // Concatena dos cadenas (opera como strcpy)
#define DIVBC_stradd 98  // Suma dos strings "en el aire" y deja en pila el puntero al aire
#define DIVBC_strdec 99  // Añade o quita caracteres a una cadena
#define DIVBC_strsub 100 // Quita caracteres a una cadena (-=)
#define DIVBC_strlen 101 // Sustituye una cadena por su longitud
#define DIVBC_strigu 102 // Comparacion de igualdad de dos cadenas
#define DIVBC_strdis 103 // Cadenas distintas
#define DIVBC_strmay 104 // Cadena mayor
#define DIVBC_strmen 105 // Cadena menor
#define DIVBC_strmei 106 // Cadena mayor o igual
#define DIVBC_strmai 107 // Cadena menor o igual
#define DIVBC_cpastr 108 // Carga un parámetro en una cadena

// Instrucciones añadidas para el manejo de Words

#define DIVBC_ptrwor 109 // Pointer, saca (index, offset) y mete [offset+byte index]
#define DIVBC_asiwor 110 // Saca (valor, index, offset) y mete el valor en [offset+byte index]
#define DIVBC_iptwor 111 // Incremento y pointer
#define DIVBC_ptiwor 112 // Pointer e incremento
#define DIVBC_dptwor 113 // Decremento y pointer
#define DIVBC_ptdwor 114 // Pointer y decremento
#define DIVBC_adawor 115 // Add-asignación
#define DIVBC_suawor 116 // Sub-asignación
#define DIVBC_muawor 117 // Mul-asignación
#define DIVBC_diawor 118 // Div-asignación
#define DIVBC_moawor 119 // Mod-asignación
#define DIVBC_anawor 120 // And-asignación
#define DIVBC_orawor 121 // Or-asignación
#define DIVBC_xoawor 122 // Xor-asignación
#define DIVBC_srawor 123 // Shr-asignación
#define DIVBC_slawor 124 // Shl-asignación
#define DIVBC_cpawor 125 // Saca offset, lee parámetro [offset] y bp++

// Miscelánea

#define DIVBC_nul    126 // Comprueba que un puntero no sea NULL

//═════════════════════════════════════════════════════════════════════════════
//      Prototipos
//═════════════════════════════════════════════════════════════════════════════

void c_error(word,word);
void analiza_comandos(word,byte**);
void prepara_compilacion(void);
void analiza_ltlex(void);
void lexico(void);
void sintactico(void);
void tglo_init(memptrsize);
void sentencia(void);
void expresion(void);
int constante(void);

static jmp_buf buf;

void comp(void)
{
	if (!setjmp(buf))
		compilar();
}

void comp_exit(void)
{
     longjmp(buf,1);        
}



//═════════════════════════════════════════════════════════════════════════════
//      Variables globales del compilador
//═════════════════════════════════════════════════════════════════════════════

#include "div_stub.h"

byte file_in[_MAX_PATH+1],file_out[_MAX_PATH+1];

FILE * fin, * fout;
byte * _source=NULL;
byte * source, * ierror, * ierror_end;

int longitud_textos;
int inicio_textos;
int max_process;
int ignore_errors;
int free_sintax;
int extended_conditions;
int simple_conditions;
int comprueba_rango;
int comprueba_id;
int comprueba_null;
int hacer_strfix;
int optimizar;

//═════════════════════════════════════════════════════════════════════════════

FILE * div_open_file(char * file);

//═════════════════════════════════════════════════════════════════════════════

int old_linea; // Situación en el fichero del token anterior al último leido
byte * old_ierror, * old_ierror_end;

// El último token leido está en (linea,ierror,ierror_end)

byte * ultima_linea,cero=0;

//═════════════════════════════════════════════════════════════════════════════

// Informacion para EXEC.LIN (posicion en mem y fuente de cada sentencia)

int inicio,final;     // Direcciones inicial y final de mem[] de la sentencia
int linea1,columna1;  // Posicion en la que comienza la sentencia en el fuente
int linea2,columna2;  // Posicion en la que acaba la sentencia en el fuente

void inicio_sentencia(void);  // Fija inicio, linea1, columna1
void final_sentencia(void);   // Fija final, linea2, columna2
void grabar_sentencia(void);  // Guarda las seis variables en el fichero

//═════════════════════════════════════════════════════════════════════════════

// Tabla de objetos ───────────────────────────────── *iobj primer objeto libre

struct objeto {
  byte tipo;    // Tipo de objeto
  byte usado;   // Indica si el objeto ha sido usado antes de definirse
  byte * name;  // Puntero al nombre, para algún listado
  byte * ierror;// Puntero al código para determinar la columna si es necesario
  int linea;    // Línea de código fuente, para informar del error
  int param;    // Indica que es un objeto declarado en los parámetros
  struct objeto * anterior; // Anterior objeto de igual nombre
  struct objeto * bloque;   // Bloque de este proceso (0-global/local N-private)
  struct objeto * member;   // Indica a que struct pertenece (0-n/a)
  union {
    struct {                                            // Constante
      int valor;
      int literal;                                      // 0-no, 1-si
    } cons;
    struct {                                            // Variable global
      int offset;
    } vglo;
    struct {                                            // Tabla global
      int offset;
      int totalen;                                      // numero total de elementos
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } tglo,pigl;
    struct {                                            // Byte global
      int offset;
      int totalen;                                      // numero total de elementos
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } bglo,pbgl;
    struct {                                            // Word global
      int offset;
      int totalen;                                      // numero total de elementos
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } wglo,pwgl;
    struct {                                            // Cadena (string) global
      int offset;
      int totalen;                                      // numero total de elementos
    } cglo,pcgl;
    struct {                                            // Struct
      int offset;
      int len_item;                                     // Número de campos
      int totalitems;                                   // Numero total de registros
      int items1,items2,items3;                         // -1 si n/a
    } sglo,sloc;
    struct {                                            // Struct pointer
      int offset;
      struct objeto * ostruct;                          // Puntero al struct
      int totalitems;                                   // Número total de registros
      int items1,items2,items3;                         // -1 si n/a
    } psgl,pslo;
    struct {                                            // Variable local
      int offset;
    } vloc;
    struct {                                            // Tabla local
      int offset;
      int totalen;
      int len1,len2,len3;
    } tloc,pilo;
    struct {                                            // Byte local
      int offset;
      int totalen;                                      // numero total de elementos
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } bloc,pblo;
    struct {                                            // Word local
      int offset;
      int totalen;                                      // numero total de elementos
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } wloc,pwlo;
    struct {                                            // Cadena (string) local
      int offset;
      int totalen;                                      // numero total de elementos
    } cloc,pclo;
    struct {                                            // Proceso
      struct objeto * bloque;
      int offset;
      int num_par;
    } proc;
    struct {                                            // Funcion interna
      int codigo;
      int num_par;
      int ret;      // 0-Int 1-String
      byte par[12]; // 0-Int 1-(Struct_name,name)
    } func;
    struct {                                            // Funcion externa
      int codigo;
      int num_par;
    } fext;
  };
} obj[max_obj], * iobj; // ────────────────────────────────────────────────────

//═════════════════════════════════════════════════════════════════════════════

byte * vnom=NULL; // Vector de nombres (cad_hash:int, pieza (o iobj):int, asciiz)
union { byte*b; byte**p; } ivnom;

byte * inicio_objetos; // Para el crear listado de la tabla de objetos

byte * vhash[256]; // Pointer to the vector of names

//═════════════════════════════════════════════════════════════════════════════

struct exp_ele {
  byte tipo; // econs, eoper, erango, ewhoami, ecall
  union {
    int valor;
    int token;
    struct objeto * objeto;
  };
} tabexp[max_exp], *_exp;

struct lex_ele { byte caracter;
         byte token;
                 struct lex_ele * alternativa;
                 struct lex_ele * siguiente; } 
       lex_simb[max_nodos], * ilex_simb, * lex_case[256];

int pieza, pieza_num;
struct objeto * o; // Cuando pieza=p_id, objeto de tipo (**o).tipo
struct objeto * bloque_actual; // Bloque que esta siendo analizado
struct objeto * bloque_lexico; // Es 0 hasta las privadas del program

struct objeto * member; // !=0 al declarar/acceder un miembro de un struct

int linea, num_nodos, num_obj;

int num_obj_predefinidos;

//═════════════════════════════════════════════════════════════════════════════

int parametros;

int acceso_remoto; // Para no permitir id.private

//═════════════════════════════════════════════════════════════════════════════

#define max_break 512
int tbreak[max_break];
int itbreak;

#define max_cont 256
int tcont[max_cont];
int itcont;

//═════════════════════════════════════════════════════════════════════════════

int pila[long_pila+max_exp+64]; // cálculo de expresiones (compilación y ejecución)

int32_t * mem_ory=NULL, * frm=NULL;
int32_t * mem=NULL, * loc=NULL;
int imem, imem_max;
int iloc, iloc_max;
int itxt, ifrm_max;

byte * imemptr;

int iloc_len; // final de las variables locales incluyendo las privadas

// mem[]        vector de memoria destino
// imem         puntero para la generación de datos y código
// imem_max     fin de la memoria destino (indice máximo de mem[])
// iloc         inicio en mem[] de las variables locales (inicializadas)
// iloc_len     longitud de las variables locales

//───────────── Inserción en la tabla de expresiones, acceso a variables

//vglo           &vglo ptr
//tglo[_exp]     &tglo <_exp> add rng ptr

//vloc           &vloc aid ptr
//tloc[_exp]      &tloc <_exp> add rng aid ptr

//proc.vloc      &proc ptr &vloc add ptr
//proc.tloc[_exp] &proc ptr &tloc <_exp> add rng add ptr

//═════════════════════════════════════════════════════════════════════════════
//      Variables relacionadas con el listado formateado
//═════════════════════════════════════════════════════════════════════════════

int coment=0;           // 0-Código, 1-Dentro de comentario, 2-Anidado, ...
int convert=0;          // Indica si se deben generar ya los token en ls

//═════════════════════════════════════════════════════════════════════════════
//      Información sobre los errores
//═════════════════════════════════════════════════════════════════════════════

int numero_error=-1;
int linea_error;
int columna_error;

int error_27=27;        // Para emitir "se esperaba una sentencia"

//═════════════════════════════════════════════════════════════════════════════

int program_type=0;

//═════════════════════════════════════════════════════════════════════════════

struct {      // Peephole, "mirilla" para el optimizador
  int dir;    // Dirección
  int param;  // Indica si ese opcode tiene parámetro
  int op;     // Opcode
} code[16];   // En code[15] debe quedar siempre la última instrucción generada

//═════════════════════════════════════════════════════════════════════════════
//      Inicialización del compilador
//═════════════════════════════════════════════════════════════════════════════

FILE * linf; // EXEC.LIN
FILE * lprg; // EXEC.PGM
FILE * lins; // EXEC.INS

FILE * def; // Para el analizador de "ltlex.def"
byte *_buf;

void inicializa_compilador(void) {
  int n;
  for (n=0;n<256;n++) if (lower[n]==' ') lower[n]=0;
}

//═════════════════════════════════════════════════════════════════════════════
//      Prepara la compilacion para un programa
//═════════════════════════════════════════════════════════════════════════════

void mensaje_compilacion(byte * p) {
  if(compilemode) {
	  fprintf(stdout,"%s\n",p);
  } else {
   wbox(v.ptr,v.an/big2,v.al/big2,c2,2,20,v.an/big2-4,7);
   wwrite(v.ptr,v.an/big2,v.al/big2,3,20,0,p,c3);
   vuelca_ventana(0); volcado_copia();
 }
}

#ifndef ZLIB
#define uLongf unsigned long
#endif

void compilar(void) {
  int n;
  uLongf m;
  struct objeto * i;
  byte * q, * p;

  program_type=0;

free_resources();

  mensaje_compilacion(texto[201]);

  vnom=NULL; mem_ory=NULL;
  mem=NULL; loc=NULL; frm=NULL;
  def=NULL; _buf=NULL; _source=NULL;

  itbreak=0; itcont=0; convert=0; coment=0;

  member=NULL; bloque_actual=NULL; bloque_lexico=NULL;

  delete_code();

  memset(obj,0,sizeof(obj)); iobj=obj; num_obj=0;
  memset(lex_simb,0,sizeof(lex_simb)); ilex_simb=lex_simb; num_nodos=0;
  memset(vhash,0,sizeof(vhash));

  for (n=0;n<256;n++)
    if (lower[n])
      if (n>='0' && n<='9') lex_case[n]=(struct lex_ele*)l_num;
      else lex_case[n]=(struct lex_ele*)l_id;
    else lex_case[n]=(struct lex_ele*)l_err;

  if ((vnom=(byte *) malloc(max_obj*long_med_id+1024))==NULL) c_error(0,0);

  ivnom.b=vnom;

  analiza_ltlex();

  lex_case[' ']=(struct lex_ele*)l_spc;
  lex_case[tab]=(struct lex_ele*)l_spc;
  lex_case[cr]=(struct lex_ele*)l_cr;
/*
  for (n=0;n<256;n++)
	printf("vhash[%d] = %x\n",n,vhash[n]);
*/

  inicio_objetos=ivnom.b;

  // *** OJO *** Estos dos errores no son "memoria insuficiente"
	linf=fopen("system/exec.fs","wb");
	if(linf) {
		fwrite(&fsmode,1,1,linf);
		fclose(linf);
	}
	linf = fopen("system/exec.path","wb");
	if(linf) {
		fputs((char *)&tipo[8],linf);
		fclose(linf);
	}
  if ((linf=fopen("system/exec.lin","wb"))==NULL) c_error(0,0);
  if ((lprg=fopen("system/exec.pgm","wb"))==NULL) c_error(0,0);

  

  imem_max=default_buffer; imem=0;
  if ((mem_ory=mem=(int*)malloc(imem_max*sizeof(memptrsize)))==NULL) c_error(0,0);
  memset(mem,0,imem_max*sizeof(memptrsize));

  iloc_max=default_buffer/2; iloc=0; iloc_len=0;
  if ((loc=(int*)malloc(iloc_max*sizeof(memptrsize)))==NULL) c_error(0,0);
  memset(loc,0,iloc_max*sizeof(memptrsize));

  ifrm_max=default_buffer/2;
  if ((frm=(int*)malloc(ifrm_max*sizeof(memptrsize)))==NULL) c_error(0,0);
  memset(frm,0,ifrm_max*sizeof(memptrsize));

  mensaje_compilacion(texto[203]);

  imem=long_header;

  precarga_obj(); // No literals in the preloaded objects

  mensaje_compilacion(texto[200]);

  source=source_ptr; _source=source;
  *(source+source_len)=cr; *(source+source_len+1)=cr;

  convert=1; ultima_linea=source;
  acceso_remoto=0; parametros=0; linea=1;

  itxt=inicio_textos=imem;
  psintactico(); // Para obtener "longitud_textos"
  imem+=longitud_textos;

  test_buffer(&mem,&imem_max,imem);

  num_obj_predefinidos=num_obj;

  convert=1; ultima_linea=source; fwrite(&cero,1,1,lprg);
  acceso_remoto=0; parametros=0; linea=1;

  sintactico();

  i=obj; while (i<iobj) {
    if (i->usado) {
      linea=i->linea; ierror=i->ierror; c_error(0,121);
    } i++;
  }

  mem[2]=imem;
  mem[3]=max_process; // Antes long_header, ahora no se utiliza
  mem[4]=0; // Antes mem[1]-mem[3] (long datos globales), ahora no se utiliza
  mem[5]=iloc_len-iloc;
  mem[6]=iloc;
  mem[7]=0; // Antes imem+iloc (inicio textos), ahora no se utiliza
  mem[8]=imem+iloc; // Número de elementos ocupados en mem[]

  mensaje_compilacion(texto[204]);

  save_dbg();
  save_exec_bin();

  #ifdef listados
  l_objetos();
  l_ensamblador();
  #endif

  mensaje_compilacion(texto[205]);

  #ifdef SHARE
  mem[0]+=1024;
  #endif

  if (program_type==1) if ((f=fopen("install/setup.ovl","wb"))!=NULL) {
    fwrite(div_stub,1,602,f);
//    fwrite(mem,4,imem,f); fwrite(loc,4,iloc,f);
//    fclose(f);
    p=(byte*)malloc((imem+iloc)*4);
    m=(imem+iloc)*4+1024;
    q=(byte*)malloc(m);
    if (p!=NULL && q!=NULL) {
      fwrite(mem,4,9,f);
      memcpy(p,&mem[9],(imem-9)*4);
      memcpy(p+(imem-9)*4,loc,iloc*4);
      n=(imem-9+iloc)*4;
#ifndef ZLIB
      if (true) 
#else
	if (!compress(q,(uLongf*)&m,p,(unsigned long)n)) 
#endif 
	{
        fwrite(&n,1,4,f); // mem[0]..mem[8],longitud_datos_descomp,datos comp...
        fwrite(q,1,m,f);
        free(q); free(p);
        fclose(f);
      } else {
        free(q); free(p);
        fclose(f);
        c_error(0,0);
      }
    } else {
      if (p!=NULL) free(p);
      if (q!=NULL) free(q);
      fclose(f);
      c_error(0,0);
    }
  }

//if ((f=fopen(ExeGen,"wb"))!=NULL) {

  if (ejecutar_programa==3) mem[0]+=128;
  if (ignore_errors) mem[0]+=512;

  if ((f=fopen("system/EXEC.EXE","wb"))!=NULL) {
    fwrite(div_stub,1,602,f);
//    fwrite(mem,4,imem,f); fwrite(loc,4,iloc,f);
//    fclose(f);
    p=(byte*)malloc((imem+iloc)*4);
    m=(imem+iloc)*4+1024;
    q=(byte*)malloc(m);
    if (p!=NULL && q!=NULL) {
      fwrite(mem,4,9,f); // mem[0..8]
      memcpy(p,&mem[9],(imem-9)*4);
      memcpy(p+(imem-9)*4,loc,iloc*4);
      n=(imem-9+iloc)*4;
#ifndef ZLIB
      if (true) 
#else
	  if (!compress(q,(uLongf*)&m,p,n)) 
#endif
	  {
        fwrite(&n,1,4,f); // mem[0]..mem[8],longitud_datos_descomp,datos comp...
        fwrite(q,1,m,f);
        free(q); free(p);
        fclose(f);
      } else {
        free(q); free(p);
        fclose(f);
        c_error(0,0);
      }
    } else {
      if (p!=NULL) free(p);
      if (q!=NULL) free(q);
      fclose(f);
      c_error(0,0);
      //fwrite(mem,4,imem,f); fwrite(loc,4,iloc,f);
    }
  }

  // _case_sensitive
  memcpy(lower+129,"ueaaaaçeeeiiiaaeææooouuyouø£Ø×ƒaiou",35);
  memcpy(lower+'A',"abcdefghijklmnopqrstuvwxyz",26);
  lower['\xa5' /*'Ñ'*/]='\xa4' /*'ñ'*/;

  comp_exit();

}

void free_resources(void) {
  if (def!=NULL) fclose(def); 
  if (_buf!=NULL) free(_buf);
  if (frm!=NULL) free(frm);
  if (loc!=NULL) free(loc);
  if (mem!=NULL) free(mem);
  if (lins!=NULL) fclose(lins);
  if (lprg!=NULL) fclose(lprg);
  if (linf!=NULL) fclose(linf);
  if (vnom!=NULL) free(vnom);

  def = NULL;
  _buf = NULL;
  frm = NULL;
  loc = NULL;
  mem = NULL;
  lins = NULL;
  lprg = NULL;
  linf = NULL;
  vnom = NULL;
}

//═════════════════════════════════════════════════════════════════════════════
//      Errores internos del compilador
//═════════════════════════════════════════════════════════════════════════════

int _le,_t;
byte *_ie;

void save_error(word tipo) { // Guarda una posicion de error (de 0 .. 3)
  switch(tipo) {
    case 0: _le=linea; _ie=ierror; break;
    case 1: _le=old_linea; _ie=old_ierror_end; break;
    case 2: _le=old_linea; _ie=old_ierror; break;
    case 3: _le=old_linea; _ie=old_ierror_end; break;
  } _t=tipo;
}

// 0 - Inicio de la pieza actual
// 1 - Final de la pieza anterior mas un espacio (si se puede)
// 2 - Inicio de la pieza anterior
// 3 - Final de la pieza anterior
// 4 - Error guardado con save_error(0..3)

void c_error(word tipo, word e) {
  int columna=0;
  byte *_p,*p;

  numero_error=e;

  switch(tipo) {
    case 0: linea_error=linea; p=ierror; break;
    case 1: linea_error=old_linea; _p=p=old_ierror_end; break;
    case 2: linea_error=old_linea; p=old_ierror; break;
    case 3: linea_error=old_linea; _p=p=old_ierror_end; break;
    case 4: linea_error=_le; _p=p=_ie; tipo=_t; break;
  }

  if (e>=10) {
    while (--p>=source_ptr) {
      if (*p==cr) break;
      columna++;
    } if (p<source_ptr) columna++;
  }

  if (tipo==1) {
    if (*(_p+1)==' ' || *(_p+1)==cr || *(_p+1)==lf) columna++; columna++;
  } else if (tipo==3) columna++;

  columna_error=columna;

  // _case_sensitive
  memcpy(lower+129,"ueaaaaçeeeiiiaaeææooouuyouø£Ø×ƒaiou",35);
  memcpy(lower+'A',"abcdefghijklmnopqrstuvwxyz",26);
  lower['\xa5' /*'Ñ'*/]='\xa4' /*'ñ'*/;

  comp_exit();
}

//═════════════════════════════════════════════════════════════════════════════
//  Con el primer token leido guarda el inicio de una sentencia
//═════════════════════════════════════════════════════════════════════════════

void inicio_sentencia(void) {
  byte * p=ierror-1;
  inicio=imem;
  linea1=linea;
  columna1=0;
  if(p<_source)
		p=_source;
		
  while (*p!=cr && *p!=lf && p>_source) { columna1++; p--; }
}

//═════════════════════════════════════════════════════════════════════════════
//  Con el primer token que no es de la sentecia guarda el fin de una sentencia
//═════════════════════════════════════════════════════════════════════════════

void final_sentencia(void) {
  byte * p=old_ierror_end-1;
  final=imem-1;
  linea2=old_linea;
  columna2=0;

    if(p<_source)
		p=_source;

  while (*p!=cr && *p!=lf && p>_source) { columna2++; p--; }
}

//═════════════════════════════════════════════════════════════════════════════
//  Guarda un registro (inicio,final,linea1,...) en EXEC.LIN
//═════════════════════════════════════════════════════════════════════════════

void grabar_sentencia(void) {
  fwrite(&inicio,4,1,linf);
  fwrite(&final,4,1,linf);
  fwrite(&linea1,4,1,linf);
  fwrite(&columna1,4,1,linf);
  fwrite(&linea2,4,1,linf);
  fwrite(&columna2,4,1,linf);
}

//═════════════════════════════════════════════════════════════════════════════
//      Comprueba los límites de un buffer de generación de código
//═════════════════════════════════════════════════════════════════════════════

void test_buffer(int * * buffer,int * maximo,int n) {
  int max;

  if (n+buffer_grow>2*1000*1000) c_error(0,0);

  if (n>*maximo-security_distance) {
    max=*maximo; *maximo=n+buffer_grow; max=*maximo-max;
    if (*buffer==mem) {
      if ((*buffer=mem_ory=(int *)realloc(*buffer,*maximo*sizeof(memptrsize)))==NULL) c_error(0,0);
    } else {
      if ((*buffer=(int *)realloc(*buffer,*maximo*sizeof(memptrsize)))==NULL) c_error(0,0);
    }
    memset((byte*)*buffer+(*maximo-max)*sizeof(memptrsize),0,max*sizeof(memptrsize));
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Precarga de las estructuras léxicas, analiza el fichero ltlex.def
//═════════════════════════════════════════════════════════════════════════════

void analiza_ltlex(void){

  byte * buf, cont=1;
  int len;
  struct lex_ele * e;

  int t; //token (pieza)
  byte h; //hash (for id)
  byte * _ivnom;
  byte * * ptr;

  if ((def=fopen("system/ltlex.def","rb"))==NULL) c_error(0,1);
  fseek(def,0,SEEK_END); len=ftell(def);
  if ((_buf=buf=(byte *) malloc(len+2))==NULL) c_error(0,0);
  fseek(def,0,SEEK_SET);
  len=fread(buf,1,len,def);
  *(buf+len)=cr; *(buf+len+1)=cr;

  linea=1;
  do {

	 switch (*buf++) {
    case ' ': case tab:
      break;
    case  cr:
      if (*buf==cr) cont=0; else { linea++; buf++; } break;
    case ';':
      while (*buf!=cr) buf++; break;
    case '&':
      *buf=lower[*buf]; if (*buf>='0' && *buf<='9') t=(*buf++-'0')<<4;
      else if (*buf>='a' && *buf<='f') t=(*buf++-'a'+10)<<4; else c_error(0,2);
      *buf=lower[*buf]; if (*buf>='0' && *buf<='9') t+=(*buf++-'0');
      else if (*buf>='a' && *buf<='f') t+=(*buf++-'a'+10); else c_error(0,2);
      if (*buf==cr || *buf==' ' || *buf==tab) break;
      else if (lower[*buf]) {           //Analyzes a keyword
        _ivnom=ivnom.b; 
        *ivnom.p++=0; 
        *ivnom.p++=(byte*)t; 
        h=0;
        while (*ivnom.b=lower[*buf++]) h=((byte)(h<<1)+(h>>7))^(*ivnom.b++);
        ptr=&vhash[h]; while (*ptr) ptr=(byte **)*ptr; 
	*ptr=_ivnom;
        
	buf--; ivnom.b++;
      } else if (t>=0x78 && t<=0x7b) {  //Analiza un delimitador de literal
        lex_case[*buf]=(struct lex_ele*)l_lit;
      } else {                          //Analiza un nuevo símbolo
        if ((e=lex_case[*buf])==0) {
          if (num_nodos++==max_nodos) c_error(0,3);
          e=lex_case[*buf]=ilex_simb++; (*e).caracter=*buf++;
        } else buf++;
        while (*buf!=' ' && *buf!=tab && *buf!=cr) {
          if (lower[*buf]) c_error(0,4);
          if ((*e).siguiente==0)
            if (num_nodos++==max_nodos) c_error(0,3);
            else e=(*e).siguiente=ilex_simb++;
          else {
            e=(*e).siguiente;
            while ((*e).caracter!=*buf && (*e).alternativa)
              e=(*e).alternativa;
            if ((*e).caracter!=*buf)
              if (num_nodos++==max_nodos) c_error(0,3);
              else e=(*e).alternativa=ilex_simb++;
          } (*e).caracter=*buf++;
        } (*e).token=t;
      } break;
  } 
  }while (cont);

  free(_buf); _buf=NULL;
  fclose(def); def=NULL;

}

//═════════════════════════════════════════════════════════════════════════════
//      Precarga de la tabla de objetos, analiza el fichero ltobj.def
//═════════════════════════════════════════════════════════════════════════════

//      OJO, en este proceso no se comprueban los límites de memoria


void precarga_obj (void) {

  int len,ret,cod;
  struct objeto * ob;
  struct objeto * ob2;

  if ((def=fopen("system/ltobj.def","rb"))==NULL) c_error(0,6);
  fseek(def,0,SEEK_END); len=ftell(def);
  if ((_buf=source=(byte *) malloc(len+2))==NULL) c_error(0,0);
  fseek(def,0,SEEK_SET);
  len=fread(source,1,len,def);
  *(source+len)=cr; *(source+len+1)=cr;

  linea=1; lexico();

  while (pieza!=p_ultima) switch (pieza) {
    case p_const:
      lexico(); if (pieza!=p_id) c_error(0,7);
      ob=o; if ((*ob).tipo!=tnone) c_error(0,7); (*ob).tipo=tcons;
      lexico(); if (pieza!=p_asig) c_error(0,7);
      lexico();
      if (pieza==p_num) (*ob).cons.valor=pieza_num;
      else if (pieza==p_sub) {
        lexico(); if (pieza!=p_num) c_error(0,7);
        (*ob).cons.valor=-pieza_num;
      } else c_error(0,7);
      (*ob).cons.literal=0;
      lexico(); break;
    case p_global:
      lexico(); if (pieza==p_id) {
        ob=o; if ((*ob).tipo!=tnone) c_error(0,7);
        lexico(); if (pieza==p_corab) {
          (*ob).tipo=ttglo;
          (*ob).tglo.offset=imem;
          lexico(); if (pieza!=p_num) c_error(0,7);
          (*ob).tglo.len1=len=pieza_num;
          (*ob).tglo.len2=-1;
          (*ob).tglo.len3=-1;
          (*ob).tglo.totalen=len+1;
          do { mem[imem++]=0; } while (len--);
          lexico(); if (pieza!=p_corce) c_error(0,7);
          lexico();
        } else {
          (*ob).tipo=tvglo;
          (*ob).vglo.offset=imem;
          if (pieza!=p_asig) c_error(0,7); lexico();
          if (pieza==p_num) mem[imem++]=pieza_num;
          else if (pieza==p_sub) {
            lexico(); if (pieza!=p_num) c_error(0,7);
            mem[imem++]=-pieza_num;
          } else c_error(0,7); lexico();
        }
      } else if (pieza==p_struct) {
        lexico(); if (pieza!=p_id) c_error(0,7);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,7);
        lexico(); if (pieza!=p_corab) c_error(0,7);
        (*ob).tipo=tsglo; (*ob).sglo.offset=imem;
        lexico(); if (pieza!=p_num) c_error(0,7);
        (*ob).sglo.items1=pieza_num;
        (*ob).sglo.items2=(*ob).sglo.items3=-1;
        (*ob).sglo.totalitems=pieza_num+1;
        lexico(); if (pieza!=p_corce) c_error(0,7);
        (*ob).sglo.len_item=0; member=ob; lexico();
        if (pieza==p_end) c_error(0,7); len=0;
        while (pieza!=p_end) {

          if (pieza==p_string) {

            lexico();
            if (pieza!=p_id) c_error(0,7);
            if ((*o).tipo!=tnone) c_error(0,7);
            ob2=o; lexico();
            (*ob2).tipo=tcglo;
            (*ob2).cglo.offset=len+1;
            if (pieza!=p_corab) c_error(0,7); lexico();
            if (((*ob2).cglo.totalen=constante())<0) c_error(0,7);
            if ((*ob2).cglo.totalen>0xFFFFF) c_error(0,7);
            if (pieza!=p_corce) c_error(0,7); lexico();

            mem[imem]=0xDAD00000|(*ob2).cglo.totalen;
            len+=1+((*ob2).cglo.totalen+5)/4;
            mem[imem+1]=0;
            imem+=1+((*ob2).cglo.totalen+5)/4;
            (*ob).sglo.len_item+=1+((*ob2).cglo.totalen+5)/4;

          } else {

            if (pieza!=p_id) c_error(0,7);
            if ((*o).tipo!=tnone) c_error(0,7);
            ob2=o; lexico();
            if (pieza==p_corab) {
              (*ob2).tipo=ttglo;
              (*ob2).tglo.offset=len;
              lexico(); if (pieza!=p_num) c_error(0,7);
              (*ob2).tglo.len1=pieza_num;
              (*ob2).tglo.len2=-1;
              (*ob2).tglo.len3=-1;
              (*ob2).tglo.totalen=pieza_num+1;
              len+=pieza_num+1;
              memset(&mem[imem],0,(pieza_num+1)*4);
              imem+=pieza_num+1;
              (*ob).sglo.len_item+=pieza_num+1;
              lexico();
              if (pieza!=p_corce) c_error(0,7);
              lexico();
            } else {
              (*ob2).tipo=tvglo;
              (*ob2).vglo.offset=len++;
              mem[imem++]=0;
              (*ob).sglo.len_item++;
            }

          }

        } member=NULL; lexico();

        if ((*ob).sglo.totalitems>1) {
          len=((*ob).sglo.totalitems-1)*(*ob).sglo.len_item-1;
          do { mem[imem]=mem[imem-(*ob).sglo.len_item]; imem++; } while (len--);
        }

      } else c_error(0,7); break;
    case p_local:
      lexico(); if (pieza==p_id) {
        ob=o; if ((*ob).tipo!=tnone) c_error(0,7); (*ob).tipo=tvloc;
        (*ob).vloc.offset=iloc;
        lexico(); if (pieza!=p_asig) c_error(0,7);
        lexico();
        if (pieza==p_num) loc[iloc++]=pieza_num;
        else if (pieza==p_sub) {
          lexico(); if (pieza!=p_num) c_error(0,7);
          loc[iloc++]=-pieza_num;
        } else c_error(0,7);
        lexico();
      } else if (pieza==p_struct) {
        lexico(); if (pieza!=p_id) c_error(0,7);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,7);
        lexico(); if (pieza!=p_corab) c_error(0,7);
        (*ob).tipo=tsloc; (*ob).sloc.offset=iloc;
        lexico(); if (pieza!=p_num) c_error(0,7);
        (*ob).sloc.items1=pieza_num;
        (*ob).sloc.items2=(*ob).sloc.items3=-1;
        (*ob).sloc.totalitems=pieza_num+1;
        lexico(); if (pieza!=p_corce) c_error(0,7);
        (*ob).sloc.len_item=0; member=ob; lexico();
        if (pieza==p_end) c_error(0,7);

        len=0;

        while (pieza!=p_end) {
          if (pieza!=p_id) c_error(0,7);
          if ((*o).tipo!=tnone) c_error(0,7);
          (*o).tipo=tvloc; (*o).vloc.offset=len;
          (*ob).sloc.len_item++;
          lexico();
          if (pieza==p_asig) {
            lexico();
            if (pieza==p_num) loc[iloc+len]=pieza_num;
            else if (pieza==p_sub) {
              lexico(); if (pieza!=p_num) c_error(0,7);
              loc[iloc+len]=-pieza_num;
            } else c_error(0,7);
            lexico();
          } else loc[iloc+len]=0;
          len++;
        }

        iloc+=(*ob).sloc.totalitems*(*ob).sloc.len_item;

        member=NULL; lexico();
      } else c_error(0,7); break;

    case p_function: // function 0 int signal(0,0) // ....
      lexico(); if (pieza!=p_num) c_error(0,7);
      cod=pieza_num; lexico();
      if (pieza==p_int) ret=0;
      else if (pieza==p_string) ret=1;
      else c_error(0,7);
      lexico(); if (pieza!=p_id) c_error(0,7);
      ob=o; if ((*ob).tipo!=tnone) c_error(0,7); (*ob).tipo=tfunc;
      lexico(); if (pieza!=p_abrir) c_error(0,7);
      lexico(); len=0;
      while (pieza!=p_cerrar) {
        if (pieza!=p_num) c_error(0,7);
        (*ob).func.par[len++]=pieza_num;
        lexico(); if (pieza==p_coma) lexico();
      }
      (*ob).func.codigo=cod;
      (*ob).func.ret=ret;
      (*ob).func.num_par=len;
      lexico(); break;

    default: c_error(0,7); break;
  }

  free(_buf); _buf=NULL;
  fclose(def); def=NULL;

}

//═════════════════════════════════════════════════════════════════════════════
//  Adivina cual será la siguiente pieza lexica leida (y donde estará)
//═════════════════════════════════════════════════════════════════════════════

int next_pieza;
int next_linea;
byte * next_source;

byte * next_lexico(byte * _source, int coment, int linea) { // No genera nunca errores

  byte ** ptr, * _ivnom, h;
  struct lex_ele * e;

  if (!coment && linea) {
    old_linea=linea; old_ierror=ierror; old_ierror_end=ierror_end;
  }

  lex_scan:

  if (linea) ierror=_source;
  next_source=_source;

  switch ((memptrsize)lex_case[*_source]) {

    case l_err:
      if (coment) { _source++; goto lex_scan; }
      next_pieza=0; break;

    case l_cr :
      if (linea) {

        if (convert) {
          fwrite(ultima_linea,1,_source-ultima_linea,lprg);
          fwrite(&cero,1,1,lprg);
        }

        linea++; if ((*++_source)==lf) { _source++; ultima_linea=_source; goto lex_scan; }
        next_pieza=0; break;

      } else {

        if ((*++_source)==lf) { _source++; goto lex_scan; }
        next_pieza=0; break;

      }

    case l_id :
      if (coment) { _source++; goto lex_scan; }
      _ivnom=ivnom.b; *ivnom.p++=0; *ivnom.p++=0; h=0;
      while (*ivnom.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*ivnom.b++);
      ivnom.b++; _source--;
      ptr=&vhash[h];
      while (*ptr!=NULL && strcmp((char *)(ptr+2),(char *)_ivnom+ptr8)) ptr=(byte **)*ptr;
      if (!strcmp((char *)(ptr+2),(char *)_ivnom+ptr8)) { // id encontrado
        ivnom.b=_ivnom; // lo saca de vnom
        next_pieza=(memptrsize)*(ptr+1);
        if (next_pieza<256 && next_pieza>=0) { // palabra reservada (token)
          if (next_pieza==p_rem) { while (*_source!=cr) _source++; goto lex_scan; }
        } else { // objeto (id anterior)
          next_pieza=p_id;
        }
      } else {
        ivnom.b=_ivnom; // lo saca de vnom
        next_pieza=p_id; // id nuevo
      } break;

    case l_spc:
      while ((*++_source)==' '); goto lex_scan;

    case l_lit:
      if (coment) { _source++; goto lex_scan; }
      next_pieza=p_lit;
      break;

    case l_num:
      if (coment) { _source++; goto lex_scan; }
      next_pieza=p_num;
      break;

    default: // puntero a un lex_ele
      e=lex_case[*_source++]; next_pieza=(*e).token; _ivnom=_source;

      while (e=(*e).siguiente) {
        while (*_source!=(*e).caracter && (*e).alternativa) e=(*e).alternativa;
        if (*_source++==(*e).caracter && (*e).token) {
          next_pieza=(*e).token; _ivnom=_source;
        }
      } _source=_ivnom;

      if (next_pieza==p_rem && !coment) {
        while (*_source!=cr) _source++; goto lex_scan;
      }

      if (next_pieza==p_ini_rem) {
        coment++; do {
          _source=next_lexico(_source,coment,linea);
          linea=next_linea;
        } while (next_pieza!=p_end_rem);
        coment--;
        goto lex_scan;
      } break;

  }
  next_linea=linea;
  return(_source);
}


//═════════════════════════════════════════════════════════════════════════════
//      Lexico (lee una nueva pieza del *source)
//═════════════════════════════════════════════════════════════════════════════

int IsWAV(char *FileName);
int en_fopen=0;

void lexico(void) {

  struct objeto * * ptr_o;
  byte ** ptr, * _ivnom, h, * _source=source;
  struct lex_ele * e;
  char cwork[66];
  FILE * f;

  int empaquetable;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  int n;

  if (!coment) {old_linea=linea; old_ierror=ierror; old_ierror_end=ierror_end;}

  lex_scan: ierror=_source;
  switch ((memptrsize)lex_case[*_source]) { // Puntero a un lex_ele o l_???

    case l_err:
      if (coment) { pieza=p_rem; _source++; } else c_error(0,10); break;

    case l_cr :

      if (convert) {
        fwrite(ultima_linea,1,_source-ultima_linea,lprg);
        fwrite(&cero,1,1,lprg);
      }

      linea++; if ((*++_source)==lf) { _source++; ultima_linea=_source; goto lex_scan; }
      pieza=p_ultima; break; // eof

    case l_id :
      if (coment) { pieza=p_rem; _source++; break; }
      _ivnom=ivnom.b; *ivnom.p++=0; *ivnom.p++=0; h=0;
      while (*ivnom.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*ivnom.b++);
      ivnom.b++; 
      _source--; 
      if (ivnom.b-vnom>max_obj*long_med_id) c_error(0,100);

      ptr=&vhash[h];
      while (*ptr && strcmp((char *)(ptr+2),((char *)_ivnom+ptr8))!=0) {
		  ptr=(byte**)*ptr; 
	  }
	  
      if (!strcmp((char *)(ptr+2),(char *)_ivnom+ptr8)) { // id found
        ivnom.b=_ivnom; // lo saca de vnom
        pieza=(memptrsize)*(ptr+1);
        if (pieza<256 && pieza>=0) { // reserved word (t	oken)
			
          if (pieza==p_rem) { while (*_source!=cr) _source++; goto lex_scan; }

        } else { // objeto (id anterior)
          ptr_o=(struct objeto **)(ptr+1); o=*ptr_o; pieza=p_id;
          while(o!=NULL && ( ((*o).bloque && bloque_lexico!=(*o).bloque) ||
                             ((*o).member!=member) )) o=(*o).anterior;
          if(o==NULL) { // Not found
            o=iobj++; (*o).anterior=*ptr_o; *ptr_o=o;
            (*o).name=(byte*)(ptr_o+1);
            (*o).member=member;
            (*o).param=0;
            if (parametros) (*o).bloque=bloque_actual;
            #ifndef SHARE
            if (num_obj++==max_obj) c_error(0,102);
            #else
            if (num_obj++==max_obj) c_error(0,101);
            #endif
          } else {
            if ((*o).tipo==tcons) pieza_num=(*o).cons.valor;
          }
        }
      } else {
        *ptr=_ivnom; ptr_o=(struct objeto **)(_ivnom+ptr4); *ptr_o=o=iobj++; pieza=p_id; // id nuevo
        (*o).name=(byte *)_ivnom+ptr8;
        (*o).member=member;
        if (parametros) (*o).bloque=bloque_actual;
        if (num_obj++==max_obj) c_error(0,102);
      } break;

    case l_spc:
      while ((*++_source)==' '); goto lex_scan;

    case l_lit:
      if (coment) { pieza=p_rem; _source++; break; }
      pieza=p_lit; h=*_source; _ivnom=ivnom.b; // Literal entre dos h
      do {
        if (*++_source==cr) c_error(0,11);
        if (*_source==h)
          if (*(_source+1)==h) *_ivnom=*++_source; else *_ivnom=0;
        else *_ivnom=*_source;
      } while (*_ivnom++); _source++;

      next_lexico(_source,0,0);
      while (next_pieza==p_lit) {
        next_lexico(_source,0,linea);
        linea=next_linea;
        _source=next_source;
        h=*_source; _ivnom--;
        do {
          if (*++_source==cr) c_error(0,11);
          if (*_source==h)
            if (*(_source+1)==h) *_ivnom=*++_source; else *_ivnom=0;
          else *_ivnom=*_source;
        } while (*_ivnom++); _source++;
        next_lexico(_source,0,0);
      }

      n=(strlen((char *)ivnom.b)+ptr4)/4;
      memcpy(&mem_ory[itxt],ivnom.b,strlen((char *)ivnom.b)+1);

//fprintf(stdout,"[COMPILER: LEXICO] FILE: %s\n",(char *)ivnom.b);
//fprintf(stdout,"[COMPILER: LEXICO] LOOKING FOR FILE: %s [%s] [%s]\n",(char *)ivnom.b,full,(char *)&tipo[8]);

      if ((!ivnom.b[0]!='.' && ivnom.b[1]!=0) && (ivnom.b[0]!='/' && ivnom.b[1]!=0) && strcmp("/",(char *)ivnom.b) && (f=div_open_file((char *)ivnom.b))!=NULL) {
	      fprintf(stdout,"[COMPILER: LEXICO] FOUND FILE: [%s] [%s] [%s]\n",(char *)ivnom.b,full,(char *)&tipo[8]);

        empaquetable=0;

        // Determinar si el archivo es empaquetable ...

        _splitpath(full,(char *)drive,(char *)dir,(char *)fname,(char *)ext);
        strupr((char *)ext);

        if (!stricmp((char *)ext,".MOD") || !stricmp((char *)ext,".S3M") || !stricmp((char *)ext,".XM")) {
          empaquetable=1;
        } else if (!stricmp((char *)ext,".PCM")) {
          empaquetable=1;
        } else if (!stricmp((char *)ext,".PCX")) {
          if (fread(cwork,1,66,f)>0) {
            if(cwork[2]==1 && cwork[3]==8 && cwork[65]==1) empaquetable=1;
          }
        } else if (fread(cwork,1,8,f)>0) {
          if (!strcmp((char *)cwork,"pal\x1a\x0d\x0a")) empaquetable=1;
          if (!strcmp((char *)cwork,"map\x1a\x0d\x0a")) empaquetable=1;
          if (!strcmp((char *)cwork,"fpg\x1a\x0d\x0a")) empaquetable=1;
          if (!strcmp((char *)cwork,"fnt\x1a\x0d\x0a")) empaquetable=1;
          if (!strcmp((char *)cwork,"wld\x1a\x0d\x0a")) empaquetable=1;
          if (!strcmp((char *)cwork,"wld\x1a\x0d\x0a\x01")) empaquetable=1;
        }

        fclose(f);

        if (IsWAV(full)) empaquetable=1;

        /*// ???
		empaquetable=1;*/

        if (!empaquetable || en_fopen) {
fprintf(stdout,"[COMPILER: LEXICO] NOT PACKEABLE FILE: [%s] [%s] [%s]\n",(char *)ivnom.b,full,(char *)&tipo[8]);

					fwrite("+",1,1,lins);
				}
				else {
fprintf(stdout,"[COMPILER: LEXICO] PACKEABLE FILE: [%s] [%s] [%s]\n",(char *)ivnom.b,full,(char *)&tipo[8]);
				}
        fwrite(full,1,strlen(full)+1,lins);
        fflush(lins);
      }

      pieza_num=itxt; itxt+=n; break;

    case l_num:
      if (coment) { pieza=p_rem; _source++; break; }

      pieza=p_num; pieza_num=0;
      if (*_source=='0' && lower[*(_source+1)]=='x') {
        _source+=2;
        while ((memptrsize)lex_case[*_source]==l_num ||
               (lower[*_source]>='a' && lower[*_source]<='f')) {
          if ((memptrsize)lex_case[*_source]==l_num)
            pieza_num=pieza_num*16+*_source++-0x30;
          else pieza_num=pieza_num*16+lower[*_source++]-'a'+10;
        }
      } else do {
        pieza_num=pieza_num*10+*_source++-0x30;
      } while ((memptrsize)lex_case[*_source]==l_num);
      break;

    default: // puntero a un lex_ele

      e=lex_case[*_source++]; _ivnom=_source; pieza=(*e).token;
      while (e=(*e).siguiente) {
        while (*_source!=(*e).caracter && (*e).alternativa) e=(*e).alternativa;
        if (*_source++==(*e).caracter && (*e).token) {
          pieza=(*e).token; _ivnom=_source;
        }
      } _source=_ivnom;

      if (pieza==p_rem && !coment) {
        while (*_source!=cr) _source++;
        goto lex_scan;
      }
      if (pieza==p_ini_rem) {
        coment++;
        do {
           source=_source; lexico(); _source=source;
        } while (pieza!=p_end_rem);
        coment--;
        goto lex_scan;
      }

      if (pieza==p_ultima) if (coment) c_error(0,55); else c_error(0,12);

      break;

  } source=_source; ierror_end=_source-1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Pasa varias comas o puntos y coma seguidos
//═════════════════════════════════════════════════════════════════════════════

void pasa_ptocoma(void) {
  do {
    lexico();
  } while (pieza==p_ptocoma);
}

//═════════════════════════════════════════════════════════════════════════════
//      Análisis de una declaración pointer (int, word o byte)
//═════════════════════════════════════════════════════════════════════════════

struct objeto * analiza_pointer(int tipo, int offset) {
  struct objeto * ob;
  int len1,len2,len3;

  if (pieza!=p_id) c_error(1,148);
  ob=o; if ((*ob).tipo!=tnone) {
    if (parametros==-1 && (*ob).param==1 && (*ob).bloque==bloque_actual) {
      if ((*ob).tipo==tipo) { // Se repite un pointer parámetro como private
        save_error(0); lexico();
        len1=-1; len2=-1; len3=-1;
        if (pieza==p_corab) { lexico();
          if ((len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if ((len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if ((len3=constante())<0) c_error(4,35);
            }
          } if (pieza!=p_corce) c_error(3,26); lexico();
        }
        if (len1!=(*ob).pilo.len1 || len2!=(*ob).pilo.len2 || len3!=(*ob).pilo.len3) c_error(4,141);
        else if (pieza==p_asig) c_error(0,54);
        else {
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
          (*ob).param++; // No permite volver a redeclararlo
          return(NULL);
        }
      } else c_error(0,117);
    } else c_error(0,117);
  } else lexico();
  if (parametros==-1) (*ob).bloque=bloque_actual;
  (*ob).pilo.len1=-1; (*ob).pilo.len2=-1; (*ob).pilo.len3=-1;
  if (pieza==p_corab) { lexico();
    if (((*ob).pilo.len1=constante())<0) c_error(4,35);
    if (pieza==p_coma) {
      lexico();
      if (((*ob).pilo.len2=constante())<0) c_error(4,35);
      if (pieza==p_coma) {
        lexico();
        if (((*ob).pilo.len3=constante())<0) c_error(4,35);
      }
    }
    if (pieza!=p_corce) c_error(3,26); lexico();
  } (*ob).pilo.totalen=0;
  if ((*ob).pilo.len1>-1) (*ob).pilo.totalen=(*ob).pilo.len1+1;
  if ((*ob).pilo.len2>-1) (*ob).pilo.totalen*=(*ob).pilo.len2+1;
  if ((*ob).pilo.len3>-1) (*ob).pilo.totalen*=(*ob).pilo.len3+1;

  (*ob).tipo=tipo; (*ob).pilo.offset=offset;

  return(ob);
}

//═════════════════════════════════════════════════════════════════════════════

int analiza_pointer_struct(int tipo, int offset, struct objeto * estructura) {
  struct objeto * ob;
  int items1,items2,items3;

  if (pieza!=p_id) c_error(1,148);
  ob=o; if ((*ob).tipo!=tnone) {
    if (parametros==-1 && (*ob).param==1 && (*ob).bloque==bloque_actual) {
      if ((*ob).tipo==tipo) { // Se repite un pointer parámetro como private
        save_error(0); lexico();
        items1=-1; items2=-1; items3=-1;
        if (pieza==p_corab) { lexico();
          if ((items1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if ((items2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if ((items3=constante())<0) c_error(4,35);
            }
          } if (pieza!=p_corce) c_error(3,26); lexico();
        }
        if (items1!=(*ob).psgl.items1 || items2!=(*ob).psgl.items2 || items3!=(*ob).psgl.items3) c_error(4,141);
        else if (pieza==p_asig) c_error(0,54);
        else {
          (*ob).param++; return(0); // No permite volver a redeclararlo
        }
      } else c_error(0,117);
    } else c_error(0,117);
  } else lexico();
  if (parametros==-1) (*ob).bloque=bloque_actual;
  (*ob).psgl.items1=-1; (*ob).psgl.items2=-1; (*ob).psgl.items3=-1;
  if (pieza==p_corab) { lexico();
    if (((*ob).psgl.items1=constante())<0) c_error(4,35);
    if (pieza==p_coma) {
      lexico();
      if (((*ob).psgl.items2=constante())<0) c_error(4,35);
      if (pieza==p_coma) {
        lexico();
        if (((*ob).psgl.items3=constante())<0) c_error(4,35);
      }
    }
    if (pieza!=p_corce) c_error(3,26); lexico();
  } (*ob).psgl.totalitems=0;
  if ((*ob).psgl.items1>-1) (*ob).psgl.totalitems=(*ob).psgl.items1+1;
  if ((*ob).psgl.items2>-1) (*ob).psgl.totalitems*=(*ob).psgl.items2+1;
  if ((*ob).psgl.items3>-1) (*ob).psgl.totalitems*=(*ob).psgl.items3+1;

  (*ob).tipo=tipo;                // tpsgl o tpslo
  (*ob).psgl.offset=offset;       // del pointer
  (*ob).psgl.ostruct=estructura;  // struct original

  // (*ob).psgl.len_item ­ (*((*ob).psgl.ostruct)).len_item;

  return(1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Analisis sintáctico (dirige todo el proceso de compilación)
//═════════════════════════════════════════════════════════════════════════════

int analiza_struct(int offstruct) { // tras " struct id [ <const> ] " // id­member
  int len=0,dup,i,_itxt,_imem;
  struct objeto * ob;
  struct objeto * old_member,* member2;
  byte * oimemptr;

  while (pieza==p_ptocoma) lexico();

  while (pieza!=p_end) {

    if (pieza==p_struct) {                              // Miembro struct ───

      lexico();

      if (pieza==p_pointer) { // Se define un puntero a struct

        old_member=member; member=NULL; lexico(); member=old_member;
        if (pieza!=p_id) c_error(1,110); ob=o;

        if ((*ob).tipo==tnone) c_error(0,150); // No se define el pointer así
        if ((*ob).tipo!=tsglo && (*ob).tipo!=tsloc) c_error(0,150);
        lexico();
        puntero_a_struct:
        analiza_pointer_struct(tpsgl,len,ob);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;

        if (pieza==p_coma) {
          lexico();
          if (pieza==p_pointer) lexico();
          goto puntero_a_struct;
        } else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else {

        if (pieza!=p_id) c_error(1,110);
        ob=o; 
        old_member=member; member=ob; lexico();
        if ((*ob).tipo!=tnone) c_error(2,22);

        (*ob).tipo=tsglo; (*ob).sglo.offset=len;
        if (pieza==p_corab) {
          member2=member; member=NULL; lexico();
          if (((*ob).sglo.items1=constante())<0) c_error(4,123);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).sglo.items2=constante())<0) c_error(4,123);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).sglo.items3=constante())<0) c_error(4,123);
            } else (*ob).sglo.items3=-1;
          } else { (*ob).sglo.items2=-1; (*ob).sglo.items3=-1; }
          member=member2;
          (*ob).sglo.totalitems=(*ob).sglo.items1+1;
          if ((*ob).sglo.items2>-1) (*ob).sglo.totalitems*=(*ob).sglo.items2+1;
          if ((*ob).sglo.items3>-1) (*ob).sglo.totalitems*=(*ob).sglo.items3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
        } else {
          (*ob).sglo.totalitems=1;
          (*ob).sglo.items1=0; (*ob).sglo.items2=-1; (*ob).sglo.items3=-1;
        }
        if (((*ob).sglo.len_item=analiza_struct(offstruct+len))==0) c_error(0,57);
        i=offstruct+len; dup=(*ob).sglo.totalitems+1;
        if (dup>1) {
          test_buffer(&mem,&imem_max,i+(*ob).sglo.len_item*(*ob).sglo.totalitems);
          test_buffer(&frm,&ifrm_max,i+(*ob).sglo.len_item*(*ob).sglo.totalitems);
          while (--dup) {
          	if(i!=offstruct+len) {
	            memcpy(&mem[i],&mem[offstruct+len],(*ob).sglo.len_item<<2);
	            memcpy(&frm[i],&frm[offstruct+len],(*ob).sglo.len_item<<2);
	        }
            i+=(*ob).sglo.len_item;
          }
        }
        len+=(*ob).sglo.len_item*(*ob).sglo.totalitems;
        member=old_member; lexico();
        while (pieza==p_ptocoma) lexico();
      }
    } else if (pieza==p_string) {                     // Miembro cadena

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a byte

        lexico(); puntero1:
        ob=analiza_pointer(tpcgl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;

        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero1;
        }

      } else {

        if (pieza!=p_id) c_error(1,125);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=tcglo; (*ob).cglo.offset=len+1;
        if (pieza==p_corab) {
          lexico();
          if (pieza==p_corce) {
            lexico();
            (*ob).cglo.totalen=255;
          } else {
            if (((*ob).cglo.totalen=constante())<0) c_error(4,127);
            if ((*ob).cglo.totalen>0xFFFFF) c_error(4,135);
            if (pieza!=p_corce) c_error(3,26);
            lexico();
          }
        } else (*ob).cglo.totalen=255;

        test_buffer(&frm,&ifrm_max,offstruct+len);
        frm[offstruct+len]=0xDAD00000;

        if (pieza==p_asig) {
          save_error(1);
          _itxt=itxt;
          lexico();
          if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
            c_error(3,128);
          if (strlen((char*)&mem[pieza_num])>(*ob).cglo.totalen+1)
            c_error(4,129);
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cglo.totalen;
          strcpy((char*)&mem[offstruct+len+1],(char*)&mem[pieza_num]);
          len+=1+((*ob).cglo.totalen+5)/4;
          itxt=_itxt; // Saca la cadena del segmento de textos
          lexico();
        } else {
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cglo.totalen;
          len+=1+((*ob).cglo.totalen+5)/4;
        }
      }

      if (pieza==p_coma) pieza=p_string; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_byte) {                         // Miembro byte ───

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a byte

        lexico(); puntero2:
        ob=analiza_pointer(tpbgl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;

        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero2;
        }

      } else {

        if (pieza!=p_id) c_error(1,142);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=tbglo; (*ob).bglo.offset=len;
        if (pieza==p_corab) { lexico();
          if (((*ob).bglo.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).bglo.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).bglo.len3=constante())<0) c_error(4,35);
            } else (*ob).bglo.len3=-1;
          } else { (*ob).bglo.len2=-1; (*ob).bglo.len3=-1; }
          (*ob).bglo.totalen=(*ob).bglo.len1+1;
          if ((*ob).bglo.len2>-1) (*ob).bglo.totalen*=(*ob).bglo.len2+1;
          if ((*ob).bglo.len3>-1) (*ob).bglo.totalen*=(*ob).bglo.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).bglo.totalen+3)/4);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).bglo.totalen+3)/4);
          memset(&frm[offstruct+len],2,(*ob).bglo.totalen);

          if (pieza==p_asig) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexico();
            oimemptr=(byte*)&mem[imem];
            tglo_init(2);
            if (imemptr-oimemptr>(*ob).bglo.totalen) c_error(4,33);
            imem=_imem;
          } (*ob).bglo.totalen=((*ob).bglo.totalen+3)/4;
        } else {
          (*ob).tipo=tbglo; (*ob).bglo.offset=len;
          (*ob).bglo.len1=0;
          (*ob).bglo.len2=-1;
          (*ob).bglo.len3=-1;
          (*ob).bglo.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=2;

          if (pieza==p_asig) {
            save_error(1);
            lexico();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constante();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>255) c_error(4,143);
          }
        } len+=(*ob).bglo.totalen;
      }

      if (pieza==p_coma) pieza=p_byte; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_word) {                         // Miembro word ───

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a word

        lexico(); puntero3:
        ob=analiza_pointer(tpwgl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero3;
        }

      } else {

        if (pieza!=p_id) c_error(1,145);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=twglo; (*ob).wglo.offset=len;
        if (pieza==p_corab) { lexico();
          if (((*ob).wglo.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).wglo.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).wglo.len3=constante())<0) c_error(4,35);
            } else (*ob).wglo.len3=-1;
          } else { (*ob).wglo.len2=-1; (*ob).wglo.len3=-1; }
          (*ob).wglo.totalen=(*ob).wglo.len1+1;
          if ((*ob).wglo.len2>-1) (*ob).wglo.totalen*=(*ob).wglo.len2+1;
          if ((*ob).wglo.len3>-1) (*ob).wglo.totalen*=(*ob).wglo.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).wglo.totalen+1)/2);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).wglo.totalen+1)/2);
          memset(&frm[offstruct+len],1,(*ob).wglo.totalen*2);

          if (pieza==p_asig) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexico();
            oimemptr=(byte*)&mem[imem];
            tglo_init(1);
            if (imemptr-oimemptr>(*ob).wglo.totalen*2) c_error(4,33);
            imem=_imem;
          } (*ob).wglo.totalen=((*ob).wglo.totalen+1)/2;
        } else {
          (*ob).tipo=twglo; (*ob).wglo.offset=len;
          (*ob).wglo.len1=0;
          (*ob).wglo.len2=-1;
          (*ob).wglo.len3=-1;
          (*ob).wglo.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=1;

          if (pieza==p_asig) {
            save_error(1);
            lexico();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constante();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>65535) c_error(4,144);
          }
        } len+=(*ob).wglo.totalen;
      }

      if (pieza==p_coma) pieza=p_word; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_int || pieza==p_id || pieza==p_pointer) {

      if (pieza==p_int) {
        lexico();
        if (pieza!=p_id && pieza!=p_pointer) c_error(1,149);
      }

      if (pieza==p_pointer) { // Miembro puntero a int

        lexico(); puntero4:
        ob=analiza_pointer(tpigl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero4;
        }

      } else {

        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        if (pieza==p_corab) { lexico();                   // Miembro tabla ───
          (*ob).tipo=ttglo; (*ob).tglo.offset=len;
          if (((*ob).tglo.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).tglo.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).tglo.len3=constante())<0) c_error(4,35);
            } else (*ob).tglo.len3=-1;
          } else { (*ob).tglo.len2=-1; (*ob).tglo.len3=-1; }
          (*ob).tglo.totalen=(*ob).tglo.len1+1;
          if ((*ob).tglo.len2>-1) (*ob).tglo.totalen*=(*ob).tglo.len2+1;
          if ((*ob).tglo.len3>-1) (*ob).tglo.totalen*=(*ob).tglo.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();

          test_buffer(&mem,&imem_max,offstruct+len+(*ob).tglo.totalen);
          if (pieza==p_asig) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexico(); tglo_init(0);
            if (imem-(offstruct+len)>(*ob).tglo.totalen) c_error(4,33);
            imem=_imem;
          } len+=(*ob).tglo.totalen;

        } else {                                          // Miembro variable ───

          (*ob).tipo=tvglo; (*ob).vglo.offset=len;
          test_buffer(&mem,&imem_max,offstruct+len);
          if (pieza==p_asig) {
            lexico(); mem[offstruct+len]=constante();
          } len+=1;

        }
      }
      if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
      while (pieza==p_ptocoma || pieza==p_coma) lexico();
    } else c_error(0,58);
  } return(len);
}

//───────────────────────────────────────────────────────────────────────────

int analiza_struct_local(int offstruct) { // tras " struct id [ <const> ] " // id­member
  int len=0,dup,i,_itxt,_iloc;
  struct objeto * ob;
  struct objeto * old_member,* member2;
  byte * oimemptr;

  while (pieza==p_ptocoma) lexico();

  while (pieza!=p_end) {

    if (pieza==p_struct) {                              // Miembro struct ───

      lexico();

      if (pieza==p_pointer) { // Se define un puntero a struct

        old_member=member; member=NULL; lexico(); member=old_member;
        if (pieza!=p_id) c_error(1,110); ob=o;

        if ((*ob).tipo==tnone) c_error(0,150); // No se define el pointer así
        if ((*ob).tipo!=tsglo && (*ob).tipo!=tsloc) c_error(0,150);
        lexico();
        puntero_a_struct:
        analiza_pointer_struct(tpslo,len,ob);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); loc[offstruct+len]=constante();
        } len+=1;

        if (pieza==p_coma) {
          lexico();
          if (pieza==p_pointer) lexico();
          goto puntero_a_struct;
        } else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else {

        if (pieza!=p_id) c_error(1,110);
        ob=o; 

        old_member=member; member=ob; lexico();
        if ((*ob).tipo!=tnone) c_error(2,22);

        (*ob).tipo=tsloc; (*ob).sloc.offset=len;
        if (pieza==p_corab) {
          member2=member; member=NULL; lexico();
          if (((*ob).sloc.items1=constante())<0) c_error(4,123);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).sloc.items2=constante())<0) c_error(4,123);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).sloc.items3=constante())<0) c_error(4,123);
            } else (*ob).sloc.items3=-1;
          } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
          member=member2;
          (*ob).sloc.totalitems=(*ob).sloc.items1+1;
          if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
          if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
        } else {
          (*ob).sloc.totalitems=1;
          (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
        }
        if (((*ob).sloc.len_item=analiza_struct_local(offstruct+len))==0) c_error(0,57);
        i=offstruct+len; dup=(*ob).sloc.totalitems+1;
        if (dup>1) {
          test_buffer(&loc,&iloc_max,i+(*ob).sloc.len_item*(*ob).sloc.totalitems);
          test_buffer(&frm,&ifrm_max,i+(*ob).sloc.len_item*(*ob).sloc.totalitems);
          while (--dup) {
            memcpy(&loc[i],&loc[offstruct+len],(*ob).sloc.len_item<<2);
            memcpy(&frm[i],&frm[offstruct+len],(*ob).sloc.len_item<<2);
            i+=(*ob).sloc.len_item;
          }
        }
        len+=(*ob).sloc.len_item*(*ob).sloc.totalitems;
        member=old_member; lexico();
        while (pieza==p_ptocoma) lexico();
      }

    } else if (pieza==p_string) {                     // Miembro cadena

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a byte

        lexico(); puntero1:
        ob=analiza_pointer(tpclo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); loc[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero1;
        }

      } else {

        if (pieza!=p_id) c_error(1,125);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=tcloc; (*ob).cloc.offset=len+1;
        if (pieza==p_corab) {
          lexico();
          if (pieza==p_corce) {
            lexico();
            (*ob).cloc.totalen=255;
          } else {
            if (((*ob).cloc.totalen=constante())<0) c_error(4,127);
            if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
            if (pieza!=p_corce) c_error(3,26);
            lexico();
          }
        } else (*ob).cloc.totalen=255;

        test_buffer(&frm,&ifrm_max,offstruct+len);
        frm[offstruct+len]=0xDAD00000;

        if (pieza==p_asig) {
          save_error(1);
          _itxt=itxt;
          lexico();
          if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
            c_error(3,128);
          if (strlen((char*)&mem[pieza_num])>(*ob).cloc.totalen+1)
            c_error(4,129);
          test_buffer(&loc,&iloc_max,offstruct+len);
          loc[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          strcpy((char*)&loc[offstruct+len+1],(char*)&mem[pieza_num]);
          len+=1+((*ob).cloc.totalen+5)/4;
          itxt=_itxt; // Saca la cadena del segmento de textos
          lexico();
        } else {
          test_buffer(&loc,&iloc_max,offstruct+len);
          loc[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          len+=1+((*ob).cloc.totalen+5)/4;
        }
      }

      if (pieza==p_coma) pieza=p_string; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_byte) {                         // Miembro byte ───

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a byte

        lexico(); puntero2:
        ob=analiza_pointer(tpblo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); loc[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero2;
        }

      } else {

        if (pieza!=p_id) c_error(1,142);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=tbloc; (*ob).bloc.offset=len;
        if (pieza==p_corab) { lexico();
          if (((*ob).bloc.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).bloc.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).bloc.len3=constante())<0) c_error(4,35);
            } else (*ob).bloc.len3=-1;
          } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }
          (*ob).bloc.totalen=(*ob).bloc.len1+1;
          if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
          if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();

          test_buffer(&loc,&iloc_max,offstruct+len+((*ob).bloc.totalen+3)/4);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).bloc.totalen+3)/4);
          memset(&frm[offstruct+len],2,(*ob).bloc.totalen);

          if (pieza==p_asig) {
            _iloc=iloc; iloc=offstruct+len;
            save_error(1); lexico();
            oimemptr=(byte*)&loc[iloc];
            tloc_init(2);
            if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
            iloc=_iloc;
          } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
        } else {
          (*ob).tipo=tbloc; (*ob).bloc.offset=len;
          (*ob).bloc.len1=0;
          (*ob).bloc.len2=-1;
          (*ob).bloc.len3=-1;
          (*ob).bloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=2;

          if (pieza==p_asig) {
            save_error(1);
            lexico();
            test_buffer(&loc,&iloc_max,offstruct+len);
            loc[offstruct+len]=constante();
            if (loc[offstruct+len]<0 || loc[offstruct+len]>255) c_error(4,143);
          }
        } len+=(*ob).bloc.totalen;
      }

      if (pieza==p_coma) pieza=p_byte; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_word) {                         // Miembro word ───

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a word

        lexico(); puntero3:
        ob=analiza_pointer(tpwlo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); loc[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero3;
        }

      } else {

        if (pieza!=p_id) c_error(1,145);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=twloc; (*ob).wloc.offset=len;
        if (pieza==p_corab) { lexico();
          if (((*ob).wloc.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).wloc.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).wloc.len3=constante())<0) c_error(4,35);
            } else (*ob).wloc.len3=-1;
          } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }
          (*ob).wloc.totalen=(*ob).wloc.len1+1;
          if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
          if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();

          test_buffer(&loc,&iloc_max,offstruct+len+((*ob).wloc.totalen+1)/2);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).wloc.totalen+1)/2);
          memset(&frm[offstruct+len],1,(*ob).wloc.totalen*2);

          if (pieza==p_asig) {
            _iloc=iloc; iloc=offstruct+len;
            save_error(1); lexico();
            oimemptr=(byte*)&loc[iloc];
            tloc_init(1);
            if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
            iloc=_iloc;
          } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
        } else {
          (*ob).tipo=twloc; (*ob).wloc.offset=len;
          (*ob).wloc.len1=0;
          (*ob).wloc.len2=-1;
          (*ob).wloc.len3=-1;
          (*ob).wloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=1;

          if (pieza==p_asig) {
            save_error(1);
            lexico();
            test_buffer(&loc,&iloc_max,offstruct+len);
            loc[offstruct+len]=constante();
            if (loc[offstruct+len]<0 || loc[offstruct+len]>65535) c_error(4,144);
          }
        } len+=(*ob).wloc.totalen;
      }

      if (pieza==p_coma) pieza=p_word; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_int || pieza==p_id || pieza==p_pointer) {

      if (pieza==p_int) {
        lexico();
        if (pieza!=p_id && pieza!=p_pointer) c_error(1,149);
      }

      if (pieza==p_pointer) { // Miembro puntero a int

        lexico(); puntero4:
        ob=analiza_pointer(tpilo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); loc[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero4;
        }

      } else {

        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        if (pieza==p_corab) { lexico();    // Miembro tabla ───
          (*ob).tipo=ttloc; (*ob).tloc.offset=len;
          if (((*ob).tloc.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).tloc.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).tloc.len3=constante())<0) c_error(4,35);
            } else (*ob).tloc.len3=-1;
          } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
          (*ob).tloc.totalen=(*ob).tloc.len1+1;
          if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
          if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();

          test_buffer(&loc,&iloc_max,offstruct+len+(*ob).tloc.totalen);
          if (pieza==p_asig) {
            _iloc=iloc; iloc=offstruct+len;
            save_error(1); lexico(); tloc_init(0);
            if (iloc-(offstruct+len)>(*ob).tloc.totalen) c_error(4,33);
            iloc=_iloc;
          } len+=(*ob).tloc.totalen;

        } else {                                          // Miembro variable ───

          (*ob).tipo=tvloc; (*ob).vloc.offset=len;
          test_buffer(&loc,&iloc_max,offstruct+len);
          if (pieza==p_asig) {
            lexico(); loc[offstruct+len]=constante();
          } len+=1;

        }
      }
      if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
      while (pieza==p_ptocoma || pieza==p_coma) lexico();
    } else c_error(0,58);
    (*ob).bloque=bloque_lexico;
  } return(len);
}

//───────────────────────────────────────────────────────────────────────────

int analiza_struct_private(int offstruct) { // tras " struct id [ <const> ] " // id­member
  int len=0,dup,i,_itxt,_imem;
  struct objeto * ob;
  struct objeto * old_member,* member2;
  byte * oimemptr;

  while (pieza==p_ptocoma) lexico();

  while (pieza!=p_end) {

    if (pieza==p_struct) {                              // Miembro struct ───

      lexico();

      if (pieza==p_pointer) { // Se define un puntero a struct

        old_member=member; member=NULL; lexico(); member=old_member;
        if (pieza!=p_id) c_error(1,110); ob=o;

        if ((*ob).tipo==tnone) c_error(0,150); // No se define el pointer así
        if ((*ob).tipo!=tsglo && (*ob).tipo!=tsloc) c_error(0,150);
        lexico();
        puntero_a_struct:
        analiza_pointer_struct(tpslo,len,ob);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;

        if (pieza==p_coma) {
          lexico();
          if (pieza==p_pointer) lexico();
          goto puntero_a_struct;
        } else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else {

        if (pieza!=p_id) c_error(1,110);
        ob=o;

        old_member=member; member=ob; lexico();
        if ((*ob).tipo!=tnone) c_error(2,22);

        (*ob).tipo=tsloc; (*ob).sloc.offset=len;
        if (pieza==p_corab) {
          member2=member; member=NULL; lexico();
          if (((*ob).sloc.items1=constante())<0) c_error(4,123);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).sloc.items2=constante())<0) c_error(4,123);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).sloc.items3=constante())<0) c_error(4,123);
            } else (*ob).sloc.items3=-1;
          } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
          member=member2;
          (*ob).sloc.totalitems=(*ob).sloc.items1+1;
          if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
          if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
        } else {
          (*ob).sloc.totalitems=1;
          (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
        }
        if (((*ob).sloc.len_item=analiza_struct_private(offstruct+len))==0) c_error(0,57);
        i=offstruct+len; dup=(*ob).sloc.totalitems+1;
        if (dup>1) {
          test_buffer(&mem,&imem_max,i+(*ob).sloc.len_item*(*ob).sloc.totalitems);
          test_buffer(&frm,&ifrm_max,i+(*ob).sloc.len_item*(*ob).sloc.totalitems);
          while (--dup) {
            memcpy(&mem[i],&mem[offstruct+len],(*ob).sloc.len_item<<2);
            memcpy(&frm[i],&frm[offstruct+len],(*ob).sloc.len_item<<2);
            i+=(*ob).sloc.len_item;
          }
        }
        len+=(*ob).sloc.len_item*(*ob).sloc.totalitems;
        member=old_member; lexico();
        while (pieza==p_ptocoma) lexico();
      }

    } else if (pieza==p_string) {                     // Miembro cadena

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a byte

        lexico(); puntero1:
        ob=analiza_pointer(tpclo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero1;
        }

      } else {

        if (pieza!=p_id) c_error(1,125);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=tcloc; (*ob).cloc.offset=len+1;
        if (pieza==p_corab) {
          lexico();
          if (pieza==p_corce) {
            lexico();
            (*ob).cloc.totalen=255;
          } else {
            if (((*ob).cloc.totalen=constante())<0) c_error(4,127);
            if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
            if (pieza!=p_corce) c_error(3,26);
            lexico();
          }
        } else (*ob).cloc.totalen=255;

        test_buffer(&frm,&ifrm_max,offstruct+len);
        frm[offstruct+len]=0xDAD00000;

        if (pieza==p_asig) {
          save_error(1);
          _itxt=itxt;
          lexico();
          if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
            c_error(3,128);
          if (strlen((char*)&mem[pieza_num])>(*ob).cloc.totalen+1)
            c_error(4,129);
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          strcpy((char*)&mem[offstruct+len+1],(char*)&mem[pieza_num]);
          len+=1+((*ob).cloc.totalen+5)/4;
          itxt=_itxt; // Saca la cadena del segmento de textos
          lexico();
        } else {
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          len+=1+((*ob).cloc.totalen+5)/4;
        }
      }

      if (pieza==p_coma) pieza=p_string; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_byte) {                         // Miembro byte ───

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a byte

        lexico(); puntero2:
        ob=analiza_pointer(tpblo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero2;
        }

      } else {

        if (pieza!=p_id) c_error(1,142);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=tbloc; (*ob).bloc.offset=len;
        if (pieza==p_corab) { lexico();
          if (((*ob).bloc.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).bloc.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).bloc.len3=constante())<0) c_error(4,35);
            } else (*ob).bloc.len3=-1;
          } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }
          (*ob).bloc.totalen=(*ob).bloc.len1+1;
          if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
          if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).bloc.totalen+3)/4);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).bloc.totalen+3)/4);
          memset(&frm[offstruct+len],2,(*ob).bloc.totalen);

          if (pieza==p_asig) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexico();
            oimemptr=(byte*)&mem[imem];
            tglo_init(2);
            if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
            imem=_imem;
          } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
        } else {
          (*ob).tipo=tbloc; (*ob).bloc.offset=len;
          (*ob).bloc.len1=0;
          (*ob).bloc.len2=-1;
          (*ob).bloc.len3=-1;
          (*ob).bloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=2;

          if (pieza==p_asig) {
            save_error(1);
            lexico();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constante();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>255) c_error(4,143);
          }
        } len+=(*ob).bloc.totalen;
      }

      if (pieza==p_coma) pieza=p_byte; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_word) {                         // Miembro word ───

      lexico();

      if (pieza==p_pointer) { // Miembro puntero a word

        lexico(); puntero3:
        ob=analiza_pointer(tpwlo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero3;
        }

      } else {

        if (pieza!=p_id) c_error(1,145);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        (*ob).tipo=twloc; (*ob).wloc.offset=len;
        if (pieza==p_corab) { lexico();
          if (((*ob).wloc.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).wloc.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).wloc.len3=constante())<0) c_error(4,35);
            } else (*ob).wloc.len3=-1;
          } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }
          (*ob).wloc.totalen=(*ob).wloc.len1+1;
          if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
          if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).wloc.totalen+1)/2);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).wloc.totalen+1)/2);
          memset(&frm[offstruct+len],1,(*ob).wloc.totalen*2);

          if (pieza==p_asig) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexico();
            oimemptr=(byte*)&mem[imem];
            tglo_init(1);
            if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
            imem=_imem;
          } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
        } else {
          (*ob).tipo=twloc; (*ob).wloc.offset=len;
          (*ob).wloc.len1=0;
          (*ob).wloc.len2=-1;
          (*ob).wloc.len3=-1;
          (*ob).wloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=1;

          if (pieza==p_asig) {
            save_error(1);
            lexico();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constante();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>65535) c_error(4,144);
          }
        } len+=(*ob).wloc.totalen;
      }

      if (pieza==p_coma) pieza=p_word; else {
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }

    } else if (pieza==p_int || pieza==p_id || pieza==p_pointer) {

      if (pieza==p_int) {
        lexico();
        if (pieza!=p_id && pieza!=p_pointer) c_error(1,149);
      }

      if (pieza==p_pointer) { // Miembro puntero a int

        lexico(); puntero4:
        ob=analiza_pointer(tpilo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (pieza==p_asig) {
          lexico(); mem[offstruct+len]=constante();
        } len+=1;
        if (pieza==p_coma) {
          lexico(); if (pieza==p_pointer) lexico();
          goto puntero4;
        }

      } else {

        ob=o; if ((*ob).tipo!=tnone) c_error(0,114); lexico();
        if (pieza==p_corab) { lexico();    // Miembro tabla ───
          (*ob).tipo=ttloc; (*ob).tloc.offset=len;
          if (((*ob).tloc.len1=constante())<0) c_error(4,35);
          if (pieza==p_coma) {
            lexico();
            if (((*ob).tloc.len2=constante())<0) c_error(4,35);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).tloc.len3=constante())<0) c_error(4,35);
            } else (*ob).tloc.len3=-1;
          } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
          (*ob).tloc.totalen=(*ob).tloc.len1+1;
          if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
          if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
          if (pieza!=p_corce) c_error(3,26); lexico();

          test_buffer(&mem,&imem_max,offstruct+len+(*ob).tloc.totalen);
          if (pieza==p_asig) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexico(); tglo_init(0);
            if (imem-(offstruct+len)>(*ob).tloc.totalen) c_error(4,33);
            imem=_imem;
          } len+=(*ob).tloc.totalen;

        } else {                                          // Miembro variable ───

          (*ob).tipo=tvloc; (*ob).vloc.offset=len;

          test_buffer(&mem,&imem_max,offstruct+len);
          if (pieza==p_asig) {
            lexico(); mem[offstruct+len]=constante();
          } len+=1;

        }
      }

      if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
      while (pieza==p_ptocoma || pieza==p_coma) lexico();

    } else c_error(0,58);
    (*ob).bloque=bloque_lexico;
  } return(len);
}

//───────────────────────────────────────────────────────────────────────────
#ifdef DIVDLL
typedef struct _DLL{
        char    *Name;
        int     nParms;
        int     Order;
}DLL;

DLL *ImpDLL;
int PrevOrder;
int nFuns;

PE    *divcpe;

void CNT_export(char *name,void *dir,int nparms)
{
  name=name;
  dir=dir;
  nparms=nparms;
  nFuns++;
}
void CMP_export(char *name,void *dir,int nparms)
{
  dir=dir;
  ImpDLL[nFuns].Name  =name;
  ImpDLL[nFuns].nParms=nparms;
  ImpDLL[nFuns].Order =PrevOrder++;
  nFuns++;
}

int ImportDll(char *name)
{
debugprintf("Looking for funcs in %s\n",name);
  nFuns=0;
#ifdef DIVDLL
  COM_export=CNT_export;
  divcpe=DIV_ImportDll(name);
  if (divcpe==NULL) return 0;
  DIV_UnImportDll(divcpe);
  //
  if (nFuns==0) return -1;
  ImpDLL=(DLL*)malloc(sizeof(DLL)*nFuns);
  if (ImpDLL==NULL) return 0;
  nFuns=0;
  COM_export=CMP_export;
  divcpe=DIV_ImportDll(name);
  if (divcpe==NULL) { free(ImpDLL); return 0; }
return nFuns;
#endif
}

void UnimportDll()
{
#ifdef DIVDLL
  DIV_UnImportDll(divcpe);
#endif
}

struct _dlls {
  int filename;         // Puntero al nombre asciiz
  int linea1,columna1;  // Posicion en la que comienza la sentencia en el fuente
  int linea2,columna2;  // Posicion en la que acaba la sentencia en el fuente
} dlls[64];

int idlls;              // Indice de la estructura anterior
#endif

//───────────────────────────────────────────────────────────────────────────

void sintactico (void) {

  struct objeto * ob, * member2;
  int _imem,_imem_old,num_par,n;
  byte * old_source,*nombre_dll,*oimemptr, *nombre_include;
  int _itxt,dup;
  char cWork[256];

  int num_extern;

  lexico();

  //───────────────────────────────────────────────────────────────────────────
  // Opciones de compilacion
  //───────────────────────────────────────────────────────────────────────────
  // p_compiler_options {opcion} {;}
  //───────────────────────────────────────────────────────────────────────────

  max_process=0; // Valores de las opciones por defecto
  ignore_errors=0;
  free_sintax=0;
  extended_conditions=0;
  simple_conditions=0;
  comprueba_rango=1;
  comprueba_id=1;
  comprueba_null=1;
  hacer_strfix=1;
  optimizar=1;

  if (pieza==p_compiler_options) {
    lexico();
    do {
      if (pieza==p_ptocoma) { lexico(); break; }
      if (pieza==p_coma) lexico();
      if (pieza==p_id && (*o).tipo==tcons) {
        switch((*o).cons.valor) {
          case 0: // _max_process
            lexico();
            if (pieza!=p_asig) c_error(3,139);
            lexico();
            max_process=constante();
            if (max_process<0) max_process=0;
            break;
          case 1: // _extended_conditions
            lexico();
            extended_conditions=1;
            break;
          case 2: // _simple_conditions
            lexico();
            simple_conditions=1;
            break;
          case 3: // _case_sensitive
            lexico();
            memcpy(lower+129,"üéâäàåçêëèïîìäåéææôöòûùÿöüø£Ø×ƒáíóú",35);
            memcpy(lower+'A',"ABCDEFGHIJKLMNOPQRSTUVWXYZ",26);
            lower['\xa5' /*'Ñ'*/]='\xa4' /*'ñ'*/;
            break;
          case 4: // _ignore_errors
            lexico();
            ignore_errors=1;
            break;
          case 5: // _free_sintax
            lexico();
            free_sintax=1;
            break;
          case 6: // _no_check
            lexico();
            comprueba_rango=0;
            comprueba_id=0;
            comprueba_null=0;
            break;
          case 7: // _no_strfix
            lexico();
            hacer_strfix=0;
            break;
          case 8: // _no_optimization
            lexico();
            optimizar=0;
            break;
          case 9: // _no_range_check
            lexico();
            comprueba_rango=0;
            break;
          case 10: // _no_id_check
            lexico();
            comprueba_id=0;
            break;
          case 11: // _no_null_check
            lexico();
            comprueba_null=0;
            break;
          default:
            c_error(0,138);
            break;
        }
      } else {
        if (!free_sintax) {
          if (pieza==p_program) c_error(3,66);
          else c_error(0,138);
        } else break;
      }
    } while (1);
  }

  //───────────────────────────────────────────────────────────────────────────
  // Cabecera
  //───────────────────────────────────────────────────────────────────────────
  // p_program p_id {;}
  //───────────────────────────────────────────────────────────────────────────

  save_error(0);
  if (pieza!=p_program && pieza!=p_setup_program) c_error(4,20);

  if (pieza==p_setup_program) {
    program_type=1; 
    strcpy((char *)cWork,"install/setup.ins");
  } else strcpy((char *)cWork,"system/exec.ins");
  if ((lins=fopen((char *)cWork,"wb"))==NULL) c_error(0,0);

  inicio_sentencia(); lexico();
  if (pieza!=p_id) c_error(1,21);
  ob=o; if ((*ob).tipo!=tnone) c_error(0,115); (*ob).tipo=tproc;
  (*ob).proc.bloque=bloque_actual=ob;
  (*ob).proc.offset=0; (*ob).proc.num_par=0;
  lexico();
  if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
  while (pieza==p_ptocoma || pieza==p_coma) lexico();
  final_sentencia();

#ifdef DIVDLL
  dlls[0].linea1=linea1;
  dlls[0].columna1=columna1;
  dlls[0].linea2=linea2;
  dlls[0].columna2=columna2;
  idlls=1;

  //───────────────────────────────────────────────────────────────────────────
  // Import de librerias dinámicas
  //───────────────────────────────────────────────────────────────────────────
  // {p_import p_lit {;}}
  //───────────────────────────────────────────────────────────────────────────

  PrevOrder=0;

  while (pieza==p_import) {
    inicio_sentencia();
    lexico();
    if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
		c_error(1,62);
	lexico();
    if (!free_sintax) 
		if (pieza!=p_ptocoma) 
			c_error(3,66);

    old_source=source;
    nombre_dll=(byte*)&mem[pieza_num];

    if (idlls<64) 
		dlls[idlls].filename=pieza_num;

//printf("DLL: %lx %d %s\n",&mem[pieza_num],pieza_num,nombre_dll);

    if (nombre_dll==NULL) c_error(0,63);
    if ((num_extern=ImportDll((char *)nombre_dll))==0) c_error(0,63);
    if (num_extern>0) {
		printf("num funcs: %d\n",num_extern);
      save_error(0);
      for (n=0;n<num_extern;n++) {
        source=(byte *)ImpDLL[n].Name;
        lexico();
        if (pieza!=p_id) c_error(4,63);
        if ((*o).tipo!=tnone) c_error(4,63);
        (*o).tipo=tfext;
        (*o).fext.codigo=ImpDLL[n].Order;
        (*o).fext.num_par=ImpDLL[n].nParms;
      }
      free(ImpDLL);
      UnimportDll();
    }
    while (pieza==p_ptocoma || pieza==p_coma) lexico();
    final_sentencia();

    if (idlls<64) {
      dlls[idlls].linea1=linea1;
      dlls[idlls].columna1=columna1;
      dlls[idlls].linea2=linea2;
      dlls[idlls].columna2=columna2;
      idlls++;
    }
  }

#endif

	while(pieza==p_include) {
		inicio_sentencia();
		lexico();
		if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal)) c_error(1,62);
		old_source=source;
		nombre_include=(byte*)&mem[pieza_num];


	//
		
		source=old_source; lexico();
		if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
		while (pieza==p_ptocoma || pieza==p_coma) lexico();
		final_sentencia();
	}
  //───────────────────────────────────────────────────────────────────────────
  // Zona de constantes
  //───────────────────────────────────────────────────────────────────────────
  // [ p_const {;} { p_id p_asig <exp> {; | ,} } ]
  //───────────────────────────────────────────────────────────────────────────

  if (pieza==p_const) {
    pasa_ptocoma();
    while (pieza==p_id) {
      ob=o; if ((*ob).tipo!=tnone && (*ob).tipo!=tcons) c_error(0,116);
      (*ob).tipo=tcons;
      lexico(); if (pieza!=p_asig) c_error(3,119);
      lexico();
      if (pieza==p_lit || (pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
        (*ob).cons.literal=1; else (*ob).cons.literal=0;
      (*ob).cons.valor=constante();
      if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
      while (pieza==p_ptocoma || pieza==p_coma) lexico();
    }
  }

  //───────────────────────────────────────────────────────────────────────────
  // Variables globales, pueden usarse en expresiones constantes
  //───────────────────────────────────────────────────────────────────────────
  // <variable> := p_id [ p_asig <exp> ] {;}
  //
  // <tabla> := p_id p_corab ( p_corce p_asig <init>
  //                       | <exp> p_corce [ p_asig <init> ] ) {;}
  //
  // <init>  := ( <string> | <exp> [ [p_dup] p_abrir <init> p_cerrar ] )
  //            [ p_coma <init>]
  //───────────────────────────────────────────────────────────────────────────

  if (pieza==p_global) {
    pasa_ptocoma();
    while (pieza==p_id || pieza==p_int || pieza==p_pointer || pieza==p_struct || pieza==p_string || pieza==p_byte || pieza==p_word) {

      if (pieza==p_struct) {                            // Struct global

        lexico();

        if (pieza==p_pointer) { // Se define un puntero a struct

          lexico(); if (pieza!=p_id) c_error(1,110); ob=o;
          if ((*ob).tipo==tnone) c_error(0,150); // No se define el pointer así
          if ((*ob).tipo!=tsglo && (*ob).tipo!=tsloc) c_error(0,150);
          lexico();
          puntero_a_struct:
          analiza_pointer_struct(tpsgl,imem,ob);
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,imem++);

          if (pieza==p_coma) {
            lexico();
            if (pieza==p_pointer) lexico();
            goto puntero_a_struct;
          } else {
            if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
            while (pieza==p_ptocoma || pieza==p_coma) lexico();
          }

        } else {

          if (pieza!=p_id) c_error(1,110);
          ob=o; member=ob; lexico();

          if ((*ob).tipo!=tnone) c_error(2,22);

          (*ob).tipo=tsglo; (*ob).sglo.offset=_imem=imem;
          if (pieza==p_corab) {
            member2=member; member=NULL; lexico();
            if (((*ob).sglo.items1=constante())<0) c_error(4,123);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).sglo.items2=constante())<0) c_error(4,123);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).sglo.items3=constante())<0) c_error(4,123);
              } else (*ob).sglo.items3=-1;
            } else { (*ob).sglo.items2=-1; (*ob).sglo.items3=-1; }
            member=member2;
            (*ob).sglo.totalitems=(*ob).sglo.items1+1;
            if ((*ob).sglo.items2>-1) (*ob).sglo.totalitems*=(*ob).sglo.items2+1;
            if ((*ob).sglo.items3>-1) (*ob).sglo.totalitems*=(*ob).sglo.items3+1;
            if (pieza!=p_corce) c_error(3,26); lexico();
          } else {
            (*ob).sglo.totalitems=1;
            (*ob).sglo.items1=0; (*ob).sglo.items2=-1; (*ob).sglo.items3=-1;
          }
          if (((*ob).sglo.len_item=analiza_struct(_imem))==0) c_error(0,57);
          member=NULL; lexico();
          imem=(*ob).sglo.offset; dup=(*ob).sglo.totalitems+1;
          if (dup>1) {
            test_buffer(&mem,&imem_max,imem+(*ob).sglo.len_item*(*ob).sglo.totalitems);
            test_buffer(&frm,&ifrm_max,imem+(*ob).sglo.len_item*(*ob).sglo.totalitems);
            while (--dup) {
            	if(imem!=_imem) {
	              memcpy(&mem[imem],&mem[_imem],(*ob).sglo.len_item<<2);
	              memcpy(&frm[imem],&frm[_imem],(*ob).sglo.len_item<<2);
	            }
              imem+=(*ob).sglo.len_item;
            }
          } imem=_imem;
          if (pieza==p_asig) {
            save_error(1);
            lexico(); tglo_init(0);
            if (imem-_imem-1>=(*ob).sglo.len_item*(*ob).sglo.totalitems) c_error(4,120);
          } while (pieza==p_ptocoma) lexico();

          imem=_imem+(*ob).sglo.len_item*(*ob).sglo.totalitems;
          test_buffer(&mem,&imem_max,imem);
        }

      } else if (pieza==p_string) {                     // Cadena global

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero1:
          ob=analiza_pointer(tpcgl,imem);
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero1;
          }

        } else {

          if (pieza!=p_id) c_error(1,125);
          ob=o; if ((*ob).tipo!=tnone) c_error(0,126); lexico();
          (*ob).tipo=tcglo;
          _imem=imem;
          (*ob).cglo.offset=_imem+1;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico();
              (*ob).cglo.totalen=255;
            } else {
              if (((*ob).cglo.totalen=constante())<0) c_error(4,127);
              if ((*ob).cglo.totalen>0xFFFFF) c_error(4,135);
              if (pieza!=p_corce) c_error(3,26);
              lexico();
            }
          } else (*ob).cglo.totalen=255;
          if (pieza==p_asig) {
            save_error(1);
            _itxt=itxt;
            lexico();
            if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
              c_error(3,128);
            if (strlen((char*)&mem[pieza_num])>(*ob).cglo.totalen+1)
              c_error(4,129);
            imem=_imem+1+((*ob).cglo.totalen+5)/4; // ej. c[32] -> c[0]..c[32],NUL
            test_buffer(&mem,&imem_max,imem);
            strcpy((char*)&mem[_imem+1],(char*)&mem[pieza_num]);
            itxt=_itxt; // Saca la cadena del segmento de textos
            lexico();
          } else {
            imem=_imem+1+((*ob).cglo.totalen+5)/4;
            test_buffer(&mem,&imem_max,imem);
          } mem[_imem]=0xDAD00000|(*ob).cglo.totalen;
        }

        if (pieza==p_coma) pieza=p_string; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_byte) {                       // Tabla Byte global

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero2:
          ob=analiza_pointer(tpbgl,imem);
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero2;
          }

        } else {

          if (pieza!=p_id) c_error(1,142);
          ob=o; if ((*ob).tipo!=tnone) c_error(0,117); lexico();
          (*ob).tipo=tbglo; (*ob).bglo.offset=_imem=imem;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              oimemptr=(byte*)&mem[imem];
              tglo_init(2);
              (*ob).bglo.len1=imemptr-oimemptr-1;
              (*ob).bglo.len2=-1;
              (*ob).bglo.len3=-1;
              (*ob).bglo.totalen=((*ob).bglo.len1+4)/4;
            } else {
              if (((*ob).bglo.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).bglo.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).bglo.len3=constante())<0) c_error(4,35);
                } else (*ob).bglo.len3=-1;
              } else { (*ob).bglo.len2=-1; (*ob).bglo.len3=-1; }

              (*ob).bglo.totalen=(*ob).bglo.len1+1;
              if ((*ob).bglo.len2>-1) (*ob).bglo.totalen*=(*ob).bglo.len2+1;
              if ((*ob).bglo.len3>-1) (*ob).bglo.totalen*=(*ob).bglo.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico();
                oimemptr=(byte*)&mem[imem];
                tglo_init(2);
                if (imemptr-oimemptr>(*ob).bglo.totalen) c_error(4,33);
              } (*ob).bglo.totalen=((*ob).bglo.totalen+3)/4;
            }
          } else {                                          // Byte global
            (*ob).tipo=tbglo; (*ob).bglo.offset=imem;
            (*ob).bglo.len1=0;
            (*ob).bglo.len2=-1;
            (*ob).bglo.len3=-1;
            (*ob).bglo.totalen=1; // 1 int
            if (pieza==p_asig) {
              save_error(1);
              lexico();
              mem[imem]=constante();
              if (mem[imem]<0 || mem[imem]>255) c_error(4,143);
            }
          } imem=_imem+(*ob).bglo.totalen; test_buffer(&mem,&imem_max,imem);
        }

        if (pieza==p_coma) pieza=p_byte; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_word) {                       // Tabla Word global

        lexico();

        if (pieza==p_pointer) { // Puntero a word

          lexico(); puntero3:
          ob=analiza_pointer(tpwgl,imem);
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero3;
          }

        } else {

          if (pieza!=p_id) c_error(1,145);
          ob=o; if ((*ob).tipo!=tnone) c_error(0,117); lexico();
          (*ob).tipo=twglo; (*ob).wglo.offset=_imem=imem;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              oimemptr=(byte*)&mem[imem];
              tglo_init(1);
              (*ob).wglo.len1=(imemptr-oimemptr)/2-1;
              (*ob).wglo.len2=-1;
              (*ob).wglo.len3=-1;
              (*ob).wglo.totalen=((*ob).wglo.len1+2)/2;
            } else {
              if (((*ob).wglo.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).wglo.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).wglo.len3=constante())<0) c_error(4,35);
                } else (*ob).wglo.len3=-1;
              } else { (*ob).wglo.len2=-1; (*ob).wglo.len3=-1; }

              (*ob).wglo.totalen=(*ob).wglo.len1+1;
              if ((*ob).wglo.len2>-1) (*ob).wglo.totalen*=(*ob).wglo.len2+1;
              if ((*ob).wglo.len3>-1) (*ob).wglo.totalen*=(*ob).wglo.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico();
                oimemptr=(byte*)&mem[imem];
                tglo_init(1);
                if (imemptr-oimemptr>(*ob).wglo.totalen*2) c_error(4,33);
              } (*ob).wglo.totalen=((*ob).wglo.totalen+1)/2;
            }
          } else {                                          // Word global
            (*ob).tipo=twglo; (*ob).wglo.offset=imem;
            (*ob).wglo.len1=0;
            (*ob).wglo.len2=-1;
            (*ob).wglo.len3=-1;
            (*ob).wglo.totalen=1; // 1 int
            if (pieza==p_asig) {
              save_error(1);
              lexico();
              mem[imem]=constante();
              if (mem[imem]<0 || mem[imem]>65535) c_error(4,144);
            }
          } imem=_imem+(*ob).wglo.totalen; test_buffer(&mem,&imem_max,imem);
        }

        if (pieza==p_coma) pieza=p_word; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_int || pieza==p_id || pieza==p_pointer) {

        if (pieza==p_int) {
          lexico();
          if (pieza!=p_id && pieza!=p_pointer) c_error(1,149);
        }

        if (pieza==p_pointer) { // Puntero a int

          lexico(); puntero4:
          ob=analiza_pointer(tpigl,imem);
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero4;
          }

        } else {

          ob=o; if ((*ob).tipo!=tnone) c_error(0,117); lexico();
          if (pieza==p_corab) {                           // Tabla global
            lexico();
            (*ob).tipo=ttglo; (*ob).tglo.offset=_imem=imem;
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              tglo_init(3);
              (*ob).tglo.len1=imem-_imem-1;
              (*ob).tglo.len2=-1;
              (*ob).tglo.len3=-1;
              (*ob).tglo.totalen=imem-_imem;
            } else {
              if (((*ob).tglo.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).tglo.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).tglo.len3=constante())<0) c_error(4,35);
                } else (*ob).tglo.len3=-1;
              } else { (*ob).tglo.len2=-1; (*ob).tglo.len3=-1; }
              (*ob).tglo.totalen=(*ob).tglo.len1+1;
              if ((*ob).tglo.len2>-1) (*ob).tglo.totalen*=(*ob).tglo.len2+1;
              if ((*ob).tglo.len3>-1) (*ob).tglo.totalen*=(*ob).tglo.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico(); tglo_init(3);
                if (imem-_imem>(*ob).tglo.totalen) c_error(4,33); }
            }
            imem=_imem+(*ob).tglo.totalen;
            test_buffer(&mem,&imem_max,imem);

          } else {                                          // Variable global

            (*ob).tipo=tvglo; (*ob).vglo.offset=imem;
            if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
            test_buffer(&mem,&imem_max,++imem);

          }
        }
        if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }
    }
  }

  //───────────────────────────────────────────────────────────────────────────
  // Variables locales, no se pueden usar en expresiones constantes
  //───────────────────────────────────────────────────────────────────────────
  // <variable> := p_id [ p_asig <exp> ] {;}
  //
  // <tabla> := p_id p_corab ( p_corce p_asig <init>
  //                       | <exp> p_corce [ p_asig <init> ] ) {;}
  //───────────────────────────────────────────────────────────────────────────

  if (pieza==p_local) {
    pasa_ptocoma();
    while (pieza==p_id || pieza==p_int || pieza==p_pointer || pieza==p_struct || pieza==p_string || pieza==p_byte || pieza==p_word) {

      if (pieza==p_struct) {

        lexico();

        if (pieza==p_pointer) { // Se define un puntero a struct

          lexico(); if (pieza!=p_id) c_error(1,110); ob=o;
          if ((*ob).tipo==tnone) c_error(0,150); // No se define el pointer así
          if ((*ob).tipo!=tsglo && (*ob).tipo!=tsloc) c_error(0,150);
          lexico();
          puntero_a_struct_local:
          analiza_pointer_struct(tpslo,iloc,ob);
          if (pieza==p_asig) { lexico(); loc[iloc]=constante(); }
          test_buffer(&loc,&iloc_max,iloc++);

          if (pieza==p_coma) {
            lexico();
            if (pieza==p_pointer) lexico();
            goto puntero_a_struct_local;
          } else {
            if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
            while (pieza==p_ptocoma || pieza==p_coma) lexico();
          }

        } else {

          if (pieza!=p_id) c_error(1,110);
          ob=o; member=ob; lexico();

          if ((*ob).tipo!=tnone) c_error(2,22);

          (*ob).tipo=tsloc; (*ob).sloc.offset=_imem=iloc;
          if (pieza==p_corab) {
            member2=member; member=NULL; lexico();
            if (((*ob).sloc.items1=constante())<0) c_error(4,123);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).sloc.items2=constante())<0) c_error(4,123);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).sloc.items3=constante())<0) c_error(4,123);
              } else (*ob).sloc.items3=-1;
            } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
            member=member2;
            (*ob).sloc.totalitems=(*ob).sloc.items1+1;
            if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
            if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
            if (pieza!=p_corce) c_error(3,26); lexico();
          } else {
            (*ob).sloc.totalitems=1;
            (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
          }
          if (((*ob).sloc.len_item=analiza_struct_local(_imem))==0) c_error(0,57);
          member=NULL; lexico();
          iloc=(*ob).sloc.offset; dup=(*ob).sloc.totalitems+1;
          if (dup>1) {
            test_buffer(&loc,&iloc_max,iloc+(*ob).sloc.len_item*(*ob).sloc.totalitems);
            test_buffer(&frm,&ifrm_max,imem+(*ob).sloc.len_item*(*ob).sloc.totalitems);
            while (--dup) {
              memcpy(&loc[iloc],&loc[_imem],(*ob).sloc.len_item<<2);
              memcpy(&frm[iloc],&frm[_imem],(*ob).sloc.len_item<<2);
              iloc+=(*ob).sloc.len_item;
            }
          } iloc=_imem;
          if (pieza==p_asig) {
            save_error(1); lexico(); tloc_init(0);
            if (iloc-_imem-1>=(*ob).sloc.len_item*(*ob).sloc.totalitems) c_error(4,120);
          } while (pieza==p_ptocoma) lexico();
          iloc=_imem+(*ob).sloc.len_item*(*ob).sloc.totalitems;
          test_buffer(&loc,&iloc_max,iloc);
        }

      } else if (pieza==p_string) {                     // Cadena local

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero5:
          ob=analiza_pointer(tpclo,iloc);
          if (pieza==p_asig) { lexico(); loc[iloc]=constante(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero5;
          }

        } else {

          if (pieza!=p_id) c_error(1,125);
          ob=o; if ((*ob).tipo!=tnone) c_error(0,126); lexico();
          (*ob).tipo=tcloc;
          _imem=iloc;
          (*ob).cloc.offset=_imem+1;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico();
              (*ob).cloc.totalen=255;
            } else {
              if (((*ob).cloc.totalen=constante())<0) c_error(4,127);
              if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
              if (pieza!=p_corce) c_error(3,26);
              lexico();
            }
          } else (*ob).cloc.totalen=255;
          if (pieza==p_asig) {
            save_error(1);
            _itxt=itxt;
            lexico();
            if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
              c_error(3,128);
            if (strlen((char*)&mem[pieza_num])>(*ob).cloc.totalen+1)
              c_error(4,129);
            iloc=_imem+1+((*ob).cloc.totalen+5)/4; // ej. c[32] -> c[0]..c[32],NUL
            test_buffer(&loc,&iloc_max,iloc);
            strcpy((char*)&loc[_imem+1],(char*)&mem[pieza_num]);
            itxt=_itxt; // Saca la cadena del segmento de textos
            lexico();
          } else {
            iloc=_imem+1+((*ob).cloc.totalen+5)/4;
            test_buffer(&loc,&iloc_max,iloc);
          } loc[_imem]=0xDAD00000|(*ob).cloc.totalen;
        }

        if (pieza==p_coma) pieza=p_string; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_byte) {                       // Tabla Byte local

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero6:
          ob=analiza_pointer(tpblo,iloc);
          if (pieza==p_asig) { lexico(); loc[iloc]=constante(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero6;
          }

        } else {

          if (pieza!=p_id) c_error(1,142);
          ob=o; if ((*ob).tipo!=tnone) c_error(0,117); lexico();
          (*ob).tipo=tbloc; (*ob).bloc.offset=_imem=iloc;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              oimemptr=(byte*)&loc[iloc];
              tloc_init(2);
              (*ob).bloc.len1=imemptr-oimemptr-1;
              (*ob).bloc.len2=-1;
              (*ob).bloc.len3=-1;
              (*ob).bloc.totalen=((*ob).bloc.len1+4)/4;
            } else {
              if (((*ob).bloc.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).bloc.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).bloc.len3=constante())<0) c_error(4,35);
                } else (*ob).bloc.len3=-1;
              } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }

              (*ob).bloc.totalen=(*ob).bloc.len1+1;
              if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
              if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico();
                oimemptr=(byte*)&loc[iloc];
                tloc_init(2);
                if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
              } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
            }
          } else {                                          // Byte local
            (*ob).tipo=tbloc; (*ob).bloc.offset=iloc;
            (*ob).bloc.len1=0;
            (*ob).bloc.len2=-1;
            (*ob).bloc.len3=-1;
            (*ob).bloc.totalen=1; // 1 int
            if (pieza==p_asig) {
              save_error(1);
              lexico();
              loc[iloc]=constante();
              if (loc[iloc]<0 || loc[iloc]>255) c_error(4,143);
            }
          } iloc=_imem+(*ob).bloc.totalen; test_buffer(&loc,&iloc_max,iloc);
        }

        if (pieza==p_coma) pieza=p_byte; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_word) {                       // Tabla Word local

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero7:
          ob=analiza_pointer(tpwlo,iloc);
          if (pieza==p_asig) { lexico(); loc[iloc]=constante(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero7;
          }

        } else {

          if (pieza!=p_id) c_error(1,145);
          ob=o; if ((*ob).tipo!=tnone) c_error(0,117); lexico();
          (*ob).tipo=twloc; (*ob).wloc.offset=_imem=iloc;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              oimemptr=(byte*)&loc[iloc];
              tloc_init(1);
              (*ob).wloc.len1=(imemptr-oimemptr)/2-1;
              (*ob).wloc.len2=-1;
              (*ob).wloc.len3=-1;
              (*ob).wloc.totalen=((*ob).wloc.len1+2)/2;
            } else {
              if (((*ob).wloc.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).wloc.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).wloc.len3=constante())<0) c_error(4,35);
                } else (*ob).wloc.len3=-1;
              } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }

              (*ob).wloc.totalen=(*ob).wloc.len1+1;
              if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
              if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico();
                oimemptr=(byte*)&loc[iloc];
                tloc_init(1);
                if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
              } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
            }
          } else {                                          // Word local
            (*ob).tipo=twloc; (*ob).wloc.offset=iloc;
            (*ob).wloc.len1=0;
            (*ob).wloc.len2=-1;
            (*ob).wloc.len3=-1;
            (*ob).wloc.totalen=1; // 1 int
            if (pieza==p_asig) {
              save_error(1);
              lexico();
              loc[iloc]=constante();
              if (loc[iloc]<0 || loc[iloc]>65535) c_error(4,144);
            }
          } iloc=_imem+(*ob).wloc.totalen; test_buffer(&loc,&iloc_max,iloc);
        }

        if (pieza==p_coma) pieza=p_word; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_int || pieza==p_id || pieza==p_pointer) {

        if (pieza==p_int) {
          lexico();
          if (pieza!=p_id && pieza!=p_pointer) c_error(1,149);
        }

        if (pieza==p_pointer) { // Miembro puntero a int

          lexico(); puntero8:
          ob=analiza_pointer(tpilo,iloc);
          if (pieza==p_asig) { lexico(); loc[iloc]=constante(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero8;
          }

        } else {

          ob=o; if ((*ob).tipo!=tnone) c_error(0,117); lexico();
          if (pieza==p_corab) {                             // Tabla local
            lexico();
            (*ob).tipo=ttloc; (*ob).tloc.offset=_imem=iloc;
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              tloc_init(3);
              (*ob).tloc.len1=iloc-_imem-1;
              (*ob).tloc.len2=-1;
              (*ob).tloc.len3=-1;
              (*ob).tloc.totalen=iloc-_imem;
            } else {
              if (((*ob).tloc.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).tloc.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).tloc.len3=constante())<0) c_error(4,35);
                } else (*ob).tloc.len3=-1;
              } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
              (*ob).tloc.totalen=(*ob).tloc.len1+1;
              if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
              if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico(); tloc_init(3);
                if (iloc-_imem>(*ob).tloc.totalen) c_error(4,33); }
            }
            iloc=_imem+(*ob).tloc.totalen;
            test_buffer(&loc,&iloc_max,iloc);

          } else {                                          // Variable local

            (*ob).tipo=tvloc; (*ob).vloc.offset=iloc;
            if (pieza==p_asig) { lexico(); loc[iloc]=constante(); }
            test_buffer(&loc,&iloc_max,++iloc);

          }
        }
        if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      }
    }
  }

  //───────────────────────────────────────────────────────────────────────────
  // Código principal
  //───────────────────────────────────────────────────────────────────────────

  // Genera el salto al inicio del código (long_header)

  mem[0]=program_type; mem[1]=imem; iloc_len=iloc;

  g2(DIVBC_typ,(memptrsize)bloque_actual);
  g2(DIVBC_cbp,0);
#ifdef DIVDLL
  linea1=dlls[0].linea1;
  columna1=dlls[0].columna1;
  linea2=dlls[0].linea2;
  columna2=dlls[0].columna2;
  inicio=0; final=imem-1;
  grabar_sentencia();

  if (pieza==p_import) c_error(0,147); // Advertir a usuarios de DIV 1

  // Genera el código para cargar las librerías

  for (n=1;n<idlls;n++) {
    inicio=imem; final=imem+1;
    g2(limp,dlls[n].filename);
    linea1=dlls[n].linea1;
    columna1=dlls[n].columna1;
    linea2=dlls[n].linea2;
    columna2=dlls[n].columna2;
    grabar_sentencia();
  }

#endif

  // El primer FRAME, y la carga de variables PRIVATE, se ejecutan
  // conjuntamente en el BEGIN del programa principal.

  final=imem;

  parametros=-1;
  bloque_lexico=bloque_actual;
  n=iloc; analiza_private();
  if (iloc>iloc_len) iloc_len=iloc; iloc=n;
  parametros=0;

  g1(DIVBC_frm);

  if (pieza!=p_begin) c_error(0,29);
  inicio_sentencia(); inicio=final;
  lexico(); final_sentencia(); grabar_sentencia();

  sentencia();

  if (pieza!=p_end) c_error(0,40); inicio_sentencia();
  g1(DIVBC_ret);
  pasa_ptocoma(); final_sentencia(); grabar_sentencia();

  //───────────────────────────────────────────────────────────────────────────
  // Procesos
  //───────────────────────────────────────────────────────────────────────────
  // p_process p_id p_abrir [ id { , id } ] p_cerrar { ; } p_begin ... p_end
  //───────────────────────────────────────────────────────────────────────────

  save_error(0);
  while (pieza==p_process || pieza==p_function) {
    n=pieza; inicio_sentencia(); lexico();
    if (pieza!=p_id) c_error(1,111); ob=o; lexico();
    if ((*ob).tipo==tproc && (*ob).usado) {
      num_par=(*ob).proc.num_par; bloque_lexico=bloque_actual=ob;
      _imem=(*ob).proc.offset; while(_imem) {
         _imem_old=mem[_imem]; mem[_imem]=imem; _imem=_imem_old; }
    } else if ((*ob).tipo==tnone) {
      (*ob).usado=0; (*ob).tipo=tproc;
      (*ob).proc.bloque=bloque_lexico=bloque_actual=ob;
    } else c_error(2,118);
    (*ob).proc.offset=imem; (*ob).proc.num_par=0;

    g2(DIVBC_typ,(memptrsize)bloque_actual);
    if (n==p_function) g1(DIVBC_nop);
    g2(DIVBC_cbp,0); _imem=imem-1;
    if (pieza!=p_abrir) c_error(3,36);

    parametros=1; n=iloc; lexico();

    while (pieza!=p_cerrar) {
      (*ob).proc.num_par++; expresion_cpa();
      if (pieza!=p_cerrar) if (pieza!=p_coma) c_error(3,25);
        else { lexico(); if (pieza==p_cerrar) c_error(3,45); }
    } if ((*ob).usado)
      if (num_par==(*ob).proc.num_par) (*ob).usado=0; else c_error(0,46);

    pasa_ptocoma(); final_sentencia();

    if (parametros>1) {
      g2(DIVBC_par,parametros-1);
    }

    parametros=-1; // Para que los parámetros se puedan repetir como PRIVATE

    num_par=mem[_imem]=(*ob).proc.num_par;

    analiza_private(); parametros=0;

    if (iloc>iloc_len) iloc_len=iloc; iloc=n;

    if (pieza!=p_begin) c_error(0,29); lexico();
    final=imem-1; grabar_sentencia();
    sentencia();
    if (pieza!=p_end) c_error(0,40); inicio_sentencia();
    g1(DIVBC_ret);
    pasa_ptocoma(); final_sentencia(); grabar_sentencia();

    save_error(0);
  }

  if (pieza!=p_ultima) c_error(4,52);

}

  //───────────────────────────────────────────────────────────────────────────
  // Analiza las variables privadas de un bloque, como las locales
  //───────────────────────────────────────────────────────────────────────────

    // *** OJO *** No se debe permitir #id.tvpri
    //             pues fallaría a no ser que #id fuera del mismo tipo que el
    //             proceso actual (hermano)

void analiza_private(void) {
  struct objeto * ob, * member2;
  int _imem,_imem_old,_itxt,dup;
  byte *oimemptr;

  if (pieza==p_private) {
    pasa_ptocoma();

    g2(DIVBC_pri,0); _imem_old=imem-1;

    while (pieza==p_id || pieza==p_int || pieza==p_pointer || pieza==p_struct || pieza==p_string || pieza==p_byte || pieza==p_word) {

      if (pieza==p_struct) {

        lexico();

        if (pieza==p_pointer) { // Se define un puntero a struct

          lexico(); if (pieza!=p_id) c_error(1,110); ob=o;
          if ((*ob).tipo==tnone) c_error(0,150); // No se define el pointer así
          if ((*ob).tipo!=tsglo && (*ob).tipo!=tsloc) c_error(0,150);
          lexico();
          puntero_a_struct:
          if (analiza_pointer_struct(tpslo,iloc,ob)==1) {
            iloc++;
            if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
            test_buffer(&mem,&imem_max,++imem);
          }

          if (pieza==p_coma) {
            lexico();
            if (pieza==p_pointer) lexico();
            goto puntero_a_struct;
          } else {
            if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
            while (pieza==p_ptocoma || pieza==p_coma) lexico();
          }

        } else {

          if (pieza!=p_id) c_error(1,110);
          ob=o; member=ob; lexico();

          if ((*ob).tipo!=tnone) c_error(2,22);

          (*ob).tipo=tsloc; (*ob).sloc.offset=_imem=imem;
          if (pieza==p_corab) {
            member2=member; member=NULL; lexico();
            if (((*ob).sloc.items1=constante())<0) c_error(4,123);
            if (pieza==p_coma) {
              lexico();
              if (((*ob).sloc.items2=constante())<0) c_error(4,123);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).sloc.items3=constante())<0) c_error(4,123);
              } else (*ob).sloc.items3=-1;
            } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
            member=member2;
            (*ob).sloc.totalitems=(*ob).sloc.items1+1;
            if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
            if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
            if (pieza!=p_corce) c_error(3,26); lexico();
          } else {
            (*ob).sloc.totalitems=1;
            (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
          }
          if (((*ob).sloc.len_item=analiza_struct_private(_imem))==0) c_error(0,57);

          member=NULL; lexico();

          imem=(*ob).sloc.offset; dup=(*ob).sloc.totalitems+1;
          if (dup>1) {
            test_buffer(&mem,&imem_max,imem+(*ob).sloc.len_item*(*ob).sloc.totalitems);
            test_buffer(&frm,&ifrm_max,imem+(*ob).sloc.len_item*(*ob).sloc.totalitems);
            while (--dup) {
            	if(imem!=_imem) {
	              memcpy(&mem[imem],&mem[_imem],(*ob).sloc.len_item<<2);
	              memcpy(&frm[imem],&frm[_imem],(*ob).sloc.len_item<<2);
	          	}
              	imem+=(*ob).sloc.len_item;
            }
          } imem=_imem;

          if (pieza==p_asig) {
            save_error(1); lexico(); tglo_init(0);
            if (imem-_imem-1>=(*ob).sloc.len_item*(*ob).sloc.totalitems) c_error(4,120);
          } while (pieza==p_ptocoma) lexico();

          imem=_imem+(*ob).sloc.len_item*(*ob).sloc.totalitems;
          (*ob).sloc.offset=iloc; iloc+=(*ob).sloc.len_item*(*ob).sloc.totalitems;
          test_buffer(&mem,&imem_max,imem);
        }

      } else if (pieza==p_string) {                     // Cadena privada

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero1:
          ob=analiza_pointer(tpclo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero1;
          }

        } else {

          if (pieza!=p_id) c_error(1,125);
          ob=o; if ((*ob).tipo!=tnone) { // Mira si se repite un parámetro ...
            if (parametros==-1 && (*ob).param==1 && (*ob).bloque==bloque_actual) {
              if ((*ob).tipo==tcloc) { // Se repite un string
                save_error(0);
                lexico();
                if (pieza==p_corab) {
                  lexico();
                  if (pieza==p_corce) {
                    lexico();
                    dup=255;
                  } else {
                    dup=constante();
                    if (pieza!=p_corce) c_error(3,26);
                    lexico();
                  }
                } else dup=255;
                if (dup!=(*ob).cloc.totalen) c_error(4,141);
                else if (pieza==p_asig) c_error(0,54);
                else {
                  while (pieza==p_ptocoma || pieza==p_coma) lexico();
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,126);
            } else c_error(0,126);
          } else lexico();

          (*ob).tipo=tcloc;
          _imem=imem;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico();
              (*ob).cloc.totalen=255;
            } else {
              if (((*ob).cloc.totalen=constante())<0) c_error(4,127);
              if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
              if (pieza!=p_corce) c_error(3,26);
              lexico();
            }
          } else (*ob).cloc.totalen=255;
          if (pieza==p_asig) {
            save_error(1);
            _itxt=itxt;
            lexico();
            if (pieza!=p_lit && !(pieza==p_id && (*o).tipo==tcons && (*o).cons.literal))
              c_error(3,128);
            if (strlen((char*)&mem[pieza_num])>(*ob).cloc.totalen+1)
              c_error(4,129);
            imem=_imem+1+((*ob).cloc.totalen+5)/4; // ej. c[32] -> c[0]..c[32],NUL
            test_buffer(&mem,&imem_max,imem);
            strcpy((char*)&mem[_imem+1],(char*)&mem[pieza_num]);
            itxt=_itxt; // Saca la cadena del segmento de textos
            lexico();
          } else {
            imem=_imem+1+((*ob).cloc.totalen+5)/4;
            test_buffer(&mem,&imem_max,imem);
          } mem[_imem]=0xDAD00000|(*ob).cloc.totalen;
          (*ob).cloc.offset=iloc+1;
          iloc+=1+((*ob).cloc.totalen+5)/4;
        }

        if (pieza==p_coma) pieza=p_string; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_byte) {                       // Tabla Byte privado

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero2:
          ob=analiza_pointer(tpblo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero2;
          }

        } else {

          if (pieza!=p_id) c_error(1,142);
          ob=o; if ((*ob).tipo!=tnone) {
            if (parametros==-1 && (*ob).param==1 && (*ob).bloque==bloque_actual) {
              if ((*ob).tipo==tbloc) { // Se repite un byte parámetro
                lexico();
                if (pieza==p_corab) c_error(2,53);
                else if (pieza==p_asig) c_error(0,54);
                else {
                  while (pieza==p_ptocoma || pieza==p_coma) {
                    lexico();
                  }
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,117);
            } else c_error(0,117);
          } else lexico();

          (*ob).tipo=tbloc; _imem=imem;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              oimemptr=(byte*)&mem[imem];
              tglo_init(2);
              (*ob).bloc.len1=imemptr-oimemptr-1;
              (*ob).bloc.len2=-1;
              (*ob).bloc.len3=-1;
              (*ob).bloc.totalen=((*ob).bloc.len1+4)/4;
            } else {
              if (((*ob).bloc.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).bloc.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).bloc.len3=constante())<0) c_error(4,35);
                } else (*ob).bloc.len3=-1;
              } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }

              (*ob).bloc.totalen=(*ob).bloc.len1+1;
              if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
              if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico();
                oimemptr=(byte*)&mem[imem];
                tglo_init(2);
                if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
              } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
            }
          } else {                                          // Byte privado
            (*ob).tipo=tbloc; (*ob).bloc.offset=imem;
            (*ob).bloc.len1=0;
            (*ob).bloc.len2=-1;
            (*ob).bloc.len3=-1;
            (*ob).bloc.totalen=1; // 1 int
            if (pieza==p_asig) {
              save_error(1);
              lexico();
              mem[imem]=constante();
              if (mem[imem]<0 || mem[imem]>255) c_error(4,143);
            }
          }
          imem=_imem+(*ob).bloc.totalen; test_buffer(&mem,&imem_max,imem);
          (*ob).bloc.offset=iloc; iloc+=(*ob).bloc.totalen;
        }

        if (pieza==p_coma) pieza=p_byte; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_word) {                       // Tabla Word privada

        lexico();

        if (pieza==p_pointer) { // Puntero a byte

          lexico(); puntero3:
          ob=analiza_pointer(tpwlo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero3;
          }

        } else {

          if (pieza!=p_id) c_error(1,145);
          ob=o; if ((*ob).tipo!=tnone) {
            if (parametros==-1 && (*ob).param==1 && (*ob).bloque==bloque_actual) {
              if ((*ob).tipo==twloc) { // Se repite un word parámetro
                lexico();
                if (pieza==p_corab) c_error(2,53);
                else if (pieza==p_asig) c_error(0,54);
                else {
                  while (pieza==p_ptocoma || pieza==p_coma) {
                    lexico();
                  }
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,117);
            } else c_error(0,117);
          } else lexico();

          (*ob).tipo=twloc; _imem=imem;
          if (pieza==p_corab) {
            lexico();
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              oimemptr=(byte*)&mem[imem];
              tglo_init(1);
              (*ob).wloc.len1=(imemptr-oimemptr)/2-1;
              (*ob).wloc.len2=-1;
              (*ob).wloc.len3=-1;
              (*ob).wloc.totalen=((*ob).wloc.len1+2)/2;
            } else {
              if (((*ob).wloc.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).wloc.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).wloc.len3=constante())<0) c_error(4,35);
                } else (*ob).wloc.len3=-1;
              } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }

              (*ob).wloc.totalen=(*ob).wloc.len1+1;
              if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
              if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1);
                lexico();
                oimemptr=(byte*)&mem[imem];
                tglo_init(1);
                if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
              } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
            }
          } else {                                          // Word privado
            (*ob).tipo=twloc; (*ob).wloc.offset=imem;
            (*ob).wloc.len1=0;
            (*ob).wloc.len2=-1;
            (*ob).wloc.len3=-1;
            (*ob).wloc.totalen=1; // 1 int
            if (pieza==p_asig) {
              save_error(1);
              lexico();
              mem[imem]=constante();
              if (mem[imem]<0 || mem[imem]>65535) c_error(4,144);
            }
          }
          imem=_imem+(*ob).wloc.totalen; test_buffer(&mem,&imem_max,imem);
          (*ob).wloc.offset=iloc; iloc+=(*ob).wloc.totalen;
        }

        if (pieza==p_coma) pieza=p_word; else {
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,137);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
        }

      } else if (pieza==p_int || pieza==p_id || pieza==p_pointer) {

        if (pieza==p_int) {
          lexico();
          if (pieza!=p_id && pieza!=p_pointer) c_error(1,149);
        }

        if (pieza==p_pointer) { // Miembro puntero a int

          lexico(); puntero4:
          ob=analiza_pointer(tpilo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
          test_buffer(&mem,&imem_max,++imem);
          if (pieza==p_coma) {
            lexico(); if (pieza==p_pointer) lexico();
            goto puntero4;
          }

        } else {

          // Si el objeto no es tnone, se repite un parámetro o bien es un error

          ob=o; if ((*ob).tipo!=tnone) {
            if (parametros==-1 && (*ob).param==1 && (*ob).bloque==bloque_actual) {
              if ((*ob).tipo==tvloc) { // Se repite una variable local
                lexico();
                if (pieza==p_corab) c_error(2,53);
                else if (pieza==p_asig) c_error(0,54);
                else {
                  while (pieza==p_ptocoma || pieza==p_coma) {
                    lexico();
                  }
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,117);
            } else c_error(0,117);
          } else lexico();

          if (pieza==p_corab) {                             // Tabla privada
            lexico();
            (*ob).tipo=ttloc; _imem=imem;
            if (pieza==p_corce) {
              lexico(); if (pieza!=p_asig) c_error(3,23); lexico();
              tglo_init(3);
              (*ob).tloc.len1=imem-_imem-1;
              (*ob).tloc.len2=-1;
              (*ob).tloc.len3=-1;
              (*ob).tloc.totalen=imem-_imem;
            } else {
              if (((*ob).tloc.len1=constante())<0) c_error(4,35);
              if (pieza==p_coma) {
                lexico();
                if (((*ob).tloc.len2=constante())<0) c_error(4,35);
                if (pieza==p_coma) {
                  lexico();
                  if (((*ob).tloc.len3=constante())<0) c_error(4,35);
                } else (*ob).tloc.len3=-1;
              } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
              (*ob).tloc.totalen=(*ob).tloc.len1+1;
              if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
              if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
              if (pieza!=p_corce) c_error(3,26); lexico();
              if (pieza==p_asig) {
                save_error(1); lexico(); tglo_init(3);
                if (imem-_imem>(*ob).tloc.totalen) c_error(4,33); }
            }
            imem=_imem+(*ob).tloc.totalen;
            test_buffer(&mem,&imem_max,imem);
            (*ob).tloc.offset=iloc; iloc+=(*ob).tloc.totalen;

          } else {                                          // Variable privada

            (*ob).tipo=tvloc; (*ob).vloc.offset=iloc++;
            if (pieza==p_asig) { lexico(); mem[imem]=constante(); }
            test_buffer(&mem,&imem_max,++imem);
          }
        }
        if (!free_sintax) if (pieza!=p_ptocoma && pieza!=p_coma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
      } (*ob).bloque=bloque_actual;
    }

    mem[_imem_old]=imem;

  };

}

  //───────────────────────────────────────────────────────────────────────────
  // Inicialización de tablas (a mem[imem++])
  //───────────────────────────────────────────────────────────────────────────
  // <init>  := ( <string> | <exp> [ [p_dup] p_abrir <init> p_cerrar ] )
  //            [ p_coma <init>]
  //───────────────────────────────────────────────────────────────────────────


void tglo_init(memptrsize tipo) { // Tipo: 0-Int o variado (struct), 1-Word, 2-Byte
  if (tipo==3) { memset(frm,0,ifrm_max*sizeof(memptrsize)); tipo=0; }
  imemptr=(byte*)&mem[imem];
  tglo_init2(tipo);
  if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
}

void tloc_init(int tipo) {
  int * ptr;

  if (tipo==3) { memset(frm,0,ifrm_max*sizeof(memptrsize)); tipo=0; }

  ptr=mem; mem=loc; loc=ptr;
  swap(imem,iloc); swap(imem_max,iloc_max);

  imemptr=(byte*)&mem[imem];
  tglo_init2(tipo);

  ptr=mem; mem=loc; loc=ptr;
  swap(imem,iloc); swap(imem_max,iloc_max);

  if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
}

// OJO!!! tglo_init no puede llamar a test_buffer !!!

#if __WORDSIZE == 64
#define memptrsize int64_t
#else
#define memptrsize int32_t
#endif


void tglo_init2(int tipo) {
  int valor,dup,_imem,len,n;
  byte *oimemptr;

  while (1) {

    // Mira si finaliza la inicialización de datos

    if (pieza==p_cerrar || pieza==p_ptocoma) {
      if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || tipo==1) {
        imemptr+=2;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && tipo!=1) imemptr=(byte*)&mem[imem];
      } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || tipo==2) {
        imemptr++;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && tipo!=2) imemptr=(byte*)&mem[imem];
      } else {
        imemptr+=4;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
      }
//      test_buffer(&mem,&imem_max,imem);
      return;
    }

    // Una coma sola, como "3,,4", avanza una posición (y define un 0)

    if (pieza==p_coma) {
      if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || tipo==1) {
        (*(word*)imemptr)=0;
        imemptr+=2;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && tipo!=1) imemptr=(byte*)&mem[imem];
      } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || tipo==2) {
        *imemptr++=0;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && tipo!=2) imemptr=(byte*)&mem[imem];
      } else {
        imemptr+=4;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
      }
//      test_buffer(&mem,&imem_max,imem);
      lexico(); continue;
    }

    if (pieza==p_abrir) { // Dup x 1

      dup=1;

    } else if (pieza==p_dup) { // Dup x 2

      dup=2; lexico();
      if (pieza!=p_abrir) c_error(3,36);

    } else if (pieza==p_lit || (pieza==p_id && (*o).tipo==tcons && (*o).cons.literal)) {

      valor=pieza_num; lexico();
      if (pieza==p_abrir || pieza==p_dup) c_error(2,38);

      // Mete un literal en un dato de tipo string

      if ((((memptrsize)imemptr-(memptrsize)mem)%4)==0) {
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (frm[imem]==0xdad00000) {
          if (strlen((char*)&mem_ory[valor])>(mem[imem]&0xFFFFF)+1) c_error(2,129);
          _imem=imem;
          imem+=1+((mem[imem]&0xFFFFF)+5)/4;
//          test_buffer(&mem,&imem_max,imem);
          imemptr=(byte*)&mem[imem];
          strcpy((char*)&mem[_imem+1],(char*)&mem_ory[valor]);
          if (pieza!=p_coma) return;
          lexico(); continue;
        }
      }

      // Mete un valor literal en otro tipo de dato (en lugar de un string)

      if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || tipo==1) { // Un string en words (el puntero short)

        imemptr=(byte*)(((memptrsize)imemptr+1)&-2); // Lo hace par
        *((word*)imemptr)=valor;
        imemptr+=2;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && tipo!=1) imemptr=(byte*)&mem[imem];
        if (pieza!=p_coma) return;
        lexico(); continue;

      } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || tipo==2) { // Un string en bytes

        oimemptr=imemptr;
        while (*(oimemptr-(memptrsize)mem+(memptrsize)frm)==2) oimemptr++;
        if (tipo==0) if (strlen((char*)&mem_ory[valor])>(memptrsize)(oimemptr-imemptr)) c_error(2,33);
        strcpy((char *)imemptr,(char*)&mem_ory[valor]);
        imemptr+=strlen((char *)imemptr);
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && tipo!=2) imemptr=(byte*)&mem[imem];
        if (pieza!=p_coma) return;
        lexico(); continue;

      } else { // Un string en un int (el puntero long, como en DIV 1)

        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        mem[imem++]=valor;
//        test_buffer(&mem,&imem_max,imem);
        imemptr=(byte*)&mem[imem];
        if (pieza!=p_coma) return;
        lexico(); continue;

      }

    } else {
      valor=constante();
      if (pieza!=p_abrir) {
        if (pieza!=p_dup) {

          if ((((memptrsize)imemptr-(memptrsize)mem)%4)==0) {
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            if (frm[imem]==0xdad00000) c_error(2,128);
          }

          // Mete un valor numérico en la memoria

          if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || tipo==1) { // En un word

            if (valor<0 || valor>65535) c_error(2,144);
            imemptr=(byte*)(((memptrsize)imemptr+1)&-2); // Lo hace par
            *((word*)imemptr)=valor;
            imemptr+=2;
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && tipo!=1) imemptr=(byte*)&mem[imem];
            if (pieza!=p_coma) return;
            lexico(); continue;

          } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || tipo==2) { // En un byte

            if (valor<0 || valor>255) c_error(2,143);
            *imemptr++=valor;
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && tipo!=2) imemptr=(byte*)&mem[imem];
            if (pieza!=p_coma) return;
            lexico(); continue;

          } else { // En un int (lo normal en DIV 1)

            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            mem[imem++]=valor;
//            test_buffer(&mem,&imem_max,imem);
            imemptr=(byte*)&mem[imem];
            if (pieza!=p_coma) return;
            lexico(); continue;

          }

        } else {
          dup=valor; lexico();
          if (pieza!=p_abrir) c_error(3,36);
        }
      } else dup=valor;
    }

    // Duplica una secuencia de valores

    if (dup<1) c_error(2,38);
    lexico();

    // Analiza la secuencia como otra inicialización ...

    oimemptr=imemptr;
    tglo_init2(tipo);
    if (pieza!=p_cerrar) c_error(3,25);

    // Y la duplica el número de veces indicado

    if (dup>1) {
      len=imemptr-oimemptr;
//      test_buffer(&mem,&imem_max,(((memptrsize)oimemptr-(memptrsize)mem)+len*dup+3)/4);
      while (--dup) {
        for (n=0;n<len;n++) {

          // Comprueba que las cadenas caigan sobre si mismas en los dup()...
          if ((((memptrsize)imemptr-(memptrsize)mem)%4)==0) {
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            _imem=((memptrsize)oimemptr+n-(memptrsize)mem+3)/4;
            if (frm[imem]==0xdad00000) {
              if (frm[_imem]!=0xdad00000) c_error(3,136);
              else if (mem[imem]!=mem[_imem]) c_error(3,136);
            } else if (frm[_imem]==0xdad00000) c_error(3,136);
          }

          // Comprueba que los words caigan sobre words
          if (*(byte*)((memptrsize)imemptr-(memptrsize)mem+(memptrsize)frm)==1) {
            if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)!=1) c_error(3,136);
          } else if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)==1) c_error(3,136);

          // Comprueba que los bytes caigan sobre bytes
          if (*(byte*)((memptrsize)imemptr-(memptrsize)mem+(memptrsize)frm)==2) {
            if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)!=2) c_error(3,136);
          } else if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)==2) c_error(3,136);

          *imemptr++=*(oimemptr+n);
        }
      }
    }

    lexico();
    if (pieza!=p_coma) {
      imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
      return;
    }
    lexico();
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Analisis de un bloque de sentencias;
//═════════════════════════════════════════════════════════════════════════════

void sentencia() {

  int im1,im2,im3,im4;
  int dir,from,to,step;

  while (pieza>=p_return) {
    test_buffer(&mem,&imem_max,imem);
    switch (pieza) {
      case p_return:
        inicio_sentencia();
        lexico(); if (pieza==p_abrir) {
          lexico(); if (pieza!=p_cerrar) {
            expresion(); if (pieza!=p_cerrar) c_error(3,25);
            g1(DIVBC_rtf);
            } else {
            g1(DIVBC_ret);
            } lexico();
          } else {
          g1(DIVBC_ret);
        }
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
        final_sentencia(); grabar_sentencia(); break;
      case p_if:
        inicio_sentencia();
        lexico();
        if (!free_sintax) if (pieza!=p_abrir) c_error(3,36);
        if (pieza==p_abrir) lexico();
        condicion();
        if (!free_sintax) if (pieza!=p_cerrar) c_error(3,25);
        if (pieza==p_cerrar) lexico();
        g2(DIVBC_jpf,0); im1=imem-1;
        final_sentencia(); grabar_sentencia();
        sentencia();
        if (pieza==p_else) {
          inicio_sentencia();
          lexico();
          g2(DIVBC_jmp,0); mem[im1]=imem; im1=imem-1;
          final_sentencia(); grabar_sentencia();
          sentencia();
          }
        mem[im1]=imem; if (pieza!=p_end) c_error(0,40); lexico();
        break;
      case p_loop:
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexico();
        im1=imem; sentencia();
        if (pieza!=p_end) c_error(0,40);
        inicio_sentencia(); lexico();
        g2(DIVBC_jmp,im1);
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im1;
        final_sentencia(); grabar_sentencia();
        break;
      case p_while:
        inicio_sentencia();
        tbreak[itbreak++]=0; tcont[itcont++]=0; im1=imem;
        lexico();
        if (!free_sintax) if (pieza!=p_abrir) c_error(3,36);
        if (pieza==p_abrir) lexico();
        condicion();
        if (!free_sintax) if (pieza!=p_cerrar) c_error(3,25);
        if (pieza==p_cerrar) lexico();
        g2(DIVBC_jpf,0); im2=imem-1;
        final_sentencia(); grabar_sentencia();
        sentencia();
        if (pieza!=p_end) c_error(0,40); inicio_sentencia();
        lexico();
        g2(DIVBC_jmp,im1); mem[im2]=imem;
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im1;
        final_sentencia(); grabar_sentencia();
        break;
      case p_repeat:
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexico();
        im1=imem; sentencia();
        if (pieza!=p_until) c_error(0,42);
        inicio_sentencia();
        lexico();
        if (!free_sintax) if (pieza!=p_abrir) c_error(3,36);
        if (pieza==p_abrir) lexico();
        condicion();
        if (!free_sintax) if (pieza!=p_cerrar) c_error(3,25);
        if (pieza==p_cerrar) lexico();
        g2(DIVBC_jpf,im1);
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im1;
        final_sentencia(); grabar_sentencia();
        break;
      case p_from:
        inicio_sentencia();
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexico();
        if (pieza!=p_id) c_error(0,67);

        if ((*o).tipo==tvglo) {
          dir=(*o).vglo.offset; g2(DIVBC_car,dir);
        } else if ((*o).tipo==tvloc && (!(*o).bloque || (*o).bloque==bloque_actual)) {
          dir=-(*o).vloc.offset;
          g2(DIVBC_car,-dir); g1(DIVBC_aid);
        } else c_error(0,67);

        lexico();
        if (pieza!=p_asig) c_error(3,68); lexico();
        from=constante();
        if (pieza!=p_to) c_error(1,69); lexico();
        to=constante();
        if (from==to) c_error(4,71);
        if (pieza==p_step) {
          lexico();
          step=constante();
          if (from<to && step<=0) c_error(4,70);
          if (from>to && step>=0) c_error(4,70);
        } else {
          if (from<to) step=1; else step=-1;
        }
        g2(DIVBC_car,from); // Asignación del from
        g1(DIVBC_asi); g1(DIVBC_asp);

        im1=imem; // Inicio del bucle

        if (dir>=0) { // Comparación de la condición de permanencia
          g2(DIVBC_car,dir);
        } else {
          g2(DIVBC_car,-dir); g1(DIVBC_aid);
        }
        g1(DIVBC_ptr); g2(DIVBC_car,to);
        if (step>0) g1(DIVBC_mei); else g1(DIVBC_mai);
        g2(DIVBC_jpf,0); im2=imem-1;

        final_sentencia(); grabar_sentencia();
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();

        sentencia();
        if (pieza!=p_end) c_error(0,40); inicio_sentencia();
        lexico();

        im3=imem; // Posición del continue

        if (dir>=0) { // Incremento y vuelta al inicio del bucle
          g2(DIVBC_car,dir); 
        } else {
          g2(DIVBC_car,-dir); g1(DIVBC_aid);
        }
        g2(DIVBC_car,step);
        g1(DIVBC_ada); g1(DIVBC_asp);
        g2(DIVBC_jmp,im1); mem[im2]=imem;

        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im3;
        final_sentencia(); grabar_sentencia();

        break;

      case p_for:
        inicio_sentencia();
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexico();
        if (pieza!=p_abrir) c_error(3,36); lexico();
        if (pieza!=p_ptocoma) {
          expresion(); g1(DIVBC_asp);
          while (pieza==p_coma) { lexico(); expresion(); g1(DIVBC_asp); }
        } im1=imem;
        if (pieza!=p_ptocoma) c_error(3,47); lexico();
        if (pieza==p_ptocoma) {
          g2(DIVBC_car,1);
        } else expresion();
        g2(DIVBC_jpf,0); im2=imem-1;
        while (pieza==p_coma) { lexico(); expresion();
          g2(DIVBC_jpf,im2); im2=imem-1; }
        g2(DIVBC_jmp,0); im3=imem-1;
        if (pieza!=p_ptocoma) c_error(3,47); lexico();
        if (pieza!=p_cerrar) {
          expresion(); g1(DIVBC_asp);
          while (pieza==p_coma) { lexico(); expresion(); g1(DIVBC_asp); }
        }
        g2(DIVBC_jmp,im1);
        if (pieza!=p_cerrar) c_error(3,25); lexico();
        final_sentencia(); grabar_sentencia();
        mem[im3++]=imem; sentencia();
        if (pieza!=p_end) c_error(0,40); inicio_sentencia(); lexico();
          g2(DIVBC_jmp,im3);
          do { im1=mem[im2]; mem[im2]=imem; im2=im1; } while(im2);
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
          while (tcont[--itcont]!=0) mem[tcont[itcont]]=im3;
        final_sentencia(); grabar_sentencia();
        break;
      case p_switch:
        inicio_sentencia();
        lexico();
        if (!free_sintax) if (pieza!=p_abrir) c_error(3,36);
        if (pieza==p_abrir) lexico();
        condicion();
        if (!free_sintax) if (pieza!=p_cerrar) c_error(3,25);
        if (pieza==p_cerrar) lexico();
        while (pieza==p_ptocoma) {
          lexico();
        }
        final_sentencia(); grabar_sentencia();
        im1=0; im2=0;
        while (pieza!=p_end) {
          inicio_sentencia();
          if (pieza==p_case) {
            im3=0; do {

              lexico();
              if (im1) mem[im1]=imem;
              expresion(); if (pieza!=p_rango) {
                g2(DIVBC_cse,0); im1=imem-1;
              } else {
                lexico(); expresion();
                g2(DIVBC_csr,0); im1=imem-1;
              }

              if (pieza==p_coma) {
                g2(DIVBC_jmp,im3); im3=imem-1;
              }

            } while (pieza==p_coma);

            while(im3) { im4=mem[im3]; mem[im3]=imem; im3=im4; }

          } else if (pieza==p_default) {
            lexico();
            if (im1) mem[im1]=imem; im1=0;
          } else c_error(0,50);
          if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,65);
          while (pieza==p_ptocoma || pieza==p_coma) lexico();
          g1(DIVBC_asp);
          final_sentencia(); grabar_sentencia();
          sentencia();
          if (pieza!=p_end) c_error(0,51); inicio_sentencia();
            g2(DIVBC_jmp,im2); im2=imem-1;
          pasa_ptocoma();
          final_sentencia(); grabar_sentencia();
        } inicio_sentencia();
        if (im1) mem[im1]=imem; g1(DIVBC_asp);
        while(im2) { im1=mem[im2]; mem[im2]=imem; im2=im1; }
        lexico();
        final_sentencia(); grabar_sentencia(); break;
      case p_frame:
        inicio_sentencia();
        lexico(); if (pieza==p_abrir) {
          lexico(); if (pieza!=p_cerrar) {
            expresion(); if (pieza!=p_cerrar) c_error(3,25);
            g1(DIVBC_frf);
          } else {
            g1(DIVBC_frm);
          } lexico();
        } else {
          g1(DIVBC_frm);
        }
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
        final_sentencia(); grabar_sentencia(); break;
      case p_debug:
        inicio_sentencia();
        g1(DIVBC_dbg); lexico();
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
        final_sentencia(); grabar_sentencia();
        break;
      case p_break:
        inicio_sentencia();
        if (itbreak==0) c_error(0,48); lexico();
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
        g2(DIVBC_jmp,0); tbreak[itbreak++]=imem-1;
        final_sentencia(); grabar_sentencia(); break;
      case p_continue:
        inicio_sentencia();
        if (itcont==0) c_error(0,49); lexico();
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
        g2(DIVBC_jmp,0); tcont[itcont++]=imem-1;
        final_sentencia(); grabar_sentencia(); break;
      case p_clone:
        inicio_sentencia();
        lexico(); g2(DIVBC_clo,0); im1=imem-1;
        final_sentencia(); grabar_sentencia();
        sentencia();
        if (pieza!=p_end) c_error(0,40); lexico();
        mem[im1]=imem; break;
      case p_ptocoma:
        lexico(); break;
      default:
        inicio_sentencia();
        error_27=124;
        expresion();
        do {
          _exp--;
        } while ((*_exp).tipo==eoper && (*_exp).token==p_string);
        error_27=27;
        switch((*_exp).tipo) {
          case ecall: break;
          case efunc: break;
          case efext: break;
          case eoper:
            switch((*_exp).token) {
              case p_asig: case p_inc: case p_suma: case p_dec: case p_resta:
              case p_add_asig: case p_sub_asig: case p_mul_asig:
              case p_div_asig: case p_mod_asig: case p_and_asig:
              case p_or_asig: case p_xor_asig: case p_shr_asig: case p_shl_asig:

              case p_asigword: case p_incword: case p_sumaword: case p_decword: case p_restaword:
              case p_add_asigword: case p_sub_asigword: case p_mul_asigword:
              case p_div_asigword: case p_mod_asigword: case p_and_asigword:
              case p_or_asigword: case p_xor_asigword: case p_shr_asigword: case p_shl_asigword:

              case p_asigchar: case p_incchar: case p_sumachar: case p_decchar: case p_restachar:
              case p_add_asigchar: case p_sub_asigchar: case p_mul_asigchar:
              case p_div_asigchar: case p_mod_asigchar: case p_and_asigchar:
              case p_or_asigchar: case p_xor_asigchar: case p_shr_asigchar: case p_shl_asigchar:

              case p_strcpy: case p_strcat: case p_strsub: break;
              default: c_error(4,64); break;
            } break;
          default: c_error(4,64);
        }
        if (!free_sintax) if (pieza!=p_ptocoma) c_error(3,66);
        while (pieza==p_ptocoma || pieza==p_coma) lexico();
        g1(DIVBC_asp);
        final_sentencia(); grabar_sentencia();
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Analisis de una condición (genera código para calcularla)
//═════════════════════════════════════════════════════════════════════════════

int tipo_expresion=0; // 0 - int, 1 - char o byte, 2 - string
int tipo_factor=0;    // idem

void condicion(void) {
  byte * __ierror, * _ierror;
  int _linea;

  if (extended_conditions) { expresion(); return; } // Como en DIV 1

  _exp=tabexp;
  save_error(0);
  _linea=linea; _ierror=ierror; con00(0);
  swap(linea,_linea); __ierror=ierror; ierror=_ierror;
  generar_expresion();
  linea=_linea; ierror=__ierror;
}

void con00(int tipo_exp) {
  int te=tipo_expresion;
  tipo_expresion=tipo_exp;
  con0();
  tipo_expresion=te;
}

void con0() { // Operadores con asociatividad de derecha a izquierda <-
  int p;

  con1();
  if ((p=pieza)==p_asig || (p>=p_add_asig && p<=p_shl_asig)) {
    c_error(0,140);
  }
}

void con1() { // Operadores con asociatividad de izquierda a derecha ->
  int p;
  con2();
  while ((p=pieza)>=p_or && p<=p_andofs) {
    if (p>=p_xorptr) p-=p_xorptr-p_xor; // Convierte xorptr,andofs en xor,and
    lexico(); con2();
    (*_exp).tipo=eoper; (*_exp++).token=p;
    tipo_factor=0;
  }
}

void con2() {
  int p,tf;
  exp3();
  if (simple_conditions) {
    p=pieza;
    while ((p>=p_igu && p<=p_mai)||p==p_asig)  {
      tf=tipo_factor;
      if (p==p_asig) p=p_igu;
      lexico(); exp3();
      if (tf==2 || tipo_factor==2) {
        p+=p_strigu-p_igu;
        (*_exp).tipo=eoper; (*_exp++).token=p;
      } else {
        (*_exp).tipo=eoper; (*_exp++).token=p;
      } tipo_factor=0; p=pieza;
    }
  } else {
    while ((p=pieza)>=p_igu && p<=p_mai) {
      tf=tipo_factor;
      lexico(); exp3();
      if (tf==2 || tipo_factor==2) {
        p+=p_strigu-p_igu;
        (*_exp).tipo=eoper; (*_exp++).token=p;
      } else {
        (*_exp).tipo=eoper; (*_exp++).token=p;
      } tipo_factor=0;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Analisis de una expresión (genera código para calcularla)
//═════════════════════════════════════════════════════════════════════════════

void expresion(void) {
  byte * __ierror, * _ierror;
  int _linea;

  _exp=tabexp;
  save_error(0);
  _linea=linea; _ierror=ierror; exp00(0);
  swap(linea,_linea); __ierror=ierror; ierror=_ierror;
  generar_expresion();
  linea=_linea; ierror=__ierror;
}

void expresion_cpa(void) {
  byte * __ierror, * _ierror;
  int _linea;

  _exp=tabexp;
  save_error(0);
  _linea=linea; _ierror=ierror; exp00(0);
  swap(linea,_linea); __ierror=ierror; ierror=_ierror;
  if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointer) {
    _exp--; generar_expresion(); g1(DIVBC_cpa);
  } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerchar) {
    _exp--; generar_expresion();
    if (hacer_strfix) { g2(DIVBC_car,0); g1(DIVBC_strfix); g1(DIVBC_asp); }
    g1(DIVBC_cpachr);
  } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerbyte) {
    _exp--; generar_expresion();
    g1(DIVBC_cpachr);
  } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerword) {
    _exp--; generar_expresion();
    g1(DIVBC_cpawor);
  } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_string) {
    _exp--; generar_expresion(); g1(DIVBC_cpastr);
  } else c_error(4,44);
  linea=_linea; ierror=__ierror;
}

void generar_expresion(void) {
  struct exp_ele * e=tabexp;
  struct objeto * ob;

  do {
    switch ((*e).tipo) {
      case econs:
        g2(DIVBC_car,(*e).valor); break;
      case estring:
        g2(DIVBC_car,(*e).valor); break;
      case erango:
        g2(DIVBC_rng,(*e).valor); break;
      case ewhoami:
        g1(DIVBC_cid); break;
      case ecall:
        ob=(*e).objeto; g2(DIVBC_cal,(*ob).proc.offset);
        if ((*ob).usado) (*ob).proc.offset=imem-1; break;
      case efunc:
        ob=(*e).objeto; g2(DIVBC_fun,(*ob).func.codigo); break;
      case efext:
        ob=(*e).objeto; g2(DIVBC_ext,(*ob).fext.codigo); break;
      case echeck:
        g1(DIVBC_chk); break;
      case enull:
        g1(DIVBC_nul); break;
      case eoper: switch((*e).token) {
        case p_asig: g1(DIVBC_asi); break;
        case p_or: g1(DIVBC_ori); break;
        case p_xor: g1(DIVBC_xor); break;
        case p_and: g1(DIVBC_and); break;
        case p_igu: g1(DIVBC_igu); break;
        case p_dis: g1(DIVBC_dis); break;
        case p_may: g1(DIVBC_may); break;
        case p_men: g1(DIVBC_men); break;
        case p_mei: g1(DIVBC_mei); break;
        case p_mai: g1(DIVBC_mai); break;
        case p_add: g1(DIVBC_add); break;
        case p_sub: g1(DIVBC_sub); break;
        case p_mul: g1(DIVBC_mul); break;
        case p_div: g1(DIVBC_div); break;
        case p_mod: g1(DIVBC_mod); break;
        case p_neg: g1(DIVBC_neg); break;
        case p_pointer: g1(DIVBC_ptr); break;
        case p_not: g1(DIVBC_not); break;
        case p_punto: g1(DIVBC_aid); break;
        case p_shr: g1(DIVBC_shr); break;
        case p_shl: g1(DIVBC_shl); break;
        case p_inc: g1(DIVBC_ipt); break;
        case p_suma: g1(DIVBC_pti); break;
        case p_dec: g1(DIVBC_dpt); break;
        case p_resta: g1(DIVBC_ptd); break;
        case p_add_asig: g1(DIVBC_ada); break;
        case p_sub_asig: g1(DIVBC_sua); break;
        case p_mul_asig: g1(DIVBC_mua); break;
        case p_div_asig: g1(DIVBC_dia); break;
        case p_mod_asig: g1(DIVBC_moa); break;
        case p_and_asig: g1(DIVBC_ana); break;
        case p_or_asig: g1(DIVBC_ora); break;
        case p_xor_asig: g1(DIVBC_xoa); break;
        case p_shr_asig: g1(DIVBC_sra); break;
        case p_shl_asig: g1(DIVBC_sla); break;

        case p_pointerword: g1(DIVBC_ptrwor); break;
        case p_sumaword: g1(DIVBC_ptiwor); break;
        case p_restaword: g1(DIVBC_ptdwor); break;
        case p_incword: g1(DIVBC_iptwor); break;
        case p_decword: g1(DIVBC_dptwor); break;
        case p_asigword: g1(DIVBC_asiwor); break;
        case p_add_asigword: g1(DIVBC_adawor); break;
        case p_sub_asigword: g1(DIVBC_suawor); break;
        case p_mul_asigword: g1(DIVBC_muawor); break;
        case p_div_asigword: g1(DIVBC_diawor); break;
        case p_mod_asigword: g1(DIVBC_moawor); break;
        case p_and_asigword: g1(DIVBC_anawor); break;
        case p_or_asigword: g1(DIVBC_orawor); break;
        case p_xor_asigword: g1(DIVBC_xoawor); break;
        case p_shr_asigword: g1(DIVBC_srawor); break;
        case p_shl_asigword: g1(DIVBC_slawor); break;

        case p_pointerchar: g1(DIVBC_ptrchr); break;
        case p_sumachar: g1(DIVBC_ptichr); break;
        case p_restachar: g1(DIVBC_ptdchr); break;
        case p_incchar: g1(DIVBC_iptchr); break;
        case p_decchar: g1(DIVBC_dptchr); break;
        case p_asigchar: g1(DIVBC_asichr); break;
        case p_add_asigchar: g1(DIVBC_adachr); break;
        case p_sub_asigchar: g1(DIVBC_suachr); break;
        case p_mul_asigchar: g1(DIVBC_muachr); break;
        case p_div_asigchar: g1(DIVBC_diachr); break;
        case p_mod_asigchar: g1(DIVBC_moachr); break;
        case p_and_asigchar: g1(DIVBC_anachr); break;
        case p_or_asigchar: g1(DIVBC_orachr); break;
        case p_xor_asigchar: g1(DIVBC_xoachr); break;
        case p_shr_asigchar: g1(DIVBC_srachr); break;
        case p_shl_asigchar: g1(DIVBC_slachr); break;

        case p_strcpy: g1(DIVBC_strcpy); break;
        case p_strfix: g1(DIVBC_strfix); break;
        case p_strcat: g1(DIVBC_strcat); break;
        case p_stradd: g1(DIVBC_stradd); break;
        case p_strdec: g1(DIVBC_strdec); break;
        case p_strsub: g1(DIVBC_strsub); break;
        case p_strlen: g1(DIVBC_strlen); break;
        case p_strigu: g1(DIVBC_strigu); break;
        case p_strdis: g1(DIVBC_strdis); break;
        case p_strmay: g1(DIVBC_strmay); break;
        case p_strmen: g1(DIVBC_strmen); break;
        case p_strmei: g1(DIVBC_strmei); break;
        case p_strmai: g1(DIVBC_strmai); break;

        case p_pointerbyte: g1(DIVBC_ptrchr); break;
        case p_string: break; // Es como un p_pointer nulo, que no se hace

        default: c_error(4,41);
      } break;
      default: c_error(4,41);
    }
  } while (++e!=_exp);
}

//═════════════════════════════════════════════════════════════════════════════
//      Analisis de una expresión constante
//      No se procesan valores locales ni llamadas a otros procesos
//═════════════════════════════════════════════════════════════════════════════

int constante (void) {

  byte * __ierror, * _ierror;
  struct exp_ele * e=_exp=tabexp;
  int _linea, i=0;

  save_error(0);

  _linea=linea; _ierror=ierror; exp00(0);
  swap(linea,_linea); __ierror=ierror; ierror=_ierror;

  do switch ((*e).tipo) {
    case econs:
      pila[++i]=(*e).valor; break;
    case estring:
      pila[++i]=(*e).valor; break;

//    case erango:
//      if (pila[i]<0||pila[i]>(*e).valor) c_error(4,31); break;

    case eoper: switch((*e).token) {
      case p_or: pila[i-1]|=pila[i]; i--; break;
      case p_xor: pila[i-1]^=pila[i]; i--; break;
      case p_and: pila[i-1]&=pila[i]; i--; break;
      case p_igu: pila[i-1]=pila[i-1]==pila[i]; i--; break;
      case p_dis: pila[i-1]=pila[i-1]!=pila[i]; i--; break;
      case p_may: pila[i-1]=pila[i-1]>pila[i]; i--; break;
      case p_men: pila[i-1]=pila[i-1]<pila[i]; i--; break;
      case p_mei: pila[i-1]=pila[i-1]<=pila[i]; i--; break;
      case p_mai: pila[i-1]=pila[i-1]>=pila[i]; i--; break;
      case p_add: pila[i-1]+=pila[i]; i--; break;
      case p_sub: pila[i-1]-=pila[i]; i--; break;
      case p_mul: pila[i-1]*=pila[i]; i--; break;
      case p_div: if (pila[i]==0) c_error(4,34); pila[i-1]/=pila[i]; i--; break;
      case p_mod: if (pila[i]==0) c_error(4,34); pila[i-1]%=pila[i]; i--; break;
      case p_neg: pila[i]=-pila[i]; break;

// Creo que se quitó los pointer (y los rangos, por consiguiente) por los FROM,
// ya que provocaba confusion el tomar como constante una variable ...

//      case p_pointer: pila[i]=mem[pila[i]]; break;

      case p_not: pila[i]^=-1; break;
      case p_shr: pila[i-1]>>=pila[i]; i--; break;
      case p_shl: pila[i-1]<<=pila[i]; i--; break;
      default: c_error(4,30);
    } break;
    default: c_error(4,30);
  }while (++e!=_exp);

  linea=_linea; ierror=__ierror; return(pila[i]);
}

//═════════════════════════════════════════════════════════════════════════════
//      Análisis de una expresión a *tabexp
//═════════════════════════════════════════════════════════════════════════════

void exp00(int tipo_exp) {
  int te=tipo_expresion;
  tipo_expresion=tipo_exp;
  exp0();
  tipo_expresion=te;
}

void exp0() { // Operadores con asociatividad de derecha a izquierda <-
  int p;

  exp1();
  if ((p=pieza)==p_asig || (p>=p_add_asig && p<=p_shl_asig)) {
    if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointer) {
      _exp--; lexico();
      exp00(0);
      (*_exp).tipo=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerchar) {
      if (p==p_asig) p=p_asigchar; else p+=p_add_asigchar-p_add_asig;
      _exp--; lexico();
      exp00(1); // cadena[1]="A";
      if (hacer_strfix) { (*_exp).tipo=eoper; (*_exp++).token=p_strfix; }
      (*_exp).tipo=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerbyte) {
      if (p==p_asig) p=p_asigchar; else p+=p_add_asigchar-p_add_asig;
      _exp--; lexico();
      exp00(1); // mis_bytes[1]="A";
      (*_exp).tipo=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerword) {
      if (p==p_asig) p=p_asigword; else p+=p_add_asigword-p_add_asig;
      _exp--; lexico();
      exp00(0);
      (*_exp).tipo=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_string) {
      if (p==p_asig) { // cadena="..."; // Hacer el strcpy
        _exp--; lexico();
        exp00(2);
        (*_exp).tipo=eoper; (*_exp++).token=p_strcpy;
        (*_exp).tipo=eoper; (*_exp++).token=p_string; //Sigue habiendo una cadena en la pila
      } else if (p==p_add_asig) { // cadena+="..."; // Hacer el strcat
        _exp--; lexico();
        exp00(2);
        (*_exp).tipo=eoper; (*_exp++).token=p_strcat;
        (*_exp).tipo=eoper; (*_exp++).token=p_string; //Sigue habiendo una cadena en la pila
      } else if (p==p_sub_asig) { // cadena-=n; // Quita caracteres
        _exp--; lexico();
        exp00(-1);
        (*_exp).tipo=eoper; (*_exp++).token=p_strsub;
        (*_exp).tipo=eoper; (*_exp++).token=p_string; //Sigue habiendo una cadena en la pila
      } else c_error(0,133);
    } else c_error(0,39);
  } tipo_factor=0;
}

void exp1() { // Operadores con asociatividad de izquierda a derecha ->
  int p;
  div_exp2();
  while ((p=pieza)>=p_or && p<=p_andofs) {
    if (p>=p_xorptr) p-=p_xorptr-p_xor; // Convierte xorptr,andofs en xor,and
    lexico(); div_exp2();
    (*_exp).tipo=eoper; (*_exp++).token=p;
    tipo_factor=0;
  }
}

void div_exp2() {
  int p,tf;
  exp3();
  while ((p=pieza)>=p_igu && p<=p_mai) {
    tf=tipo_factor;
    lexico(); exp3();
    if (tf==2 || tipo_factor==2) {
      // OJO, p==q no se hará el strcmp() si son dos punteros a cadenas
      p+=p_strigu-p_igu;
      (*_exp).tipo=eoper; (*_exp++).token=p;
      tipo_factor=0;
    } else {
      (*_exp).tipo=eoper; (*_exp++).token=p;
    } tipo_factor=0;
  }
}

void exp3() {
  int p;
  exp4();
  while ((p=pieza)>=p_shr && p<=p_shl) {
    lexico(); exp4();
    (*_exp).tipo=eoper; (*_exp++).token=p;
    tipo_factor=0;
  }
}

void exp4() {
  int p,tf;
  exp5();
  while ((p=pieza)>=p_suma && p<=p_sub && (p<=p_resta || p>=p_add) ) {
    tf=tipo_factor;
    if (p<=p_resta) p+=p_add-p_suma; // Convierte suma,resta en add,sub
    lexico(); exp5();
    if (p==p_add && (tf==2 || tipo_factor==2 || tipo_expresion==2)) {
      (*_exp).tipo=eoper; (*_exp++).token=p_stradd;
      (*_exp).tipo=eoper; (*_exp++).token=p_string; // Hay una cadena en la pila
      tipo_factor=2;
    } else if (p==p_sub && tf==2) {
      if (tipo_factor==2) { (*_exp).tipo=eoper; (*_exp++).token=p_strlen; }
      (*_exp).tipo=eoper; (*_exp++).token=p_strdec;
      (*_exp).tipo=eoper; (*_exp++).token=p_string; // Hay una cadena en la pila
      tipo_factor=2;
    } else {
      (*_exp).tipo=eoper; (*_exp++).token=p;
      tipo_factor=0;
    }
  }
}

void exp5() {
  int p;
  unario();
  while ((p=pieza)>=p_mul && p<=p_multi) {
    if (p==p_multi) p=p_mul; // Convierte multi en mul
    lexico();  unario();
    (*_exp).tipo=eoper; (*_exp++).token=p;
    tipo_factor=0;
  }
}

void unario() {
  int p;

  while ((p=pieza)==p_add) { lexico();  }
  if ((p>=p_xorptr && p<=p_mul) || (p==p_inc || p==p_dec)) {

    if (parametros>0 && p==p_pointer) { factor(); return; }

    if (p>=p_sub && p<=p_mul) p-=p_sub-p_neg; // Conv. sub,mul en neg,pointer
    else if (p>=p_xorptr && p<=p_andofs) p+=p_pointer-p_xorptr; //xorptr,andofs pointer,offset

    lexico();  unario(); // p = ( neg, pointer, offset, not )

    if (p==p_offset) {
      if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointer) _exp--;
      else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerchar) {
        c_error(4,132); // &cadena[<exp>] ???
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerbyte) {
        if ((*(_exp-2)).tipo==econs && (*(_exp-2)).valor==0) _exp-=2;
        else c_error(4,132); // &bytes[<exp>] ???
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerword) {
        if ((*(_exp-2)).tipo==econs && (*(_exp-2)).valor==0) _exp-=2;
        else c_error(4,132); // &words[<exp>] ???
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_string) {
        // &cadena ­ cadena
      } else c_error(4,32);
    } else if (p==p_inc || p==p_dec) {
      if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointer) {
        _exp--;
        (*_exp).tipo=eoper; (*_exp++).token=p;
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerchar) {
        _exp--;
        (*_exp).tipo=eoper; (*_exp++).token=p+p_incchar-p_inc;
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerbyte) {
        _exp--;
        (*_exp).tipo=eoper; (*_exp++).token=p+p_incchar-p_inc;
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerword) {
        _exp--;
        (*_exp).tipo=eoper; (*_exp++).token=p+p_incword-p_inc;
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_string) {
        _exp--;
        (*_exp).tipo=econs;
        if (p==p_inc) (*_exp++).valor=-1; else (*_exp++).valor=1;
        (*_exp).tipo=eoper; (*_exp++).token=p_strsub;
        (*_exp).tipo=eoper; (*_exp++).token=p_string;
      } else c_error(4,122);
    } else if (p==p_pointer) {
      if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerchar) {
        // *cadena[0] ­ cadena[0]
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerbyte) {
        // *cadena[0] ­ cadena[0]
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerword) {
        // *cadena[0] ­ cadena[0]
      } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_string) {
        // *cadena == cadena
      } else {
        (*_exp).tipo=eoper; (*_exp++).token=p;
      }
    } else {
      (*_exp).tipo=eoper; (*_exp++).token=p;
    }

  } else exp6();

}

void exp6() { // Operador de acceso a variables o tablas locales ajenas
  struct exp_ele * e;

  factor();
  while (pieza==p_punto) {
    if (comprueba_id) (*_exp++).tipo=echeck;
    e=_exp+1;
    acceso_remoto=1; lexico(); factor(); acceso_remoto=0;

    // OJO, el <p_punto> DEBE SER LO SEGUNDO que el <factor>
    // introduzca en la <expresión>, no quedan más cojones.
    // (bueno, si, hacer aquí una búsqueda del p_punto entre e-1 y _exp ...)

    if ((*e).tipo==eoper && (*e).token==p_punto)
      (*e).token=p_add; else c_error(4,43);

  }
}

int struct_pointer; // p_pointer / p_pointerchar / p_string / p_pointerbyte / p_pointerword

void factor(void) {
  struct objeto * ob;
  struct objeto * obs;
  int p,offset,_imem;
  struct exp_ele * e;

  tipo_factor=0;

  switch (pieza) { // analisis de un factor

    case p_abrir:
      lexico(); exp00(tipo_expresion);
      if (pieza!=p_cerrar) c_error(3,25); lexico();  break;

    case p_corab:
      lexico();
      exp00(0);
      if (pieza!=p_corce) c_error(3,26);
      (*_exp).tipo=eoper; (*_exp++).token=p_pointer; lexico(); break;

    case p_whoami:
      (*_exp++).tipo=ewhoami; lexico();  break;

    case p_num:
      (*_exp).tipo=econs; (*_exp++).valor=pieza_num; lexico(); break;

    case p_lit:
      tipo_factor=2;
      if (tipo_expresion==1) {
        (*_exp).tipo=econs; (*_exp++).valor=(byte)mem[pieza_num];
      } else {
        (*_exp).tipo=estring; (*_exp++).valor=pieza_num;
        if (tipo_expresion==-1) { (*_exp).tipo=eoper; (*_exp++).token=p_strlen; }
      } lexico(); break;

    case p_type:
      lexico(); if (pieza!=p_id) c_error(0,112);
      switch((*o).tipo) {
        case tnone:
          (*o).linea=linea; (*o).ierror=ierror;
          (*_exp).tipo=econs; (*_exp++).valor=(memptrsize)o; (*o).usado=1; break;
        case tproc:
          (*_exp).tipo=econs; (*_exp++).valor=(memptrsize)o; break;
        case tsglo:
          if ((*o).sglo.offset==long_header) { // type mouse ­ 0
            (*_exp).tipo=econs; (*_exp++).valor=0; break;
          }
        default: c_error(0,60);
      } lexico(); break;

    case p_sizeof:

      lexico(); if (pieza!=p_abrir) c_error(3,36);
      lexico(); if (pieza!=p_id) c_error(3,113);;
      (*_exp).tipo=econs;
      switch((*o).tipo) {
        case tsglo: case tsloc:
          (*_exp++).valor=(*o).sglo.len_item*(*o).sglo.totalitems; break;
        case tpsgl: case tpslo:
          (*_exp++).valor=(*((*o).psgl.ostruct)).sglo.len_item*(*o).sglo.totalitems; break;
        case ttglo: case ttloc:
        case tbglo: case tbloc:
        case twglo: case twloc:
          (*_exp++).valor=(*o).tglo.totalen; break;
        case tcglo: case tcloc:
          (*_exp++).valor=((*o).cglo.totalen+5)/4; break;
        case tvglo: case tvloc:
          (*_exp++).valor=1; break;
        default: c_error(0,61);
      } lexico(); if (pieza!=p_cerrar) c_error(3,25); lexico();
      break;

    case p_struct: // Se comprueba la declaración de un puntero a struct parámetro
      if (parametros<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexico();
        if (pieza!=p_pointer) c_error(0,151); // No se define el pointer así
        lexico(); if (pieza!=p_id) c_error(1,110); obs=o;
        if ((*obs).tipo==tnone) c_error(0,150); // No se define el pointer así
        if ((*obs).tipo!=tsglo && (*obs).tipo!=tsloc) c_error(0,150);
        lexico(); ob=o;
        analiza_pointer_struct(tpslo,iloc++,obs);
        (*ob).linea=linea; (*ob).ierror=ierror;
        (*ob).param=1; parametros++;
        (*_exp).tipo=econs; (*_exp++).valor=(*ob).pslo.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
      } break;

    case p_string: // Se comprueba la declaración de un string parámetro
      if (parametros<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexico();
        if (pieza==p_pointer) { // Recibe un puntero a string
          lexico();
          ob=analiza_pointer(tpclo,iloc++);
          (*ob).linea=linea; (*ob).ierror=ierror;
          (*ob).param=1; parametros++;
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).pclo.offset;
          (*_exp).tipo=eoper; (*_exp++).token=p_punto;
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          break;
        }
        if (pieza!=p_id) c_error(1,125);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,126);
        (*ob).linea=linea; (*ob).ierror=ierror;
        (*ob).param=1;
        (*ob).tipo=tcloc; lexico();
        if (pieza==p_corab) {
          lexico();
          if (pieza==p_corce) {
            lexico();
            (*ob).cloc.totalen=255;
          } else {
            e=_exp;
            if (((*ob).cloc.totalen=constante())<0) c_error(4,127);
            if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
            if (pieza!=p_corce) c_error(3,26);
            lexico();
            _exp=e;
          }
        } else (*ob).cloc.totalen=255;
        if (parametros>1) g2(DIVBC_par,parametros-1); parametros=1;
        g2(DIVBC_pri,0); _imem=imem;
        imem+=1+((*ob).cloc.totalen+5)/4;
        test_buffer(&mem,&imem_max,imem);
        mem[_imem]=0xDAD00000|(*ob).cloc.totalen;
        memset(&mem[imem+1],0,(((*ob).cloc.totalen+5)&-5));
        (*ob).cloc.offset=iloc+1;
        iloc+=1+((*ob).cloc.totalen+5)/4;
        mem[_imem-1]=imem; // pri
        (*_exp).tipo=estring; (*_exp++).valor=(*ob).cloc.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        (*_exp).tipo=eoper; (*_exp++).token=p_string;
      } break;

    case p_byte: // Se comprueba la declaración de un byte parámetro
      if (parametros<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexico();
        if (pieza==p_pointer) {
          lexico();
          ob=analiza_pointer(tpblo,iloc++);
          (*ob).linea=linea; (*ob).ierror=ierror;
          (*ob).param=1; parametros++;
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).pblo.offset;
          (*_exp).tipo=eoper; (*_exp++).token=p_punto;
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          break;
        }
        if (pieza!=p_id) c_error(1,142);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,126);
        (*ob).linea=linea; (*ob).ierror=ierror;
        (*ob).param=1;
        (*ob).tipo=tbloc; lexico();
        if (pieza==p_corab) c_error(2,146);
        parametros++; (*ob).bloc.offset=iloc++;
        (*ob).bloc.len1=0;
        (*ob).bloc.len2=-1;
        (*ob).bloc.len3=-1;
        (*ob).bloc.totalen=1;
        (*_exp).tipo=econs; (*_exp++).valor=(*ob).bloc.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        (*_exp).tipo=econs; (*_exp++).valor=0;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointerbyte;
      } break;

    case p_word: // Se comprueba la declaración de un word parámetro
      if (parametros<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexico();
        if (pieza==p_pointer) {
          lexico();
          ob=analiza_pointer(tpwlo,iloc++);
          (*ob).linea=linea; (*ob).ierror=ierror;
          (*ob).param=1; parametros++;
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).pwlo.offset;
          (*_exp).tipo=eoper; (*_exp++).token=p_punto;
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          break;
        }
        if (pieza!=p_id) c_error(1,145);
        ob=o; if ((*ob).tipo!=tnone) c_error(0,126);
        (*ob).linea=linea; (*ob).ierror=ierror;
        (*ob).param=1;
        (*ob).tipo=twloc; lexico();
        if (pieza==p_corab) c_error(2,146);
        parametros++; (*ob).wloc.offset=iloc++;
        (*ob).wloc.len1=0;
        (*ob).wloc.len2=-1;
        (*ob).wloc.len3=-1;
        (*ob).wloc.totalen=1;
        (*_exp).tipo=econs; (*_exp++).valor=(*ob).wloc.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        (*_exp).tipo=econs; (*_exp++).valor=0;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointerword;
      } break;

    case p_int: // Se comprueba la declaración de un int parámetro
      if (parametros<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexico();
        if (pieza==p_pointer) {

    case p_pointer: // Declaración de un puntero a int parámetro
          if (parametros<=0) {
            if (error_27==27) c_error(1,27); else c_error(0,error_27);
          } parametros++;
          lexico();
          ob=analiza_pointer(tpilo,iloc++);
          (*ob).linea=linea; (*ob).ierror=ierror;
          (*ob).param=1;
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).pilo.offset;
          (*_exp).tipo=eoper; (*_exp++).token=p_punto;
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          break;
        } else if (pieza!=p_id) c_error(1,149);
      }

    case p_id: switch ((*o).tipo) {

      case tnone:
        ob=o; (*ob).linea=linea; (*ob).ierror=ierror; lexico();
        if (pieza!=p_abrir) {
          if (parametros>0) {
            if (pieza==p_corab) c_error(2,146);
            parametros++;
            (*ob).tipo=tvloc; (*ob).vloc.offset=iloc++;
            (*ob).param=1;
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).vloc.offset;
            (*_exp).tipo=eoper; (*_exp++).token=p_punto;
            (*_exp).tipo=eoper; (*_exp++).token=p_pointer; break;
          } else c_error(2,28);
        }
        (*ob).usado=1; (*ob).tipo=tproc;
        (*ob).proc.bloque=ob; (*ob).proc.offset=0; (*ob).proc.num_par=0;
        lexico();
        while (pieza!=p_cerrar) {
          (*ob).proc.num_par++;
          exp00(0);
          if (pieza!=p_cerrar) if (pieza!=p_coma) c_error(3,25);
          else { lexico();  if (pieza==p_cerrar) c_error(3,45); }
        } (*_exp).tipo=ecall; (*_exp++).objeto=ob; lexico();  break;

      case tcons:
        if ((*o).cons.literal) {
          tipo_factor=2;
          if (tipo_expresion==1) {
            (*_exp).tipo=econs; (*_exp++).valor=(byte)mem[pieza_num];
          } else if (tipo_expresion==-1) {
            (*_exp).tipo=econs; (*_exp++).valor=pieza_num;
            (*_exp).tipo=eoper; (*_exp++).token=p_strlen;
          } else {
            (*_exp).tipo=econs; (*_exp++).valor=(*o).cons.valor;
          }
        } else {
          (*_exp).tipo=econs; (*_exp++).valor=(*o).cons.valor;
        } lexico(); break;

      case tvglo:
        (*_exp).tipo=econs; (*_exp++).valor=(*o).vglo.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer; lexico();  break;
        break;

      case ttglo:
      case tpigl:
        ob=o; offset=(*ob).tglo.offset;
        lexico();
        (*_exp).tipo=econs; (*_exp++).valor=offset;
        if (pieza==p_corab) {
          if ((*ob).tipo==tpigl) {
            (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
            if (comprueba_null) (*_exp++).tipo=enull;
          }
          lexico();
          exp00(0);
          if ((*ob).tglo.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tglo.len1; }
          if ((*ob).tglo.len2>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tglo.len2; }
            if ((*ob).tglo.len3>-1) {
              if (pieza!=p_coma) c_error(3,130);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tglo.len3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).tglo.len2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).tglo.len1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        } (*_exp).tipo=eoper; (*_exp++).token=p_pointer; break;

      case tbglo:
      case tpbgl:
      case tpcgl:
        ob=o; offset=(*ob).bglo.offset;
        lexico();
        (*_exp).tipo=econs; (*_exp++).valor=offset;
        if ((*ob).tipo==tpbgl || (*ob).tipo==tpcgl) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (pieza!=p_corab) break;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        if (pieza==p_corab) {
          lexico();
          exp00(0);
          if ((*ob).bglo.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bglo.len1; }
          if ((*ob).bglo.len2>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bglo.len2; }
            if ((*ob).bglo.len3>-1) {
              if (pieza!=p_coma) c_error(3,130);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bglo.len3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).bglo.len2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).bglo.len1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).tipo=econs; (*_exp++).valor=0;
        } (*_exp).tipo=eoper; (*_exp++).token=p_pointerbyte ; break;

      case twglo:
      case tpwgl:
        ob=o; offset=(*ob).wglo.offset;
        lexico();
        (*_exp).tipo=econs; (*_exp++).valor=offset;
        if ((*ob).tipo==tpwgl) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (pieza!=p_corab) break;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        if (pieza==p_corab) {
          lexico();
          exp00(0);
          if ((*ob).wglo.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wglo.len1; }
          if ((*ob).wglo.len2>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wglo.len2; }
            if ((*ob).wglo.len3>-1) {
              if (pieza!=p_coma) c_error(3,130);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wglo.len3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).wglo.len2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).wglo.len1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).tipo=econs; (*_exp++).valor=0;
        } (*_exp).tipo=eoper; (*_exp++).token=p_pointerword; break;

      case tcglo:
        ob=o; offset=(*ob).cglo.offset;
        lexico();
        (*_exp).tipo=estring; (*_exp++).valor=offset;
        if (pieza==p_corab) {
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).cglo.totalen; }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
          (*_exp).tipo=eoper; (*_exp++).token=p_pointerchar;
        } else {
          tipo_factor=2;
          if (tipo_expresion==1) {
            (*_exp).tipo=econs; (*_exp++).valor=0;
            (*_exp).tipo=eoper; (*_exp++).token=p_pointerchar;
          } else {
            (*_exp).tipo=eoper; (*_exp++).token=p_string;
            if (tipo_expresion==-1) { (*_exp).tipo=eoper; (*_exp++).token=p_strlen; }
          }
        } break;

      case tsglo:
      case tpsgl:
        ob=o; lexico(); (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.offset;
        if ((*ob).tipo==tpsgl) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (pieza!=p_corab && pieza!=p_punto) break;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        if (pieza==p_corab) {
          lexico();
          exp00(0);
          if ((*ob).sglo.items1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sglo.items1; }
          if ((*ob).sglo.items2>-1) {
            if (pieza!=p_coma) c_error(3,131);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sglo.items2; }
            if ((*ob).sglo.items3>-1) {
              if (pieza!=p_coma) c_error(3,131);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sglo.items3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.items2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.items1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26); lexico();
          if ((*ob).tipo==tpsgl) {
            (*_exp).tipo=econs; (*_exp++).valor=(*((*ob).psgl.ostruct)).sglo.len_item;
          } else {
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.len_item;
          }
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza==p_punto) {
          struct_pointer=p_pointer;
          if ((*ob).tipo==tpsgl) member=(*ob).psgl.ostruct; else member=ob;
          lexico(); factor_struct();
          (*_exp).tipo=eoper; (*_exp++).token=struct_pointer;
        } else {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        } break;

      case tvloc:
        if (acceso_remoto && (*o).bloque) c_error(0,56);
        (*_exp).tipo=econs; (*_exp++).valor=(*o).vloc.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer; lexico(); break;

      case ttloc:
      case tpilo:
        if (acceso_remoto && (*o).bloque) c_error(0,56);
        ob=o; offset=(*ob).tloc.offset;
        lexico();
        (*_exp).tipo=econs; (*_exp++).valor=offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        if (pieza==p_corab) {
          if ((*ob).tipo==tpilo) {
            (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
            if (comprueba_null) (*_exp++).tipo=enull;
          }
          acceso_remoto=0;
          lexico();
          exp00(0);
          if ((*ob).tloc.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tloc.len1; }
          if ((*ob).tloc.len2>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tloc.len2; }
            if ((*ob).tloc.len3>-1) {
              if (pieza!=p_coma) c_error(3,130);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tloc.len3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).tloc.len2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).tloc.len1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer; break;

      case tbloc:
      case tpblo:
      case tpclo:
        if (acceso_remoto && (*o).bloque) c_error(0,56);
        ob=o; offset=(*ob).bloc.offset;
        lexico();
        (*_exp).tipo=econs; (*_exp++).valor=offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        if ((*ob).tipo==tpblo || (*ob).tipo==tpclo) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (pieza!=p_corab) break;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        if (pieza==p_corab) {
          acceso_remoto=0;
          lexico();
          exp00(0);
          if ((*ob).bloc.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bloc.len1; }
          if ((*ob).bloc.len2>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bloc.len2; }
            if ((*ob).bloc.len3>-1) {
              if (pieza!=p_coma) c_error(3,130);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bloc.len3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).bloc.len2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).bloc.len1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).tipo=econs; (*_exp++).valor=0;
        } (*_exp).tipo=eoper; (*_exp++).token=p_pointerbyte ; break;

      case twloc:
      case tpwlo:
        if (acceso_remoto && (*o).bloque) c_error(0,56);
        ob=o; offset=(*ob).wloc.offset;
        lexico();
        (*_exp).tipo=econs; (*_exp++).valor=offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        if ((*ob).tipo==tpwlo) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (pieza!=p_corab) break;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        if (pieza==p_corab) {
          acceso_remoto=0;
          lexico();
          exp00(0);
          if ((*ob).wloc.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wloc.len1; }
          if ((*ob).wloc.len2>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wloc.len2; }
            if ((*ob).wloc.len3>-1) {
              if (pieza!=p_coma) c_error(3,130);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wloc.len3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).wloc.len2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).wloc.len1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).tipo=econs; (*_exp++).valor=0;
        } (*_exp).tipo=eoper; (*_exp++).token=p_pointerword; break;

      case tcloc:
        if (acceso_remoto && (*o).bloque) c_error(0,56);
        ob=o; offset=(*ob).cloc.offset;
        lexico();
        (*_exp).tipo=estring; (*_exp++).valor=offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        if (pieza==p_corab) {
          acceso_remoto=0;
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).cloc.totalen; }
          if (pieza!=p_corce) c_error(3,26);
          lexico();
          (*_exp).tipo=eoper; (*_exp++).token=p_pointerchar;
        } else {
          tipo_factor=2;
          if (tipo_expresion==1) {
            (*_exp).tipo=econs; (*_exp++).valor=0;
            (*_exp).tipo=eoper; (*_exp++).token=p_pointerchar;
          } else {
            (*_exp).tipo=eoper; (*_exp++).token=p_string;
            if (tipo_expresion==-1) { (*_exp).tipo=eoper; (*_exp++).token=p_strlen; }
          }
        } break;

      case tsloc:
      case tpslo:
        if (acceso_remoto && (*o).bloque) c_error(0,56);
        ob=o; lexico(); (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.offset;
        (*_exp).tipo=eoper; (*_exp++).token=p_punto;
        if ((*ob).tipo==tpslo) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (pieza!=p_corab && pieza!=p_punto) break;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        if (pieza==p_corab) {
          acceso_remoto=0;
          lexico();
          exp00(0);
          if ((*ob).sloc.items1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sloc.items1; }
          if ((*ob).sloc.items2>-1) {
            if (pieza!=p_coma) c_error(3,131);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sloc.items2; }
            if ((*ob).sloc.items3>-1) {
              if (pieza!=p_coma) c_error(3,131);
              lexico();
              exp00(0);
              if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sloc.items3; }
              (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.items2+1;
              (*_exp).tipo=eoper; (*_exp++).token=p_mul;
              (*_exp).tipo=eoper; (*_exp++).token=p_add;
            }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.items1+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          if (pieza!=p_corce) c_error(3,26); lexico();
          if ((*ob).tipo==tpslo) {
            (*_exp).tipo=econs; (*_exp++).valor=(*((*ob).pslo.ostruct)).sloc.len_item;
          } else {
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.len_item;
          }
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza==p_punto) {
          struct_pointer=p_pointer;
          if ((*ob).tipo==tpslo) member=(*ob).psgl.ostruct; else member=ob;
          lexico(); factor_struct();
          (*_exp).tipo=eoper; (*_exp++).token=struct_pointer;
        } else {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        } break;

      case tproc:
        ob=o; lexico(); if (pieza!=p_abrir) c_error(3,36); lexico();
        p=0; while (pieza!=p_cerrar) { // p­num_par
          p++;
          exp00(0);
          if (pieza!=p_cerrar) if (pieza!=p_coma) c_error(3,25);
          else { lexico(); if (pieza==p_cerrar) c_error(3,45); }
        }
        if (p!=(*ob).proc.num_par) c_error(1,46);
        (*_exp).tipo=ecall; (*_exp++).objeto=ob; lexico(); break;

      case tfunc:
        ob=o; lexico(); if (pieza!=p_abrir) c_error(3,36);

        if ((*ob).func.codigo==122) { // fopen("..."
          en_fopen=1;
          lexico();
          en_fopen=0;
        } else lexico();

        p=0; while (pieza!=p_cerrar) { // p­num_par

          if ((*ob).func.par[p]==1) { // Parámetro del tipo 1 (struct,campo)

            // RECIBE: nombre_struct,nombre_campo
            // GENERA: offset_struct,len_item,totalen,offset_campo,tipo_campo

            if (pieza!=p_id) c_error(0,152);
            if ((*o).member!=NULL) c_error(0,152);
            if ((*o).tipo==tsglo) {
              (*_exp).tipo=econs; (*_exp++).valor=(*o).sglo.offset;
            } else if ((*o).tipo==tsloc) {
              (*_exp).tipo=econs; (*_exp++).valor=(*o).sloc.offset;
              (*_exp).tipo=eoper; (*_exp++).token=p_punto;
            } else c_error(0,152);
            (*_exp).tipo=econs; (*_exp++).valor=(*o).sglo.len_item;
            (*_exp).tipo=econs; (*_exp++).valor=(*o).sglo.totalitems;
            obs=o; lexico(); if (pieza!=p_coma) c_error(0,46);
            member=obs; lexico(); member=NULL;
            if (pieza!=p_id) c_error(0,153);
            if ((*o).member!=obs) c_error(0,153);
            (*_exp).tipo=econs; (*_exp++).valor=(*o).vglo.offset;
            (*_exp).tipo=econs;
            if ((*o).tipo==tcglo || (*o).tipo==tcloc) (*_exp++).valor=1;
            else if ((*o).tipo==tpcgl || (*o).tipo==tpclo) (*_exp++).valor=2;
            else (*_exp++).valor=0;
            lexico();
          } else {                           // Parámetro del tipo 0 (genérico)
            exp00(0);
          } p++;

          if (pieza!=p_cerrar) if (pieza!=p_coma) c_error(3,25);
          else { lexico(); if (pieza==p_cerrar) c_error(3,45); }
        }
        if (p!=(*ob).func.num_par) c_error(1,46);
        (*_exp).tipo=efunc; (*_exp++).objeto=ob;
        if ((*ob).func.ret==1) tipo_factor=2;
        lexico(); break;

      case tfext:
        ob=o; lexico(); if (pieza!=p_abrir) c_error(3,36); lexico();
        p=0; while (pieza!=p_cerrar) { // p­num_par
          p++;
          exp00(0);
          if (pieza!=p_cerrar) if (pieza!=p_coma) c_error(3,25);
          else { lexico(); if (pieza==p_cerrar) c_error(3,45); }
        } if (p!=(*ob).fext.num_par) c_error(1,46);
        (*_exp).tipo=efext; (*_exp++).objeto=ob; lexico(); break;

      default:
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
        break;

    } break;

    default:
      if (error_27==27) c_error(1,27); else c_error(0,error_27);
      break;
  }

  if (pieza==p_inc || pieza==p_dec) {
    if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointer) {
      _exp--;
      (*_exp).tipo=eoper;
      if (pieza==p_inc) (*_exp++).token=p_suma;
        else (*_exp++).token=p_resta;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerchar) {
      _exp--;
      (*_exp).tipo=eoper;
      if (pieza==p_inc) (*_exp++).token=p_sumachar;
        else (*_exp++).token=p_restachar;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerbyte) {
      _exp--;
      (*_exp).tipo=eoper;
      if (pieza==p_inc) (*_exp++).token=p_sumachar;
        else (*_exp++).token=p_restachar;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_pointerword) {
      _exp--;
      (*_exp).tipo=eoper;
      if (pieza==p_inc) (*_exp++).token=p_sumaword;
        else (*_exp++).token=p_restaword;
    } else if ((*(_exp-1)).tipo==eoper && (*(_exp-1)).token==p_string) {
      _exp--; (*_exp).tipo=econs;
      if (pieza==p_inc) (*_exp++).valor=-1;
        else (*_exp++).valor=1;
      (*_exp).tipo=eoper;
      (*_exp++).token=p_strsub;
      (*_exp).tipo=eoper;
      (*_exp++).token=p_string;
    } else c_error(4,122);
    lexico();
  }
}

void factor_struct(void) {
  struct objeto * ob;

  if (pieza!=p_id) c_error(3,58);
  switch ((*o).tipo) {

    case tvglo:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).vglo.offset;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      break;

    case ttglo:
    case tpigl:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).tglo.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if (pieza==p_corab) {
        if ((*ob).tipo==tpigl) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        lexico();
        exp00(0);
        if ((*ob).tglo.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tglo.len1; }
        if ((*ob).tglo.len2>-1) {
          if (pieza!=p_coma) c_error(3,130);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tglo.len2; }
          if ((*ob).tglo.len3>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tglo.len3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).tglo.len2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).tglo.len1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
        (*_exp).tipo=eoper; (*_exp++).token=p_add;
      } break;

    case tbglo:
    case tpbgl:
    case tpcgl:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).bglo.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if ((*ob).tipo==tpbgl || (*ob).tipo==tpcgl) {
        if (pieza!=p_corab) break;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        if (comprueba_null) (*_exp++).tipo=enull;
      }
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if ((*ob).bglo.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bglo.len1; }
        if ((*ob).bglo.len2>-1) {
          if (pieza!=p_coma) c_error(3,130);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bglo.len2; }
          if ((*ob).bglo.len3>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bglo.len3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).bglo.len2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).bglo.len1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).tipo=econs; (*_exp++).valor=0;
      } struct_pointer=p_pointerbyte; break;

    case twglo:
    case tpwgl:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).wglo.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if ((*ob).tipo==tpwgl) {
        if (pieza!=p_corab) break;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        if (comprueba_null) (*_exp++).tipo=enull;
      }
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if ((*ob).wglo.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wglo.len1; }
        if ((*ob).wglo.len2>-1) {
          if (pieza!=p_coma) c_error(3,130);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wglo.len2; }
          if ((*ob).wglo.len3>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wglo.len3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).wglo.len2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).wglo.len1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).tipo=econs; (*_exp++).valor=0;
      } struct_pointer=p_pointerword; break;

    case tcglo:
      (*_exp).tipo=estring; (*_exp++).valor=(*o).cglo.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).cglo.totalen; }
        if (pieza!=p_corce) c_error(3,26);
        lexico();
        struct_pointer=p_pointerchar;
      } else {
        tipo_factor=2;
        if (tipo_expresion==1) {
          (*_exp).tipo=econs; (*_exp++).valor=0;
          struct_pointer=p_pointerchar;
        } else {
          if (tipo_expresion==-1) { (*_exp).tipo=eoper; (*_exp++).token=p_strlen; }
          struct_pointer=p_string;
        }
      } break;

    case tsglo:
    case tpsgl:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).sglo.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if ((*ob).tipo==tpsgl) {
        if (pieza!=p_corab && pieza!=p_punto) break;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        if (comprueba_null) (*_exp++).tipo=enull;
      }
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if ((*ob).sglo.items1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sglo.items1; }
        if ((*ob).sglo.items2>-1) {
          if (pieza!=p_coma) c_error(3,131);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sglo.items2; }
          if ((*ob).sglo.items3>-1) {
            if (pieza!=p_coma) c_error(3,131);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sglo.items3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.items2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.items1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
        if ((*ob).tipo==tpsgl) {
          (*_exp).tipo=econs; (*_exp++).valor=(*((*ob).psgl.ostruct)).sglo.len_item;
        } else {
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).sglo.len_item;
        }
        (*_exp).tipo=eoper; (*_exp++).token=p_mul;
        (*_exp).tipo=eoper; (*_exp++).token=p_add;
      }
      if (pieza==p_punto) {
        if ((*ob).tipo==tpsgl) member=(*ob).psgl.ostruct; else member=ob;
        lexico(); factor_struct();
      } break;

    case tvloc:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).vloc.offset;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico(); break;

    case ttloc:
    case tpilo:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).tloc.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if (pieza==p_corab) {
        if ((*ob).tipo==tpilo) {
          (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
          if (comprueba_null) (*_exp++).tipo=enull;
        }
        lexico();
        exp00(0);
        if ((*ob).tloc.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tloc.len1; }
        if ((*ob).tloc.len2>-1) {
          if (pieza!=p_coma) c_error(3,130);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tloc.len2; }
          if ((*ob).tloc.len3>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).tloc.len3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).tloc.len2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).tloc.len1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
        (*_exp).tipo=eoper; (*_exp++).token=p_add;
      } break;

    case tbloc:
    case tpblo:
    case tpclo:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).bloc.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if ((*ob).tipo==tpblo || (*ob).tipo==tpcgl) {
        if (pieza!=p_corab) break;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        if (comprueba_null) (*_exp++).tipo=enull;
      }
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if ((*ob).bloc.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bloc.len1; }
        if ((*ob).bloc.len2>-1) {
          if (pieza!=p_coma) c_error(3,130);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bloc.len2; }
          if ((*ob).bloc.len3>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).bloc.len3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).bloc.len2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).bloc.len1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).tipo=econs; (*_exp++).valor=0;
      } struct_pointer=p_pointerbyte; break;

    case twloc:
    case tpwlo:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).wloc.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if ((*ob).tipo==tpwlo) {
        if (pieza!=p_corab) break;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        if (comprueba_null) (*_exp++).tipo=enull;
      }
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if ((*ob).wloc.len1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wloc.len1; }
        if ((*ob).wloc.len2>-1) {
          if (pieza!=p_coma) c_error(3,130);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wloc.len2; }
          if ((*ob).wloc.len3>-1) {
            if (pieza!=p_coma) c_error(3,130);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).wloc.len3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).wloc.len2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).wloc.len1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).tipo=econs; (*_exp++).valor=0;
      } struct_pointer=p_pointerword; break;

    case tcloc:
      (*_exp).tipo=estring; (*_exp++).valor=(*o).cloc.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).cloc.totalen; }
        if (pieza!=p_corce) c_error(3,26);
        lexico();
        struct_pointer=p_pointerchar;
      } else {
        tipo_factor=2;
        if (tipo_expresion==1) {
          (*_exp).tipo=econs; (*_exp++).valor=0;
          struct_pointer=p_pointerchar;
        } else {
          if (tipo_expresion==-1) { (*_exp).tipo=eoper; (*_exp++).token=p_strlen; }
          struct_pointer=p_string;
        }
      } break;

    case tsloc:
    case tpslo:
      (*_exp).tipo=econs; (*_exp++).valor=(*o).sloc.offset; ob=o;
      (*_exp).tipo=eoper; (*_exp++).token=p_add;
      member=NULL; lexico();
      if ((*ob).tipo==tpslo) {
        if (pieza!=p_corab && pieza!=p_punto) break;
        (*_exp).tipo=eoper; (*_exp++).token=p_pointer;
        if (comprueba_null) (*_exp++).tipo=enull;
      }
      if (pieza==p_corab) {
        lexico();
        exp00(0);
        if ((*ob).sloc.items1>-1) if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sloc.items1; }
        if ((*ob).sloc.items2>-1) {
          if (pieza!=p_coma) c_error(3,131);
          lexico();
          exp00(0);
          if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sloc.items2; }
          if ((*ob).sloc.items3>-1) {
            if (pieza!=p_coma) c_error(3,131);
            lexico();
            exp00(0);
            if (comprueba_rango) { (*_exp).tipo=erango; (*_exp++).valor=(*ob).sloc.items3; }
            (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.items2+1;
            (*_exp).tipo=eoper; (*_exp++).token=p_mul;
            (*_exp).tipo=eoper; (*_exp++).token=p_add;
          }
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.items1+1;
          (*_exp).tipo=eoper; (*_exp++).token=p_mul;
          (*_exp).tipo=eoper; (*_exp++).token=p_add;
        }
        if (pieza!=p_corce) c_error(3,26); lexico();
        if ((*ob).tipo==tpslo) {
          (*_exp).tipo=econs; (*_exp++).valor=(*((*ob).pslo.ostruct)).sloc.len_item;
        } else {
          (*_exp).tipo=econs; (*_exp++).valor=(*ob).sloc.len_item;
        }
        (*_exp).tipo=eoper; (*_exp++).token=p_mul;
        (*_exp).tipo=eoper; (*_exp++).token=p_add;
      }
      if (pieza==p_punto) {
        if ((*ob).tipo==tpslo) member=(*ob).pslo.ostruct; else member=ob;
        lexico(); factor_struct();
      } break;

    default: c_error(3,58); break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Graba el fichero de la tabla de objetos
//═════════════════════════════════════════════════════════════════════════════

#ifdef listados

void l_objetos (void) {
  FILE * sta;
  int n,o,l;

  sta=fopen("div.tab","wb");

  fprintf(sta,"Objetos de %s - DIV Versión " version "\n\n",file_in);

  for (n=1;n<=8;n++) fprintf(sta,"[%u]: %u\n",n,mem[n]); fprintf(sta,"\n");

  for (n=0;n<num_obj;n++) {
    switch(obj[n].tipo) {
      case tnone: fprintf(sta,"%5u\ttnone: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        break;
      case tcons: fprintf(sta,"%5u\ttcons: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\tvalor=%u\n",obj[n].cons.valor);
        break;
      case tvglo: fprintf(sta,"%5u\ttvglo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].vglo.offset);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        else fprintf(sta,"\tvalor=%u\n",mem[o]);
        break;
      case ttglo: fprintf(sta,"%5u\tttglo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpbgl: case tpwgl: case tpcgl:
      case tpigl: fprintf(sta,"%5u\ttp?gl: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tsglo: fprintf(sta,"%5u\ttsglo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sglo.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sglo.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sglo.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sglo.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sglo.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sglo.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpsgl: fprintf(sta,"%5u\ttpsgl: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sglo.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sglo.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sglo.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sglo.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sglo.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sglo.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tvloc: fprintf(sta,"%5u\ttvloc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].vloc.offset);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        else fprintf(sta,"\tvalor=%u\n",loc[o]);
        break;
      case ttloc: fprintf(sta,"%5u\tttloc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tloc.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tloc.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tloc.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tloc.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tloc.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpblo: case tpwlo: case tpclo:
      case tpilo: fprintf(sta,"%5u\ttp?lo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tloc.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tloc.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tloc.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tloc.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tloc.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tsloc: fprintf(sta,"%5u\ttsloc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sloc.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sloc.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sloc.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sloc.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sloc.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sloc.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpslo: fprintf(sta,"%5u\ttpslo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sloc.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sloc.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sloc.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sloc.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sloc.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sloc.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tproc: fprintf(sta,"%5u\ttproc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\ttipo=%u\n",(memptrsize)(obj[n].proc.bloque));
        fprintf(sta,"\toffset=%u\n",obj[n].proc.offset);
        fprintf(sta,"\tnum_par=%u\n",obj[n].proc.num_par);
        break;
      case tfunc: fprintf(sta,"%5u\ttfunc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\tcodigo=%u\n",obj[n].func.codigo);
        fprintf(sta,"\tnum_par=%u\n",obj[n].func.num_par);
        break;
      case tfext: fprintf(sta,"%5u\ttfext: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\tcodigo=%u\n",obj[n].fext.codigo);
        fprintf(sta,"\tnum_par=%u\n",obj[n].fext.num_par);
        break;
      case tbglo: fprintf(sta,"%5u\ttbglo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tbloc: fprintf(sta,"%5u\ttbloc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case twglo: fprintf(sta,"%5u\ttwglo: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case twloc: fprintf(sta,"%5u\ttwloc: %s",n,obj[n].name);
        if (obj[n].usado) fprintf(sta," (*) usado\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
    }
    fprintf(sta,"\tbloque(%u), anterior(%u)\n",obj[n].bloque,obj[n].anterior);
    fprintf(sta,"\n");
  }

  fclose(sta);
}

#endif

//═════════════════════════════════════════════════════════════════════════════
//      Graba el fichero con información para debugging
//═════════════════════════════════════════════════════════════════════════════

// Ojo, falta grabar items1,items2 e items3 (o len1,len2,len3 para tablas)

void save_dbg(void) {
  FILE * sta;
  int n;

  struct {
    int tipo;
    int nombre;
    int bloque;
    int miembro;
    int v0,v1,v2,v3,v4,v5;
  } ob;

  sta=fopen("system/exec.dbg","wb");

  fwrite(&num_obj,4,1,sta);
  fwrite(&num_obj_predefinidos,4,1,sta);
  n=(memptrsize)&obj[0];
  fwrite(&n,4,1,sta);
  n=sizeof(struct objeto);
  fwrite(&n,4,1,sta);

  for (n=0;n<num_obj;n++) {
    ob.tipo=(memptrsize)obj[n].tipo;
    ob.nombre=(memptrsize)obj[n].name-(memptrsize)vnom;
    ob.bloque=(memptrsize)obj[n].bloque;
    ob.miembro=(memptrsize)obj[n].member;
    ob.v0=(memptrsize)obj[n].sglo.offset;
    ob.v1=(memptrsize)obj[n].sglo.len_item;
    ob.v2=(memptrsize)obj[n].sglo.totalitems;
    ob.v3=(memptrsize)obj[n].sglo.items1;
    ob.v4=(memptrsize)obj[n].sglo.items2;
    ob.v5=(memptrsize)obj[n].sglo.items3;
    if (obj[n].tipo==tpsgl || obj[n].tipo==tpslo) ob.v1=(ob.v1-(memptrsize)&obj[0])/sizeof(struct objeto);
    // OJO ! que no se pueden añadir objetos aquí (ver uso de &obj[0] y sizeof(struct objeto))
    fwrite(&ob,sizeof(ob),1,sta);
  }

  n=(memptrsize)ivnom.b-(memptrsize)vnom;
  fwrite(&n,4,1,sta);

  fwrite(vnom,1,n,sta);

  fclose(sta);
}

//═════════════════════════════════════════════════════════════════════════════
//    Graba los parámetros de ejecución para el intérprete
//═════════════════════════════════════════════════════════════════════════════

void save_exec_bin(void) {
  FILE * sta;

  sta=fopen("system/exec.bin","wb");

  fwrite(&Setupfile.vol_fx,1,1,sta);
  fwrite(&Setupfile.vol_cd,1,1,sta);
  fwrite(&Setupfile.vol_ma,1,1,sta);

  fclose(sta);
}

//═════════════════════════════════════════════════════════════════════════════
//      Graba el fichero ensamblador
//═════════════════════════════════════════════════════════════════════════════

#ifdef listados

void l_ensamblador (void) {

  FILE * sta;
  int i=mem[1];

  sta=fopen("div.eml","wt");

  fprintf(sta,"Código EML de %s - DIV Versión " version "\n",file_in);

  while (i<imem) { switch ((byte)mem[i]) {
    case lnop: fprintf(sta,"%5u\tnop",i); break;
    case lcar: fprintf(sta,"%5u\tcar %u",i,mem[i+1]); i++; break;
    case lasi: fprintf(sta,"%5u\tasi",i); break;
    case lori: fprintf(sta,"%5u\tori",i); break;
    case lxor: fprintf(sta,"%5u\txor",i); break;
    case land: fprintf(sta,"%5u\tand",i); break;
    case ligu: fprintf(sta,"%5u\tigu",i); break;
    case ldis: fprintf(sta,"%5u\tdis",i); break;
    case lmay: fprintf(sta,"%5u\tmay",i); break;
    case lmen: fprintf(sta,"%5u\tmen",i); break;
    case lmei: fprintf(sta,"%5u\tmei",i); break;
    case lmai: fprintf(sta,"%5u\tmai",i); break;
    case ladd: fprintf(sta,"%5u\tadd",i); break;
    case lsub: fprintf(sta,"%5u\tsub",i); break;
    case lmul: fprintf(sta,"%5u\tmul",i); break;
    case ldiv: fprintf(sta,"%5u\tdiv",i); break;
    case lmod: fprintf(sta,"%5u\tmod",i); break;
    case lneg: fprintf(sta,"%5u\tneg",i); break;
    case lptr: fprintf(sta,"%5u\tptr",i); break;
    case lnot: fprintf(sta,"%5u\tnot",i); break;
    case laid: fprintf(sta,"%5u\taid",i); break;
    case lcid: fprintf(sta,"%5u\tcid",i); break;
    case lrng: fprintf(sta,"%5u\trng %u",i,mem[i+1]); i++; break;
    case ljmp: fprintf(sta,"%5u\tjmp %u",i,mem[i+1]); i++; break;
    case ljpf: fprintf(sta,"%5u\tjpf %u",i,mem[i+1]); i++; break;
    case lfun: fprintf(sta,"%5u\tfun %u",i,mem[i+1]); i++; break;
    case lcal: fprintf(sta,"%5u\tcal %u",i,mem[i+1]); i++; break;
    case lret: fprintf(sta,"%5u\tret",i); break;
    case lasp: fprintf(sta,"%5u\tasp",i); break;
    case lfrm: fprintf(sta,"%5u\tfrm",i); break;
    case lcbp: fprintf(sta,"%5u\tcbp %u",i,mem[i+1]); i++; break;
    case lcpa: fprintf(sta,"%5u\tcpa",i); break;
    case ltyp: fprintf(sta,"\n%5u\ttyp %u",i,mem[i+1]); i++; break;
    case lpri: fprintf(sta,"%5u\tpri %u",i,mem[i+1]); i++; break;
    case lcse: fprintf(sta,"%5u\tcse %u",i,mem[i+1]); i++; break;
    case lcsr: fprintf(sta,"%5u\tcsr %u",i,mem[i+1]); i++; break;
    case lshr: fprintf(sta,"%5u\tshr",i); break;
    case lshl: fprintf(sta,"%5u\tshl",i); break;
    case lipt: fprintf(sta,"%5u\tipt",i); break;
    case lpti: fprintf(sta,"%5u\tpti",i); break;
    case ldpt: fprintf(sta,"%5u\tdpt",i); break;
    case lptd: fprintf(sta,"%5u\tptd",i); break;
    case lada: fprintf(sta,"%5u\tada",i); break;
    case lsua: fprintf(sta,"%5u\tsua",i); break;
    case lmua: fprintf(sta,"%5u\tmua",i); break;
    case ldia: fprintf(sta,"%5u\tdia",i); break;
    case lmoa: fprintf(sta,"%5u\tmoa",i); break;
    case lana: fprintf(sta,"%5u\tana",i); break;
    case lora: fprintf(sta,"%5u\tora",i); break;
    case lxoa: fprintf(sta,"%5u\txoa",i); break;
    case lsra: fprintf(sta,"%5u\tsra",i); break;
    case lsla: fprintf(sta,"%5u\tsla",i); break;
    case lpar: fprintf(sta,"%5u\tpar %u",i,mem[i+1]); i++; break;
    case lrtf: fprintf(sta,"%5u\trtf",i); break;
    case lclo: fprintf(sta,"%5u\tclo %u",i,mem[i+1]); i++; break;
    case lfrf: fprintf(sta,"%5u\tfrf",i); break;
    case limp: fprintf(sta,"%5u\timp %u",i,mem[i+1]); i++; break;
    case lext: fprintf(sta,"%5u\text %u",i,mem[i+1]); i++; break;
    case lchk: fprintf(sta,"%5u\tchk",i); break;
    case ldbg: fprintf(sta,"%5u\tdbg",i); break;

    case lcar2: fprintf(sta,"%5u\tcar2 %u %u",i,mem[i+1],mem[i+2]); i+=2; break;
    case lcar3: fprintf(sta,"%5u\tcar3 %u %u %u",i,mem[i+1],mem[i+2],mem[i+3]); i+=3; break;
    case lcar4: fprintf(sta,"%5u\tcar4 %u %u %u %u",i,mem[i+1],mem[i+2],mem[i+3],mem[i+4]); i+=4; break;
    case lasiasp: fprintf(sta,"%5u\tasiasp",i); break;
    case lcaraid: fprintf(sta,"%5u\tcaraid %u",i,mem[i+1]); i++; break;
    case lcarptr: fprintf(sta,"%5u\tcarptr %u",i,mem[i+1]); i++; break;
    case laidptr: fprintf(sta,"%5u\taidptr",i); break;
    case lcaraidptr: fprintf(sta,"%5u\tcaraidptr %u",i,mem[i+1]); i++; break;
    case lcaraidcpa: fprintf(sta,"%5u\tcaraidcpa %u",i,mem[i+1]); i++; break;
    case laddptr: fprintf(sta,"%5u\taddptr",i); break;
    case lfunasp: fprintf(sta,"%5u\tfunasp %u",i,mem[i+1]); i++; break;
    case lcaradd: fprintf(sta,"%5u\tcaradd %u",i,mem[i+1]); i++; break;
    case lcaraddptr: fprintf(sta,"%5u\tcaraddptr %u",i,mem[i+1]); i++; break;
    case lcarmul: fprintf(sta,"%5u\tcarmul %u",i,mem[i+1]); i++; break;
    case lcarmuladd: fprintf(sta,"%5u\tcarmuladd %u",i,mem[i+1]); i++; break;
    case lcarasiasp: fprintf(sta,"%5u\tcarasiasp %u",i,mem[i+1]); i++; break;
    case lcarsub: fprintf(sta,"%5u\tcarsub %u",i,mem[i+1]); i++; break;
    case lcardiv: fprintf(sta,"%5u\tcardiv %u",i,mem[i+1]); i++; break;

    case lptrwor: fprintf(sta,"%5u\tptrwor",i); break;
    case lasiwor: fprintf(sta,"%5u\tasiwor",i); break;
    case liptwor: fprintf(sta,"%5u\tiptwor",i); break;
    case lptiwor: fprintf(sta,"%5u\tptiwor",i); break;
    case ldptwor: fprintf(sta,"%5u\tdptwor",i); break;
    case lptdwor: fprintf(sta,"%5u\tptdwor",i); break;
    case ladawor: fprintf(sta,"%5u\tadawor",i); break;
    case lsuawor: fprintf(sta,"%5u\tsuawor",i); break;
    case lmuawor: fprintf(sta,"%5u\tmuawor",i); break;
    case ldiawor: fprintf(sta,"%5u\tdiawor",i); break;
    case lmoawor: fprintf(sta,"%5u\tmoawor",i); break;
    case lanawor: fprintf(sta,"%5u\tanawor",i); break;
    case lorawor: fprintf(sta,"%5u\torawor",i); break;
    case lxoawor: fprintf(sta,"%5u\txoawor",i); break;
    case lsrawor: fprintf(sta,"%5u\tsrawor",i); break;
    case lslawor: fprintf(sta,"%5u\tslawor",i); break;
    case lcpawor: fprintf(sta,"%5u\tcpawor",i); break;

    case lptrchr: fprintf(sta,"%5u\tptrchr",i); break;
    case lasichr: fprintf(sta,"%5u\tasichr",i); break;
    case liptchr: fprintf(sta,"%5u\tiptchr",i); break;
    case lptichr: fprintf(sta,"%5u\tptichr",i); break;
    case ldptchr: fprintf(sta,"%5u\tdptchr",i); break;
    case lptdchr: fprintf(sta,"%5u\tptdchr",i); break;
    case ladachr: fprintf(sta,"%5u\tadachr",i); break;
    case lsuachr: fprintf(sta,"%5u\tsuachr",i); break;
    case lmuachr: fprintf(sta,"%5u\tmuachr",i); break;
    case ldiachr: fprintf(sta,"%5u\tdiachr",i); break;
    case lmoachr: fprintf(sta,"%5u\tmoachr",i); break;
    case lanachr: fprintf(sta,"%5u\tanachr",i); break;
    case lorachr: fprintf(sta,"%5u\torachr",i); break;
    case lxoachr: fprintf(sta,"%5u\txoachr",i); break;
    case lsrachr: fprintf(sta,"%5u\tsrachr",i); break;
    case lslachr: fprintf(sta,"%5u\tslachr",i); break;
    case lcpachr: fprintf(sta,"%5u\tcpachr",i); break;

    case lstrcpy: fprintf(sta,"%5u\tstrcpy",i); break;
    case lstrfix: fprintf(sta,"%5u\tstrfix",i); break;
    case lstrcat: fprintf(sta,"%5u\tstrcat",i); break;
    case lstradd: fprintf(sta,"%5u\tstradd",i); break;
    case lstrdec: fprintf(sta,"%5u\tstrdec",i); break;
    case lstrsub: fprintf(sta,"%5u\tstrsub",i); break;
    case lstrlen: fprintf(sta,"%5u\tstrlen",i); break;
    case lstrigu: fprintf(sta,"%5u\tstrigu",i); break;
    case lstrdis: fprintf(sta,"%5u\tstrdis",i); break;
    case lstrmay: fprintf(sta,"%5u\tstrmay",i); break;
    case lstrmen: fprintf(sta,"%5u\tstrmen",i); break;
    case lstrmei: fprintf(sta,"%5u\tstrmei",i); break;
    case lstrmai: fprintf(sta,"%5u\tstrmai",i); break;
    case lcpastr: fprintf(sta,"%5u\tcpastr",i); break;

    default: fprintf(sta,"***"); break;
  } fprintf(sta,"\n"); i++; }
  fclose (sta);
}

#endif

//════════════════════════════════════════════════════════════════════════════
//  Funcion para localizar y abrir un fichero (pal,fpg,fnt,...)
//  Esta función debe seguir el mismo algoritmo en F.CPP y DIVC.CPP
//════════════════════════════════════════════════════════════════════════════
FILE *__fpopen (byte *file, char *mode) {
	
	char fprgpath[_MAX_PATH*2];
	FILE *f;

	strcpy(fprgpath,(char *)&tipo[8]);
	strcat(fprgpath,"/");
	strcat(fprgpath,full);
	
	if ((f=fopen(fprgpath,mode))) { // prgpath/file
		strcpy(full, fprgpath);
		printf("Found %s in prg dir [%s]\n",file, prgpath);
		return f;
	}

	return NULL;
	
}


FILE * fpopen ( byte * file) {
	return __fpopen(file,"rb");
}

FILE * open_multi(char *file, char *mode) {
  FILE *f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  char remote[255];

  char *ff = (char *)file;

  while (*ff!=0) {
    if(*ff =='\\') *ff='/';
      ff++;
  }

  strcpy(full,(char*)file); // full filename
//fprintf(stdout,"Trying to open %s\n",file);
  if ( f = fpopen(full))
    return f;

  if ((f=fopen(full,mode))) // "paz\fixero.est"
    return f;

    
  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) 
    return(NULL);

  _splitpath(full,drive,dir,fname,ext);

  if (strchr(ext,'.')==NULL) 
    strcpy(full,ext); 
  else 
    strcpy(full,strchr(ext,'.')+1);
    
    if (strlen(full) && file[0]!='/') 
    strcat(full,"/");

    strcat(full,(char*)file);

  if ((f=fopen(full,mode))) // "est\paz\fixero.est"
    return f;

  if ( f = fpopen(full))
    return f;

  strupr(full);

  if ((f=fopen(full,mode))) // "est\paz\fixero.est"
  return f;

  if ( f=fpopen(full) )
    return f;
    
  strcpy(full,fname);
  strcat(full,ext);

  if ((f=fopen(full,mode))) // "fixero.est"
    return f;

  if ( f = fpopen(full))
    return f;

  strupr(full);

  if ((f=fopen(full,mode))) // "fixero.est"
    return f;

  if ( f = fpopen(full))
    return f;

  strlwr(full);

  if ((f=fopen(full,mode))) // "fixero.est"
    return f;

  if ( f = fpopen(full))
    return f;

  if (strchr(ext,'.')==NULL)
    strcpy(full,ext); 
  else 
    strcpy(full,strchr(ext,'.')+1);
  
  if (strlen(full))
    strcat(full,"/");

  strcat(full,fname);
  strcat(full,ext);

  if ((f=fopen(full,mode))) // "est\fixero.est"
    return f;

  if ( f = fpopen(full))
    return f;

  strlwr(full);

  if ((f=fopen(full,mode))) // "est\fixero.est"
    return f;

  if ( f = fpopen(full))
    return f;


}

FILE * div_open_file(char * file) {
  FILE * f,*fe;
  char *ff = (char *)file;

#ifdef DEBUG
  printf("opening file: [%s]\n",file);
#endif

  if(strlen((const char *)file)<1)
  	return NULL;

  if(strlen((char *)file)==0) 
    return NULL;

  f=open_multi(file,"r");

  if(!f)
  	strcpy(full,"");
  return(f);
}

//═════════════════════════════════════════════════════════════════════════════
//      Dialogo de compilar
//═════════════════════════════════════════════════════════════════════════════

uint8_t e_msg[32];
int compilado=0;

void compilar1(void) {
  _show_items();
  wwrite(v.ptr,v.an/big2,v.al/big2,3,12,0,texto[206],c3);
  wwrite(v.ptr,v.an/big2,v.al/big2,6+text_len(texto[206]),12,0,ventana[v_ventana+1].titulo,c4);
}

extern uint8_t cerror[128];
void get_error(int n);

void compilar2(void) {
  if (compilado==0) {
    compilado=1; mouse_graf=3; numero_error=-1;
    comp();
    if (numero_error>=0) {
      //      strcpy(e_msg,texto[207]);
      //      itoa(numero_error,e_msg+strlen(e_msg),10);
//      if (numero_error>=10) {
        get_error(500+numero_error);
      //        strcat(e_msg,texto[208]);
      //        itoa(linea_error,e_msg+strlen(e_msg),10);
//      }
      //      strcat(e_msg,".");
      //      mensaje_compilacion(e_msg);
      mensaje_compilacion(cerror);
    } else {
      mensaje_compilacion(texto[202]);
      if (ejecutar_programa) fin_dialogo=1;
    }
    mouse_graf=1; v.volcar=1;
  }
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
  else if (v.active_item==1) { v_ayuda=1; fin_dialogo=1; }
}

void compilar0(void) {
  v.tipo=1; v.an=300; v.al=46;
  switch (ejecutar_programa) {
    case 0: v.titulo=texto[200]; break;
    case 1: v.titulo=texto[235]; break;
    case 2: v.titulo=texto[236]; break;
    case 3: v.titulo=texto[241]; break;
  }
  v.paint_handler=compilar1;
  v.click_handler=compilar2;
  _button(100,7,v.al-14,0);
  _button(125,v.an-8,v.al-14,2);
  compilado=0; v_ayuda=0;
}

void compilar_programa(void) {
  dialogo(compilar0);
  tecla(); scan_code=0; ascii=0; mouse_b=0;
}

//═════════════════════════════════════════════════════════════════════════════
//  Funciones de generación de código
//═════════════════════════════════════════════════════════════════════════════

// void g1(int op) { mem[imem++]=op; }
// void g2(int op, int pa) { mem[imem++]=op; mem[imem++]=pa; }

//═════════════════════════════════════════════════════════════════════════════
//  Optimización peephole de código intermedio EML
//═════════════════════════════════════════════════════════════════════════════

// *** OJO!!! *** quizá se pueda quitar "dir" y esa comprobación absurda ...

//struct {      // Peephole, "mirilla" de optimizacion
//  int dir;    // Dirección
//  int param;  // Indica el número de parametros de la instruccion
//  int op;     // Opcode
//} code[16];   // En code[15] debe quedar siempre la última instrucción generada

void g1(int op) {
  if (optimizar) gen(0,op,0); else mem[imem++]=op;
}

void g2(int op, int pa) {
  if (optimizar) gen(1,op,pa); else { mem[imem++]=op; mem[imem++]=pa; }
}

int optimizado;

void gen(int param, int op, int pa) {
//  int n;

  optimizado=0;

  switch(op) {
    case DIVBC_car:
      if (code[15].op==DIVBC_car) {
        code[15].op=mem[imem-2]=DIVBC_car2;
        code[15].param=2;
        mem[imem++]=pa; optimizado=1;
      } else if (code[15].op==DIVBC_car2) {
        code[15].op=mem[imem-3]=DIVBC_car3;
        code[15].param=3;
        mem[imem++]=pa; optimizado=1;
      } else if (code[15].op==DIVBC_car3) {
        code[15].op=mem[imem-4]=DIVBC_car4;
        code[15].param=4;
        mem[imem++]=pa; optimizado=1;
      } break;
    case DIVBC_asp:
      if (code[15].op==DIVBC_asi) {
        if (code[14].op==DIVBC_car) {
          remove_code(1);
          code[15].op=mem[imem-2]=DIVBC_carasiasp;
        } else {
          code[15].op=mem[imem-1]=DIVBC_asiasp;
          optimizado=1;
        }
      } else if (code[15].op==DIVBC_fun) {
        code[15].op=mem[imem-2]=DIVBC_funasp;
        optimizado=1;
      } break;
    case DIVBC_aid:
      if (code[15].op==DIVBC_car) {
        code[15].op=mem[imem-2]=DIVBC_caraid;
        optimizado=1;
      } break;
    case DIVBC_ptr:
      if (code[15].op==DIVBC_car) {
        code[15].op=mem[imem-2]=DIVBC_carptr;
        optimizado=1;
      } else if (code[15].op==DIVBC_caradd) {
        code[15].op=mem[imem-2]=DIVBC_caraddptr;
        optimizado=1;
      } else if (code[15].op==DIVBC_aid) {
        code[15].op=mem[imem-1]=DIVBC_aidptr;
        optimizado=1;
      } else if (code[15].op==DIVBC_add) {
        code[15].op=mem[imem-1]=DIVBC_addptr;
        optimizado=1;
      } else if (code[15].op==DIVBC_caraid) {
        code[15].op=mem[imem-2]=DIVBC_caraidptr;
        optimizado=1;
      } break;
    case DIVBC_cpa:
      if (code[15].op==DIVBC_caraid) {
        code[15].op=mem[imem-2]=DIVBC_caraidcpa;
        optimizado=1;
      } break;
    case DIVBC_add:
      if (code[15].op==DIVBC_car2){
        code[15].op=mem[imem-3]=DIVBC_car;
        code[15].param=1;
        mem[imem-2]+=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car3){
        code[15].op=mem[imem-4]=DIVBC_car2;
        code[15].param=2;
        mem[imem-2]+=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car4){
        code[15].op=mem[imem-5]=DIVBC_car3;
        code[15].param=3;
        mem[imem-2]+=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car){
        if (mem[imem-1]==0) remove_code(1); else {
          code[15].op=mem[imem-2]=DIVBC_caradd;
          optimizado=1;
        }
      } else if (code[15].op==DIVBC_carmul){
        code[15].op=mem[imem-2]=DIVBC_carmuladd;
        optimizado=1;
      } break;
    case DIVBC_mul:
      if (code[15].op==DIVBC_car2){
        code[15].op=mem[imem-3]=DIVBC_car;
        code[15].param=1;
        mem[imem-2]*=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car3){
        code[15].op=mem[imem-4]=DIVBC_car2;
        code[15].param=2;
        mem[imem-2]*=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car4){
        code[15].op=mem[imem-5]=DIVBC_car3;
        code[15].param=3;
        mem[imem-2]*=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car){
        if (mem[imem-1]==1) remove_code(1); else {
          code[15].op=mem[imem-2]=DIVBC_carmul;
          optimizado=1;
        }
      } break;
    case DIVBC_sub:
      if (code[15].op==DIVBC_car2){
        code[15].op=mem[imem-3]=DIVBC_car;
        code[15].param=1;
        mem[imem-2]-=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car3){
        code[15].op=mem[imem-4]=DIVBC_car2;
        code[15].param=2;
        mem[imem-2]-=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car4){
        code[15].op=mem[imem-5]=DIVBC_car3;
        code[15].param=3;
        mem[imem-2]-=mem[imem-1];
        imem--; optimizado=1;
      } else if (code[15].op==DIVBC_car){
        if (mem[imem-1]==0) remove_code(1); else {
          code[15].op=mem[imem-2]=DIVBC_carsub;
          optimizado=1;
        }
      } break;
    case DIVBC_div:
      if (code[15].op==DIVBC_car2){
        if (mem[imem-1]!=0) {
          code[15].op=mem[imem-3]=DIVBC_car;
          code[15].param=1;
          mem[imem-2]/=mem[imem-1];
          imem--; optimizado=1;
        }
      } else if (code[15].op==DIVBC_car3){
        if (mem[imem-1]!=0) {
          code[15].op=mem[imem-4]=DIVBC_car2;
          code[15].param=2;
          mem[imem-2]/=mem[imem-1];
          imem--; optimizado=1;
        }
      } else if (code[15].op==DIVBC_car4){
        if (mem[imem-1]!=0) {
          code[15].op=mem[imem-5]=DIVBC_car3;
          code[15].param=3;
          mem[imem-2]/=mem[imem-1];
          imem--; optimizado=1;
        }
      } else if (code[15].op==DIVBC_car){
        if (mem[imem-1]==1) remove_code(1);
        else if (mem[imem-1]!=0) {
          code[15].op=mem[imem-2]=DIVBC_cardiv; // Un cardiv nunca será "cardiv 0"
          optimizado=1;
        }
      } break;
    case DIVBC_neg:
      if (code[15].op==DIVBC_car || code[15].op==DIVBC_car2 || code[15].op==DIVBC_car3 || code[15].op==DIVBC_car4) {
        mem[imem-1]=-mem[imem-1]; optimizado=1;
      }
      break;
    case DIVBC_not:
      if (code[15].op==DIVBC_car || code[15].op==DIVBC_car2 || code[15].op==DIVBC_car3 || code[15].op==DIVBC_car4) {
        mem[imem-1]=mem[imem-1]^-1; optimizado=1;
      }
      break;
  }

  if (!optimizado) {
    if (imem-code[15].dir>2) delete_code();
    add_code(imem,param,op);
    if (param) { mem[imem++]=op; mem[imem++]=pa; } else mem[imem++]=op;
  }

}

void remove_code(int i) {
  int n;
  optimizado=1;
  while (i--) {
    imem-=code[15].param+1;
    for (n=15;n>0;n--) {
      code[n].dir=code[n-1].dir;
      code[n].param=code[n-1].param;
      code[n].op=code[n-1].op;
    } code[0].dir=0; code[0].param=0; code[0].op=0;
  }
}

void delete_code(void) {
  int n;
  for (n=0;n<16;n++) {
    code[n].dir=0; code[n].param=0; code[n].op=0;
  }
}

void add_code(int dir, int param, int op) {
  int n;
  for (n=0;n<15;n++) {
    code[n].dir=code[n+1].dir;
    code[n].param=code[n+1].param;
    code[n].op=code[n+1].op;
  } code[15].dir=dir; code[15].param=param; code[15].op=op;
}

//═════════════════════════════════════════════════════════════════════════════
//      Precompilación
//═════════════════════════════════════════════════════════════════════════════

void plexico(void) {
  byte ** ptr, * _ivnom, h, * _source=source;
  struct lex_ele * e;

  if (!coment) {old_linea=linea; old_ierror=ierror; old_ierror_end=ierror_end;}

  lex_scan: ierror=_source;
  switch ((memptrsize)lex_case[*_source]) { // Puntero a un lex_ele o l_???

    case l_err:
      if (coment) { pieza=p_rem; _source++; } else c_error(0,10); break;

    case l_cr :
      linea++; if ((*++_source)==lf) { _source++; ultima_linea=_source; goto lex_scan; }
      pieza=p_ultima; break; // eof

    case l_id :
      if (coment) { pieza=p_rem; _source++; break; }
      _ivnom=ivnom.b; *ivnom.p++=0; *ivnom.p++=0; h=0;
      while (*ivnom.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*ivnom.b++);
      ivnom.b++; _source--; if (ivnom.b-vnom>max_obj*long_med_id) c_error(0,100);
      ptr=&vhash[h];
      while (*ptr && strcmp((char *)(ptr+2),(char *)(_ivnom+ptr8))) ptr=(byte **)*ptr;
      if (!strcmp((char *)(ptr+2),(char *)_ivnom+ptr8)) { // id found
        ivnom.b=_ivnom; // lo saca de vnom
        pieza=(memptrsize)*(ptr+1);
        if (pieza<256 && pieza>=0) { // palabra reservada (token)
          if (pieza==p_rem) { while (*_source!=cr) _source++; goto lex_scan; }
        } else { // objeto (id anterior)
          pieza=p_id;
        }
      } else {
        ivnom.b=_ivnom; // lo saca de vnom
        pieza=p_id; // id nuevo
      } break;

    case l_spc:
      while ((*++_source)==' '); goto lex_scan;

    case l_lit:
      if (coment) { pieza=p_rem; _source++; break; }
      pieza=p_lit; h=*_source; _ivnom=ivnom.b; // Literal entre dos h
      do { if (*++_source==cr) c_error(0,11);
           if (*_source==h)
             if (*(_source+1)==h) *_ivnom=*++_source; else *_ivnom=0;
           else *_ivnom=*_source;
      } while (*_ivnom++); _source++;
      longitud_textos+=(strlen((char *)ivnom.b)+ptr4)/4;
      ivnom.b=_ivnom; // lo saca de vnom
      break;

    case l_num:
      if (coment) { pieza=p_rem; _source++; break; }
      pieza=p_num; pieza_num=0;
      if (*_source=='0' && lower[*(_source+1)]=='x') {
        _source+=2;
        while ((memptrsize)lex_case[*_source]==l_num ||
               (lower[*_source]>='a' && lower[*_source]<='f')) {
          if ((memptrsize)lex_case[*_source]==l_num)
            pieza_num=pieza_num*16+*_source++-0x30;
          else pieza_num=pieza_num*16+lower[*_source++]-'a'+10;
        }
      } else do {
        pieza_num=pieza_num*10+*_source++-0x30;
      } while ((memptrsize)lex_case[*_source]==l_num);
      break;

    default: // puntero a un lex_ele
      e=lex_case[*_source++]; _ivnom=_source; pieza=(*e).token;
      while (e=(*e).siguiente) {
        while (*_source!=(*e).caracter && (*e).alternativa) e=(*e).alternativa;
        if (*_source++==(*e).caracter && (*e).token) {
          pieza=(*e).token; _ivnom=_source;
        }
      } _source=_ivnom;

      if (pieza==p_rem && !coment) {
        while (*_source!=cr) _source++;
        goto lex_scan;
      }

      if (pieza==p_ini_rem) {
        coment++;
        do {
           source=_source; plexico(); _source=source;
        } while (pieza!=p_end_rem);
        coment--;
        goto lex_scan;
      }

      if (pieza==p_ultima) if (coment) c_error(0,55); else c_error(0,12);
      break;

  } source=_source; ierror_end=_source-1;
}

void psintactico(void) {
  byte * _ivnom=ivnom.b;

  longitud_textos=0;

  do {
    plexico();
  } while (pieza!=p_ultima);

  ivnom.b=_ivnom;
  source=source_ptr;
  _source=source;
  coment=0;

}
