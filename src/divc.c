
// Strings, new: p_string, cglo, cloc, p_*char, l*chr, ...
// 00 - intermediate code optimization
// 00 - tables and structures of up to 3 dimensions
// 01 - add global strings
// 02 - test in PRG, and trigger all the new errors
// 03 - add a new instruction set to the interpreter
// 04 - Prepare the debugger for string support
// 05 - add local and private strings
// 06 - runtime range checking (strings with prefix 0xdad00000)
// 07 - long_header=9, instead of 36
// 08 - global member strings
// 09 - local member strings
// 10 - structure initialization taking member strings into account
// 11 - STRING p,q; defines 2 strings (instead of a string and an int)
// 12 - Be able to initialize the records in structures globally (struct pepe[9] x=10; end)
// 13 - compiler_options statement (_max_process,_extended_conditions,_simple_conditions,_case_sensitive,_ignore_errors,_free_sintax)
// 14 - Auto "\0" and ZERO2SPACE when putting a char into a string (strfix)
// 15 - New type system (int/char/string) in expressions and conditions (expression_type)
// 16 - Auto conversion of "A" to char("A") in assignments to char (literal/literal constant/string -> char)
// 17 - Overloading of = += + in string operations (strcpy,strcat,stradd)
// 18 - Add and remove characters with string++/--, string-=n, string-n (strdec,strsub,strlen)
// 19 - Be able to define literals over several source lines (several consecutive ones become one)
// 20 - Fixed bug in the listing colorizer (on reaching an eof with an unclosed /*)
// 21 - Comparisons between strings (strigu/dis/may/men/mai/mei)
// 22 - New system with 4 strings "in the air" to allow more string operations (+/-)
// 23 - Discarded the idea of converting the offsets to bytes (it was meant to be done for &string[n],
//      bytes and words: but in [ptr+n] there is no way to know what to multiply n by)
// 24 - Fixed some bugs related to remote accesses
// 25 - Merged the mode7 reserved words with the mode 8 ones
// 26 - Added the options _no_check (do not check ranges or ids) and _no_strfix
// 27 - Ascii functions: char,strcpy,strcat,strlen,strcmp,strchr,strstr,strset,upper,lower and strdel
// 28 - Enabled a 'patch' to tell the compiler that some functions return a string
// 29 - Be able to receive a parameter in a global or local string/char (cpachr, cpastr)
// 30 - Enabled a multi-par/pri system and fixed some issues with process calls
// 31 - Enabled the idea of receiving a parameter in a private string, even specifying its length (e.g., proc(string s[2]))
// 32 - Allow redeclaring the string parameter inside the private section (with the same length)
// 33 - "function" type processes (they do not return to the caller until they have finished) (FCount,CallerId)
// 34 - Fixed the issue of function returns (to the caller of the last process)
// 35 - Be able to STEP over functions (a go here! to the next statement is done)
// 36 - Add global bytes and a new, once again, data initialization system
//      (see p_byte, bglo and p_pointerbyte)
// 37 - Prepare the debugger for byte and word support
// 38 - Last changes in the Debugger (in View data), and conversion to Spanish.
// 39 - Add local and private bytes (bloc)
// 40 - Bytes as structure members (global/local and private)
// 41 - Structure initialization taking member bytes into account (frm,ifrm_max)
// 42 - Added interpreter instructions for handling words (???wor)
// 43 - Words just like bytes (wglo wloc *word ???wor)
// 44 - Be able to receive a byte or word as a parameter (and redeclare it as private)
// 45 - Colorizer bug (below process ... put ?????????)
// 46 - Make functions also appear in the process lists (F5) (editor and debugger)
// 47 - Bug where sprites (completely) disappear in modes 8
// 48 - Move the IMPORTs to the start of the program and emit a suitable error for old IMPORTs (and adapt help)
// 49 - Removed the listing formatter (all left in divfrm.cpp in case it is wanted in the future, separately from divc.cpp)
// 50 - Let functions return a value with return after having done FRAME (multi-stack system)
// 51 - Added the optional INT type for conventional integer data in DIV 1
// 52 - Declaration of pointers to INT, global/local/private and structure members (tpigl,tpilo) of up to 3 dimensions
// 53 - Use of INT pointers, code generation for them (and tests)
// 54 - Debugger support for global and local INT pointers (p, p[0], p[1], ...)
// 55 - Pointers to int as parameters and redeclaration in the PRIVATE section
// 56 - Forbid the redeclaration of private variables (only if once as a parameter and again as private)
// 57 - Pointers to byte/word, both global and local and private, member or not, declaration and code
// 58 - Debugger support for byte and word type pointers (global and local)
// 59 - Removed the possibility of using * to define a pointer (and the debugger uses POINTER)
//      (this is important so as not to encourage using *pointer afterwards ...)
// 60 - Test int/word/byte pointers (glo/loc/pri/mem/par)
// 61 - Implemented "string pointer" exactly like "byte pointer" (only the debug display varies)
// 62 - Pointers to structures STRUCT POINTER STRUCTNAME such; (defaults to 0)
// 63 - Solved the problem of accessing the members of the original structure
// 64 - Debugger support for pointers to structures (!!! harder than I thought)
// 65 - "cap()" function for addresses in DEBUG to avoid page faults as much as possible
// 66 - Solved "a zillion" problems with the damn pointers, the debugger and the whole lot ...
// 67 - Make int/byte/word/string pointer p1,p2; define two pointers
// 68 - screen_copy function that can copy a screen region to a region of a graphic
// 69 - Argc/argv just like in C, with the program parameters
// 70 - New function loading system, defining the return and the type of the parameters
//      (and removed the exceptions previously made for the string functions)
// 71 - Sort and unsort function (structure_name,field,mode)
// 72 - Minimized the circular keyboard buffer when running games
// 73 - Check read/write accesses to "NULL pointed data"
// 74 - Options _no_id_check _no_range_check _no_null_check (included in _no_check)
// 75 - Compilation option _no_optimize (to be able to disable optimization)

//    fopen, fclose, fread, fwrite, fseek, ftell, filelen (see flushall, fcloseall)
//    findfisrt, findnext, fullpath, splitpath, set/getcwd, set/getdrive, fprintf
//    remove (danidel), chdir, mkdir

//    - explain the new errors (strings, bytes, words and pointers) in help
//    - malloc/free functions
//    - write book

// A - Be able to include {constants/globals/locals/processes}
// C - Multi-assignments, using hidden private variables (x=3,45,y*10,2..4,-8;)
// D - think of some "macro"-like trick that is easy to implement and looks good
// E - Finish the listing auto-formatter
// F - Generate an informative NFO of the compiled program (TAB+LST+EML)

// ─────────────────────────────────────────────────────────────────────────────

// SOLUTION 1: BYTE/WORD POINTERS LIMITED TO BYTE/WORD TABLES

// NOTE! It will not be possible to do ptrbyte++ (since it will increment 4 bytes)
//      nor ptrword++, which will increment 2 words

// (*)  When making *ptrbyte be ptrbyte[0] (I guess by adding
//      a new type, maybe p_pointerb?), an error should be emitted when doing
//      ++, --, +=, etc. on a p_pointerb/w ...
//    - Problem, how to forbid ptrbyte=ptrbyte+1;?
//    - The operations could also be allowed, even if the results were wrong

//    - Even so, how do you explain to people that ptrint++ works, but ptrbyte++ does not?

//    - Advantages: it is (almost) done already, it has no incompatibilities with
//      DIV1, there is little time available ..., even so, it allows the use
//      of dynamic memory ... and it can always be moved to SOLUTION 3
//      later

//    - although maybe this should not be called pointer ... but something like
//      "dynamic tables" (because they can be malloc'ed)

//    - Well, it would also be useful to pass tables and structures as parameters.
//    - Maybe using "p_mul" (*) should be forbidden to tell them apart from
//      C pointers.

//    - The question is: Is operating (additions and so on) with ptrbyte/word forbidden? -> NO

//    - Be able to use *ptrbyte as ptrbyte[0]? -> NO, since that would lead
//      to accessing bytes with *ptrbyte, when it is not possible

// SOLUTION 2: BYTE/WORD POINTERS RELATIVE TO &MEM[0]

//    - NOTE, maybe byte and word pointers should be different
//    - (pointerb/w being a byte/word offset relative to &memb[0])
//    - New design for: tpwgl tpwlo tpbgl tpblo? (always keeping them multiplied *2 or *4)
//    - a) on initialization the constant must be multiplied *2 or *4 (ptrbyte=&bytetable)
//    - b) how the hell do you do ptrbyte=&whatever ? (as parameter passing, assignment, etc...)
//    - c) and what about ptrword=ptrbyte?
//    - d) what's more, what happens if a ptrbyte is sent as a parameter and a ptrint is received?

//    - It could only be done so that all read accesses to ptrbyte were divided by 4
//      and write accesses were multiplied by four, and even so:
//      ptrbyte1=ptrbyte2 would fail !!! (or passing and receiving a ptrbyte parameter)
//      [ptrbyte+n] ... and *ptrbyte?

//    - It is nonsense, since ptrbyte=ptrbyte+1 would still add 4 !!!

//    - Advantages: Well, I do not know whether in this case virtue will lie in the
//      middle ground ...

// SOLUTION 3: CONVERT ALL OFFSETS TO BYTES

//    - There should be some way to have all offsets multiplied by 4 (in bytes)

//    - Problems: ["ptr"+n] (what to multiply n by), besides how to resolve
//      [ptrint+ptrbyte+2*x...], interpreter access to all the data in mem
//      mem[id+_Graph], the ids themselves (multiply them or not? -> better not, and have p_point multiply them,
//      since, besides, they must be odd), old programs that made use of pointers
//      (p=&kk; *p..., p++, *p... for that a compilation error should be emitted when using *p or [p],
//      forcing them to redeclare p as a ptrint), that ptrint++ be ptrint+=4 and ptrword++ be ptrword+=2,

//    - The sizeof() would also change, now in bytes, and the file
//      operations, increasing the incompatibility with DIV1 (since people could
//      have used a save("pepe.dat",&table,10) ...

//    - In short: the big problems are the interpreter (above all, since
//      that "#define memo(x) *(int*)(memb+x)" thing was a pain ),
//      the [ptr???+???] one (and ptrint++), and the incompatibilities with DIV1

//    - Advantages: Free access to any data type, the simplification
//      of strings, being able to make "byte a" really take only one byte, ...
//      which is the right way to do it, and if it is not done now maybe
//      it can never be done.

// SOLUTION 4: CONVERT THE OFFSETS TO FLOAT

//    - I know at first glance it seems nonsense, but wouldn't the ideal be
//      to be able to have numbers .25, .50 and .75 as byte offsets?

//    - The cool thing would be that ptrint++ would still add 1.0

//    - and for byte accesses .25 would be added, the problem is that everything
//      would fill up with calls to _CHP and it would be very slow

//    - Besides, how would offset or sizeof() return a float if the
//      language does not handle them?

// NOTE! IN THIS VERSION I HAVE NOT YET DONE ANY CRAZY THING
//      RELATED TO MULTIPLYING THE OFFSETS BY 4 !!!!!!!!!!!
//      (once I tried it .. and I ended up undoing everything)

// ─────────────────────────────────────────────────────────────────────────────

// Intermediate code optimization

// 1st step: Create a mnemonic generation function, instead of a
//           free access to mem[], to locate the optimization problem

// byte code cannot be generated, but the "byte" system can be used
// as compression of the EXEs, decompressing in the interpreter on load

// ─────────────────────────────────────────────────────────────────────────────

// Multidimensional tables and structures
// review errors: 35, ...
// NOTE in the help with the tables and structures of ONLY one dimension

//═════════════════════════════════════════════════════════════════════════════
//      DIV - Internal Compiler
//═════════════════════════════════════════════════════════════════════════════

//#define listings // To generate the object and EML listings (LST/TAB/EML)

#include "global.h"
#include "divdll.h"

#ifdef ZLIB
#include <zlib.h>
#endif

//extern char ExeGen[_MAX_PATH];
void l_objects (void);
void l_assembler (void);

void delete_code(void);
void preload_obj (void);
void psyntactic(void);
void test_buffer(int * * buffer,int * maximum,int n);
void save_dbg(void);
void save_exec_bin(void);
void tloc_init(int type);
void g2(int op, int pa);
void analyze_private(void);
void g1(int op);
void expression_cpa(void);
void tglo_init2(int type);
void condition(void);
void const_expr00(int exp_type);
void generate_expression(void);
void const_expr0(void);
void const_expr1(void);
void const_expr2(void);
void exp00(int exp_type);
void exp0(void);
void exp1(void);
void div_exp2(void);
void exp3(void);
void exp4(void);
void exp5(void);
void exp6(void);
void unary(void);
void factor(void);
void factor_struct(void);
void gen(int param, int op, int pa);
void remove_code(int i);
void add_code(int dir, int param, int op);





//═════════════════════════════════════════════════════════════════════════════
//      Constants
//═════════════════════════════════════════════════════════════════════════════

#define version "0c88b" //Compiler version

#ifndef SHARE
#define max_obj 8192    //Compiler object maximum limit compiler
#define long_med_id 16  //Average length of the identifiers (+4+4+1)
#else
#define max_obj 700     //The nasty one has 626 ...
#define long_med_id 20  //Average length of the identifiers (+4+4+1)
#endif

#define max_nodes 128   //Maximum number of lexer nodes for symbols
#define max_exp 512     //Maximum number of elements in an expression
#define long_header 9   //Length of the header at the start of the programs
#define long_stack 2048  //Length of the execution stack

#define default_buffer    (16384*8)
#define security_distance (4096*8)
#define buffer_grow       (16384*8)

#define cr 13           //Carriage return
#define lf 10           //Line feed
#define tab 9           //Tab

//═════════════════════════════════════════════════════════════════════════════

#define l_err 0         //Values of lex_case, if they are not pointers to lex_sym
#define l_cr  1         //End of line (l_err Unexpected character)
#define l_id  2         //Identifier or reserved word
#define l_spc 3         //Spaces and tabs
#define l_lit 4         //Literal
#define l_num 5         //Numeric constant

//═════════════════════════════════════════════════════════════════════════════
//      Syntactic values or tokens (piece)
//═════════════════════════════════════════════════════════════════════════════

#define p_last        0x00 //End of file <EOF>

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

#define p_semicolon       0x30
#define p_comma          0x31
#define p_lbracket         0x32
#define p_rbracket         0x33

#define p_assign          0x38

#define p_dup           0x3C

#define p_lparen         0x40
#define p_rparen        0x41

#define p_plus          0x44
#define p_minus         0x45

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

#define p_eq           0x5b
#define p_neq           0x5c

#define p_gt           0x5d
#define p_lt           0x5e
#define p_le           0x5f
#define p_ge           0x60

#define p_sizeof        0x66

#define p_type          0x67

#define p_whoami        0x68

#define p_point         0x69

#define p_range         0x6a

#define p_add_assign      0x6b
#define p_sub_assign      0x6c
#define p_mul_assign      0x6d
#define p_div_assign      0x6e
#define p_mod_assign      0x6f
#define p_and_assign      0x70
#define p_or_assign       0x71
#define p_xor_assign      0x72
#define p_shr_assign      0x73
#define p_shl_assign      0x74

#define p_start_rem       0x7d //Comment start
#define p_end_rem       0x7e //Comment end
#define p_rem           0x7f //Single-line comment

#define p_streq        0xc0
#define p_strneq        0xc1
#define p_strgt        0xc2
#define p_strlt        0xc3
#define p_strle        0xc4
#define p_strge        0xc5

#define p_pointerword   0xd0
#define p_plusword      0xd1
#define p_minusword     0xd2
#define p_incword       0xd3
#define p_decword       0xd4
#define p_assignword      0xd5
#define p_add_assignword  0xd6
#define p_sub_assignword  0xd7
#define p_mul_assignword  0xd8
#define p_div_assignword  0xd9
#define p_mod_assignword  0xda
#define p_and_assignword  0xdb
#define p_or_assignword   0xdc
#define p_xor_assignword  0xdd
#define p_shr_assignword  0xde
#define p_shl_assignword  0xdf

#define p_pointerchar   0xe0
#define p_pluschar      0xe1
#define p_minuschar     0xe2
#define p_incchar       0xe3
#define p_decchar       0xe4
#define p_assignchar      0xe5
#define p_add_assignchar  0xe6
#define p_sub_assignchar  0xe7
#define p_mul_assignchar  0xe8
#define p_div_assignchar  0xe9
#define p_mod_assignchar  0xea
#define p_and_assignchar  0xeb
#define p_or_assignchar   0xec
#define p_xor_assignchar  0xed
#define p_shr_assignchar  0xee
#define p_shl_assignchar  0xef

#define p_strcpy        0xf0
#define p_strfix        0xf1
#define p_strcat        0xf2
#define p_stradd        0xf3
#define p_strdec        0xf4
#define p_strsub        0xf5
#define p_strlen        0xf6

#define p_pointerbyte   0xf7

#define p_lit           0xfc //Pointer to the literal (txt) in piece_num
#define p_id            0xfd //or it is a ptr to vnames (to a ptr to the object)
#define p_num           0xfe //Number in piece_num

//═════════════════════════════════════════════════════════════════════════════

#define tnone   0       // Object types in obj[]
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
#define tfext   12      // Function from an external library

#define tbglo   13      // Byte global
#define twglo   14
#define tbloc   15
#define twloc   16

#define tpigl   17      // Pointer to global ints (an addressable ttglo)
#define tpilo   18      // Pointer to local ints (ditto)

#define tpwgl   19      // Pointers to word
#define tpwlo   20
#define tpbgl   21      // Pointers to byte
#define tpblo   22

#define tpcgl   23      // Pointers to string
#define tpclo   24
#define tpsgl   25      // Pointers to struct
#define tpslo   26

//═════════════════════════════════════════════════════════════════════════════

#define econs   0       // Constant           // Types of elements in tabexp[]
#define eoper   1       // Operator
#define erange  2       // Range check
#define ewhoami 3       // Process identifier
#define ecall   4       // Creation of a process
#define efunc   5       // Call to an internal function
#define efext   6       // Call to an external function
#define echeck  7       // Validity check of an identifier
#define estring 8       // Text string (its constant offset)
#define enull   9       // NULL pointer check

//═════════════════════════════════════════════════════════════════════════════

//─Mnemonic──Code──Operands (EML code generation, "*" = "not used yet")

#define lnop  0 // *            No operation
#define lcar  1 // value        Load a constant onto the stack
#define lasi  2 //              Pop value, offset and put the value into [offset]
#define lori  3 //              Logical or
#define lxor  4 //              Xor, exclusive or
#define land  5 //              Logical and, operator on conditions
#define ligu  6 //              Equal, logical comparison operator
#define ldis  7 //              Not equal, true if the 2 values are different
#define lmay  8 //              Greater than, signed comparison
#define lmen  9 //              Less than, ditto
#define lmei 10 //              Less than or equal
#define lmai 11 //              Greater than or equal
#define ladd 12 //              Add two constants
#define lsub 13 //              Subtraction, binary operation
#define lmul 14 //              Multiplication
#define ldiv 15 //              Integer division
#define lmod 16 //              Modulo, remainder of the division
#define lneg 17 //              Negation, changes the sign of a constant
#define lptr 18 //              Pointer, pop offset and push [offset]
#define lnot 19 //              Binary negation, bitwise
#define laid 20 //              Add id to the constant on the stack
#define lcid 21 //              Load id onto the stack
#define lrng 22 // range        Perform a range comparison
#define ljmp 23 // offset       Jump to an address in mem[]
#define ljpf 24 // offset       Jump to an address if a value is false
#define lfun 25 // code         Call to an internal process, e.g. signal()
#define lcal 26 // offset       Create a new process in the program
#define lret 27 //              Self-removal of the process
#define lasp 28 //              Discard a stacked value
#define lfrm 29 //              Stop the execution of the process for this frame
#define lcbp 30 // num_params      Initialize the pointer to the local parameters
#define lcpa 31 //              Pop offset, read parameter [offset] and bp++
#define ltyp 32 // block        Define the type of the current process (collisions)
#define lpri 33 // offset       Jump to the address, and load private vars.
#define lcse 34 // offset       If switch <> expression, jump to the offset
#define lcsr 35 // offset       If switch is not in the range, jump to the offset
#define lshr 36 //              Rotate right (C mode, >>)
#define lshl 37 //              Rotate left (C mode, <<)
#define lipt 38 //              Increment and pointer
#define lpti 39 //              Pointer and increment
#define ldpt 40 //              Decrement and pointer
#define lptd 41 //              Pointer and decrement
#define lada 42 //              Add-assignment
#define lsua 43 //              Sub-assignment
#define lmua 44 //              Mul-assignment
#define ldia 45 //              Div-assignment
#define lmoa 46 //              Mod-assignment
#define lana 47 //              And-assignment
#define lora 48 //              Or-assignment
#define lxoa 49 //              Xor-assignment
#define lsra 50 //              Shr-assignment
#define lsla 51 //              Shl-assignment
#define lpar 52 // num_par_pri  Define the number of private parameters
#define lrtf 53 //              Self-removal of the process, returns a value
#define lclo 54 // offset       Create a clone of the current process
#define lfrf 55 //              Pseudo-Frame (frame at a percentage, frame(100)==frame)
#define limp 56 // offset text  Import an external DLL
#define lext 57 // code         Call an external function
#define lchk 58 //              Check the validity of an identifier
#define ldbg 59 //              Invoke the debugger

// Instructions added for optimization (DIV 2.0)

#define lcar2 60
#define lcar3 61
#define lcar4 62
#define lasiasp 63
#define lcaraid 64
#define lcarptr 65
#define laidptr 66
#define lcaraidptr 67
#define lcaraidcpa 68
#define laddptr 69
#define lfunasp 70
#define lcaradd 71
#define lcaraddptr 72
#define lcarmul 73
#define lcarmuladd 74
#define lcarasiasp 75
#define lcarsub 76
#define lcardiv 77

// Instructions added for character handling

#define lptrchr 78  // Pointer, pop (index, offset) and push [offset+byte index]
#define lasichr 79  // Pop (value, index, offset) and put the value into [offset+byte index]
#define liptchr 80  // Increment and pointer
#define lptichr 81  // Pointer and increment
#define ldptchr 82  // Decrement and pointer
#define lptdchr 83  // Pointer and decrement
#define ladachr 84  // Add-assignment
#define lsuachr 85  // Sub-assignment
#define lmuachr 86  // Mul-assignment
#define ldiachr 87  // Div-assignment
#define lmoachr 88  // Mod-assignment
#define lanachr 89  // And-assignment
#define lorachr 90  // Or-assignment
#define lxoachr 91  // Xor-assignment
#define lsrachr 92  // Shr-assignment
#define lslachr 93  // Shl-assignment
#define lcpachr 94  // Pop offset, read parameter [offset] and bp++

// Instructions added for string handling

#define lstrcpy 95  // Pop si, di, and do strcpy(mem[di],[si]) (leaves di on the stack)
#define lstrfix 96  // Extend a string before putting a char into it
#define lstrcat 97  // Concatenate two strings (works like strcpy)
#define lstradd 98  // Add two strings "in the air" and leave the pointer to the air on the stack
#define lstrdec 99  // Add or remove characters to/from a string
#define lstrsub 100 // Remove characters from a string (-=)
#define lstrlen 101 // Replace a string with its length
#define lstrigu 102 // Equality comparison of two strings
#define lstrdis 103 // Different strings
#define lstrmay 104 // Greater string
#define lstrmen 105 // Lesser string
#define lstrmei 106 // Greater or equal string
#define lstrmai 107 // Lesser or equal string
#define lcpastr 108 // Load a parameter into a string

// Instructions added for handling Words

#define lptrwor 109 // Pointer, pop (index, offset) and push [offset+byte index]
#define lasiwor 110 // Pop (value, index, offset) and put the value into [offset+byte index]
#define liptwor 111 // Increment and pointer
#define lptiwor 112 // Pointer and increment
#define ldptwor 113 // Decrement and pointer
#define lptdwor 114 // Pointer and decrement
#define ladawor 115 // Add-assignment
#define lsuawor 116 // Sub-assignment
#define lmuawor 117 // Mul-assignment
#define ldiawor 118 // Div-assignment
#define lmoawor 119 // Mod-assignment
#define lanawor 120 // And-assignment
#define lorawor 121 // Or-assignment
#define lxoawor 122 // Xor-assignment
#define lsrawor 123 // Shr-assignment
#define lslawor 124 // Shl-assignment
#define lcpawor 125 // Pop offset, read parameter [offset] and bp++

// Miscellaneous

#define lnul    126 // Check that a pointer is not NULL

//═════════════════════════════════════════════════════════════════════════════
//      Prototypes
//═════════════════════════════════════════════════════════════════════════════

void c_error(word,word);
void analyze_commands(word,byte**);
void prepare_compilation(void);
void analyze_ltlex(void);
void lexer(void);
void syntactic(void);
void tglo_init(memptrsize);
void sentence(void);
void expression(void);
int constant(void);

static jmp_buf buf;

void comp(void)
{
	if (!setjmp(buf))
		compile();
}

void comp_exit(void)
{
     longjmp(buf,1);        
}



//═════════════════════════════════════════════════════════════════════════════
//      Global variables of the compiler
//═════════════════════════════════════════════════════════════════════════════

#include "div_stub.h"

byte file_in[_MAX_PATH+1],file_out[_MAX_PATH+1];

FILE * end, * fout;
byte * _source=NULL;
byte * source, * ierror, * ierror_end;

int texts_length;
int start_texts;
int max_process;
int ignore_errors;
int free_syntax;
int extended_conditions;
int simple_conditions;
int check_range;
int check_id;
int check_null;
int do_strfix;
int optimize;

//═════════════════════════════════════════════════════════════════════════════

FILE * div_open_file(char * file);

//═════════════════════════════════════════════════════════════════════════════

int old_line; // Location in the file of the token before the last one read
byte * old_ierror, * old_ierror_end;

// The last token read is at (line_num,ierror,ierror_end)

byte * last_line,zero=0;

//═════════════════════════════════════════════════════════════════════════════

// Information for EXEC.LIN (position in mem and in the source of each statement)

int start,final;     // Start and end addresses in mem[] of the statement
int line1,column1;  // Position where the statement starts in the source
int line2,column2;  // Position where the statement ends in the source

void start_sentence(void);  // Set start, line1, column1
void end_sentence(void);   // Set end, line2, column2
void record_sentence(void);  // Save the six variables to the file

//═════════════════════════════════════════════════════════════════════════════

// Object table ─────────────────────────────────────── *iobj first free object

struct object {
  byte type;    // Object type
  byte used;   // Tells whether the object has been used before being defined
  byte * name;  // Pointer to the name, for some listing
  byte * ierror;// Pointer to the code to determine the column if necessary
  int line_num;    // Source code line, to report the error
  int param;    // Tells that it is an object declared in the parameters
  struct object * previous; // Previous object with the same name
  struct object * block;   // Block of this process (0-global/local N-private)
  struct object * member;   // Tells which struct it belongs to (0-n/a)
  union {
    struct {                                            // Constant
      int value;
      int literal;                                      // 0-no, 1-si
    } cons;
    struct {                                            // Variable global
      int offset;
    } vglo;
    struct {                                            // Global table
      int offset;
      int totalen;                                      // total number of elements
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } tglo,pigl;
    struct {                                            // Byte global
      int offset;
      int totalen;                                      // total number of elements
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } bglo,pbgl;
    struct {                                            // Word global
      int offset;
      int totalen;                                      // total number of elements
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } wglo,pwgl;
    struct {                                            // Global string
      int offset;
      int totalen;                                      // total number of elements
    } cglo,pcgl;
    struct {                                            // Struct
      int offset;
      int len_item;                                     // Number of fields
      int totalitems;                                   // Total number of records
      int items1,items2,items3;                         // -1 si n/a
    } sglo,sloc;
    struct {                                            // Struct pointer
      int offset;
      struct object * ostruct;                          // Pointer to the struct
      int totalitems;                                   // Total number of records
      int items1,items2,items3;                         // -1 si n/a
    } psgl,pslo;
    struct {                                            // Variable local
      int offset;
    } vloc;
    struct {                                            // Local table
      int offset;
      int totalen;
      int len1,len2,len3;
    } tloc,pilo;
    struct {                                            // Byte local
      int offset;
      int totalen;                                      // total number of elements
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } bloc,pblo;
    struct {                                            // Word local
      int offset;
      int totalen;                                      // total number of elements
      int len1,len2,len3;                               // len2/3==-1 si n/a
    } wloc,pwlo;
    struct {                                            // Local string
      int offset;
      int totalen;                                      // total number of elements
    } cloc,pclo;
    struct {                                            // Process
      struct object * block;
      int offset;
      int num_params;
    } proc;
    struct {                                            // Internal function
      int code;
      int num_params;
      int ret;      // 0-Int 1-String
      byte par[12]; // 0-Int 1-(Struct_name,name)
    } func;
    struct {                                            // External function
      int code;
      int num_params;
    } fext;
  };
} obj[max_obj], * iobj; // ────────────────────────────────────────────────────

//═════════════════════════════════════════════════════════════════════════════

byte * vnames=NULL; // Names vector (cad_hash:int, piece (or iobj):int, asciiz)
union { byte*b; byte**p; } ivnames;

byte * objects_start; // To create the listing of the object table

byte * vhash[256]; // Pointer to the vector of names

//═════════════════════════════════════════════════════════════════════════════

struct exp_ele {
  byte type; // econs, eoper, erange, ewhoami, ecall
  union {
    int value;
    int token;
    struct object * object;
  };
} tabexp[max_exp], *_exp;

struct lex_ele { byte character;
         byte token;
                 struct lex_ele * alternative;
                 struct lex_ele * next; } 
       lex_sym[max_nodes], * ilex_sym, * lex_case[256];

int piece, piece_num;
struct object * o; // When piece=p_id, object of type (**o).type
struct object * current_block; // Block being analyzed
struct object * lexical_block; // It is 0 until the program's privates

struct object * member; // !=0 when declaring/accessing a member of a struct

int line_num, num_nodes, obj_cnt;

int predefined_obj_count;

//═════════════════════════════════════════════════════════════════════════════

int parameters;

int remote_access; // To not allow id.private

//═════════════════════════════════════════════════════════════════════════════

#define max_break 512
int tbreak[max_break];
int itbreak;

#define max_cont 256
int tcont[max_cont];
int itcont;

//═════════════════════════════════════════════════════════════════════════════

int opstack[long_stack+max_exp+64]; // expression evaluation (compilation and execution)

int32_t * mem_ory=NULL, * frm=NULL;
int32_t * mem=NULL, * loc=NULL;
int imem, imem_max;
int iloc, iloc_max;
int itxt, ifrm_max;

byte * imemptr;

int iloc_len; // end of the local variables including the private ones

// mem[]        target memory vector
// imem         pointer for data and code generation
// imem_max     end of the target memory (maximum index of mem[])
// iloc         start in mem[] of the local variables (initialized)
// iloc_len     length of the local variables

//───────────── Insertion into the expression table, variable access

//vglo           &vglo ptr
//tglo[_exp]     &tglo <_exp> add rng ptr

//vloc           &vloc aid ptr
//tloc[_exp]      &tloc <_exp> add rng aid ptr

//proc.vloc      &proc ptr &vloc add ptr
//proc.tloc[_exp] &proc ptr &tloc <_exp> add rng add ptr

//═════════════════════════════════════════════════════════════════════════════
//      Variables related to the formatted listing
//═════════════════════════════════════════════════════════════════════════════

int comment=0;           // 0-Code, 1-Inside a comment, 2-Nested, ...
int convert=0;          // Tells whether the tokens must already be generated in ls

//═════════════════════════════════════════════════════════════════════════════
//      Information about the errors
//═════════════════════════════════════════════════════════════════════════════

int error_number=-1;
int error_line;
int error_column;

int error_27=27;        // To emit "a statement was expected"

//═════════════════════════════════════════════════════════════════════════════

int program_type=0;

//═════════════════════════════════════════════════════════════════════════════

struct {      // Peephole, "sight" for the optimizer
  int dir;    // Address
  int param;  // Tells whether that opcode has a parameter
  int op;     // Opcode
} code[16];   // code[15] must always hold the last generated instruction

//═════════════════════════════════════════════════════════════════════════════
//      Compiler initialization
//═════════════════════════════════════════════════════════════════════════════

FILE * linf; // EXEC.LIN
FILE * lprg; // EXEC.PGM
FILE * lins; // EXEC.INS

FILE * def; // For the "ltlex.def" analyzer
byte *_buf;

void initialize_compiler(void) {
  int n;
  for (n=0;n<256;n++) if (lower[n]==' ') lower[n]=0;
}

//═════════════════════════════════════════════════════════════════════════════
//      Prepare the compilation of a program
//═════════════════════════════════════════════════════════════════════════════

void compilation_message(byte * p) {
  if(compilemode) {
	  fprintf(stdout,"%s\n",p);
  } else {
   wbox(v.ptr,v.w/big2,v.h/big2,c2,2,20,v.w/big2-4,7);
   wwrite(v.ptr,v.w/big2,v.h/big2,3,20,0,p,c3);
   blit_window(0); blit_copy();
 }
}

#ifndef ZLIB
#define uLongf unsigned long
#endif

void compile(void) {
  int n;
  uLongf m;
  struct object * i;
  byte * q, * p;

  program_type=0;

free_resources();

  compilation_message(texts[201]);

  vnames=NULL; mem_ory=NULL;
  mem=NULL; loc=NULL; frm=NULL;
  def=NULL; _buf=NULL; _source=NULL;

  itbreak=0; itcont=0; convert=0; comment=0;

  member=NULL; current_block=NULL; lexical_block=NULL;

  delete_code();

  memset(obj,0,sizeof(obj)); iobj=obj; obj_cnt=0;
  memset(lex_sym,0,sizeof(lex_sym)); ilex_sym=lex_sym; num_nodes=0;
  memset(vhash,0,sizeof(vhash));

  for (n=0;n<256;n++)
    if (lower[n])
      if (n>='0' && n<='9') lex_case[n]=(struct lex_ele*)l_num;
      else lex_case[n]=(struct lex_ele*)l_id;
    else lex_case[n]=(struct lex_ele*)l_err;

  if ((vnames=(byte *) malloc(max_obj*long_med_id+1024))==NULL) c_error(0,0);

  ivnames.b=vnames;

  analyze_ltlex();

  lex_case[' ']=(struct lex_ele*)l_spc;
  lex_case[tab]=(struct lex_ele*)l_spc;
  lex_case[cr]=(struct lex_ele*)l_cr;
/*
  for (n=0;n<256;n++)
	printf("vhash[%d] = %x\n",n,vhash[n]);
*/

  objects_start=ivnames.b;

  // *** NOTE *** These two errors are not "insufficient memory"
	linf=fopen("system/exec.fs","wb");
	if(linf) {
		fwrite(&fsmode,1,1,linf);
		fclose(linf);
	}
	linf = fopen("system/exec.path","wb");
	if(linf) {
		fputs((char *)&type[8],linf);
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

  compilation_message(texts[203]);

  imem=long_header;

  preload_obj(); // No literals in the preloaded objects

  compilation_message(texts[200]);

  source=source_ptr; _source=source;
  *(source+source_len)=cr; *(source+source_len+1)=cr;

  convert=1; last_line=source;
  remote_access=0; parameters=0; line_num=1;

  itxt=start_texts=imem;
  psyntactic(); // To obtain "texts_length"
  imem+=texts_length;

  test_buffer(&mem,&imem_max,imem);

  predefined_obj_count=obj_cnt;

  convert=1; last_line=source; fwrite(&zero,1,1,lprg);
  remote_access=0; parameters=0; line_num=1;

  syntactic();

  i=obj; while (i<iobj) {
    if (i->used) {
      line_num=i->line_num; ierror=i->ierror; c_error(0,121);
    } i++;
  }

  mem[2]=imem;
  mem[3]=max_process; // Formerly long_header, now unused
  mem[4]=0; // Formerly mem[1]-mem[3] (global data length), now unused
  mem[5]=iloc_len-iloc;
  mem[6]=iloc;
  mem[7]=0; // Formerly imem+iloc (start of texts), now unused
  mem[8]=imem+iloc; // Number of elements used in mem[]

  compilation_message(texts[204]);

  save_dbg();
  save_exec_bin();

  #ifdef listings
  l_objects();
  l_assembler();
  #endif

  compilation_message(texts[205]);

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
        fwrite(&n,1,4,f); // mem[0]..mem[8],uncompressed_data_length,compressed data...
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

  if (run_program==3) mem[0]+=128;
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
        fwrite(&n,1,4,f); // mem[0]..mem[8],uncompressed_data_length,compressed data...
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
  if (vnames!=NULL) free(vnames);

  def = NULL;
  _buf = NULL;
  frm = NULL;
  loc = NULL;
  mem = NULL;
  lins = NULL;
  lprg = NULL;
  linf = NULL;
  vnames = NULL;
}

//═════════════════════════════════════════════════════════════════════════════
//      Internal compiler errors
//═════════════════════════════════════════════════════════════════════════════

int _le,_t;
byte *_ie;

void save_error(word type) { // Save an error position (from 0 .. 3)
  switch(type) {
    case 0: _le=line_num; _ie=ierror; break;
    case 1: _le=old_line; _ie=old_ierror_end; break;
    case 2: _le=old_line; _ie=old_ierror; break;
    case 3: _le=old_line; _ie=old_ierror_end; break;
  } _t=type;
}

// 0 - Start of the current piece
// 1 - End of the previous piece plus one space (if possible)
// 2 - Start of the previous piece
// 3 - End of the previous piece
// 4 - Error saved with save_error(0..3)

void c_error(word type, word e) {
  int column=0;
  byte *_p,*p;

  error_number=e;

  switch(type) {
    case 0: error_line=line_num; p=ierror; break;
    case 1: error_line=old_line; _p=p=old_ierror_end; break;
    case 2: error_line=old_line; p=old_ierror; break;
    case 3: error_line=old_line; _p=p=old_ierror_end; break;
    case 4: error_line=_le; _p=p=_ie; type=_t; break;
  }

  if (e>=10) {
    while (--p>=source_ptr) {
      if (*p==cr) break;
      column++;
    } if (p<source_ptr) column++;
  }

  if (type==1) {
    if (*(_p+1)==' ' || *(_p+1)==cr || *(_p+1)==lf) column++; column++;
  } else if (type==3) column++;

  error_column=column;

  // _case_sensitive
  memcpy(lower+129,"ueaaaaçeeeiiiaaeææooouuyouø£Ø×ƒaiou",35);
  memcpy(lower+'A',"abcdefghijklmnopqrstuvwxyz",26);
  lower['Ñ']='ñ';

  comp_exit();
}

//═════════════════════════════════════════════════════════════════════════════
//  With the first token read, save the start of a statement
//═════════════════════════════════════════════════════════════════════════════

void start_sentence(void) {
  byte * p=ierror-1;
  start=imem;
  line1=line_num;
  column1=0;
  if(p<_source)
		p=_source;
		
  while (*p!=cr && *p!=lf && p>_source) { column1++; p--; }
}

//═════════════════════════════════════════════════════════════════════════════
//  With the first token that is not part of the statement, save the end of a statement
//═════════════════════════════════════════════════════════════════════════════

void end_sentence(void) {
  byte * p=old_ierror_end-1;
  final=imem-1;
  line2=old_line;
  column2=0;

    if(p<_source)
		p=_source;

  while (*p!=cr && *p!=lf && p>_source) { column2++; p--; }
}

//═════════════════════════════════════════════════════════════════════════════
//  Save a record (start,end,line1,...) to EXEC.LIN
//═════════════════════════════════════════════════════════════════════════════

void record_sentence(void) {
  fwrite(&start,4,1,linf);
  fwrite(&final,4,1,linf);
  fwrite(&line1,4,1,linf);
  fwrite(&column1,4,1,linf);
  fwrite(&line2,4,1,linf);
  fwrite(&column2,4,1,linf);
}

//═════════════════════════════════════════════════════════════════════════════
//      Check the limits of a code generation buffer
//═════════════════════════════════════════════════════════════════════════════

void test_buffer(int * * buffer,int * maximum,int n) {
  int max;

  if (n+buffer_grow>2*1000*1000) c_error(0,0);

  if (n>*maximum-security_distance) {
    max=*maximum; *maximum=n+buffer_grow; max=*maximum-max;
    if (*buffer==mem) {
      if ((*buffer=mem_ory=(int *)realloc(*buffer,*maximum*sizeof(memptrsize)))==NULL) c_error(0,0);
    } else {
      if ((*buffer=(int *)realloc(*buffer,*maximum*sizeof(memptrsize)))==NULL) c_error(0,0);
    }
    memset((byte*)*buffer+(*maximum-max)*sizeof(memptrsize),0,max*sizeof(memptrsize));
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Preload of the lexical structures, parses the ltlex.def file
//═════════════════════════════════════════════════════════════════════════════

void analyze_ltlex(void){

  byte * buf, cont=1;
  int len;
  struct lex_ele * e;

  int t; //token (piece)
  byte h; //hash (for id)
  byte * _ivnames;
  byte * * ptr;

  if ((def=fopen("system/ltlex.def","rb"))==NULL) c_error(0,1);
  fseek(def,0,SEEK_END); len=ftell(def);
  if ((_buf=buf=(byte *) malloc(len+2))==NULL) c_error(0,0);
  fseek(def,0,SEEK_SET);
  len=fread(buf,1,len,def);
  *(buf+len)=cr; *(buf+len+1)=cr;

  line_num=1;
  do {

	 switch (*buf++) {
    case ' ': case tab:
      break;
    case  cr:
      if (*buf==cr) cont=0; else { line_num++; buf++; } break;
    case ';':
      while (*buf!=cr) buf++; break;
    case '&':
      *buf=lower[*buf]; if (*buf>='0' && *buf<='9') t=(*buf++-'0')<<4;
      else if (*buf>='a' && *buf<='f') t=(*buf++-'a'+10)<<4; else c_error(0,2);
      *buf=lower[*buf]; if (*buf>='0' && *buf<='9') t+=(*buf++-'0');
      else if (*buf>='a' && *buf<='f') t+=(*buf++-'a'+10); else c_error(0,2);
      if (*buf==cr || *buf==' ' || *buf==tab) break;
      else if (lower[*buf]) {           //Analyzes a keyword
        _ivnames=ivnames.b; 
        *ivnames.p++=0; 
        *ivnames.p++=(byte*)t; 
        h=0;
        while (*ivnames.b=lower[*buf++]) h=((byte)(h<<1)+(h>>7))^(*ivnames.b++);
        ptr=&vhash[h]; while (*ptr) ptr=(byte **)*ptr; 
	*ptr=_ivnames;
        
	buf--; ivnames.b++;
      } else if (t>=0x78 && t<=0x7b) {  //Parse a literal delimiter
        lex_case[*buf]=(struct lex_ele*)l_lit;
      } else {                          //Parse a new symbol
        if ((e=lex_case[*buf])==0) {
          if (num_nodes++==max_nodes) c_error(0,3);
          e=lex_case[*buf]=ilex_sym++; (*e).character=*buf++;
        } else buf++;
        while (*buf!=' ' && *buf!=tab && *buf!=cr) {
          if (lower[*buf]) c_error(0,4);
          if ((*e).next==0)
            if (num_nodes++==max_nodes) c_error(0,3);
            else e=(*e).next=ilex_sym++;
          else {
            e=(*e).next;
            while ((*e).character!=*buf && (*e).alternative)
              e=(*e).alternative;
            if ((*e).character!=*buf)
              if (num_nodes++==max_nodes) c_error(0,3);
              else e=(*e).alternative=ilex_sym++;
          } (*e).character=*buf++;
        } (*e).token=t;
      } break;
  } 
  }while (cont);

  free(_buf); _buf=NULL;
  fclose(def); def=NULL;

}

//═════════════════════════════════════════════════════════════════════════════
//      Preload of the object table, parses the ltobj.def file
//═════════════════════════════════════════════════════════════════════════════

//      NOTE, memory limits are not checked in this process


void preload_obj (void) {

  int len,ret,code;
  struct object * ob;
  struct object * ob2;

  if ((def=fopen("system/ltobj.def","rb"))==NULL) c_error(0,6);
  fseek(def,0,SEEK_END); len=ftell(def);
  if ((_buf=source=(byte *) malloc(len+2))==NULL) c_error(0,0);
  fseek(def,0,SEEK_SET);
  len=fread(source,1,len,def);
  *(source+len)=cr; *(source+len+1)=cr;

  line_num=1; lexer();

  while (piece!=p_last) switch (piece) {
    case p_const:
      lexer(); if (piece!=p_id) c_error(0,7);
      ob=o; if ((*ob).type!=tnone) c_error(0,7); (*ob).type=tcons;
      lexer(); if (piece!=p_assign) c_error(0,7);
      lexer();
      if (piece==p_num) (*ob).cons.value=piece_num;
      else if (piece==p_sub) {
        lexer(); if (piece!=p_num) c_error(0,7);
        (*ob).cons.value=-piece_num;
      } else c_error(0,7);
      (*ob).cons.literal=0;
      lexer(); break;
    case p_global:
      lexer(); if (piece==p_id) {
        ob=o; if ((*ob).type!=tnone) c_error(0,7);
        lexer(); if (piece==p_lbracket) {
          (*ob).type=ttglo;
          (*ob).tglo.offset=imem;
          lexer(); if (piece!=p_num) c_error(0,7);
          (*ob).tglo.len1=len=piece_num;
          (*ob).tglo.len2=-1;
          (*ob).tglo.len3=-1;
          (*ob).tglo.totalen=len+1;
          do { mem[imem++]=0; } while (len--);
          lexer(); if (piece!=p_rbracket) c_error(0,7);
          lexer();
        } else {
          (*ob).type=tvglo;
          (*ob).vglo.offset=imem;
          if (piece!=p_assign) c_error(0,7); lexer();
          if (piece==p_num) mem[imem++]=piece_num;
          else if (piece==p_sub) {
            lexer(); if (piece!=p_num) c_error(0,7);
            mem[imem++]=-piece_num;
          } else c_error(0,7); lexer();
        }
      } else if (piece==p_struct) {
        lexer(); if (piece!=p_id) c_error(0,7);
        ob=o; if ((*ob).type!=tnone) c_error(0,7);
        lexer(); if (piece!=p_lbracket) c_error(0,7);
        (*ob).type=tsglo; (*ob).sglo.offset=imem;
        lexer(); if (piece!=p_num) c_error(0,7);
        (*ob).sglo.items1=piece_num;
        (*ob).sglo.items2=(*ob).sglo.items3=-1;
        (*ob).sglo.totalitems=piece_num+1;
        lexer(); if (piece!=p_rbracket) c_error(0,7);
        (*ob).sglo.len_item=0; member=ob; lexer();
        if (piece==p_end) c_error(0,7); len=0;
        while (piece!=p_end) {

          if (piece==p_string) {

            lexer();
            if (piece!=p_id) c_error(0,7);
            if ((*o).type!=tnone) c_error(0,7);
            ob2=o; lexer();
            (*ob2).type=tcglo;
            (*ob2).cglo.offset=len+1;
            if (piece!=p_lbracket) c_error(0,7); lexer();
            if (((*ob2).cglo.totalen=constant())<0) c_error(0,7);
            if ((*ob2).cglo.totalen>0xFFFFF) c_error(0,7);
            if (piece!=p_rbracket) c_error(0,7); lexer();

            mem[imem]=0xDAD00000|(*ob2).cglo.totalen;
            len+=1+((*ob2).cglo.totalen+5)/4;
            mem[imem+1]=0;
            imem+=1+((*ob2).cglo.totalen+5)/4;
            (*ob).sglo.len_item+=1+((*ob2).cglo.totalen+5)/4;

          } else {

            if (piece!=p_id) c_error(0,7);
            if ((*o).type!=tnone) c_error(0,7);
            ob2=o; lexer();
            if (piece==p_lbracket) {
              (*ob2).type=ttglo;
              (*ob2).tglo.offset=len;
              lexer(); if (piece!=p_num) c_error(0,7);
              (*ob2).tglo.len1=piece_num;
              (*ob2).tglo.len2=-1;
              (*ob2).tglo.len3=-1;
              (*ob2).tglo.totalen=piece_num+1;
              len+=piece_num+1;
              memset(&mem[imem],0,(piece_num+1)*4);
              imem+=piece_num+1;
              (*ob).sglo.len_item+=piece_num+1;
              lexer();
              if (piece!=p_rbracket) c_error(0,7);
              lexer();
            } else {
              (*ob2).type=tvglo;
              (*ob2).vglo.offset=len++;
              mem[imem++]=0;
              (*ob).sglo.len_item++;
            }

          }

        } member=NULL; lexer();

        if ((*ob).sglo.totalitems>1) {
          len=((*ob).sglo.totalitems-1)*(*ob).sglo.len_item-1;
          do { mem[imem]=mem[imem-(*ob).sglo.len_item]; imem++; } while (len--);
        }

      } else c_error(0,7); break;
    case p_local:
      lexer(); if (piece==p_id) {
        ob=o; if ((*ob).type!=tnone) c_error(0,7); (*ob).type=tvloc;
        (*ob).vloc.offset=iloc;
        lexer(); if (piece!=p_assign) c_error(0,7);
        lexer();
        if (piece==p_num) loc[iloc++]=piece_num;
        else if (piece==p_sub) {
          lexer(); if (piece!=p_num) c_error(0,7);
          loc[iloc++]=-piece_num;
        } else c_error(0,7);
        lexer();
      } else if (piece==p_struct) {
        lexer(); if (piece!=p_id) c_error(0,7);
        ob=o; if ((*ob).type!=tnone) c_error(0,7);
        lexer(); if (piece!=p_lbracket) c_error(0,7);
        (*ob).type=tsloc; (*ob).sloc.offset=iloc;
        lexer(); if (piece!=p_num) c_error(0,7);
        (*ob).sloc.items1=piece_num;
        (*ob).sloc.items2=(*ob).sloc.items3=-1;
        (*ob).sloc.totalitems=piece_num+1;
        lexer(); if (piece!=p_rbracket) c_error(0,7);
        (*ob).sloc.len_item=0; member=ob; lexer();
        if (piece==p_end) c_error(0,7);

        len=0;

        while (piece!=p_end) {
          if (piece!=p_id) c_error(0,7);
          if ((*o).type!=tnone) c_error(0,7);
          (*o).type=tvloc; (*o).vloc.offset=len;
          (*ob).sloc.len_item++;
          lexer();
          if (piece==p_assign) {
            lexer();
            if (piece==p_num) loc[iloc+len]=piece_num;
            else if (piece==p_sub) {
              lexer(); if (piece!=p_num) c_error(0,7);
              loc[iloc+len]=-piece_num;
            } else c_error(0,7);
            lexer();
          } else loc[iloc+len]=0;
          len++;
        }

        iloc+=(*ob).sloc.totalitems*(*ob).sloc.len_item;

        member=NULL; lexer();
      } else c_error(0,7); break;

    case p_function: // function 0 int signal(0,0) // ....
      lexer(); if (piece!=p_num) c_error(0,7);
      code=piece_num; lexer();
      if (piece==p_int) ret=0;
      else if (piece==p_string) ret=1;
      else c_error(0,7);
      lexer(); if (piece!=p_id) c_error(0,7);
      ob=o; if ((*ob).type!=tnone) c_error(0,7); (*ob).type=tfunc;
      lexer(); if (piece!=p_lparen) c_error(0,7);
      lexer(); len=0;
      while (piece!=p_rparen) {
        if (piece!=p_num) c_error(0,7);
        (*ob).func.par[len++]=piece_num;
        lexer(); if (piece==p_comma) lexer();
      }
      (*ob).func.code=code;
      (*ob).func.ret=ret;
      (*ob).func.num_params=len;
      lexer(); break;

    default: c_error(0,7); break;
  }

  free(_buf); _buf=NULL;
  fclose(def); def=NULL;

}

//═════════════════════════════════════════════════════════════════════════════
//  Guess what the next lexical piece read will be (and where it will be)
//═════════════════════════════════════════════════════════════════════════════

int next_piece;
int next_line;
byte * next_source;

byte * next_lexer(byte * _source, int comment, int line_num) { // It never generates errors

  byte ** ptr, * _ivnames, h;
  struct lex_ele * e;

  if (!comment && line_num) {
    old_line=line_num; old_ierror=ierror; old_ierror_end=ierror_end;
  }

  lex_scan:

  if (line_num) ierror=_source;
  next_source=_source;

  switch ((memptrsize)lex_case[*_source]) {

    case l_err:
      if (comment) { _source++; goto lex_scan; }
      next_piece=0; break;

    case l_cr :
      if (line_num) {

        if (convert) {
          fwrite(last_line,1,_source-last_line,lprg);
          fwrite(&zero,1,1,lprg);
        }

        line_num++; if ((*++_source)==lf) { _source++; last_line=_source; goto lex_scan; }
        next_piece=0; break;

      } else {

        if ((*++_source)==lf) { _source++; goto lex_scan; }
        next_piece=0; break;

      }

    case l_id :
      if (comment) { _source++; goto lex_scan; }
      _ivnames=ivnames.b; *ivnames.p++=0; *ivnames.p++=0; h=0;
      while (*ivnames.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*ivnames.b++);
      ivnames.b++; _source--;
      ptr=&vhash[h];
      while (*ptr!=NULL && strcmp((char *)(ptr+2),(char *)_ivnames+ptr8)) ptr=(byte **)*ptr;
      if (!strcmp((char *)(ptr+2),(char *)_ivnames+ptr8)) { // id found
        ivnames.b=_ivnames; // takes it out of vnames
        next_piece=(memptrsize)*(ptr+1);
        if (next_piece<256 && next_piece>=0) { // reserved word (token)
          if (next_piece==p_rem) { while (*_source!=cr) _source++; goto lex_scan; }
        } else { // object (id previous)
          next_piece=p_id;
        }
      } else {
        ivnames.b=_ivnames; // takes it out of vnames
        next_piece=p_id; // new id
      } break;

    case l_spc:
      while ((*++_source)==' '); goto lex_scan;

    case l_lit:
      if (comment) { _source++; goto lex_scan; }
      next_piece=p_lit;
      break;

    case l_num:
      if (comment) { _source++; goto lex_scan; }
      next_piece=p_num;
      break;

    default: // pointer to a lex_ele
      e=lex_case[*_source++]; next_piece=(*e).token; _ivnames=_source;

      while (e=(*e).next) {
        while (*_source!=(*e).character && (*e).alternative) e=(*e).alternative;
        if (*_source++==(*e).character && (*e).token) {
          next_piece=(*e).token; _ivnames=_source;
        }
      } _source=_ivnames;

      if (next_piece==p_rem && !comment) {
        while (*_source!=cr) _source++; goto lex_scan;
      }

      if (next_piece==p_start_rem) {
        comment++; do {
          _source=next_lexer(_source,comment,line_num);
          line_num=next_line;
        } while (next_piece!=p_end_rem);
        comment--;
        goto lex_scan;
      } break;

  }
  next_line=line_num;
  return(_source);
}


//═════════════════════════════════════════════════════════════════════════════
//      Lexer (reads a new piece from *source)
//═════════════════════════════════════════════════════════════════════════════

int IsWAV(char *FileName);
int in_fopen=0;

void lexer(void) {

  struct object * * ptr_o;
  byte ** ptr, * _ivnames, h, * _source=source;
  struct lex_ele * e;
  char cwork[66];
  FILE * f;

  int packable;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  int n;

  if (!comment) {old_line=line_num; old_ierror=ierror; old_ierror_end=ierror_end;}

  lex_scan: ierror=_source;
  switch ((memptrsize)lex_case[*_source]) { // Pointer to a lex_ele or l_???

    case l_err:
      if (comment) { piece=p_rem; _source++; } else c_error(0,10); break;

    case l_cr :

      if (convert) {
        fwrite(last_line,1,_source-last_line,lprg);
        fwrite(&zero,1,1,lprg);
      }

      line_num++; if ((*++_source)==lf) { _source++; last_line=_source; goto lex_scan; }
      piece=p_last; break; // eof

    case l_id :
      if (comment) { piece=p_rem; _source++; break; }
      _ivnames=ivnames.b; *ivnames.p++=0; *ivnames.p++=0; h=0;
      while (*ivnames.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*ivnames.b++);
      ivnames.b++; 
      _source--; 
      if (ivnames.b-vnames>max_obj*long_med_id) c_error(0,100);

      ptr=&vhash[h];
      while (*ptr && strcmp((char *)(ptr+2),((char *)_ivnames+ptr8))!=0) {
		  ptr=(byte**)*ptr; 
	  }
	  
      if (!strcmp((char *)(ptr+2),(char *)_ivnames+ptr8)) { // id found
        ivnames.b=_ivnames; // takes it out of vnames
        piece=(memptrsize)*(ptr+1);
        if (piece<256 && piece>=0) { // reserved word (t	oken)
			
          if (piece==p_rem) { while (*_source!=cr) _source++; goto lex_scan; }

        } else { // object (id previous)
          ptr_o=(struct object **)(ptr+1); o=*ptr_o; piece=p_id;
          while(o!=NULL && ( ((*o).block && lexical_block!=(*o).block) ||
                             ((*o).member!=member) )) o=(*o).previous;
          if(o==NULL) { // Not found
            o=iobj++; (*o).previous=*ptr_o; *ptr_o=o;
            (*o).name=(byte*)(ptr_o+1);
            (*o).member=member;
            (*o).param=0;
            if (parameters) (*o).block=current_block;
            #ifndef SHARE
            if (obj_cnt++==max_obj) c_error(0,102);
            #else
            if (obj_cnt++==max_obj) c_error(0,101);
            #endif
          } else {
            if ((*o).type==tcons) piece_num=(*o).cons.value;
          }
        }
      } else {
        *ptr=_ivnames; ptr_o=(struct object **)(_ivnames+ptr4); *ptr_o=o=iobj++; piece=p_id; // new id
        (*o).name=(byte *)_ivnames+ptr8;
        (*o).member=member;
        if (parameters) (*o).block=current_block;
        if (obj_cnt++==max_obj) c_error(0,102);
      } break;

    case l_spc:
      while ((*++_source)==' '); goto lex_scan;

    case l_lit:
      if (comment) { piece=p_rem; _source++; break; }
      piece=p_lit; h=*_source; _ivnames=ivnames.b; // Literal between two h
      do {
        if (*++_source==cr) c_error(0,11);
        if (*_source==h)
          if (*(_source+1)==h) *_ivnames=*++_source; else *_ivnames=0;
        else *_ivnames=*_source;
      } while (*_ivnames++); _source++;

      next_lexer(_source,0,0);
      while (next_piece==p_lit) {
        next_lexer(_source,0,line_num);
        line_num=next_line;
        _source=next_source;
        h=*_source; _ivnames--;
        do {
          if (*++_source==cr) c_error(0,11);
          if (*_source==h)
            if (*(_source+1)==h) *_ivnames=*++_source; else *_ivnames=0;
          else *_ivnames=*_source;
        } while (*_ivnames++); _source++;
        next_lexer(_source,0,0);
      }

      n=(strlen((char *)ivnames.b)+ptr4)/4;
      memcpy(&mem_ory[itxt],ivnames.b,strlen((char *)ivnames.b)+1);

//      fprintf(stdout,"FILE: %s\n",(char *)ivnames.b);
//	      fprintf(stdout,"LOOKING FOR FILE: %s [%s] [%s]\n",(char *)ivnames.b,full,(char *)&type[8]);

      if ((!ivnames.b[0]!='.' && ivnames.b[1]!=0) && (ivnames.b[0]!='/' && ivnames.b[1]!=0) && strcmp("/",(char *)ivnames.b) && (f=div_open_file((char *)ivnames.b))!=NULL) {
	      fprintf(stdout,"FOUND FILE: [%s] [%s] [%s]\n",(char *)ivnames.b,full,(char *)&type[8]);

        packable=0;

        // Determine whether the file is packable ...

        _splitpath(full,(char *)drive,(char *)dir,(char *)fname,(char *)ext);
        strupr((char *)ext);

        if (!stricmp((char *)ext,".MOD") || !stricmp((char *)ext,".S3M") || !stricmp((char *)ext,".XM")) {
          packable=1;
        } else if (!stricmp((char *)ext,".PCM")) {
          packable=1;
        } else if (!stricmp((char *)ext,".PCX")) {
          if (fread(cwork,1,66,f)>0) {
            if(cwork[2]==1 && cwork[3]==8 && cwork[65]==1) packable=1;
          }
        } else if (fread(cwork,1,8,f)>0) {
          if (!strcmp((char *)cwork,"pal\x1a\x0d\x0a")) packable=1;
          if (!strcmp((char *)cwork,"map\x1a\x0d\x0a")) packable=1;
          if (!strcmp((char *)cwork,"fpg\x1a\x0d\x0a")) packable=1;
          if (!strcmp((char *)cwork,"fnt\x1a\x0d\x0a")) packable=1;
          if (!strcmp((char *)cwork,"wld\x1a\x0d\x0a")) packable=1;
          if (!strcmp((char *)cwork,"wld\x1a\x0d\x0a\x01")) packable=1;
        }

        fclose(f);

        if (IsWAV(full)) packable=1;

        // ???
		packable=1;

        if (!packable || in_fopen) fwrite("+",1,1,lins);
        fwrite(full,1,strlen(full)+1,lins);
        fflush(lins);
      }

      piece_num=itxt; itxt+=n; break;

    case l_num:
      if (comment) { piece=p_rem; _source++; break; }

      piece=p_num; piece_num=0;
      if (*_source=='0' && lower[*(_source+1)]=='x') {
        _source+=2;
        while ((memptrsize)lex_case[*_source]==l_num ||
               (lower[*_source]>='a' && lower[*_source]<='f')) {
          if ((memptrsize)lex_case[*_source]==l_num)
            piece_num=piece_num*16+*_source++-0x30;
          else piece_num=piece_num*16+lower[*_source++]-'a'+10;
        }
      } else do {
        piece_num=piece_num*10+*_source++-0x30;
      } while ((memptrsize)lex_case[*_source]==l_num);
      break;

    default: // pointer to a lex_ele

      e=lex_case[*_source++]; _ivnames=_source; piece=(*e).token;
      while (e=(*e).next) {
        while (*_source!=(*e).character && (*e).alternative) e=(*e).alternative;
        if (*_source++==(*e).character && (*e).token) {
          piece=(*e).token; _ivnames=_source;
        }
      } _source=_ivnames;

      if (piece==p_rem && !comment) {
        while (*_source!=cr) _source++;
        goto lex_scan;
      }
      if (piece==p_start_rem) {
        comment++;
        do {
           source=_source; lexer(); _source=source;
        } while (piece!=p_end_rem);
        comment--;
        goto lex_scan;
      }

      if (piece==p_last) if (comment) c_error(0,55); else c_error(0,12);

      break;

  } source=_source; ierror_end=_source-1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Skip several consecutive commas or semicolons
//═════════════════════════════════════════════════════════════════════════════

void skip_semicolon(void) {
  do {
    lexer();
  } while (piece==p_semicolon);
}

//═════════════════════════════════════════════════════════════════════════════
//      Analysis of a pointer declaration (int, word or byte)
//═════════════════════════════════════════════════════════════════════════════

struct object * analyze_pointer(int type, int offset) {
  struct object * ob;
  int len1,len2,len3;

  if (piece!=p_id) c_error(1,148);
  ob=o; if ((*ob).type!=tnone) {
    if (parameters==-1 && (*ob).param==1 && (*ob).block==current_block) {
      if ((*ob).type==type) { // A pointer parameter is repeated as private
        save_error(0); lexer();
        len1=-1; len2=-1; len3=-1;
        if (piece==p_lbracket) { lexer();
          if ((len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if ((len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if ((len3=constant())<0) c_error(4,35);
            }
          } if (piece!=p_rbracket) c_error(3,26); lexer();
        }
        if (len1!=(*ob).pilo.len1 || len2!=(*ob).pilo.len2 || len3!=(*ob).pilo.len3) c_error(4,141);
        else if (piece==p_assign) c_error(0,54);
        else {
          while (piece==p_semicolon || piece==p_comma) lexer();
          (*ob).param++; // It does not allow redeclaring it again
          return(NULL);
        }
      } else c_error(0,117);
    } else c_error(0,117);
  } else lexer();
  if (parameters==-1) (*ob).block=current_block;
  (*ob).pilo.len1=-1; (*ob).pilo.len2=-1; (*ob).pilo.len3=-1;
  if (piece==p_lbracket) { lexer();
    if (((*ob).pilo.len1=constant())<0) c_error(4,35);
    if (piece==p_comma) {
      lexer();
      if (((*ob).pilo.len2=constant())<0) c_error(4,35);
      if (piece==p_comma) {
        lexer();
        if (((*ob).pilo.len3=constant())<0) c_error(4,35);
      }
    }
    if (piece!=p_rbracket) c_error(3,26); lexer();
  } (*ob).pilo.totalen=0;
  if ((*ob).pilo.len1>-1) (*ob).pilo.totalen=(*ob).pilo.len1+1;
  if ((*ob).pilo.len2>-1) (*ob).pilo.totalen*=(*ob).pilo.len2+1;
  if ((*ob).pilo.len3>-1) (*ob).pilo.totalen*=(*ob).pilo.len3+1;

  (*ob).type=type; (*ob).pilo.offset=offset;

  return(ob);
}

//═════════════════════════════════════════════════════════════════════════════

int analyze_pointer_struct(int type, int offset, struct object * structure) {
  struct object * ob;
  int items1,items2,items3;

  if (piece!=p_id) c_error(1,148);
  ob=o; if ((*ob).type!=tnone) {
    if (parameters==-1 && (*ob).param==1 && (*ob).block==current_block) {
      if ((*ob).type==type) { // A pointer parameter is repeated as private
        save_error(0); lexer();
        items1=-1; items2=-1; items3=-1;
        if (piece==p_lbracket) { lexer();
          if ((items1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if ((items2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if ((items3=constant())<0) c_error(4,35);
            }
          } if (piece!=p_rbracket) c_error(3,26); lexer();
        }
        if (items1!=(*ob).psgl.items1 || items2!=(*ob).psgl.items2 || items3!=(*ob).psgl.items3) c_error(4,141);
        else if (piece==p_assign) c_error(0,54);
        else {
          (*ob).param++; return(0); // It does not allow redeclaring it again
        }
      } else c_error(0,117);
    } else c_error(0,117);
  } else lexer();
  if (parameters==-1) (*ob).block=current_block;
  (*ob).psgl.items1=-1; (*ob).psgl.items2=-1; (*ob).psgl.items3=-1;
  if (piece==p_lbracket) { lexer();
    if (((*ob).psgl.items1=constant())<0) c_error(4,35);
    if (piece==p_comma) {
      lexer();
      if (((*ob).psgl.items2=constant())<0) c_error(4,35);
      if (piece==p_comma) {
        lexer();
        if (((*ob).psgl.items3=constant())<0) c_error(4,35);
      }
    }
    if (piece!=p_rbracket) c_error(3,26); lexer();
  } (*ob).psgl.totalitems=0;
  if ((*ob).psgl.items1>-1) (*ob).psgl.totalitems=(*ob).psgl.items1+1;
  if ((*ob).psgl.items2>-1) (*ob).psgl.totalitems*=(*ob).psgl.items2+1;
  if ((*ob).psgl.items3>-1) (*ob).psgl.totalitems*=(*ob).psgl.items3+1;

  (*ob).type=type;                // tpsgl or tpslo
  (*ob).psgl.offset=offset;       // of the pointer
  (*ob).psgl.ostruct=structure;  // struct original

  // (*ob).psgl.len_item ­ (*((*ob).psgl.ostruct)).len_item;

  return(1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Syntactic analysis (drives the whole compilation process)
//═════════════════════════════════════════════════════════════════════════════

int analyze_struct(int offstruct) { // after " struct id [ <const> ] " // id=member
  int len=0,dup,i,_itxt,_imem;
  struct object * ob;
  struct object * old_member,* member2;
  byte * oimemptr;

  while (piece==p_semicolon) lexer();

  while (piece!=p_end) {

    if (piece==p_struct) {                              // Struct member ────

      lexer();

      if (piece==p_pointer) { // A pointer to struct is defined

        old_member=member; member=NULL; lexer(); member=old_member;
        if (piece!=p_id) c_error(1,110); ob=o;

        if ((*ob).type==tnone) c_error(0,150); // The pointer is not defined like this
        if ((*ob).type!=tsglo && (*ob).type!=tsloc) c_error(0,150);
        lexer();
        pointer_to_struct:
        analyze_pointer_struct(tpsgl,len,ob);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;

        if (piece==p_comma) {
          lexer();
          if (piece==p_pointer) lexer();
          goto pointer_to_struct;
        } else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else {

        if (piece!=p_id) c_error(1,110);
        ob=o; 
        old_member=member; member=ob; lexer();
        if ((*ob).type!=tnone) c_error(2,22);

        (*ob).type=tsglo; (*ob).sglo.offset=len;
        if (piece==p_lbracket) {
          member2=member; member=NULL; lexer();
          if (((*ob).sglo.items1=constant())<0) c_error(4,123);
          if (piece==p_comma) {
            lexer();
            if (((*ob).sglo.items2=constant())<0) c_error(4,123);
            if (piece==p_comma) {
              lexer();
              if (((*ob).sglo.items3=constant())<0) c_error(4,123);
            } else (*ob).sglo.items3=-1;
          } else { (*ob).sglo.items2=-1; (*ob).sglo.items3=-1; }
          member=member2;
          (*ob).sglo.totalitems=(*ob).sglo.items1+1;
          if ((*ob).sglo.items2>-1) (*ob).sglo.totalitems*=(*ob).sglo.items2+1;
          if ((*ob).sglo.items3>-1) (*ob).sglo.totalitems*=(*ob).sglo.items3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
        } else {
          (*ob).sglo.totalitems=1;
          (*ob).sglo.items1=0; (*ob).sglo.items2=-1; (*ob).sglo.items3=-1;
        }
        if (((*ob).sglo.len_item=analyze_struct(offstruct+len))==0) c_error(0,57);
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
        member=old_member; lexer();
        while (piece==p_semicolon) lexer();
      }
    } else if (piece==p_string) {                     // String member

      lexer();

      if (piece==p_pointer) { // Pointer to byte member

        lexer(); pointer1:
        ob=analyze_pointer(tpcgl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;

        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer1;
        }

      } else {

        if (piece!=p_id) c_error(1,125);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=tcglo; (*ob).cglo.offset=len+1;
        if (piece==p_lbracket) {
          lexer();
          if (piece==p_rbracket) {
            lexer();
            (*ob).cglo.totalen=255;
          } else {
            if (((*ob).cglo.totalen=constant())<0) c_error(4,127);
            if ((*ob).cglo.totalen>0xFFFFF) c_error(4,135);
            if (piece!=p_rbracket) c_error(3,26);
            lexer();
          }
        } else (*ob).cglo.totalen=255;

        test_buffer(&frm,&ifrm_max,offstruct+len);
        frm[offstruct+len]=0xDAD00000;

        if (piece==p_assign) {
          save_error(1);
          _itxt=itxt;
          lexer();
          if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
            c_error(3,128);
          if (strlen((char*)&mem[piece_num])>(*ob).cglo.totalen+1)
            c_error(4,129);
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cglo.totalen;
          strcpy((char*)&mem[offstruct+len+1],(char*)&mem[piece_num]);
          len+=1+((*ob).cglo.totalen+5)/4;
          itxt=_itxt; // Take the string out of the text segment
          lexer();
        } else {
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cglo.totalen;
          len+=1+((*ob).cglo.totalen+5)/4;
        }
      }

      if (piece==p_comma) piece=p_string; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_byte) {                         // Byte member ────

      lexer();

      if (piece==p_pointer) { // Pointer to byte member

        lexer(); pointer2:
        ob=analyze_pointer(tpbgl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;

        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer2;
        }

      } else {

        if (piece!=p_id) c_error(1,142);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=tbglo; (*ob).bglo.offset=len;
        if (piece==p_lbracket) { lexer();
          if (((*ob).bglo.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).bglo.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).bglo.len3=constant())<0) c_error(4,35);
            } else (*ob).bglo.len3=-1;
          } else { (*ob).bglo.len2=-1; (*ob).bglo.len3=-1; }
          (*ob).bglo.totalen=(*ob).bglo.len1+1;
          if ((*ob).bglo.len2>-1) (*ob).bglo.totalen*=(*ob).bglo.len2+1;
          if ((*ob).bglo.len3>-1) (*ob).bglo.totalen*=(*ob).bglo.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).bglo.totalen+3)/4);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).bglo.totalen+3)/4);
          memset(&frm[offstruct+len],2,(*ob).bglo.totalen);

          if (piece==p_assign) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexer();
            oimemptr=(byte*)&mem[imem];
            tglo_init(2);
            if (imemptr-oimemptr>(*ob).bglo.totalen) c_error(4,33);
            imem=_imem;
          } (*ob).bglo.totalen=((*ob).bglo.totalen+3)/4;
        } else {
          (*ob).type=tbglo; (*ob).bglo.offset=len;
          (*ob).bglo.len1=0;
          (*ob).bglo.len2=-1;
          (*ob).bglo.len3=-1;
          (*ob).bglo.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=2;

          if (piece==p_assign) {
            save_error(1);
            lexer();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constant();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>255) c_error(4,143);
          }
        } len+=(*ob).bglo.totalen;
      }

      if (piece==p_comma) piece=p_byte; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_word) {                         // Word member ────

      lexer();

      if (piece==p_pointer) { // Pointer to word member

        lexer(); pointer3:
        ob=analyze_pointer(tpwgl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer3;
        }

      } else {

        if (piece!=p_id) c_error(1,145);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=twglo; (*ob).wglo.offset=len;
        if (piece==p_lbracket) { lexer();
          if (((*ob).wglo.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).wglo.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).wglo.len3=constant())<0) c_error(4,35);
            } else (*ob).wglo.len3=-1;
          } else { (*ob).wglo.len2=-1; (*ob).wglo.len3=-1; }
          (*ob).wglo.totalen=(*ob).wglo.len1+1;
          if ((*ob).wglo.len2>-1) (*ob).wglo.totalen*=(*ob).wglo.len2+1;
          if ((*ob).wglo.len3>-1) (*ob).wglo.totalen*=(*ob).wglo.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).wglo.totalen+1)/2);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).wglo.totalen+1)/2);
          memset(&frm[offstruct+len],1,(*ob).wglo.totalen*2);

          if (piece==p_assign) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexer();
            oimemptr=(byte*)&mem[imem];
            tglo_init(1);
            if (imemptr-oimemptr>(*ob).wglo.totalen*2) c_error(4,33);
            imem=_imem;
          } (*ob).wglo.totalen=((*ob).wglo.totalen+1)/2;
        } else {
          (*ob).type=twglo; (*ob).wglo.offset=len;
          (*ob).wglo.len1=0;
          (*ob).wglo.len2=-1;
          (*ob).wglo.len3=-1;
          (*ob).wglo.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=1;

          if (piece==p_assign) {
            save_error(1);
            lexer();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constant();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>65535) c_error(4,144);
          }
        } len+=(*ob).wglo.totalen;
      }

      if (piece==p_comma) piece=p_word; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_int || piece==p_id || piece==p_pointer) {

      if (piece==p_int) {
        lexer();
        if (piece!=p_id && piece!=p_pointer) c_error(1,149);
      }

      if (piece==p_pointer) { // Pointer to int member

        lexer(); pointer4:
        ob=analyze_pointer(tpigl,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer4;
        }

      } else {

        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        if (piece==p_lbracket) { lexer();                   // Table member ────
          (*ob).type=ttglo; (*ob).tglo.offset=len;
          if (((*ob).tglo.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).tglo.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).tglo.len3=constant())<0) c_error(4,35);
            } else (*ob).tglo.len3=-1;
          } else { (*ob).tglo.len2=-1; (*ob).tglo.len3=-1; }
          (*ob).tglo.totalen=(*ob).tglo.len1+1;
          if ((*ob).tglo.len2>-1) (*ob).tglo.totalen*=(*ob).tglo.len2+1;
          if ((*ob).tglo.len3>-1) (*ob).tglo.totalen*=(*ob).tglo.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();

          test_buffer(&mem,&imem_max,offstruct+len+(*ob).tglo.totalen);
          if (piece==p_assign) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexer(); tglo_init(0);
            if (imem-(offstruct+len)>(*ob).tglo.totalen) c_error(4,33);
            imem=_imem;
          } len+=(*ob).tglo.totalen;

        } else {                                          // Variable member ────

          (*ob).type=tvglo; (*ob).vglo.offset=len;
          test_buffer(&mem,&imem_max,offstruct+len);
          if (piece==p_assign) {
            lexer(); mem[offstruct+len]=constant();
          } len+=1;

        }
      }
      if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
      while (piece==p_semicolon || piece==p_comma) lexer();
    } else c_error(0,58);
  } return(len);
}

//───────────────────────────────────────────────────────────────────────────

int analyze_struct_local(int offstruct) { // after " struct id [ <const> ] " // id=member
  int len=0,dup,i,_itxt,_iloc;
  struct object * ob;
  struct object * old_member,* member2;
  byte * oimemptr;

  while (piece==p_semicolon) lexer();

  while (piece!=p_end) {

    if (piece==p_struct) {                              // Struct member ────

      lexer();

      if (piece==p_pointer) { // A pointer to struct is defined

        old_member=member; member=NULL; lexer(); member=old_member;
        if (piece!=p_id) c_error(1,110); ob=o;

        if ((*ob).type==tnone) c_error(0,150); // The pointer is not defined like this
        if ((*ob).type!=tsglo && (*ob).type!=tsloc) c_error(0,150);
        lexer();
        pointer_to_struct:
        analyze_pointer_struct(tpslo,len,ob);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); loc[offstruct+len]=constant();
        } len+=1;

        if (piece==p_comma) {
          lexer();
          if (piece==p_pointer) lexer();
          goto pointer_to_struct;
        } else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else {

        if (piece!=p_id) c_error(1,110);
        ob=o; 

        old_member=member; member=ob; lexer();
        if ((*ob).type!=tnone) c_error(2,22);

        (*ob).type=tsloc; (*ob).sloc.offset=len;
        if (piece==p_lbracket) {
          member2=member; member=NULL; lexer();
          if (((*ob).sloc.items1=constant())<0) c_error(4,123);
          if (piece==p_comma) {
            lexer();
            if (((*ob).sloc.items2=constant())<0) c_error(4,123);
            if (piece==p_comma) {
              lexer();
              if (((*ob).sloc.items3=constant())<0) c_error(4,123);
            } else (*ob).sloc.items3=-1;
          } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
          member=member2;
          (*ob).sloc.totalitems=(*ob).sloc.items1+1;
          if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
          if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
        } else {
          (*ob).sloc.totalitems=1;
          (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
        }
        if (((*ob).sloc.len_item=analyze_struct_local(offstruct+len))==0) c_error(0,57);
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
        member=old_member; lexer();
        while (piece==p_semicolon) lexer();
      }

    } else if (piece==p_string) {                     // String member

      lexer();

      if (piece==p_pointer) { // Pointer to byte member

        lexer(); pointer1:
        ob=analyze_pointer(tpclo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); loc[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer1;
        }

      } else {

        if (piece!=p_id) c_error(1,125);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=tcloc; (*ob).cloc.offset=len+1;
        if (piece==p_lbracket) {
          lexer();
          if (piece==p_rbracket) {
            lexer();
            (*ob).cloc.totalen=255;
          } else {
            if (((*ob).cloc.totalen=constant())<0) c_error(4,127);
            if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
            if (piece!=p_rbracket) c_error(3,26);
            lexer();
          }
        } else (*ob).cloc.totalen=255;

        test_buffer(&frm,&ifrm_max,offstruct+len);
        frm[offstruct+len]=0xDAD00000;

        if (piece==p_assign) {
          save_error(1);
          _itxt=itxt;
          lexer();
          if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
            c_error(3,128);
          if (strlen((char*)&mem[piece_num])>(*ob).cloc.totalen+1)
            c_error(4,129);
          test_buffer(&loc,&iloc_max,offstruct+len);
          loc[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          strcpy((char*)&loc[offstruct+len+1],(char*)&mem[piece_num]);
          len+=1+((*ob).cloc.totalen+5)/4;
          itxt=_itxt; // Take the string out of the text segment
          lexer();
        } else {
          test_buffer(&loc,&iloc_max,offstruct+len);
          loc[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          len+=1+((*ob).cloc.totalen+5)/4;
        }
      }

      if (piece==p_comma) piece=p_string; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_byte) {                         // Byte member ────

      lexer();

      if (piece==p_pointer) { // Pointer to byte member

        lexer(); pointer2:
        ob=analyze_pointer(tpblo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); loc[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer2;
        }

      } else {

        if (piece!=p_id) c_error(1,142);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=tbloc; (*ob).bloc.offset=len;
        if (piece==p_lbracket) { lexer();
          if (((*ob).bloc.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).bloc.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).bloc.len3=constant())<0) c_error(4,35);
            } else (*ob).bloc.len3=-1;
          } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }
          (*ob).bloc.totalen=(*ob).bloc.len1+1;
          if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
          if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();

          test_buffer(&loc,&iloc_max,offstruct+len+((*ob).bloc.totalen+3)/4);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).bloc.totalen+3)/4);
          memset(&frm[offstruct+len],2,(*ob).bloc.totalen);

          if (piece==p_assign) {
            _iloc=iloc; iloc=offstruct+len;
            save_error(1); lexer();
            oimemptr=(byte*)&loc[iloc];
            tloc_init(2);
            if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
            iloc=_iloc;
          } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
        } else {
          (*ob).type=tbloc; (*ob).bloc.offset=len;
          (*ob).bloc.len1=0;
          (*ob).bloc.len2=-1;
          (*ob).bloc.len3=-1;
          (*ob).bloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=2;

          if (piece==p_assign) {
            save_error(1);
            lexer();
            test_buffer(&loc,&iloc_max,offstruct+len);
            loc[offstruct+len]=constant();
            if (loc[offstruct+len]<0 || loc[offstruct+len]>255) c_error(4,143);
          }
        } len+=(*ob).bloc.totalen;
      }

      if (piece==p_comma) piece=p_byte; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_word) {                         // Word member ────

      lexer();

      if (piece==p_pointer) { // Pointer to word member

        lexer(); pointer3:
        ob=analyze_pointer(tpwlo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); loc[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer3;
        }

      } else {

        if (piece!=p_id) c_error(1,145);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=twloc; (*ob).wloc.offset=len;
        if (piece==p_lbracket) { lexer();
          if (((*ob).wloc.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).wloc.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).wloc.len3=constant())<0) c_error(4,35);
            } else (*ob).wloc.len3=-1;
          } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }
          (*ob).wloc.totalen=(*ob).wloc.len1+1;
          if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
          if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();

          test_buffer(&loc,&iloc_max,offstruct+len+((*ob).wloc.totalen+1)/2);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).wloc.totalen+1)/2);
          memset(&frm[offstruct+len],1,(*ob).wloc.totalen*2);

          if (piece==p_assign) {
            _iloc=iloc; iloc=offstruct+len;
            save_error(1); lexer();
            oimemptr=(byte*)&loc[iloc];
            tloc_init(1);
            if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
            iloc=_iloc;
          } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
        } else {
          (*ob).type=twloc; (*ob).wloc.offset=len;
          (*ob).wloc.len1=0;
          (*ob).wloc.len2=-1;
          (*ob).wloc.len3=-1;
          (*ob).wloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=1;

          if (piece==p_assign) {
            save_error(1);
            lexer();
            test_buffer(&loc,&iloc_max,offstruct+len);
            loc[offstruct+len]=constant();
            if (loc[offstruct+len]<0 || loc[offstruct+len]>65535) c_error(4,144);
          }
        } len+=(*ob).wloc.totalen;
      }

      if (piece==p_comma) piece=p_word; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_int || piece==p_id || piece==p_pointer) {

      if (piece==p_int) {
        lexer();
        if (piece!=p_id && piece!=p_pointer) c_error(1,149);
      }

      if (piece==p_pointer) { // Pointer to int member

        lexer(); pointer4:
        ob=analyze_pointer(tpilo,len);
        test_buffer(&loc,&iloc_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); loc[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer4;
        }

      } else {

        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        if (piece==p_lbracket) { lexer();    // Table member ────
          (*ob).type=ttloc; (*ob).tloc.offset=len;
          if (((*ob).tloc.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).tloc.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).tloc.len3=constant())<0) c_error(4,35);
            } else (*ob).tloc.len3=-1;
          } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
          (*ob).tloc.totalen=(*ob).tloc.len1+1;
          if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
          if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();

          test_buffer(&loc,&iloc_max,offstruct+len+(*ob).tloc.totalen);
          if (piece==p_assign) {
            _iloc=iloc; iloc=offstruct+len;
            save_error(1); lexer(); tloc_init(0);
            if (iloc-(offstruct+len)>(*ob).tloc.totalen) c_error(4,33);
            iloc=_iloc;
          } len+=(*ob).tloc.totalen;

        } else {                                          // Variable member ────

          (*ob).type=tvloc; (*ob).vloc.offset=len;
          test_buffer(&loc,&iloc_max,offstruct+len);
          if (piece==p_assign) {
            lexer(); loc[offstruct+len]=constant();
          } len+=1;

        }
      }
      if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
      while (piece==p_semicolon || piece==p_comma) lexer();
    } else c_error(0,58);
    (*ob).block=lexical_block;
  } return(len);
}

//───────────────────────────────────────────────────────────────────────────

int analyze_struct_private(int offstruct) { // after " struct id [ <const> ] " // id=member
  int len=0,dup,i,_itxt,_imem;
  struct object * ob;
  struct object * old_member,* member2;
  byte * oimemptr;

  while (piece==p_semicolon) lexer();

  while (piece!=p_end) {

    if (piece==p_struct) {                              // Struct member ────

      lexer();

      if (piece==p_pointer) { // A pointer to struct is defined

        old_member=member; member=NULL; lexer(); member=old_member;
        if (piece!=p_id) c_error(1,110); ob=o;

        if ((*ob).type==tnone) c_error(0,150); // The pointer is not defined like this
        if ((*ob).type!=tsglo && (*ob).type!=tsloc) c_error(0,150);
        lexer();
        pointer_to_struct:
        analyze_pointer_struct(tpslo,len,ob);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;

        if (piece==p_comma) {
          lexer();
          if (piece==p_pointer) lexer();
          goto pointer_to_struct;
        } else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else {

        if (piece!=p_id) c_error(1,110);
        ob=o;

        old_member=member; member=ob; lexer();
        if ((*ob).type!=tnone) c_error(2,22);

        (*ob).type=tsloc; (*ob).sloc.offset=len;
        if (piece==p_lbracket) {
          member2=member; member=NULL; lexer();
          if (((*ob).sloc.items1=constant())<0) c_error(4,123);
          if (piece==p_comma) {
            lexer();
            if (((*ob).sloc.items2=constant())<0) c_error(4,123);
            if (piece==p_comma) {
              lexer();
              if (((*ob).sloc.items3=constant())<0) c_error(4,123);
            } else (*ob).sloc.items3=-1;
          } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
          member=member2;
          (*ob).sloc.totalitems=(*ob).sloc.items1+1;
          if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
          if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
        } else {
          (*ob).sloc.totalitems=1;
          (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
        }
        if (((*ob).sloc.len_item=analyze_struct_private(offstruct+len))==0) c_error(0,57);
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
        member=old_member; lexer();
        while (piece==p_semicolon) lexer();
      }

    } else if (piece==p_string) {                     // String member

      lexer();

      if (piece==p_pointer) { // Pointer to byte member

        lexer(); pointer1:
        ob=analyze_pointer(tpclo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer1;
        }

      } else {

        if (piece!=p_id) c_error(1,125);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=tcloc; (*ob).cloc.offset=len+1;
        if (piece==p_lbracket) {
          lexer();
          if (piece==p_rbracket) {
            lexer();
            (*ob).cloc.totalen=255;
          } else {
            if (((*ob).cloc.totalen=constant())<0) c_error(4,127);
            if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
            if (piece!=p_rbracket) c_error(3,26);
            lexer();
          }
        } else (*ob).cloc.totalen=255;

        test_buffer(&frm,&ifrm_max,offstruct+len);
        frm[offstruct+len]=0xDAD00000;

        if (piece==p_assign) {
          save_error(1);
          _itxt=itxt;
          lexer();
          if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
            c_error(3,128);
          if (strlen((char*)&mem[piece_num])>(*ob).cloc.totalen+1)
            c_error(4,129);
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          strcpy((char*)&mem[offstruct+len+1],(char*)&mem[piece_num]);
          len+=1+((*ob).cloc.totalen+5)/4;
          itxt=_itxt; // Take the string out of the text segment
          lexer();
        } else {
          test_buffer(&mem,&imem_max,offstruct+len);
          mem[offstruct+len]=0xDAD00000|(*ob).cloc.totalen;
          len+=1+((*ob).cloc.totalen+5)/4;
        }
      }

      if (piece==p_comma) piece=p_string; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_byte) {                         // Byte member ────

      lexer();

      if (piece==p_pointer) { // Pointer to byte member

        lexer(); pointer2:
        ob=analyze_pointer(tpblo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer2;
        }

      } else {

        if (piece!=p_id) c_error(1,142);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=tbloc; (*ob).bloc.offset=len;
        if (piece==p_lbracket) { lexer();
          if (((*ob).bloc.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).bloc.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).bloc.len3=constant())<0) c_error(4,35);
            } else (*ob).bloc.len3=-1;
          } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }
          (*ob).bloc.totalen=(*ob).bloc.len1+1;
          if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
          if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).bloc.totalen+3)/4);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).bloc.totalen+3)/4);
          memset(&frm[offstruct+len],2,(*ob).bloc.totalen);

          if (piece==p_assign) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexer();
            oimemptr=(byte*)&mem[imem];
            tglo_init(2);
            if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
            imem=_imem;
          } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
        } else {
          (*ob).type=tbloc; (*ob).bloc.offset=len;
          (*ob).bloc.len1=0;
          (*ob).bloc.len2=-1;
          (*ob).bloc.len3=-1;
          (*ob).bloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=2;

          if (piece==p_assign) {
            save_error(1);
            lexer();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constant();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>255) c_error(4,143);
          }
        } len+=(*ob).bloc.totalen;
      }

      if (piece==p_comma) piece=p_byte; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_word) {                         // Word member ────

      lexer();

      if (piece==p_pointer) { // Pointer to word member

        lexer(); pointer3:
        ob=analyze_pointer(tpwlo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer3;
        }

      } else {

        if (piece!=p_id) c_error(1,145);
        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        (*ob).type=twloc; (*ob).wloc.offset=len;
        if (piece==p_lbracket) { lexer();
          if (((*ob).wloc.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).wloc.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).wloc.len3=constant())<0) c_error(4,35);
            } else (*ob).wloc.len3=-1;
          } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }
          (*ob).wloc.totalen=(*ob).wloc.len1+1;
          if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
          if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();
          test_buffer(&mem,&imem_max,offstruct+len+((*ob).wloc.totalen+1)/2);

          test_buffer(&frm,&ifrm_max,offstruct+len+((*ob).wloc.totalen+1)/2);
          memset(&frm[offstruct+len],1,(*ob).wloc.totalen*2);

          if (piece==p_assign) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexer();
            oimemptr=(byte*)&mem[imem];
            tglo_init(1);
            if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
            imem=_imem;
          } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
        } else {
          (*ob).type=twloc; (*ob).wloc.offset=len;
          (*ob).wloc.len1=0;
          (*ob).wloc.len2=-1;
          (*ob).wloc.len3=-1;
          (*ob).wloc.totalen=1; // 1 int

          test_buffer(&frm,&ifrm_max,offstruct+len);
          frm[offstruct+len]=1;

          if (piece==p_assign) {
            save_error(1);
            lexer();
            test_buffer(&mem,&imem_max,offstruct+len);
            mem[offstruct+len]=constant();
            if (mem[offstruct+len]<0 || mem[offstruct+len]>65535) c_error(4,144);
          }
        } len+=(*ob).wloc.totalen;
      }

      if (piece==p_comma) piece=p_word; else {
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }

    } else if (piece==p_int || piece==p_id || piece==p_pointer) {

      if (piece==p_int) {
        lexer();
        if (piece!=p_id && piece!=p_pointer) c_error(1,149);
      }

      if (piece==p_pointer) { // Pointer to int member

        lexer(); pointer4:
        ob=analyze_pointer(tpilo,len);
        test_buffer(&mem,&imem_max,offstruct+len);
        if (piece==p_assign) {
          lexer(); mem[offstruct+len]=constant();
        } len+=1;
        if (piece==p_comma) {
          lexer(); if (piece==p_pointer) lexer();
          goto pointer4;
        }

      } else {

        ob=o; if ((*ob).type!=tnone) c_error(0,114); lexer();
        if (piece==p_lbracket) { lexer();    // Table member ────
          (*ob).type=ttloc; (*ob).tloc.offset=len;
          if (((*ob).tloc.len1=constant())<0) c_error(4,35);
          if (piece==p_comma) {
            lexer();
            if (((*ob).tloc.len2=constant())<0) c_error(4,35);
            if (piece==p_comma) {
              lexer();
              if (((*ob).tloc.len3=constant())<0) c_error(4,35);
            } else (*ob).tloc.len3=-1;
          } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
          (*ob).tloc.totalen=(*ob).tloc.len1+1;
          if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
          if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
          if (piece!=p_rbracket) c_error(3,26); lexer();

          test_buffer(&mem,&imem_max,offstruct+len+(*ob).tloc.totalen);
          if (piece==p_assign) {
            _imem=imem; imem=offstruct+len;
            save_error(1); lexer(); tglo_init(0);
            if (imem-(offstruct+len)>(*ob).tloc.totalen) c_error(4,33);
            imem=_imem;
          } len+=(*ob).tloc.totalen;

        } else {                                          // Variable member ────

          (*ob).type=tvloc; (*ob).vloc.offset=len;

          test_buffer(&mem,&imem_max,offstruct+len);
          if (piece==p_assign) {
            lexer(); mem[offstruct+len]=constant();
          } len+=1;

        }
      }

      if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
      while (piece==p_semicolon || piece==p_comma) lexer();

    } else c_error(0,58);
    (*ob).block=lexical_block;
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
  int filename;         // Pointer to the asciiz name
  int line1,column1;  // Position where the statement starts in the source
  int line2,column2;  // Position where the statement ends in the source
} dlls[64];

int idlls;              // Index of the previous structure
#endif

//───────────────────────────────────────────────────────────────────────────

void syntactic (void) {

  struct object * ob, * member2;
  int _imem,_imem_old,num_params,n;
  byte * old_source,*dll_name,*oimemptr, *include_name;
  int _itxt,dup;
  char cWork[256];

  int num_extern;

  lexer();

  //───────────────────────────────────────────────────────────────────────────
  // Compilation options
  //───────────────────────────────────────────────────────────────────────────
  // p_compiler_options {option} {;}
  //───────────────────────────────────────────────────────────────────────────

  max_process=0; // Default option values
  ignore_errors=0;
  free_syntax=0;
  extended_conditions=0;
  simple_conditions=0;
  check_range=1;
  check_id=1;
  check_null=1;
  do_strfix=1;
  optimize=1;

  if (piece==p_compiler_options) {
    lexer();
    do {
      if (piece==p_semicolon) { lexer(); break; }
      if (piece==p_comma) lexer();
      if (piece==p_id && (*o).type==tcons) {
        switch((*o).cons.value) {
          case 0: // _max_process
            lexer();
            if (piece!=p_assign) c_error(3,139);
            lexer();
            max_process=constant();
            if (max_process<0) max_process=0;
            break;
          case 1: // _extended_conditions
            lexer();
            extended_conditions=1;
            break;
          case 2: // _simple_conditions
            lexer();
            simple_conditions=1;
            break;
          case 3: // _case_sensitive
            lexer();
            memcpy(lower+129,"üéâäàåçêëèïîìäåéææôöòûùÿöüø£Ø×ƒáíóú",35);
            memcpy(lower+'A',"ABCDEFGHIJKLMNOPQRSTUVWXYZ",26);
            lower['Ñ']='Ñ';
            break;
          case 4: // _ignore_errors
            lexer();
            ignore_errors=1;
            break;
          case 5: // _free_sintax
            lexer();
            free_syntax=1;
            break;
          case 6: // _no_check
            lexer();
            check_range=0;
            check_id=0;
            check_null=0;
            break;
          case 7: // _no_strfix
            lexer();
            do_strfix=0;
            break;
          case 8: // _no_optimization
            lexer();
            optimize=0;
            break;
          case 9: // _no_range_check
            lexer();
            check_range=0;
            break;
          case 10: // _no_id_check
            lexer();
            check_id=0;
            break;
          case 11: // _no_null_check
            lexer();
            check_null=0;
            break;
          default:
            c_error(0,138);
            break;
        }
      } else {
        if (!free_syntax) {
          if (piece==p_program) c_error(3,66);
          else c_error(0,138);
        } else break;
      }
    } while (1);
  }

  //───────────────────────────────────────────────────────────────────────────
  // Header
  //───────────────────────────────────────────────────────────────────────────
  // p_program p_id {;}
  //───────────────────────────────────────────────────────────────────────────

  save_error(0);
  if (piece!=p_program && piece!=p_setup_program) c_error(4,20);

  if (piece==p_setup_program) {
    program_type=1; 
    strcpy((char *)cWork,"install/setup.ins");
  } else strcpy((char *)cWork,"system/exec.ins");
  if ((lins=fopen((char *)cWork,"wb"))==NULL) c_error(0,0);

  start_sentence(); lexer();
  if (piece!=p_id) c_error(1,21);
  ob=o; if ((*ob).type!=tnone) c_error(0,115); (*ob).type=tproc;
  (*ob).proc.block=current_block=ob;
  (*ob).proc.offset=0; (*ob).proc.num_params=0;
  lexer();
  if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
  while (piece==p_semicolon || piece==p_comma) lexer();
  end_sentence();

#ifdef DIVDLL
  dlls[0].line1=line1;
  dlls[0].column1=column1;
  dlls[0].line2=line2;
  dlls[0].column2=column2;
  idlls=1;

  //───────────────────────────────────────────────────────────────────────────
  // Import of dynamic libraries
  //───────────────────────────────────────────────────────────────────────────
  // {p_import p_lit {;}}
  //───────────────────────────────────────────────────────────────────────────

  PrevOrder=0;

  while (piece==p_import) {
    start_sentence();
    lexer();
    if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
		c_error(1,62);
	lexer();
    if (!free_syntax) 
		if (piece!=p_semicolon) 
			c_error(3,66);

    old_source=source;
    dll_name=(byte*)&mem[piece_num];

    if (idlls<64) 
		dlls[idlls].filename=piece_num;

//printf("DLL: %lx %d %s\n",&mem[piece_num],piece_num,dll_name);

    if (dll_name==NULL) c_error(0,63);
    if ((num_extern=ImportDll((char *)dll_name))==0) c_error(0,63);
    if (num_extern>0) {
		printf("num funcs: %d\n",num_extern);
      save_error(0);
      for (n=0;n<num_extern;n++) {
        source=(byte *)ImpDLL[n].Name;
        lexer();
        if (piece!=p_id) c_error(4,63);
        if ((*o).type!=tnone) c_error(4,63);
        (*o).type=tfext;
        (*o).fext.code=ImpDLL[n].Order;
        (*o).fext.num_params=ImpDLL[n].nParms;
      }
      free(ImpDLL);
      UnimportDll();
    }
    while (piece==p_semicolon || piece==p_comma) lexer();
    end_sentence();

    if (idlls<64) {
      dlls[idlls].line1=line1;
      dlls[idlls].column1=column1;
      dlls[idlls].line2=line2;
      dlls[idlls].column2=column2;
      idlls++;
    }
  }

#endif

	while(piece==p_include) {
		start_sentence();
		lexer();
		if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal)) c_error(1,62);
		old_source=source;
		include_name=(byte*)&mem[piece_num];


	//
		
		source=old_source; lexer();
		if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
		while (piece==p_semicolon || piece==p_comma) lexer();
		end_sentence();
	}
  //───────────────────────────────────────────────────────────────────────────
  // Constants section
  //───────────────────────────────────────────────────────────────────────────
  // [ p_const {;} { p_id p_assign <exp> {; | ,} } ]
  //───────────────────────────────────────────────────────────────────────────

  if (piece==p_const) {
    skip_semicolon();
    while (piece==p_id) {
      ob=o; if ((*ob).type!=tnone && (*ob).type!=tcons) c_error(0,116);
      (*ob).type=tcons;
      lexer(); if (piece!=p_assign) c_error(3,119);
      lexer();
      if (piece==p_lit || (piece==p_id && (*o).type==tcons && (*o).cons.literal))
        (*ob).cons.literal=1; else (*ob).cons.literal=0;
      (*ob).cons.value=constant();
      if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
      while (piece==p_semicolon || piece==p_comma) lexer();
    }
  }

  //───────────────────────────────────────────────────────────────────────────
  // Global variables, they can be used in constant expressions
  //───────────────────────────────────────────────────────────────────────────
  // <variable> := p_id [ p_assign <exp> ] {;}
  //
  // <table> := p_id p_lbracket ( p_rbracket p_assign <init>
  //                       | <exp> p_rbracket [ p_assign <init> ] ) {;}
  //
  // <init>  := ( <string> | <exp> [ [p_dup] p_lparen <init> p_rparen ] )
  //            [ p_comma <init>]
  //───────────────────────────────────────────────────────────────────────────

  if (piece==p_global) {
    skip_semicolon();
    while (piece==p_id || piece==p_int || piece==p_pointer || piece==p_struct || piece==p_string || piece==p_byte || piece==p_word) {

      if (piece==p_struct) {                            // Struct global

        lexer();

        if (piece==p_pointer) { // A pointer to struct is defined

          lexer(); if (piece!=p_id) c_error(1,110); ob=o;
          if ((*ob).type==tnone) c_error(0,150); // The pointer is not defined like this
          if ((*ob).type!=tsglo && (*ob).type!=tsloc) c_error(0,150);
          lexer();
          pointer_to_struct:
          analyze_pointer_struct(tpsgl,imem,ob);
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,imem++);

          if (piece==p_comma) {
            lexer();
            if (piece==p_pointer) lexer();
            goto pointer_to_struct;
          } else {
            if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
            while (piece==p_semicolon || piece==p_comma) lexer();
          }

        } else {

          if (piece!=p_id) c_error(1,110);
          ob=o; member=ob; lexer();

          if ((*ob).type!=tnone) c_error(2,22);

          (*ob).type=tsglo; (*ob).sglo.offset=_imem=imem;
          if (piece==p_lbracket) {
            member2=member; member=NULL; lexer();
            if (((*ob).sglo.items1=constant())<0) c_error(4,123);
            if (piece==p_comma) {
              lexer();
              if (((*ob).sglo.items2=constant())<0) c_error(4,123);
              if (piece==p_comma) {
                lexer();
                if (((*ob).sglo.items3=constant())<0) c_error(4,123);
              } else (*ob).sglo.items3=-1;
            } else { (*ob).sglo.items2=-1; (*ob).sglo.items3=-1; }
            member=member2;
            (*ob).sglo.totalitems=(*ob).sglo.items1+1;
            if ((*ob).sglo.items2>-1) (*ob).sglo.totalitems*=(*ob).sglo.items2+1;
            if ((*ob).sglo.items3>-1) (*ob).sglo.totalitems*=(*ob).sglo.items3+1;
            if (piece!=p_rbracket) c_error(3,26); lexer();
          } else {
            (*ob).sglo.totalitems=1;
            (*ob).sglo.items1=0; (*ob).sglo.items2=-1; (*ob).sglo.items3=-1;
          }
          if (((*ob).sglo.len_item=analyze_struct(_imem))==0) c_error(0,57);
          member=NULL; lexer();
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
          if (piece==p_assign) {
            save_error(1);
            lexer(); tglo_init(0);
            if (imem-_imem-1>=(*ob).sglo.len_item*(*ob).sglo.totalitems) c_error(4,120);
          } while (piece==p_semicolon) lexer();

          imem=_imem+(*ob).sglo.len_item*(*ob).sglo.totalitems;
          test_buffer(&mem,&imem_max,imem);
        }

      } else if (piece==p_string) {                     // Global string

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer1:
          ob=analyze_pointer(tpcgl,imem);
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer1;
          }

        } else {

          if (piece!=p_id) c_error(1,125);
          ob=o; if ((*ob).type!=tnone) c_error(0,126); lexer();
          (*ob).type=tcglo;
          _imem=imem;
          (*ob).cglo.offset=_imem+1;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer();
              (*ob).cglo.totalen=255;
            } else {
              if (((*ob).cglo.totalen=constant())<0) c_error(4,127);
              if ((*ob).cglo.totalen>0xFFFFF) c_error(4,135);
              if (piece!=p_rbracket) c_error(3,26);
              lexer();
            }
          } else (*ob).cglo.totalen=255;
          if (piece==p_assign) {
            save_error(1);
            _itxt=itxt;
            lexer();
            if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
              c_error(3,128);
            if (strlen((char*)&mem[piece_num])>(*ob).cglo.totalen+1)
              c_error(4,129);
            imem=_imem+1+((*ob).cglo.totalen+5)/4; // ej. c[32] -> c[0]..c[32],NUL
            test_buffer(&mem,&imem_max,imem);
            strcpy((char*)&mem[_imem+1],(char*)&mem[piece_num]);
            itxt=_itxt; // Take the string out of the text segment
            lexer();
          } else {
            imem=_imem+1+((*ob).cglo.totalen+5)/4;
            test_buffer(&mem,&imem_max,imem);
          } mem[_imem]=0xDAD00000|(*ob).cglo.totalen;
        }

        if (piece==p_comma) piece=p_string; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_byte) {                       // Global Byte table

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer2:
          ob=analyze_pointer(tpbgl,imem);
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer2;
          }

        } else {

          if (piece!=p_id) c_error(1,142);
          ob=o; if ((*ob).type!=tnone) c_error(0,117); lexer();
          (*ob).type=tbglo; (*ob).bglo.offset=_imem=imem;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              oimemptr=(byte*)&mem[imem];
              tglo_init(2);
              (*ob).bglo.len1=imemptr-oimemptr-1;
              (*ob).bglo.len2=-1;
              (*ob).bglo.len3=-1;
              (*ob).bglo.totalen=((*ob).bglo.len1+4)/4;
            } else {
              if (((*ob).bglo.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).bglo.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).bglo.len3=constant())<0) c_error(4,35);
                } else (*ob).bglo.len3=-1;
              } else { (*ob).bglo.len2=-1; (*ob).bglo.len3=-1; }

              (*ob).bglo.totalen=(*ob).bglo.len1+1;
              if ((*ob).bglo.len2>-1) (*ob).bglo.totalen*=(*ob).bglo.len2+1;
              if ((*ob).bglo.len3>-1) (*ob).bglo.totalen*=(*ob).bglo.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer();
                oimemptr=(byte*)&mem[imem];
                tglo_init(2);
                if (imemptr-oimemptr>(*ob).bglo.totalen) c_error(4,33);
              } (*ob).bglo.totalen=((*ob).bglo.totalen+3)/4;
            }
          } else {                                          // Byte global
            (*ob).type=tbglo; (*ob).bglo.offset=imem;
            (*ob).bglo.len1=0;
            (*ob).bglo.len2=-1;
            (*ob).bglo.len3=-1;
            (*ob).bglo.totalen=1; // 1 int
            if (piece==p_assign) {
              save_error(1);
              lexer();
              mem[imem]=constant();
              if (mem[imem]<0 || mem[imem]>255) c_error(4,143);
            }
          } imem=_imem+(*ob).bglo.totalen; test_buffer(&mem,&imem_max,imem);
        }

        if (piece==p_comma) piece=p_byte; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_word) {                       // Global Word table

        lexer();

        if (piece==p_pointer) { // Pointer to word

          lexer(); pointer3:
          ob=analyze_pointer(tpwgl,imem);
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer3;
          }

        } else {

          if (piece!=p_id) c_error(1,145);
          ob=o; if ((*ob).type!=tnone) c_error(0,117); lexer();
          (*ob).type=twglo; (*ob).wglo.offset=_imem=imem;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              oimemptr=(byte*)&mem[imem];
              tglo_init(1);
              (*ob).wglo.len1=(imemptr-oimemptr)/2-1;
              (*ob).wglo.len2=-1;
              (*ob).wglo.len3=-1;
              (*ob).wglo.totalen=((*ob).wglo.len1+2)/2;
            } else {
              if (((*ob).wglo.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).wglo.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).wglo.len3=constant())<0) c_error(4,35);
                } else (*ob).wglo.len3=-1;
              } else { (*ob).wglo.len2=-1; (*ob).wglo.len3=-1; }

              (*ob).wglo.totalen=(*ob).wglo.len1+1;
              if ((*ob).wglo.len2>-1) (*ob).wglo.totalen*=(*ob).wglo.len2+1;
              if ((*ob).wglo.len3>-1) (*ob).wglo.totalen*=(*ob).wglo.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer();
                oimemptr=(byte*)&mem[imem];
                tglo_init(1);
                if (imemptr-oimemptr>(*ob).wglo.totalen*2) c_error(4,33);
              } (*ob).wglo.totalen=((*ob).wglo.totalen+1)/2;
            }
          } else {                                          // Word global
            (*ob).type=twglo; (*ob).wglo.offset=imem;
            (*ob).wglo.len1=0;
            (*ob).wglo.len2=-1;
            (*ob).wglo.len3=-1;
            (*ob).wglo.totalen=1; // 1 int
            if (piece==p_assign) {
              save_error(1);
              lexer();
              mem[imem]=constant();
              if (mem[imem]<0 || mem[imem]>65535) c_error(4,144);
            }
          } imem=_imem+(*ob).wglo.totalen; test_buffer(&mem,&imem_max,imem);
        }

        if (piece==p_comma) piece=p_word; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_int || piece==p_id || piece==p_pointer) {

        if (piece==p_int) {
          lexer();
          if (piece!=p_id && piece!=p_pointer) c_error(1,149);
        }

        if (piece==p_pointer) { // Pointer to int

          lexer(); pointer4:
          ob=analyze_pointer(tpigl,imem);
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer4;
          }

        } else {

          ob=o; if ((*ob).type!=tnone) c_error(0,117); lexer();
          if (piece==p_lbracket) {                           // Global table
            lexer();
            (*ob).type=ttglo; (*ob).tglo.offset=_imem=imem;
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              tglo_init(3);
              (*ob).tglo.len1=imem-_imem-1;
              (*ob).tglo.len2=-1;
              (*ob).tglo.len3=-1;
              (*ob).tglo.totalen=imem-_imem;
            } else {
              if (((*ob).tglo.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).tglo.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).tglo.len3=constant())<0) c_error(4,35);
                } else (*ob).tglo.len3=-1;
              } else { (*ob).tglo.len2=-1; (*ob).tglo.len3=-1; }
              (*ob).tglo.totalen=(*ob).tglo.len1+1;
              if ((*ob).tglo.len2>-1) (*ob).tglo.totalen*=(*ob).tglo.len2+1;
              if ((*ob).tglo.len3>-1) (*ob).tglo.totalen*=(*ob).tglo.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer(); tglo_init(3);
                if (imem-_imem>(*ob).tglo.totalen) c_error(4,33); }
            }
            imem=_imem+(*ob).tglo.totalen;
            test_buffer(&mem,&imem_max,imem);

          } else {                                          // Variable global

            (*ob).type=tvglo; (*ob).vglo.offset=imem;
            if (piece==p_assign) { lexer(); mem[imem]=constant(); }
            test_buffer(&mem,&imem_max,++imem);

          }
        }
        if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }
    }
  }

  //───────────────────────────────────────────────────────────────────────────
  // Local variables, they cannot be used in constant expressions
  //───────────────────────────────────────────────────────────────────────────
  // <variable> := p_id [ p_assign <exp> ] {;}
  //
  // <table> := p_id p_lbracket ( p_rbracket p_assign <init>
  //                       | <exp> p_rbracket [ p_assign <init> ] ) {;}
  //───────────────────────────────────────────────────────────────────────────

  if (piece==p_local) {
    skip_semicolon();
    while (piece==p_id || piece==p_int || piece==p_pointer || piece==p_struct || piece==p_string || piece==p_byte || piece==p_word) {

      if (piece==p_struct) {

        lexer();

        if (piece==p_pointer) { // A pointer to struct is defined

          lexer(); if (piece!=p_id) c_error(1,110); ob=o;
          if ((*ob).type==tnone) c_error(0,150); // The pointer is not defined like this
          if ((*ob).type!=tsglo && (*ob).type!=tsloc) c_error(0,150);
          lexer();
          pointer_to_local_struct:
          analyze_pointer_struct(tpslo,iloc,ob);
          if (piece==p_assign) { lexer(); loc[iloc]=constant(); }
          test_buffer(&loc,&iloc_max,iloc++);

          if (piece==p_comma) {
            lexer();
            if (piece==p_pointer) lexer();
            goto pointer_to_local_struct;
          } else {
            if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
            while (piece==p_semicolon || piece==p_comma) lexer();
          }

        } else {

          if (piece!=p_id) c_error(1,110);
          ob=o; member=ob; lexer();

          if ((*ob).type!=tnone) c_error(2,22);

          (*ob).type=tsloc; (*ob).sloc.offset=_imem=iloc;
          if (piece==p_lbracket) {
            member2=member; member=NULL; lexer();
            if (((*ob).sloc.items1=constant())<0) c_error(4,123);
            if (piece==p_comma) {
              lexer();
              if (((*ob).sloc.items2=constant())<0) c_error(4,123);
              if (piece==p_comma) {
                lexer();
                if (((*ob).sloc.items3=constant())<0) c_error(4,123);
              } else (*ob).sloc.items3=-1;
            } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
            member=member2;
            (*ob).sloc.totalitems=(*ob).sloc.items1+1;
            if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
            if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
            if (piece!=p_rbracket) c_error(3,26); lexer();
          } else {
            (*ob).sloc.totalitems=1;
            (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
          }
          if (((*ob).sloc.len_item=analyze_struct_local(_imem))==0) c_error(0,57);
          member=NULL; lexer();
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
          if (piece==p_assign) {
            save_error(1); lexer(); tloc_init(0);
            if (iloc-_imem-1>=(*ob).sloc.len_item*(*ob).sloc.totalitems) c_error(4,120);
          } while (piece==p_semicolon) lexer();
          iloc=_imem+(*ob).sloc.len_item*(*ob).sloc.totalitems;
          test_buffer(&loc,&iloc_max,iloc);
        }

      } else if (piece==p_string) {                     // Local string

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer5:
          ob=analyze_pointer(tpclo,iloc);
          if (piece==p_assign) { lexer(); loc[iloc]=constant(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer5;
          }

        } else {

          if (piece!=p_id) c_error(1,125);
          ob=o; if ((*ob).type!=tnone) c_error(0,126); lexer();
          (*ob).type=tcloc;
          _imem=iloc;
          (*ob).cloc.offset=_imem+1;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer();
              (*ob).cloc.totalen=255;
            } else {
              if (((*ob).cloc.totalen=constant())<0) c_error(4,127);
              if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
              if (piece!=p_rbracket) c_error(3,26);
              lexer();
            }
          } else (*ob).cloc.totalen=255;
          if (piece==p_assign) {
            save_error(1);
            _itxt=itxt;
            lexer();
            if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
              c_error(3,128);
            if (strlen((char*)&mem[piece_num])>(*ob).cloc.totalen+1)
              c_error(4,129);
            iloc=_imem+1+((*ob).cloc.totalen+5)/4; // ej. c[32] -> c[0]..c[32],NUL
            test_buffer(&loc,&iloc_max,iloc);
            strcpy((char*)&loc[_imem+1],(char*)&mem[piece_num]);
            itxt=_itxt; // Take the string out of the text segment
            lexer();
          } else {
            iloc=_imem+1+((*ob).cloc.totalen+5)/4;
            test_buffer(&loc,&iloc_max,iloc);
          } loc[_imem]=0xDAD00000|(*ob).cloc.totalen;
        }

        if (piece==p_comma) piece=p_string; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_byte) {                       // Local Byte table

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer6:
          ob=analyze_pointer(tpblo,iloc);
          if (piece==p_assign) { lexer(); loc[iloc]=constant(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer6;
          }

        } else {

          if (piece!=p_id) c_error(1,142);
          ob=o; if ((*ob).type!=tnone) c_error(0,117); lexer();
          (*ob).type=tbloc; (*ob).bloc.offset=_imem=iloc;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              oimemptr=(byte*)&loc[iloc];
              tloc_init(2);
              (*ob).bloc.len1=imemptr-oimemptr-1;
              (*ob).bloc.len2=-1;
              (*ob).bloc.len3=-1;
              (*ob).bloc.totalen=((*ob).bloc.len1+4)/4;
            } else {
              if (((*ob).bloc.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).bloc.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).bloc.len3=constant())<0) c_error(4,35);
                } else (*ob).bloc.len3=-1;
              } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }

              (*ob).bloc.totalen=(*ob).bloc.len1+1;
              if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
              if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer();
                oimemptr=(byte*)&loc[iloc];
                tloc_init(2);
                if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
              } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
            }
          } else {                                          // Byte local
            (*ob).type=tbloc; (*ob).bloc.offset=iloc;
            (*ob).bloc.len1=0;
            (*ob).bloc.len2=-1;
            (*ob).bloc.len3=-1;
            (*ob).bloc.totalen=1; // 1 int
            if (piece==p_assign) {
              save_error(1);
              lexer();
              loc[iloc]=constant();
              if (loc[iloc]<0 || loc[iloc]>255) c_error(4,143);
            }
          } iloc=_imem+(*ob).bloc.totalen; test_buffer(&loc,&iloc_max,iloc);
        }

        if (piece==p_comma) piece=p_byte; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_word) {                       // Local Word table

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer7:
          ob=analyze_pointer(tpwlo,iloc);
          if (piece==p_assign) { lexer(); loc[iloc]=constant(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer7;
          }

        } else {

          if (piece!=p_id) c_error(1,145);
          ob=o; if ((*ob).type!=tnone) c_error(0,117); lexer();
          (*ob).type=twloc; (*ob).wloc.offset=_imem=iloc;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              oimemptr=(byte*)&loc[iloc];
              tloc_init(1);
              (*ob).wloc.len1=(imemptr-oimemptr)/2-1;
              (*ob).wloc.len2=-1;
              (*ob).wloc.len3=-1;
              (*ob).wloc.totalen=((*ob).wloc.len1+2)/2;
            } else {
              if (((*ob).wloc.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).wloc.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).wloc.len3=constant())<0) c_error(4,35);
                } else (*ob).wloc.len3=-1;
              } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }

              (*ob).wloc.totalen=(*ob).wloc.len1+1;
              if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
              if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer();
                oimemptr=(byte*)&loc[iloc];
                tloc_init(1);
                if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
              } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
            }
          } else {                                          // Word local
            (*ob).type=twloc; (*ob).wloc.offset=iloc;
            (*ob).wloc.len1=0;
            (*ob).wloc.len2=-1;
            (*ob).wloc.len3=-1;
            (*ob).wloc.totalen=1; // 1 int
            if (piece==p_assign) {
              save_error(1);
              lexer();
              loc[iloc]=constant();
              if (loc[iloc]<0 || loc[iloc]>65535) c_error(4,144);
            }
          } iloc=_imem+(*ob).wloc.totalen; test_buffer(&loc,&iloc_max,iloc);
        }

        if (piece==p_comma) piece=p_word; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_int || piece==p_id || piece==p_pointer) {

        if (piece==p_int) {
          lexer();
          if (piece!=p_id && piece!=p_pointer) c_error(1,149);
        }

        if (piece==p_pointer) { // Pointer to int member

          lexer(); pointer8:
          ob=analyze_pointer(tpilo,iloc);
          if (piece==p_assign) { lexer(); loc[iloc]=constant(); }
          test_buffer(&loc,&iloc_max,++iloc);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer8;
          }

        } else {

          ob=o; if ((*ob).type!=tnone) c_error(0,117); lexer();
          if (piece==p_lbracket) {                             // Local table
            lexer();
            (*ob).type=ttloc; (*ob).tloc.offset=_imem=iloc;
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              tloc_init(3);
              (*ob).tloc.len1=iloc-_imem-1;
              (*ob).tloc.len2=-1;
              (*ob).tloc.len3=-1;
              (*ob).tloc.totalen=iloc-_imem;
            } else {
              if (((*ob).tloc.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).tloc.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).tloc.len3=constant())<0) c_error(4,35);
                } else (*ob).tloc.len3=-1;
              } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
              (*ob).tloc.totalen=(*ob).tloc.len1+1;
              if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
              if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer(); tloc_init(3);
                if (iloc-_imem>(*ob).tloc.totalen) c_error(4,33); }
            }
            iloc=_imem+(*ob).tloc.totalen;
            test_buffer(&loc,&iloc_max,iloc);

          } else {                                          // Variable local

            (*ob).type=tvloc; (*ob).vloc.offset=iloc;
            if (piece==p_assign) { lexer(); loc[iloc]=constant(); }
            test_buffer(&loc,&iloc_max,++iloc);

          }
        }
        if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
      }
    }
  }

  //───────────────────────────────────────────────────────────────────────────
  // Main code
  //───────────────────────────────────────────────────────────────────────────

  // Generate the jump to the start of the code (long_header)

  mem[0]=program_type; mem[1]=imem; iloc_len=iloc;

  g2(ltyp,(memptrsize)current_block);
  g2(lcbp,0);
#ifdef DIVDLL
  line1=dlls[0].line1;
  column1=dlls[0].column1;
  line2=dlls[0].line2;
  column2=dlls[0].column2;
  start=0; final=imem-1;
  record_sentence();

  if (piece==p_import) c_error(0,147); // Warn DIV 1 users

  // Generate the code to load the libraries

  for (n=1;n<idlls;n++) {
    start=imem; final=imem+1;
    g2(limp,dlls[n].filename);
    line1=dlls[n].line1;
    column1=dlls[n].column1;
    line2=dlls[n].line2;
    column2=dlls[n].column2;
    record_sentence();
  }

#endif

  // The first FRAME, and the loading of PRIVATE variables, are executed
  // together in the BEGIN of the main program.

  final=imem;

  parameters=-1;
  lexical_block=current_block;
  n=iloc; analyze_private();
  if (iloc>iloc_len) iloc_len=iloc; iloc=n;
  parameters=0;

  g1(lfrm);

  if (piece!=p_begin) c_error(0,29);
  start_sentence(); start=final;
  lexer(); end_sentence(); record_sentence();

  sentence();

  if (piece!=p_end) c_error(0,40); start_sentence();
  g1(lret);
  skip_semicolon(); end_sentence(); record_sentence();

  //───────────────────────────────────────────────────────────────────────────
  // Processes
  //───────────────────────────────────────────────────────────────────────────
  // p_process p_id p_lparen [ id { , id } ] p_rparen { ; } p_begin ... p_end
  //───────────────────────────────────────────────────────────────────────────

  save_error(0);
  while (piece==p_process || piece==p_function) {
    n=piece; start_sentence(); lexer();
    if (piece!=p_id) c_error(1,111); ob=o; lexer();
    if ((*ob).type==tproc && (*ob).used) {
      num_params=(*ob).proc.num_params; lexical_block=current_block=ob;
      _imem=(*ob).proc.offset; while(_imem) {
         _imem_old=mem[_imem]; mem[_imem]=imem; _imem=_imem_old; }
    } else if ((*ob).type==tnone) {
      (*ob).used=0; (*ob).type=tproc;
      (*ob).proc.block=lexical_block=current_block=ob;
    } else c_error(2,118);
    (*ob).proc.offset=imem; (*ob).proc.num_params=0;

    g2(ltyp,(memptrsize)current_block);
    if (n==p_function) g1(lnop);
    g2(lcbp,0); _imem=imem-1;
    if (piece!=p_lparen) c_error(3,36);

    parameters=1; n=iloc; lexer();

    while (piece!=p_rparen) {
      (*ob).proc.num_params++; expression_cpa();
      if (piece!=p_rparen) if (piece!=p_comma) c_error(3,25);
        else { lexer(); if (piece==p_rparen) c_error(3,45); }
    } if ((*ob).used)
      if (num_params==(*ob).proc.num_params) (*ob).used=0; else c_error(0,46);

    skip_semicolon(); end_sentence();

    if (parameters>1) {
      g2(lpar,parameters-1);
    }

    parameters=-1; // So that the parameters can be repeated as PRIVATE

    num_params=mem[_imem]=(*ob).proc.num_params;

    analyze_private(); parameters=0;

    if (iloc>iloc_len) iloc_len=iloc; iloc=n;

    if (piece!=p_begin) c_error(0,29); lexer();
    final=imem-1; record_sentence();
    sentence();
    if (piece!=p_end) c_error(0,40); start_sentence();
    g1(lret);
    skip_semicolon(); end_sentence(); record_sentence();

    save_error(0);
  }

  if (piece!=p_last) c_error(4,52);

}

  //───────────────────────────────────────────────────────────────────────────
  // Parse the private variables of a block, like the locals
  //───────────────────────────────────────────────────────────────────────────

    // *** NOTE *** #id.tvpri must not be allowed
    //             since it would fail unless #id were of the same type as the
    //             current process (brother)

void analyze_private(void) {
  struct object * ob, * member2;
  int _imem,_imem_old,_itxt,dup;
  byte *oimemptr;

  if (piece==p_private) {
    skip_semicolon();

    g2(lpri,0); _imem_old=imem-1;

    while (piece==p_id || piece==p_int || piece==p_pointer || piece==p_struct || piece==p_string || piece==p_byte || piece==p_word) {

      if (piece==p_struct) {

        lexer();

        if (piece==p_pointer) { // A pointer to struct is defined

          lexer(); if (piece!=p_id) c_error(1,110); ob=o;
          if ((*ob).type==tnone) c_error(0,150); // The pointer is not defined like this
          if ((*ob).type!=tsglo && (*ob).type!=tsloc) c_error(0,150);
          lexer();
          pointer_to_struct:
          if (analyze_pointer_struct(tpslo,iloc,ob)==1) {
            iloc++;
            if (piece==p_assign) { lexer(); mem[imem]=constant(); }
            test_buffer(&mem,&imem_max,++imem);
          }

          if (piece==p_comma) {
            lexer();
            if (piece==p_pointer) lexer();
            goto pointer_to_struct;
          } else {
            if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
            while (piece==p_semicolon || piece==p_comma) lexer();
          }

        } else {

          if (piece!=p_id) c_error(1,110);
          ob=o; member=ob; lexer();

          if ((*ob).type!=tnone) c_error(2,22);

          (*ob).type=tsloc; (*ob).sloc.offset=_imem=imem;
          if (piece==p_lbracket) {
            member2=member; member=NULL; lexer();
            if (((*ob).sloc.items1=constant())<0) c_error(4,123);
            if (piece==p_comma) {
              lexer();
              if (((*ob).sloc.items2=constant())<0) c_error(4,123);
              if (piece==p_comma) {
                lexer();
                if (((*ob).sloc.items3=constant())<0) c_error(4,123);
              } else (*ob).sloc.items3=-1;
            } else { (*ob).sloc.items2=-1; (*ob).sloc.items3=-1; }
            member=member2;
            (*ob).sloc.totalitems=(*ob).sloc.items1+1;
            if ((*ob).sloc.items2>-1) (*ob).sloc.totalitems*=(*ob).sloc.items2+1;
            if ((*ob).sloc.items3>-1) (*ob).sloc.totalitems*=(*ob).sloc.items3+1;
            if (piece!=p_rbracket) c_error(3,26); lexer();
          } else {
            (*ob).sloc.totalitems=1;
            (*ob).sloc.items1=0; (*ob).sloc.items2=-1; (*ob).sloc.items3=-1;
          }
          if (((*ob).sloc.len_item=analyze_struct_private(_imem))==0) c_error(0,57);

          member=NULL; lexer();

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

          if (piece==p_assign) {
            save_error(1); lexer(); tglo_init(0);
            if (imem-_imem-1>=(*ob).sloc.len_item*(*ob).sloc.totalitems) c_error(4,120);
          } while (piece==p_semicolon) lexer();

          imem=_imem+(*ob).sloc.len_item*(*ob).sloc.totalitems;
          (*ob).sloc.offset=iloc; iloc+=(*ob).sloc.len_item*(*ob).sloc.totalitems;
          test_buffer(&mem,&imem_max,imem);
        }

      } else if (piece==p_string) {                     // Private string

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer1:
          ob=analyze_pointer(tpclo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer1;
          }

        } else {

          if (piece!=p_id) c_error(1,125);
          ob=o; if ((*ob).type!=tnone) { // Check whether a parameter is repeated ...
            if (parameters==-1 && (*ob).param==1 && (*ob).block==current_block) {
              if ((*ob).type==tcloc) { // A string is repeated
                save_error(0);
                lexer();
                if (piece==p_lbracket) {
                  lexer();
                  if (piece==p_rbracket) {
                    lexer();
                    dup=255;
                  } else {
                    dup=constant();
                    if (piece!=p_rbracket) c_error(3,26);
                    lexer();
                  }
                } else dup=255;
                if (dup!=(*ob).cloc.totalen) c_error(4,141);
                else if (piece==p_assign) c_error(0,54);
                else {
                  while (piece==p_semicolon || piece==p_comma) lexer();
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,126);
            } else c_error(0,126);
          } else lexer();

          (*ob).type=tcloc;
          _imem=imem;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer();
              (*ob).cloc.totalen=255;
            } else {
              if (((*ob).cloc.totalen=constant())<0) c_error(4,127);
              if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
              if (piece!=p_rbracket) c_error(3,26);
              lexer();
            }
          } else (*ob).cloc.totalen=255;
          if (piece==p_assign) {
            save_error(1);
            _itxt=itxt;
            lexer();
            if (piece!=p_lit && !(piece==p_id && (*o).type==tcons && (*o).cons.literal))
              c_error(3,128);
            if (strlen((char*)&mem[piece_num])>(*ob).cloc.totalen+1)
              c_error(4,129);
            imem=_imem+1+((*ob).cloc.totalen+5)/4; // ej. c[32] -> c[0]..c[32],NUL
            test_buffer(&mem,&imem_max,imem);
            strcpy((char*)&mem[_imem+1],(char*)&mem[piece_num]);
            itxt=_itxt; // Take the string out of the text segment
            lexer();
          } else {
            imem=_imem+1+((*ob).cloc.totalen+5)/4;
            test_buffer(&mem,&imem_max,imem);
          } mem[_imem]=0xDAD00000|(*ob).cloc.totalen;
          (*ob).cloc.offset=iloc+1;
          iloc+=1+((*ob).cloc.totalen+5)/4;
        }

        if (piece==p_comma) piece=p_string; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_byte) {                       // Private Byte table

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer2:
          ob=analyze_pointer(tpblo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer2;
          }

        } else {

          if (piece!=p_id) c_error(1,142);
          ob=o; if ((*ob).type!=tnone) {
            if (parameters==-1 && (*ob).param==1 && (*ob).block==current_block) {
              if ((*ob).type==tbloc) { // A byte parameter is repeated
                lexer();
                if (piece==p_lbracket) c_error(2,53);
                else if (piece==p_assign) c_error(0,54);
                else {
                  while (piece==p_semicolon || piece==p_comma) {
                    lexer();
                  }
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,117);
            } else c_error(0,117);
          } else lexer();

          (*ob).type=tbloc; _imem=imem;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              oimemptr=(byte*)&mem[imem];
              tglo_init(2);
              (*ob).bloc.len1=imemptr-oimemptr-1;
              (*ob).bloc.len2=-1;
              (*ob).bloc.len3=-1;
              (*ob).bloc.totalen=((*ob).bloc.len1+4)/4;
            } else {
              if (((*ob).bloc.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).bloc.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).bloc.len3=constant())<0) c_error(4,35);
                } else (*ob).bloc.len3=-1;
              } else { (*ob).bloc.len2=-1; (*ob).bloc.len3=-1; }

              (*ob).bloc.totalen=(*ob).bloc.len1+1;
              if ((*ob).bloc.len2>-1) (*ob).bloc.totalen*=(*ob).bloc.len2+1;
              if ((*ob).bloc.len3>-1) (*ob).bloc.totalen*=(*ob).bloc.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer();
                oimemptr=(byte*)&mem[imem];
                tglo_init(2);
                if (imemptr-oimemptr>(*ob).bloc.totalen) c_error(4,33);
              } (*ob).bloc.totalen=((*ob).bloc.totalen+3)/4;
            }
          } else {                                          // Private byte
            (*ob).type=tbloc; (*ob).bloc.offset=imem;
            (*ob).bloc.len1=0;
            (*ob).bloc.len2=-1;
            (*ob).bloc.len3=-1;
            (*ob).bloc.totalen=1; // 1 int
            if (piece==p_assign) {
              save_error(1);
              lexer();
              mem[imem]=constant();
              if (mem[imem]<0 || mem[imem]>255) c_error(4,143);
            }
          }
          imem=_imem+(*ob).bloc.totalen; test_buffer(&mem,&imem_max,imem);
          (*ob).bloc.offset=iloc; iloc+=(*ob).bloc.totalen;
        }

        if (piece==p_comma) piece=p_byte; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_word) {                       // Private Word table

        lexer();

        if (piece==p_pointer) { // Pointer to byte

          lexer(); pointer3:
          ob=analyze_pointer(tpwlo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer3;
          }

        } else {

          if (piece!=p_id) c_error(1,145);
          ob=o; if ((*ob).type!=tnone) {
            if (parameters==-1 && (*ob).param==1 && (*ob).block==current_block) {
              if ((*ob).type==twloc) { // A word parameter is repeated
                lexer();
                if (piece==p_lbracket) c_error(2,53);
                else if (piece==p_assign) c_error(0,54);
                else {
                  while (piece==p_semicolon || piece==p_comma) {
                    lexer();
                  }
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,117);
            } else c_error(0,117);
          } else lexer();

          (*ob).type=twloc; _imem=imem;
          if (piece==p_lbracket) {
            lexer();
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              oimemptr=(byte*)&mem[imem];
              tglo_init(1);
              (*ob).wloc.len1=(imemptr-oimemptr)/2-1;
              (*ob).wloc.len2=-1;
              (*ob).wloc.len3=-1;
              (*ob).wloc.totalen=((*ob).wloc.len1+2)/2;
            } else {
              if (((*ob).wloc.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).wloc.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).wloc.len3=constant())<0) c_error(4,35);
                } else (*ob).wloc.len3=-1;
              } else { (*ob).wloc.len2=-1; (*ob).wloc.len3=-1; }

              (*ob).wloc.totalen=(*ob).wloc.len1+1;
              if ((*ob).wloc.len2>-1) (*ob).wloc.totalen*=(*ob).wloc.len2+1;
              if ((*ob).wloc.len3>-1) (*ob).wloc.totalen*=(*ob).wloc.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1);
                lexer();
                oimemptr=(byte*)&mem[imem];
                tglo_init(1);
                if (imemptr-oimemptr>(*ob).wloc.totalen*2) c_error(4,33);
              } (*ob).wloc.totalen=((*ob).wloc.totalen+1)/2;
            }
          } else {                                          // Private word
            (*ob).type=twloc; (*ob).wloc.offset=imem;
            (*ob).wloc.len1=0;
            (*ob).wloc.len2=-1;
            (*ob).wloc.len3=-1;
            (*ob).wloc.totalen=1; // 1 int
            if (piece==p_assign) {
              save_error(1);
              lexer();
              mem[imem]=constant();
              if (mem[imem]<0 || mem[imem]>65535) c_error(4,144);
            }
          }
          imem=_imem+(*ob).wloc.totalen; test_buffer(&mem,&imem_max,imem);
          (*ob).wloc.offset=iloc; iloc+=(*ob).wloc.totalen;
        }

        if (piece==p_comma) piece=p_word; else {
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,137);
          while (piece==p_semicolon || piece==p_comma) lexer();
        }

      } else if (piece==p_int || piece==p_id || piece==p_pointer) {

        if (piece==p_int) {
          lexer();
          if (piece!=p_id && piece!=p_pointer) c_error(1,149);
        }

        if (piece==p_pointer) { // Pointer to int member

          lexer(); pointer4:
          ob=analyze_pointer(tpilo,iloc);
          if (ob==NULL) continue; else iloc++;
          if (piece==p_assign) { lexer(); mem[imem]=constant(); }
          test_buffer(&mem,&imem_max,++imem);
          if (piece==p_comma) {
            lexer(); if (piece==p_pointer) lexer();
            goto pointer4;
          }

        } else {

          // If the object is not tnone, a parameter is repeated or else it is an error

          ob=o; if ((*ob).type!=tnone) {
            if (parameters==-1 && (*ob).param==1 && (*ob).block==current_block) {
              if ((*ob).type==tvloc) { // A local variable is repeated
                lexer();
                if (piece==p_lbracket) c_error(2,53);
                else if (piece==p_assign) c_error(0,54);
                else {
                  while (piece==p_semicolon || piece==p_comma) {
                    lexer();
                  }
                  (*ob).param++;
                  continue;
                }
              } else c_error(0,117);
            } else c_error(0,117);
          } else lexer();

          if (piece==p_lbracket) {                             // Private table
            lexer();
            (*ob).type=ttloc; _imem=imem;
            if (piece==p_rbracket) {
              lexer(); if (piece!=p_assign) c_error(3,23); lexer();
              tglo_init(3);
              (*ob).tloc.len1=imem-_imem-1;
              (*ob).tloc.len2=-1;
              (*ob).tloc.len3=-1;
              (*ob).tloc.totalen=imem-_imem;
            } else {
              if (((*ob).tloc.len1=constant())<0) c_error(4,35);
              if (piece==p_comma) {
                lexer();
                if (((*ob).tloc.len2=constant())<0) c_error(4,35);
                if (piece==p_comma) {
                  lexer();
                  if (((*ob).tloc.len3=constant())<0) c_error(4,35);
                } else (*ob).tloc.len3=-1;
              } else { (*ob).tloc.len2=-1; (*ob).tloc.len3=-1; }
              (*ob).tloc.totalen=(*ob).tloc.len1+1;
              if ((*ob).tloc.len2>-1) (*ob).tloc.totalen*=(*ob).tloc.len2+1;
              if ((*ob).tloc.len3>-1) (*ob).tloc.totalen*=(*ob).tloc.len3+1;
              if (piece!=p_rbracket) c_error(3,26); lexer();
              if (piece==p_assign) {
                save_error(1); lexer(); tglo_init(3);
                if (imem-_imem>(*ob).tloc.totalen) c_error(4,33); }
            }
            imem=_imem+(*ob).tloc.totalen;
            test_buffer(&mem,&imem_max,imem);
            (*ob).tloc.offset=iloc; iloc+=(*ob).tloc.totalen;

          } else {                                          // Private variable

            (*ob).type=tvloc; (*ob).vloc.offset=iloc++;
            if (piece==p_assign) { lexer(); mem[imem]=constant(); }
            test_buffer(&mem,&imem_max,++imem);
          }
        }
        if (!free_syntax) if (piece!=p_semicolon && piece!=p_comma) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
      } (*ob).block=current_block;
    }

    mem[_imem_old]=imem;

  };

}

  //───────────────────────────────────────────────────────────────────────────
  // Table initialization (to mem[imem++])
  //───────────────────────────────────────────────────────────────────────────
  // <init>  := ( <string> | <exp> [ [p_dup] p_lparen <init> p_rparen ] )
  //            [ p_comma <init>]
  //───────────────────────────────────────────────────────────────────────────


void tglo_init(memptrsize type) { // Type: 0-Int or mixed (struct), 1-Word, 2-Byte
  if (type==3) { memset(frm,0,ifrm_max*sizeof(memptrsize)); type=0; }
  imemptr=(byte*)&mem[imem];
  tglo_init2(type);
  if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
}

void tloc_init(int type) {
  int * ptr;

  if (type==3) { memset(frm,0,ifrm_max*sizeof(memptrsize)); type=0; }

  ptr=mem; mem=loc; loc=ptr;
  swap(imem,iloc); swap(imem_max,iloc_max);

  imemptr=(byte*)&mem[imem];
  tglo_init2(type);

  ptr=mem; mem=loc; loc=ptr;
  swap(imem,iloc); swap(imem_max,iloc_max);

  if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
}

// NOTE!!! tglo_init cannot call test_buffer !!!

#if __WORDSIZE == 64
#define memptrsize int64_t
#else
#define memptrsize int32_t
#endif


void tglo_init2(int type) {
  int value,dup,_imem,len,n;
  byte *oimemptr;

  while (1) {

    // Check whether the data initialization ends

    if (piece==p_rparen || piece==p_semicolon) {
      if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || type==1) {
        imemptr+=2;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && type!=1) imemptr=(byte*)&mem[imem];
      } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || type==2) {
        imemptr++;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && type!=2) imemptr=(byte*)&mem[imem];
      } else {
        imemptr+=4;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
      }
//      test_buffer(&mem,&imem_max,imem);
      return;
    }

    // A lone comma, as in "3,,4", advances one position (and defines a 0)

    if (piece==p_comma) {
      if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || type==1) {
        (*(word*)imemptr)=0;
        imemptr+=2;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && type!=1) imemptr=(byte*)&mem[imem];
      } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || type==2) {
        *imemptr++=0;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && type!=2) imemptr=(byte*)&mem[imem];
      } else {
        imemptr+=4;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
      }
//      test_buffer(&mem,&imem_max,imem);
      lexer(); continue;
    }

    if (piece==p_lparen) { // Dup x 1

      dup=1;

    } else if (piece==p_dup) { // Dup x 2

      dup=2; lexer();
      if (piece!=p_lparen) c_error(3,36);

    } else if (piece==p_lit || (piece==p_id && (*o).type==tcons && (*o).cons.literal)) {

      value=piece_num; lexer();
      if (piece==p_lparen || piece==p_dup) c_error(2,38);

      // Put a literal into a string type datum

      if ((((memptrsize)imemptr-(memptrsize)mem)%4)==0) {
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (frm[imem]==0xdad00000) {
          if (strlen((char*)&mem_ory[value])>(mem[imem]&0xFFFFF)+1) c_error(2,129);
          _imem=imem;
          imem+=1+((mem[imem]&0xFFFFF)+5)/4;
//          test_buffer(&mem,&imem_max,imem);
          imemptr=(byte*)&mem[imem];
          strcpy((char*)&mem[_imem+1],(char*)&mem_ory[value]);
          if (piece!=p_comma) return;
          lexer(); continue;
        }
      }

      // Put a literal value into another data type (instead of a string)

      if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || type==1) { // A string in words (the short pointer)

        imemptr=(byte*)(((memptrsize)imemptr+1)&-2); // Make it even
        *((word*)imemptr)=value;
        imemptr+=2;
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && type!=1) imemptr=(byte*)&mem[imem];
        if (piece!=p_comma) return;
        lexer(); continue;

      } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || type==2) { // A string in bytes

        oimemptr=imemptr;
        while (*(oimemptr-(memptrsize)mem+(memptrsize)frm)==2) oimemptr++;
        if (type==0) if (strlen((char*)&mem_ory[value])>(memptrsize)(oimemptr-imemptr)) c_error(2,33);
        strcpy((char *)imemptr,(char*)&mem_ory[value]);
        imemptr+=strlen((char *)imemptr);
        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && type!=2) imemptr=(byte*)&mem[imem];
        if (piece!=p_comma) return;
        lexer(); continue;

      } else { // A string in an int (the long pointer, as in DIV 1)

        imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
        mem[imem++]=value;
//        test_buffer(&mem,&imem_max,imem);
        imemptr=(byte*)&mem[imem];
        if (piece!=p_comma) return;
        lexer(); continue;

      }

    } else {
      value=constant();
      if (piece!=p_lparen) {
        if (piece!=p_dup) {

          if ((((memptrsize)imemptr-(memptrsize)mem)%4)==0) {
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            if (frm[imem]==0xdad00000) c_error(2,128);
          }

          // Put a numeric value into memory

          if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==1 || type==1) { // In a word

            if (value<0 || value>65535) c_error(2,144);
            imemptr=(byte*)(((memptrsize)imemptr+1)&-2); // Make it even
            *((word*)imemptr)=value;
            imemptr+=2;
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=1 && type!=1) imemptr=(byte*)&mem[imem];
            if (piece!=p_comma) return;
            lexer(); continue;

          } else if (*(imemptr-(memptrsize)mem+(memptrsize)frm)==2 || type==2) { // In a byte

            if (value<0 || value>255) c_error(2,143);
            *imemptr++=value;
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            if (*(imemptr-(memptrsize)mem+(memptrsize)frm)!=2 && type!=2) imemptr=(byte*)&mem[imem];
            if (piece!=p_comma) return;
            lexer(); continue;

          } else { // In an int (the usual in DIV 1)

            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            mem[imem++]=value;
//            test_buffer(&mem,&imem_max,imem);
            imemptr=(byte*)&mem[imem];
            if (piece!=p_comma) return;
            lexer(); continue;

          }

        } else {
          dup=value; lexer();
          if (piece!=p_lparen) c_error(3,36);
        }
      } else dup=value;
    }

    // Duplicate a sequence of values

    if (dup<1) c_error(2,38);
    lexer();

    // Parse the sequence as another initialization ...

    oimemptr=imemptr;
    tglo_init2(type);
    if (piece!=p_rparen) c_error(3,25);

    // And duplicate it the given number of times

    if (dup>1) {
      len=imemptr-oimemptr;
//      test_buffer(&mem,&imem_max,(((memptrsize)oimemptr-(memptrsize)mem)+len*dup+3)/4);
      while (--dup) {
        for (n=0;n<len;n++) {

          // Check that the strings fall onto themselves in the dup()...
          if ((((memptrsize)imemptr-(memptrsize)mem)%4)==0) {
            imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
            _imem=((memptrsize)oimemptr+n-(memptrsize)mem+3)/4;
            if (frm[imem]==0xdad00000) {
              if (frm[_imem]!=0xdad00000) c_error(3,136);
              else if (mem[imem]!=mem[_imem]) c_error(3,136);
            } else if (frm[_imem]==0xdad00000) c_error(3,136);
          }

          // Check that the words fall onto words
          if (*(byte*)((memptrsize)imemptr-(memptrsize)mem+(memptrsize)frm)==1) {
            if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)!=1) c_error(3,136);
          } else if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)==1) c_error(3,136);

          // Check that the bytes fall onto bytes
          if (*(byte*)((memptrsize)imemptr-(memptrsize)mem+(memptrsize)frm)==2) {
            if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)!=2) c_error(3,136);
          } else if (*(byte*)((memptrsize)oimemptr+n-(memptrsize)mem+(memptrsize)frm)==2) c_error(3,136);

          *imemptr++=*(oimemptr+n);
        }
      }
    }

    lexer();
    if (piece!=p_comma) {
      imem=((memptrsize)imemptr-(memptrsize)mem+3)/4;
      return;
    }
    lexer();
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Analysis of a block of statements;
//═════════════════════════════════════════════════════════════════════════════

void sentence() {

  int im1,im2,im3,im4;
  int dir,from,to,step;

  while (piece>=p_return) {
    test_buffer(&mem,&imem_max,imem);
    switch (piece) {
      case p_return:
        start_sentence();
        lexer(); if (piece==p_lparen) {
          lexer(); if (piece!=p_rparen) {
            expression(); if (piece!=p_rparen) c_error(3,25);
            g1(lrtf);
            } else {
            g1(lret);
            } lexer();
          } else {
          g1(lret);
        }
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
        end_sentence(); record_sentence(); break;
      case p_if:
        start_sentence();
        lexer();
        if (!free_syntax) if (piece!=p_lparen) c_error(3,36);
        if (piece==p_lparen) lexer();
        condition();
        if (!free_syntax) if (piece!=p_rparen) c_error(3,25);
        if (piece==p_rparen) lexer();
        g2(ljpf,0); im1=imem-1;
        end_sentence(); record_sentence();
        sentence();
        if (piece==p_else) {
          start_sentence();
          lexer();
          g2(ljmp,0); mem[im1]=imem; im1=imem-1;
          end_sentence(); record_sentence();
          sentence();
          }
        mem[im1]=imem; if (piece!=p_end) c_error(0,40); lexer();
        break;
      case p_loop:
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexer();
        im1=imem; sentence();
        if (piece!=p_end) c_error(0,40);
        start_sentence(); lexer();
        g2(ljmp,im1);
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im1;
        end_sentence(); record_sentence();
        break;
      case p_while:
        start_sentence();
        tbreak[itbreak++]=0; tcont[itcont++]=0; im1=imem;
        lexer();
        if (!free_syntax) if (piece!=p_lparen) c_error(3,36);
        if (piece==p_lparen) lexer();
        condition();
        if (!free_syntax) if (piece!=p_rparen) c_error(3,25);
        if (piece==p_rparen) lexer();
        g2(ljpf,0); im2=imem-1;
        end_sentence(); record_sentence();
        sentence();
        if (piece!=p_end) c_error(0,40); start_sentence();
        lexer();
        g2(ljmp,im1); mem[im2]=imem;
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im1;
        end_sentence(); record_sentence();
        break;
      case p_repeat:
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexer();
        im1=imem; sentence();
        if (piece!=p_until) c_error(0,42);
        start_sentence();
        lexer();
        if (!free_syntax) if (piece!=p_lparen) c_error(3,36);
        if (piece==p_lparen) lexer();
        condition();
        if (!free_syntax) if (piece!=p_rparen) c_error(3,25);
        if (piece==p_rparen) lexer();
        g2(ljpf,im1);
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im1;
        end_sentence(); record_sentence();
        break;
      case p_from:
        start_sentence();
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexer();
        if (piece!=p_id) c_error(0,67);

        if ((*o).type==tvglo) {
          dir=(*o).vglo.offset; g2(lcar,dir);
        } else if ((*o).type==tvloc && (!(*o).block || (*o).block==current_block)) {
          dir=-(*o).vloc.offset;
          g2(lcar,-dir); g1(laid);
        } else c_error(0,67);

        lexer();
        if (piece!=p_assign) c_error(3,68); lexer();
        from=constant();
        if (piece!=p_to) c_error(1,69); lexer();
        to=constant();
        if (from==to) c_error(4,71);
        if (piece==p_step) {
          lexer();
          step=constant();
          if (from<to && step<=0) c_error(4,70);
          if (from>to && step>=0) c_error(4,70);
        } else {
          if (from<to) step=1; else step=-1;
        }
        g2(lcar,from); // Assignment of the from
        g1(lasi); g1(lasp);

        im1=imem; // Start of the loop

        if (dir>=0) { // Comparison of the loop continuation condition
          g2(lcar,dir);
        } else {
          g2(lcar,-dir); g1(laid);
        }
        g1(lptr); g2(lcar,to);
        if (step>0) g1(lmei); else g1(lmai);
        g2(ljpf,0); im2=imem-1;

        end_sentence(); record_sentence();
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();

        sentence();
        if (piece!=p_end) c_error(0,40); start_sentence();
        lexer();

        im3=imem; // Position of the continue

        if (dir>=0) { // Increment and back to the start of the loop
          g2(lcar,dir); 
        } else {
          g2(lcar,-dir); g1(laid);
        }
        g2(lcar,step);
        g1(lada); g1(lasp);
        g2(ljmp,im1); mem[im2]=imem;

        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
        while (tcont[--itcont]!=0) mem[tcont[itcont]]=im3;
        end_sentence(); record_sentence();

        break;

      case p_for:
        start_sentence();
        tbreak[itbreak++]=0; tcont[itcont++]=0;
        lexer();
        if (piece!=p_lparen) c_error(3,36); lexer();
        if (piece!=p_semicolon) {
          expression(); g1(lasp);
          while (piece==p_comma) { lexer(); expression(); g1(lasp); }
        } im1=imem;
        if (piece!=p_semicolon) c_error(3,47); lexer();
        if (piece==p_semicolon) {
          g2(lcar,1);
        } else expression();
        g2(ljpf,0); im2=imem-1;
        while (piece==p_comma) { lexer(); expression();
          g2(ljpf,im2); im2=imem-1; }
        g2(ljmp,0); im3=imem-1;
        if (piece!=p_semicolon) c_error(3,47); lexer();
        if (piece!=p_rparen) {
          expression(); g1(lasp);
          while (piece==p_comma) { lexer(); expression(); g1(lasp); }
        }
        g2(ljmp,im1);
        if (piece!=p_rparen) c_error(3,25); lexer();
        end_sentence(); record_sentence();
        mem[im3++]=imem; sentence();
        if (piece!=p_end) c_error(0,40); start_sentence(); lexer();
          g2(ljmp,im3);
          do { im1=mem[im2]; mem[im2]=imem; im2=im1; } while(im2);
        while (tbreak[--itbreak]!=0) mem[tbreak[itbreak]]=imem;
          while (tcont[--itcont]!=0) mem[tcont[itcont]]=im3;
        end_sentence(); record_sentence();
        break;
      case p_switch:
        start_sentence();
        lexer();
        if (!free_syntax) if (piece!=p_lparen) c_error(3,36);
        if (piece==p_lparen) lexer();
        condition();
        if (!free_syntax) if (piece!=p_rparen) c_error(3,25);
        if (piece==p_rparen) lexer();
        while (piece==p_semicolon) {
          lexer();
        }
        end_sentence(); record_sentence();
        im1=0; im2=0;
        while (piece!=p_end) {
          start_sentence();
          if (piece==p_case) {
            im3=0; do {

              lexer();
              if (im1) mem[im1]=imem;
              expression(); if (piece!=p_range) {
                g2(lcse,0); im1=imem-1;
              } else {
                lexer(); expression();
                g2(lcsr,0); im1=imem-1;
              }

              if (piece==p_comma) {
                g2(ljmp,im3); im3=imem-1;
              }

            } while (piece==p_comma);

            while(im3) { im4=mem[im3]; mem[im3]=imem; im3=im4; }

          } else if (piece==p_default) {
            lexer();
            if (im1) mem[im1]=imem; im1=0;
          } else c_error(0,50);
          if (!free_syntax) if (piece!=p_semicolon) c_error(3,65);
          while (piece==p_semicolon || piece==p_comma) lexer();
          g1(lasp);
          end_sentence(); record_sentence();
          sentence();
          if (piece!=p_end) c_error(0,51); start_sentence();
            g2(ljmp,im2); im2=imem-1;
          skip_semicolon();
          end_sentence(); record_sentence();
        } start_sentence();
        if (im1) mem[im1]=imem; g1(lasp);
        while(im2) { im1=mem[im2]; mem[im2]=imem; im2=im1; }
        lexer();
        end_sentence(); record_sentence(); break;
      case p_frame:
        start_sentence();
        lexer(); if (piece==p_lparen) {
          lexer(); if (piece!=p_rparen) {
            expression(); if (piece!=p_rparen) c_error(3,25);
            g1(lfrf);
          } else {
            g1(lfrm);
          } lexer();
        } else {
          g1(lfrm);
        }
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
        end_sentence(); record_sentence(); break;
      case p_debug:
        start_sentence();
        g1(ldbg); lexer();
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
        end_sentence(); record_sentence();
        break;
      case p_break:
        start_sentence();
        if (itbreak==0) c_error(0,48); lexer();
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
        g2(ljmp,0); tbreak[itbreak++]=imem-1;
        end_sentence(); record_sentence(); break;
      case p_continue:
        start_sentence();
        if (itcont==0) c_error(0,49); lexer();
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
        g2(ljmp,0); tcont[itcont++]=imem-1;
        end_sentence(); record_sentence(); break;
      case p_clone:
        start_sentence();
        lexer(); g2(lclo,0); im1=imem-1;
        end_sentence(); record_sentence();
        sentence();
        if (piece!=p_end) c_error(0,40); lexer();
        mem[im1]=imem; break;
      case p_semicolon:
        lexer(); break;
      default:
        start_sentence();
        error_27=124;
        expression();
        do {
          _exp--;
        } while ((*_exp).type==eoper && (*_exp).token==p_string);
        error_27=27;
        switch((*_exp).type) {
          case ecall: break;
          case efunc: break;
          case efext: break;
          case eoper:
            switch((*_exp).token) {
              case p_assign: case p_inc: case p_plus: case p_dec: case p_minus:
              case p_add_assign: case p_sub_assign: case p_mul_assign:
              case p_div_assign: case p_mod_assign: case p_and_assign:
              case p_or_assign: case p_xor_assign: case p_shr_assign: case p_shl_assign:

              case p_assignword: case p_incword: case p_plusword: case p_decword: case p_minusword:
              case p_add_assignword: case p_sub_assignword: case p_mul_assignword:
              case p_div_assignword: case p_mod_assignword: case p_and_assignword:
              case p_or_assignword: case p_xor_assignword: case p_shr_assignword: case p_shl_assignword:

              case p_assignchar: case p_incchar: case p_pluschar: case p_decchar: case p_minuschar:
              case p_add_assignchar: case p_sub_assignchar: case p_mul_assignchar:
              case p_div_assignchar: case p_mod_assignchar: case p_and_assignchar:
              case p_or_assignchar: case p_xor_assignchar: case p_shr_assignchar: case p_shl_assignchar:

              case p_strcpy: case p_strcat: case p_strsub: break;
              default: c_error(4,64); break;
            } break;
          default: c_error(4,64);
        }
        if (!free_syntax) if (piece!=p_semicolon) c_error(3,66);
        while (piece==p_semicolon || piece==p_comma) lexer();
        g1(lasp);
        end_sentence(); record_sentence();
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Analysis of a condition (generates code to compute it)
//═════════════════════════════════════════════════════════════════════════════

int expression_type=0; // 0 - int, 1 - char o byte, 2 - string
int factor_type=0;    // ditto

void condition(void) {
  byte * __ierror, * _ierror;
  int _line;

  if (extended_conditions) { expression(); return; } // As in DIV 1

  _exp=tabexp;
  save_error(0);
  _line=line_num; _ierror=ierror; const_expr00(0);
  swap(line_num,_line); __ierror=ierror; ierror=_ierror;
  generate_expression();
  line_num=_line; ierror=__ierror;
}

void const_expr00(int exp_type) {
  int te=expression_type;
  expression_type=exp_type;
  const_expr0();
  expression_type=te;
}

void const_expr0() { // Operators with right-to-left associativity <-
  int p;

  const_expr1();
  if ((p=piece)==p_assign || (p>=p_add_assign && p<=p_shl_assign)) {
    c_error(0,140);
  }
}

void const_expr1() { // Operators with left-to-right associativity ->
  int p;
  const_expr2();
  while ((p=piece)>=p_or && p<=p_andofs) {
    if (p>=p_xorptr) p-=p_xorptr-p_xor; // Convert xorptr,andofs into xor,and
    lexer(); const_expr2();
    (*_exp).type=eoper; (*_exp++).token=p;
    factor_type=0;
  }
}

void const_expr2() {
  int p,tf;
  exp3();
  if (simple_conditions) {
    p=piece;
    while ((p>=p_eq && p<=p_ge)||p==p_assign)  {
      tf=factor_type;
      if (p==p_assign) p=p_eq;
      lexer(); exp3();
      if (tf==2 || factor_type==2) {
        p+=p_streq-p_eq;
        (*_exp).type=eoper; (*_exp++).token=p;
      } else {
        (*_exp).type=eoper; (*_exp++).token=p;
      } factor_type=0; p=piece;
    }
  } else {
    while ((p=piece)>=p_eq && p<=p_ge) {
      tf=factor_type;
      lexer(); exp3();
      if (tf==2 || factor_type==2) {
        p+=p_streq-p_eq;
        (*_exp).type=eoper; (*_exp++).token=p;
      } else {
        (*_exp).type=eoper; (*_exp++).token=p;
      } factor_type=0;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Analysis of an expression (generates code to compute it)
//═════════════════════════════════════════════════════════════════════════════

void expression(void) {
  byte * __ierror, * _ierror;
  int _line;

  _exp=tabexp;
  save_error(0);
  _line=line_num; _ierror=ierror; exp00(0);
  swap(line_num,_line); __ierror=ierror; ierror=_ierror;
  generate_expression();
  line_num=_line; ierror=__ierror;
}

void expression_cpa(void) {
  byte * __ierror, * _ierror;
  int _line;

  _exp=tabexp;
  save_error(0);
  _line=line_num; _ierror=ierror; exp00(0);
  swap(line_num,_line); __ierror=ierror; ierror=_ierror;
  if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointer) {
    _exp--; generate_expression(); g1(lcpa);
  } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerchar) {
    _exp--; generate_expression();
    if (do_strfix) { g2(lcar,0); g1(lstrfix); g1(lasp); }
    g1(lcpachr);
  } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerbyte) {
    _exp--; generate_expression();
    g1(lcpachr);
  } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerword) {
    _exp--; generate_expression();
    g1(lcpawor);
  } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_string) {
    _exp--; generate_expression(); g1(lcpastr);
  } else c_error(4,44);
  line_num=_line; ierror=__ierror;
}

void generate_expression(void) {
  struct exp_ele * e=tabexp;
  struct object * ob;

  do {
    switch ((*e).type) {
      case econs:
        g2(lcar,(*e).value); break;
      case estring:
        g2(lcar,(*e).value); break;
      case erange:
        g2(lrng,(*e).value); break;
      case ewhoami:
        g1(lcid); break;
      case ecall:
        ob=(*e).object; g2(lcal,(*ob).proc.offset);
        if ((*ob).used) (*ob).proc.offset=imem-1; break;
      case efunc:
        ob=(*e).object; g2(lfun,(*ob).func.code); break;
      case efext:
        ob=(*e).object; g2(lext,(*ob).fext.code); break;
      case echeck:
        g1(lchk); break;
      case enull:
        g1(lnul); break;
      case eoper: switch((*e).token) {
        case p_assign: g1(lasi); break;
        case p_or: g1(lori); break;
        case p_xor: g1(lxor); break;
        case p_and: g1(land); break;
        case p_eq: g1(ligu); break;
        case p_neq: g1(ldis); break;
        case p_gt: g1(lmay); break;
        case p_lt: g1(lmen); break;
        case p_le: g1(lmei); break;
        case p_ge: g1(lmai); break;
        case p_add: g1(ladd); break;
        case p_sub: g1(lsub); break;
        case p_mul: g1(lmul); break;
        case p_div: g1(ldiv); break;
        case p_mod: g1(lmod); break;
        case p_neg: g1(lneg); break;
        case p_pointer: g1(lptr); break;
        case p_not: g1(lnot); break;
        case p_point: g1(laid); break;
        case p_shr: g1(lshr); break;
        case p_shl: g1(lshl); break;
        case p_inc: g1(lipt); break;
        case p_plus: g1(lpti); break;
        case p_dec: g1(ldpt); break;
        case p_minus: g1(lptd); break;
        case p_add_assign: g1(lada); break;
        case p_sub_assign: g1(lsua); break;
        case p_mul_assign: g1(lmua); break;
        case p_div_assign: g1(ldia); break;
        case p_mod_assign: g1(lmoa); break;
        case p_and_assign: g1(lana); break;
        case p_or_assign: g1(lora); break;
        case p_xor_assign: g1(lxoa); break;
        case p_shr_assign: g1(lsra); break;
        case p_shl_assign: g1(lsla); break;

        case p_pointerword: g1(lptrwor); break;
        case p_plusword: g1(lptiwor); break;
        case p_minusword: g1(lptdwor); break;
        case p_incword: g1(liptwor); break;
        case p_decword: g1(ldptwor); break;
        case p_assignword: g1(lasiwor); break;
        case p_add_assignword: g1(ladawor); break;
        case p_sub_assignword: g1(lsuawor); break;
        case p_mul_assignword: g1(lmuawor); break;
        case p_div_assignword: g1(ldiawor); break;
        case p_mod_assignword: g1(lmoawor); break;
        case p_and_assignword: g1(lanawor); break;
        case p_or_assignword: g1(lorawor); break;
        case p_xor_assignword: g1(lxoawor); break;
        case p_shr_assignword: g1(lsrawor); break;
        case p_shl_assignword: g1(lslawor); break;

        case p_pointerchar: g1(lptrchr); break;
        case p_pluschar: g1(lptichr); break;
        case p_minuschar: g1(lptdchr); break;
        case p_incchar: g1(liptchr); break;
        case p_decchar: g1(ldptchr); break;
        case p_assignchar: g1(lasichr); break;
        case p_add_assignchar: g1(ladachr); break;
        case p_sub_assignchar: g1(lsuachr); break;
        case p_mul_assignchar: g1(lmuachr); break;
        case p_div_assignchar: g1(ldiachr); break;
        case p_mod_assignchar: g1(lmoachr); break;
        case p_and_assignchar: g1(lanachr); break;
        case p_or_assignchar: g1(lorachr); break;
        case p_xor_assignchar: g1(lxoachr); break;
        case p_shr_assignchar: g1(lsrachr); break;
        case p_shl_assignchar: g1(lslachr); break;

        case p_strcpy: g1(lstrcpy); break;
        case p_strfix: g1(lstrfix); break;
        case p_strcat: g1(lstrcat); break;
        case p_stradd: g1(lstradd); break;
        case p_strdec: g1(lstrdec); break;
        case p_strsub: g1(lstrsub); break;
        case p_strlen: g1(lstrlen); break;
        case p_streq: g1(lstrigu); break;
        case p_strneq: g1(lstrdis); break;
        case p_strgt: g1(lstrmay); break;
        case p_strlt: g1(lstrmen); break;
        case p_strle: g1(lstrmei); break;
        case p_strge: g1(lstrmai); break;

        case p_pointerbyte: g1(lptrchr); break;
        case p_string: break; // It is like a null p_pointer, which is not done

        default: c_error(4,41);
      } break;
      default: c_error(4,41);
    }
  } while (++e!=_exp);
}

//═════════════════════════════════════════════════════════════════════════════
//      Analysis of a constant expression
//      Local values and calls to other processes are not processed
//═════════════════════════════════════════════════════════════════════════════

int constant (void) {

  byte * __ierror, * _ierror;
  struct exp_ele * e=_exp=tabexp;
  int _line, i=0;

  save_error(0);

  _line=line_num; _ierror=ierror; exp00(0);
  swap(line_num,_line); __ierror=ierror; ierror=_ierror;

  do switch ((*e).type) {
    case econs:
      opstack[++i]=(*e).value; break;
    case estring:
      opstack[++i]=(*e).value; break;

//    case erange:
//      if (opstack[i]<0||opstack[i]>(*e).value) c_error(4,31); break;

    case eoper: switch((*e).token) {
      case p_or: opstack[i-1]|=opstack[i]; i--; break;
      case p_xor: opstack[i-1]^=opstack[i]; i--; break;
      case p_and: opstack[i-1]&=opstack[i]; i--; break;
      case p_eq: opstack[i-1]=opstack[i-1]==opstack[i]; i--; break;
      case p_neq: opstack[i-1]=opstack[i-1]!=opstack[i]; i--; break;
      case p_gt: opstack[i-1]=opstack[i-1]>opstack[i]; i--; break;
      case p_lt: opstack[i-1]=opstack[i-1]<opstack[i]; i--; break;
      case p_le: opstack[i-1]=opstack[i-1]<=opstack[i]; i--; break;
      case p_ge: opstack[i-1]=opstack[i-1]>=opstack[i]; i--; break;
      case p_add: opstack[i-1]+=opstack[i]; i--; break;
      case p_sub: opstack[i-1]-=opstack[i]; i--; break;
      case p_mul: opstack[i-1]*=opstack[i]; i--; break;
      case p_div: if (opstack[i]==0) c_error(4,34); opstack[i-1]/=opstack[i]; i--; break;
      case p_mod: if (opstack[i]==0) c_error(4,34); opstack[i-1]%=opstack[i]; i--; break;
      case p_neg: opstack[i]=-opstack[i]; break;

// I think pointers (and therefore ranges) were removed because of the FROMs,
// since taking a variable as a constant caused confusion ...

//      case p_pointer: opstack[i]=mem[opstack[i]]; break;

      case p_not: opstack[i]^=-1; break;
      case p_shr: opstack[i-1]>>=opstack[i]; i--; break;
      case p_shl: opstack[i-1]<<=opstack[i]; i--; break;
      default: c_error(4,30);
    } break;
    default: c_error(4,30);
  }while (++e!=_exp);

  line_num=_line; ierror=__ierror; return(opstack[i]);
}

//═════════════════════════════════════════════════════════════════════════════
//      Analysis of an expression into *tabexp
//═════════════════════════════════════════════════════════════════════════════

void exp00(int exp_type) {
  int te=expression_type;
  expression_type=exp_type;
  exp0();
  expression_type=te;
}

void exp0() { // Operators with right-to-left associativity <-
  int p;

  exp1();
  if ((p=piece)==p_assign || (p>=p_add_assign && p<=p_shl_assign)) {
    if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointer) {
      _exp--; lexer();
      exp00(0);
      (*_exp).type=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerchar) {
      if (p==p_assign) p=p_assignchar; else p+=p_add_assignchar-p_add_assign;
      _exp--; lexer();
      exp00(1); // string[1]="A";
      if (do_strfix) { (*_exp).type=eoper; (*_exp++).token=p_strfix; }
      (*_exp).type=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerbyte) {
      if (p==p_assign) p=p_assignchar; else p+=p_add_assignchar-p_add_assign;
      _exp--; lexer();
      exp00(1); // mis_bytes[1]="A";
      (*_exp).type=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerword) {
      if (p==p_assign) p=p_assignword; else p+=p_add_assignword-p_add_assign;
      _exp--; lexer();
      exp00(0);
      (*_exp).type=eoper; (*_exp++).token=p;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_string) {
      if (p==p_assign) { // string="..."; // Do the strcpy
        _exp--; lexer();
        exp00(2);
        (*_exp).type=eoper; (*_exp++).token=p_strcpy;
        (*_exp).type=eoper; (*_exp++).token=p_string; //There is still a string on the stack
      } else if (p==p_add_assign) { // string+="..."; // Do the strcat
        _exp--; lexer();
        exp00(2);
        (*_exp).type=eoper; (*_exp++).token=p_strcat;
        (*_exp).type=eoper; (*_exp++).token=p_string; //There is still a string on the stack
      } else if (p==p_sub_assign) { // string-=n; // Remove characters
        _exp--; lexer();
        exp00(-1);
        (*_exp).type=eoper; (*_exp++).token=p_strsub;
        (*_exp).type=eoper; (*_exp++).token=p_string; //There is still a string on the stack
      } else c_error(0,133);
    } else c_error(0,39);
  } factor_type=0;
}

void exp1() { // Operators with left-to-right associativity ->
  int p;
  div_exp2();
  while ((p=piece)>=p_or && p<=p_andofs) {
    if (p>=p_xorptr) p-=p_xorptr-p_xor; // Convert xorptr,andofs into xor,and
    lexer(); div_exp2();
    (*_exp).type=eoper; (*_exp++).token=p;
    factor_type=0;
  }
}

void div_exp2() {
  int p,tf;
  exp3();
  while ((p=piece)>=p_eq && p<=p_ge) {
    tf=factor_type;
    lexer(); exp3();
    if (tf==2 || factor_type==2) {
      // NOTE, for p==q the strcmp() will not be done if they are two pointers to strings
      p+=p_streq-p_eq;
      (*_exp).type=eoper; (*_exp++).token=p;
      factor_type=0;
    } else {
      (*_exp).type=eoper; (*_exp++).token=p;
    } factor_type=0;
  }
}

void exp3() {
  int p;
  exp4();
  while ((p=piece)>=p_shr && p<=p_shl) {
    lexer(); exp4();
    (*_exp).type=eoper; (*_exp++).token=p;
    factor_type=0;
  }
}

void exp4() {
  int p,tf;
  exp5();
  while ((p=piece)>=p_plus && p<=p_sub && (p<=p_minus || p>=p_add) ) {
    tf=factor_type;
    if (p<=p_minus) p+=p_add-p_plus; // Convert addition,subtraction into add,sub
    lexer(); exp5();
    if (p==p_add && (tf==2 || factor_type==2 || expression_type==2)) {
      (*_exp).type=eoper; (*_exp++).token=p_stradd;
      (*_exp).type=eoper; (*_exp++).token=p_string; // There is a string on the stack
      factor_type=2;
    } else if (p==p_sub && tf==2) {
      if (factor_type==2) { (*_exp).type=eoper; (*_exp++).token=p_strlen; }
      (*_exp).type=eoper; (*_exp++).token=p_strdec;
      (*_exp).type=eoper; (*_exp++).token=p_string; // There is a string on the stack
      factor_type=2;
    } else {
      (*_exp).type=eoper; (*_exp++).token=p;
      factor_type=0;
    }
  }
}

void exp5() {
  int p;
  unary();
  while ((p=piece)>=p_mul && p<=p_multi) {
    if (p==p_multi) p=p_mul; // Convert multi into mul
    lexer();  unary();
    (*_exp).type=eoper; (*_exp++).token=p;
    factor_type=0;
  }
}

void unary() {
  int p;

  while ((p=piece)==p_add) { lexer();  }
  if ((p>=p_xorptr && p<=p_mul) || (p==p_inc || p==p_dec)) {

    if (parameters>0 && p==p_pointer) { factor(); return; }

    if (p>=p_sub && p<=p_mul) p-=p_sub-p_neg; // Conv. sub,mul into neg,pointer
    else if (p>=p_xorptr && p<=p_andofs) p+=p_pointer-p_xorptr; //xorptr,andofs pointer,offset

    lexer();  unary(); // p = ( neg, pointer, offset, not )

    if (p==p_offset) {
      if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointer) _exp--;
      else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerchar) {
        c_error(4,132); // &string[<exp>] ???
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerbyte) {
        if ((*(_exp-2)).type==econs && (*(_exp-2)).value==0) _exp-=2;
        else c_error(4,132); // &bytes[<exp>] ???
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerword) {
        if ((*(_exp-2)).type==econs && (*(_exp-2)).value==0) _exp-=2;
        else c_error(4,132); // &words[<exp>] ???
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_string) {
        // &string = string
      } else c_error(4,32);
    } else if (p==p_inc || p==p_dec) {
      if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointer) {
        _exp--;
        (*_exp).type=eoper; (*_exp++).token=p;
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerchar) {
        _exp--;
        (*_exp).type=eoper; (*_exp++).token=p+p_incchar-p_inc;
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerbyte) {
        _exp--;
        (*_exp).type=eoper; (*_exp++).token=p+p_incchar-p_inc;
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerword) {
        _exp--;
        (*_exp).type=eoper; (*_exp++).token=p+p_incword-p_inc;
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_string) {
        _exp--;
        (*_exp).type=econs;
        if (p==p_inc) (*_exp++).value=-1; else (*_exp++).value=1;
        (*_exp).type=eoper; (*_exp++).token=p_strsub;
        (*_exp).type=eoper; (*_exp++).token=p_string;
      } else c_error(4,122);
    } else if (p==p_pointer) {
      if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerchar) {
        // *string[0] = string[0]
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerbyte) {
        // *string[0] = string[0]
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerword) {
        // *string[0] = string[0]
      } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_string) {
        // *string == string
      } else {
        (*_exp).type=eoper; (*_exp++).token=p;
      }
    } else {
      (*_exp).type=eoper; (*_exp++).token=p;
    }

  } else exp6();

}

void exp6() { // Operator to access other processes' local variables or tables
  struct exp_ele * e;

  factor();
  while (piece==p_point) {
    if (check_id) (*_exp++).type=echeck;
    e=_exp+1;
    remote_access=1; lexer(); factor(); remote_access=0;

    // NOTE, the <p_point> MUST BE THE SECOND THING that the <factor>
    // puts into the <expression>, there is no damn way around it.
    // (well, there is, doing a search for the p_point here between e-1 and _exp ...)

    if ((*e).type==eoper && (*e).token==p_point)
      (*e).token=p_add; else c_error(4,43);

  }
}

int struct_pointer; // p_pointer / p_pointerchar / p_string / p_pointerbyte / p_pointerword

void factor(void) {
  struct object * ob;
  struct object * obs;
  int p,offset,_imem;
  struct exp_ele * e;

  factor_type=0;

  switch (piece) { // analysis of a factor

    case p_lparen:
      lexer(); exp00(expression_type);
      if (piece!=p_rparen) c_error(3,25); lexer();  break;

    case p_lbracket:
      lexer();
      exp00(0);
      if (piece!=p_rbracket) c_error(3,26);
      (*_exp).type=eoper; (*_exp++).token=p_pointer; lexer(); break;

    case p_whoami:
      (*_exp++).type=ewhoami; lexer();  break;

    case p_num:
      (*_exp).type=econs; (*_exp++).value=piece_num; lexer(); break;

    case p_lit:
      factor_type=2;
      if (expression_type==1) {
        (*_exp).type=econs; (*_exp++).value=(byte)mem[piece_num];
      } else {
        (*_exp).type=estring; (*_exp++).value=piece_num;
        if (expression_type==-1) { (*_exp).type=eoper; (*_exp++).token=p_strlen; }
      } lexer(); break;

    case p_type:
      lexer(); if (piece!=p_id) c_error(0,112);
      switch((*o).type) {
        case tnone:
          (*o).line_num=line_num; (*o).ierror=ierror;
          (*_exp).type=econs; (*_exp++).value=(memptrsize)o; (*o).used=1; break;
        case tproc:
          (*_exp).type=econs; (*_exp++).value=(memptrsize)o; break;
        case tsglo:
          if ((*o).sglo.offset==long_header) { // type mouse ­ 0
            (*_exp).type=econs; (*_exp++).value=0; break;
          }
        default: c_error(0,60);
      } lexer(); break;

    case p_sizeof:

      lexer(); if (piece!=p_lparen) c_error(3,36);
      lexer(); if (piece!=p_id) c_error(3,113);;
      (*_exp).type=econs;
      switch((*o).type) {
        case tsglo: case tsloc:
          (*_exp++).value=(*o).sglo.len_item*(*o).sglo.totalitems; break;
        case tpsgl: case tpslo:
          (*_exp++).value=(*((*o).psgl.ostruct)).sglo.len_item*(*o).sglo.totalitems; break;
        case ttglo: case ttloc:
        case tbglo: case tbloc:
        case twglo: case twloc:
          (*_exp++).value=(*o).tglo.totalen; break;
        case tcglo: case tcloc:
          (*_exp++).value=((*o).cglo.totalen+5)/4; break;
        case tvglo: case tvloc:
          (*_exp++).value=1; break;
        default: c_error(0,61);
      } lexer(); if (piece!=p_rparen) c_error(3,25); lexer();
      break;

    case p_struct: // Check the declaration of a pointer to struct parameter
      if (parameters<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexer();
        if (piece!=p_pointer) c_error(0,151); // The pointer is not defined like this
        lexer(); if (piece!=p_id) c_error(1,110); obs=o;
        if ((*obs).type==tnone) c_error(0,150); // The pointer is not defined like this
        if ((*obs).type!=tsglo && (*obs).type!=tsloc) c_error(0,150);
        lexer(); ob=o;
        analyze_pointer_struct(tpslo,iloc++,obs);
        (*ob).line_num=line_num; (*ob).ierror=ierror;
        (*ob).param=1; parameters++;
        (*_exp).type=econs; (*_exp++).value=(*ob).pslo.offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
      } break;

    case p_string: // Check the declaration of a string parameter
      if (parameters<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexer();
        if (piece==p_pointer) { // Receive a pointer to string
          lexer();
          ob=analyze_pointer(tpclo,iloc++);
          (*ob).line_num=line_num; (*ob).ierror=ierror;
          (*ob).param=1; parameters++;
          (*_exp).type=econs; (*_exp++).value=(*ob).pclo.offset;
          (*_exp).type=eoper; (*_exp++).token=p_point;
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          break;
        }
        if (piece!=p_id) c_error(1,125);
        ob=o; if ((*ob).type!=tnone) c_error(0,126);
        (*ob).line_num=line_num; (*ob).ierror=ierror;
        (*ob).param=1;
        (*ob).type=tcloc; lexer();
        if (piece==p_lbracket) {
          lexer();
          if (piece==p_rbracket) {
            lexer();
            (*ob).cloc.totalen=255;
          } else {
            e=_exp;
            if (((*ob).cloc.totalen=constant())<0) c_error(4,127);
            if ((*ob).cloc.totalen>0xFFFFF) c_error(4,135);
            if (piece!=p_rbracket) c_error(3,26);
            lexer();
            _exp=e;
          }
        } else (*ob).cloc.totalen=255;
        if (parameters>1) g2(lpar,parameters-1); parameters=1;
        g2(lpri,0); _imem=imem;
        imem+=1+((*ob).cloc.totalen+5)/4;
        test_buffer(&mem,&imem_max,imem);
        mem[_imem]=0xDAD00000|(*ob).cloc.totalen;
        memset(&mem[imem+1],0,(((*ob).cloc.totalen+5)&-5));
        (*ob).cloc.offset=iloc+1;
        iloc+=1+((*ob).cloc.totalen+5)/4;
        mem[_imem-1]=imem; // pri
        (*_exp).type=estring; (*_exp++).value=(*ob).cloc.offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        (*_exp).type=eoper; (*_exp++).token=p_string;
      } break;

    case p_byte: // Check the declaration of a byte parameter
      if (parameters<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexer();
        if (piece==p_pointer) {
          lexer();
          ob=analyze_pointer(tpblo,iloc++);
          (*ob).line_num=line_num; (*ob).ierror=ierror;
          (*ob).param=1; parameters++;
          (*_exp).type=econs; (*_exp++).value=(*ob).pblo.offset;
          (*_exp).type=eoper; (*_exp++).token=p_point;
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          break;
        }
        if (piece!=p_id) c_error(1,142);
        ob=o; if ((*ob).type!=tnone) c_error(0,126);
        (*ob).line_num=line_num; (*ob).ierror=ierror;
        (*ob).param=1;
        (*ob).type=tbloc; lexer();
        if (piece==p_lbracket) c_error(2,146);
        parameters++; (*ob).bloc.offset=iloc++;
        (*ob).bloc.len1=0;
        (*ob).bloc.len2=-1;
        (*ob).bloc.len3=-1;
        (*ob).bloc.totalen=1;
        (*_exp).type=econs; (*_exp++).value=(*ob).bloc.offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        (*_exp).type=econs; (*_exp++).value=0;
        (*_exp).type=eoper; (*_exp++).token=p_pointerbyte;
      } break;

    case p_word: // Check the declaration of a word parameter
      if (parameters<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexer();
        if (piece==p_pointer) {
          lexer();
          ob=analyze_pointer(tpwlo,iloc++);
          (*ob).line_num=line_num; (*ob).ierror=ierror;
          (*ob).param=1; parameters++;
          (*_exp).type=econs; (*_exp++).value=(*ob).pwlo.offset;
          (*_exp).type=eoper; (*_exp++).token=p_point;
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          break;
        }
        if (piece!=p_id) c_error(1,145);
        ob=o; if ((*ob).type!=tnone) c_error(0,126);
        (*ob).line_num=line_num; (*ob).ierror=ierror;
        (*ob).param=1;
        (*ob).type=twloc; lexer();
        if (piece==p_lbracket) c_error(2,146);
        parameters++; (*ob).wloc.offset=iloc++;
        (*ob).wloc.len1=0;
        (*ob).wloc.len2=-1;
        (*ob).wloc.len3=-1;
        (*ob).wloc.totalen=1;
        (*_exp).type=econs; (*_exp++).value=(*ob).wloc.offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        (*_exp).type=econs; (*_exp++).value=0;
        (*_exp).type=eoper; (*_exp++).token=p_pointerword;
      } break;

    case p_int: // Check the declaration of an int parameter
      if (parameters<=0) {
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
      } else {
        lexer();
        if (piece==p_pointer) {

    case p_pointer: // Declaration of a pointer to int parameter
          if (parameters<=0) {
            if (error_27==27) c_error(1,27); else c_error(0,error_27);
          } parameters++;
          lexer();
          ob=analyze_pointer(tpilo,iloc++);
          (*ob).line_num=line_num; (*ob).ierror=ierror;
          (*ob).param=1;
          (*_exp).type=econs; (*_exp++).value=(*ob).pilo.offset;
          (*_exp).type=eoper; (*_exp++).token=p_point;
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          break;
        } else if (piece!=p_id) c_error(1,149);
      }

    case p_id: switch ((*o).type) {

      case tnone:
        ob=o; (*ob).line_num=line_num; (*ob).ierror=ierror; lexer();
        if (piece!=p_lparen) {
          if (parameters>0) {
            if (piece==p_lbracket) c_error(2,146);
            parameters++;
            (*ob).type=tvloc; (*ob).vloc.offset=iloc++;
            (*ob).param=1;
            (*_exp).type=econs; (*_exp++).value=(*ob).vloc.offset;
            (*_exp).type=eoper; (*_exp++).token=p_point;
            (*_exp).type=eoper; (*_exp++).token=p_pointer; break;
          } else c_error(2,28);
        }
        (*ob).used=1; (*ob).type=tproc;
        (*ob).proc.block=ob; (*ob).proc.offset=0; (*ob).proc.num_params=0;
        lexer();
        while (piece!=p_rparen) {
          (*ob).proc.num_params++;
          exp00(0);
          if (piece!=p_rparen) if (piece!=p_comma) c_error(3,25);
          else { lexer();  if (piece==p_rparen) c_error(3,45); }
        } (*_exp).type=ecall; (*_exp++).object=ob; lexer();  break;

      case tcons:
        if ((*o).cons.literal) {
          factor_type=2;
          if (expression_type==1) {
            (*_exp).type=econs; (*_exp++).value=(byte)mem[piece_num];
          } else if (expression_type==-1) {
            (*_exp).type=econs; (*_exp++).value=piece_num;
            (*_exp).type=eoper; (*_exp++).token=p_strlen;
          } else {
            (*_exp).type=econs; (*_exp++).value=(*o).cons.value;
          }
        } else {
          (*_exp).type=econs; (*_exp++).value=(*o).cons.value;
        } lexer(); break;

      case tvglo:
        (*_exp).type=econs; (*_exp++).value=(*o).vglo.offset;
        (*_exp).type=eoper; (*_exp++).token=p_pointer; lexer();  break;
        break;

      case ttglo:
      case tpigl:
        ob=o; offset=(*ob).tglo.offset;
        lexer();
        (*_exp).type=econs; (*_exp++).value=offset;
        if (piece==p_lbracket) {
          if ((*ob).type==tpigl) {
            (*_exp).type=eoper; (*_exp++).token=p_pointer;
            if (check_null) (*_exp++).type=enull;
          }
          lexer();
          exp00(0);
          if ((*ob).tglo.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tglo.len1; }
          if ((*ob).tglo.len2>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tglo.len2; }
            if ((*ob).tglo.len3>-1) {
              if (piece!=p_comma) c_error(3,130);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tglo.len3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).tglo.len2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).tglo.len1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
          (*_exp).type=eoper; (*_exp++).token=p_add;
        } (*_exp).type=eoper; (*_exp++).token=p_pointer; break;

      case tbglo:
      case tpbgl:
      case tpcgl:
        ob=o; offset=(*ob).bglo.offset;
        lexer();
        (*_exp).type=econs; (*_exp++).value=offset;
        if ((*ob).type==tpbgl || (*ob).type==tpcgl) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (piece!=p_lbracket) break;
          if (check_null) (*_exp++).type=enull;
        }
        if (piece==p_lbracket) {
          lexer();
          exp00(0);
          if ((*ob).bglo.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bglo.len1; }
          if ((*ob).bglo.len2>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bglo.len2; }
            if ((*ob).bglo.len3>-1) {
              if (piece!=p_comma) c_error(3,130);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bglo.len3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).bglo.len2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).bglo.len1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).type=econs; (*_exp++).value=0;
        } (*_exp).type=eoper; (*_exp++).token=p_pointerbyte ; break;

      case twglo:
      case tpwgl:
        ob=o; offset=(*ob).wglo.offset;
        lexer();
        (*_exp).type=econs; (*_exp++).value=offset;
        if ((*ob).type==tpwgl) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (piece!=p_lbracket) break;
          if (check_null) (*_exp++).type=enull;
        }
        if (piece==p_lbracket) {
          lexer();
          exp00(0);
          if ((*ob).wglo.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wglo.len1; }
          if ((*ob).wglo.len2>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wglo.len2; }
            if ((*ob).wglo.len3>-1) {
              if (piece!=p_comma) c_error(3,130);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wglo.len3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).wglo.len2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).wglo.len1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).type=econs; (*_exp++).value=0;
        } (*_exp).type=eoper; (*_exp++).token=p_pointerword; break;

      case tcglo:
        ob=o; offset=(*ob).cglo.offset;
        lexer();
        (*_exp).type=estring; (*_exp++).value=offset;
        if (piece==p_lbracket) {
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).cglo.totalen; }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
          (*_exp).type=eoper; (*_exp++).token=p_pointerchar;
        } else {
          factor_type=2;
          if (expression_type==1) {
            (*_exp).type=econs; (*_exp++).value=0;
            (*_exp).type=eoper; (*_exp++).token=p_pointerchar;
          } else {
            (*_exp).type=eoper; (*_exp++).token=p_string;
            if (expression_type==-1) { (*_exp).type=eoper; (*_exp++).token=p_strlen; }
          }
        } break;

      case tsglo:
      case tpsgl:
        ob=o; lexer(); (*_exp).type=econs; (*_exp++).value=(*ob).sglo.offset;
        if ((*ob).type==tpsgl) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (piece!=p_lbracket && piece!=p_point) break;
          if (check_null) (*_exp++).type=enull;
        }
        if (piece==p_lbracket) {
          lexer();
          exp00(0);
          if ((*ob).sglo.items1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sglo.items1; }
          if ((*ob).sglo.items2>-1) {
            if (piece!=p_comma) c_error(3,131);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sglo.items2; }
            if ((*ob).sglo.items3>-1) {
              if (piece!=p_comma) c_error(3,131);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sglo.items3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).sglo.items2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).sglo.items1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26); lexer();
          if ((*ob).type==tpsgl) {
            (*_exp).type=econs; (*_exp++).value=(*((*ob).psgl.ostruct)).sglo.len_item;
          } else {
            (*_exp).type=econs; (*_exp++).value=(*ob).sglo.len_item;
          }
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece==p_point) {
          struct_pointer=p_pointer;
          if ((*ob).type==tpsgl) member=(*ob).psgl.ostruct; else member=ob;
          lexer(); factor_struct();
          (*_exp).type=eoper; (*_exp++).token=struct_pointer;
        } else {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
        } break;

      case tvloc:
        if (remote_access && (*o).block) c_error(0,56);
        (*_exp).type=econs; (*_exp++).value=(*o).vloc.offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        (*_exp).type=eoper; (*_exp++).token=p_pointer; lexer(); break;

      case ttloc:
      case tpilo:
        if (remote_access && (*o).block) c_error(0,56);
        ob=o; offset=(*ob).tloc.offset;
        lexer();
        (*_exp).type=econs; (*_exp++).value=offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        if (piece==p_lbracket) {
          if ((*ob).type==tpilo) {
            (*_exp).type=eoper; (*_exp++).token=p_pointer;
            if (check_null) (*_exp++).type=enull;
          }
          remote_access=0;
          lexer();
          exp00(0);
          if ((*ob).tloc.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tloc.len1; }
          if ((*ob).tloc.len2>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tloc.len2; }
            if ((*ob).tloc.len3>-1) {
              if (piece!=p_comma) c_error(3,130);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tloc.len3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).tloc.len2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).tloc.len1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        (*_exp).type=eoper; (*_exp++).token=p_pointer; break;

      case tbloc:
      case tpblo:
      case tpclo:
        if (remote_access && (*o).block) c_error(0,56);
        ob=o; offset=(*ob).bloc.offset;
        lexer();
        (*_exp).type=econs; (*_exp++).value=offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        if ((*ob).type==tpblo || (*ob).type==tpclo) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (piece!=p_lbracket) break;
          if (check_null) (*_exp++).type=enull;
        }
        if (piece==p_lbracket) {
          remote_access=0;
          lexer();
          exp00(0);
          if ((*ob).bloc.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bloc.len1; }
          if ((*ob).bloc.len2>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bloc.len2; }
            if ((*ob).bloc.len3>-1) {
              if (piece!=p_comma) c_error(3,130);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bloc.len3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).bloc.len2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).bloc.len1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).type=econs; (*_exp++).value=0;
        } (*_exp).type=eoper; (*_exp++).token=p_pointerbyte ; break;

      case twloc:
      case tpwlo:
        if (remote_access && (*o).block) c_error(0,56);
        ob=o; offset=(*ob).wloc.offset;
        lexer();
        (*_exp).type=econs; (*_exp++).value=offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        if ((*ob).type==tpwlo) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (piece!=p_lbracket) break;
          if (check_null) (*_exp++).type=enull;
        }
        if (piece==p_lbracket) {
          remote_access=0;
          lexer();
          exp00(0);
          if ((*ob).wloc.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wloc.len1; }
          if ((*ob).wloc.len2>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wloc.len2; }
            if ((*ob).wloc.len3>-1) {
              if (piece!=p_comma) c_error(3,130);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wloc.len3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).wloc.len2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).wloc.len1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
        } else { // mi_byte ­ mi_byte[0]
          (*_exp).type=econs; (*_exp++).value=0;
        } (*_exp).type=eoper; (*_exp++).token=p_pointerword; break;

      case tcloc:
        if (remote_access && (*o).block) c_error(0,56);
        ob=o; offset=(*ob).cloc.offset;
        lexer();
        (*_exp).type=estring; (*_exp++).value=offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        if (piece==p_lbracket) {
          remote_access=0;
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).cloc.totalen; }
          if (piece!=p_rbracket) c_error(3,26);
          lexer();
          (*_exp).type=eoper; (*_exp++).token=p_pointerchar;
        } else {
          factor_type=2;
          if (expression_type==1) {
            (*_exp).type=econs; (*_exp++).value=0;
            (*_exp).type=eoper; (*_exp++).token=p_pointerchar;
          } else {
            (*_exp).type=eoper; (*_exp++).token=p_string;
            if (expression_type==-1) { (*_exp).type=eoper; (*_exp++).token=p_strlen; }
          }
        } break;

      case tsloc:
      case tpslo:
        if (remote_access && (*o).block) c_error(0,56);
        ob=o; lexer(); (*_exp).type=econs; (*_exp++).value=(*ob).sloc.offset;
        (*_exp).type=eoper; (*_exp++).token=p_point;
        if ((*ob).type==tpslo) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (piece!=p_lbracket && piece!=p_point) break;
          if (check_null) (*_exp++).type=enull;
        }
        if (piece==p_lbracket) {
          remote_access=0;
          lexer();
          exp00(0);
          if ((*ob).sloc.items1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sloc.items1; }
          if ((*ob).sloc.items2>-1) {
            if (piece!=p_comma) c_error(3,131);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sloc.items2; }
            if ((*ob).sloc.items3>-1) {
              if (piece!=p_comma) c_error(3,131);
              lexer();
              exp00(0);
              if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sloc.items3; }
              (*_exp).type=econs; (*_exp++).value=(*ob).sloc.items2+1;
              (*_exp).type=eoper; (*_exp++).token=p_mul;
              (*_exp).type=eoper; (*_exp++).token=p_add;
            }
            (*_exp).type=econs; (*_exp++).value=(*ob).sloc.items1+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          if (piece!=p_rbracket) c_error(3,26); lexer();
          if ((*ob).type==tpslo) {
            (*_exp).type=econs; (*_exp++).value=(*((*ob).pslo.ostruct)).sloc.len_item;
          } else {
            (*_exp).type=econs; (*_exp++).value=(*ob).sloc.len_item;
          }
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece==p_point) {
          struct_pointer=p_pointer;
          if ((*ob).type==tpslo) member=(*ob).psgl.ostruct; else member=ob;
          lexer(); factor_struct();
          (*_exp).type=eoper; (*_exp++).token=struct_pointer;
        } else {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
        } break;

      case tproc:
        ob=o; lexer(); if (piece!=p_lparen) c_error(3,36); lexer();
        p=0; while (piece!=p_rparen) { // p­num_params
          p++;
          exp00(0);
          if (piece!=p_rparen) if (piece!=p_comma) c_error(3,25);
          else { lexer(); if (piece==p_rparen) c_error(3,45); }
        }
        if (p!=(*ob).proc.num_params) c_error(1,46);
        (*_exp).type=ecall; (*_exp++).object=ob; lexer(); break;

      case tfunc:
        ob=o; lexer(); if (piece!=p_lparen) c_error(3,36);

        if ((*ob).func.code==122) { // fopen("..."
          in_fopen=1;
          lexer();
          in_fopen=0;
        } else lexer();

        p=0; while (piece!=p_rparen) { // p­num_params

          if ((*ob).func.par[p]==1) { // Parameter of type 1 (struct,field)

            // RECEIVES: struct_name,field_name
            // GENERATES: struct_offset,len_item,totalen,field_offset,field_type

            if (piece!=p_id) c_error(0,152);
            if ((*o).member!=NULL) c_error(0,152);
            if ((*o).type==tsglo) {
              (*_exp).type=econs; (*_exp++).value=(*o).sglo.offset;
            } else if ((*o).type==tsloc) {
              (*_exp).type=econs; (*_exp++).value=(*o).sloc.offset;
              (*_exp).type=eoper; (*_exp++).token=p_point;
            } else c_error(0,152);
            (*_exp).type=econs; (*_exp++).value=(*o).sglo.len_item;
            (*_exp).type=econs; (*_exp++).value=(*o).sglo.totalitems;
            obs=o; lexer(); if (piece!=p_comma) c_error(0,46);
            member=obs; lexer(); member=NULL;
            if (piece!=p_id) c_error(0,153);
            if ((*o).member!=obs) c_error(0,153);
            (*_exp).type=econs; (*_exp++).value=(*o).vglo.offset;
            (*_exp).type=econs;
            if ((*o).type==tcglo || (*o).type==tcloc) (*_exp++).value=1;
            else if ((*o).type==tpcgl || (*o).type==tpclo) (*_exp++).value=2;
            else (*_exp++).value=0;
            lexer();
          } else {                           // Parameter of type 0 (generic)
            exp00(0);
          } p++;

          if (piece!=p_rparen) if (piece!=p_comma) c_error(3,25);
          else { lexer(); if (piece==p_rparen) c_error(3,45); }
        }
        if (p!=(*ob).func.num_params) c_error(1,46);
        (*_exp).type=efunc; (*_exp++).object=ob;
        if ((*ob).func.ret==1) factor_type=2;
        lexer(); break;

      case tfext:
        ob=o; lexer(); if (piece!=p_lparen) c_error(3,36); lexer();
        p=0; while (piece!=p_rparen) { // p­num_params
          p++;
          exp00(0);
          if (piece!=p_rparen) if (piece!=p_comma) c_error(3,25);
          else { lexer(); if (piece==p_rparen) c_error(3,45); }
        } if (p!=(*ob).fext.num_params) c_error(1,46);
        (*_exp).type=efext; (*_exp++).object=ob; lexer(); break;

      default:
        if (error_27==27) c_error(1,27); else c_error(0,error_27);
        break;

    } break;

    default:
      if (error_27==27) c_error(1,27); else c_error(0,error_27);
      break;
  }

  if (piece==p_inc || piece==p_dec) {
    if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointer) {
      _exp--;
      (*_exp).type=eoper;
      if (piece==p_inc) (*_exp++).token=p_plus;
        else (*_exp++).token=p_minus;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerchar) {
      _exp--;
      (*_exp).type=eoper;
      if (piece==p_inc) (*_exp++).token=p_pluschar;
        else (*_exp++).token=p_minuschar;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerbyte) {
      _exp--;
      (*_exp).type=eoper;
      if (piece==p_inc) (*_exp++).token=p_pluschar;
        else (*_exp++).token=p_minuschar;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_pointerword) {
      _exp--;
      (*_exp).type=eoper;
      if (piece==p_inc) (*_exp++).token=p_plusword;
        else (*_exp++).token=p_minusword;
    } else if ((*(_exp-1)).type==eoper && (*(_exp-1)).token==p_string) {
      _exp--; (*_exp).type=econs;
      if (piece==p_inc) (*_exp++).value=-1;
        else (*_exp++).value=1;
      (*_exp).type=eoper;
      (*_exp++).token=p_strsub;
      (*_exp).type=eoper;
      (*_exp++).token=p_string;
    } else c_error(4,122);
    lexer();
  }
}

void factor_struct(void) {
  struct object * ob;

  if (piece!=p_id) c_error(3,58);
  switch ((*o).type) {

    case tvglo:
      (*_exp).type=econs; (*_exp++).value=(*o).vglo.offset;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      break;

    case ttglo:
    case tpigl:
      (*_exp).type=econs; (*_exp++).value=(*o).tglo.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if (piece==p_lbracket) {
        if ((*ob).type==tpigl) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (check_null) (*_exp++).type=enull;
        }
        lexer();
        exp00(0);
        if ((*ob).tglo.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tglo.len1; }
        if ((*ob).tglo.len2>-1) {
          if (piece!=p_comma) c_error(3,130);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tglo.len2; }
          if ((*ob).tglo.len3>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tglo.len3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).tglo.len2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).tglo.len1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
        (*_exp).type=eoper; (*_exp++).token=p_add;
      } break;

    case tbglo:
    case tpbgl:
    case tpcgl:
      (*_exp).type=econs; (*_exp++).value=(*o).bglo.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if ((*ob).type==tpbgl || (*ob).type==tpcgl) {
        if (piece!=p_lbracket) break;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
        if (check_null) (*_exp++).type=enull;
      }
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if ((*ob).bglo.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bglo.len1; }
        if ((*ob).bglo.len2>-1) {
          if (piece!=p_comma) c_error(3,130);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bglo.len2; }
          if ((*ob).bglo.len3>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bglo.len3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).bglo.len2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).bglo.len1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).type=econs; (*_exp++).value=0;
      } struct_pointer=p_pointerbyte; break;

    case twglo:
    case tpwgl:
      (*_exp).type=econs; (*_exp++).value=(*o).wglo.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if ((*ob).type==tpwgl) {
        if (piece!=p_lbracket) break;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
        if (check_null) (*_exp++).type=enull;
      }
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if ((*ob).wglo.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wglo.len1; }
        if ((*ob).wglo.len2>-1) {
          if (piece!=p_comma) c_error(3,130);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wglo.len2; }
          if ((*ob).wglo.len3>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wglo.len3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).wglo.len2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).wglo.len1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).type=econs; (*_exp++).value=0;
      } struct_pointer=p_pointerword; break;

    case tcglo:
      (*_exp).type=estring; (*_exp++).value=(*o).cglo.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).cglo.totalen; }
        if (piece!=p_rbracket) c_error(3,26);
        lexer();
        struct_pointer=p_pointerchar;
      } else {
        factor_type=2;
        if (expression_type==1) {
          (*_exp).type=econs; (*_exp++).value=0;
          struct_pointer=p_pointerchar;
        } else {
          if (expression_type==-1) { (*_exp).type=eoper; (*_exp++).token=p_strlen; }
          struct_pointer=p_string;
        }
      } break;

    case tsglo:
    case tpsgl:
      (*_exp).type=econs; (*_exp++).value=(*o).sglo.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if ((*ob).type==tpsgl) {
        if (piece!=p_lbracket && piece!=p_point) break;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
        if (check_null) (*_exp++).type=enull;
      }
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if ((*ob).sglo.items1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sglo.items1; }
        if ((*ob).sglo.items2>-1) {
          if (piece!=p_comma) c_error(3,131);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sglo.items2; }
          if ((*ob).sglo.items3>-1) {
            if (piece!=p_comma) c_error(3,131);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sglo.items3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).sglo.items2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).sglo.items1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
        if ((*ob).type==tpsgl) {
          (*_exp).type=econs; (*_exp++).value=(*((*ob).psgl.ostruct)).sglo.len_item;
        } else {
          (*_exp).type=econs; (*_exp++).value=(*ob).sglo.len_item;
        }
        (*_exp).type=eoper; (*_exp++).token=p_mul;
        (*_exp).type=eoper; (*_exp++).token=p_add;
      }
      if (piece==p_point) {
        if ((*ob).type==tpsgl) member=(*ob).psgl.ostruct; else member=ob;
        lexer(); factor_struct();
      } break;

    case tvloc:
      (*_exp).type=econs; (*_exp++).value=(*o).vloc.offset;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer(); break;

    case ttloc:
    case tpilo:
      (*_exp).type=econs; (*_exp++).value=(*o).tloc.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if (piece==p_lbracket) {
        if ((*ob).type==tpilo) {
          (*_exp).type=eoper; (*_exp++).token=p_pointer;
          if (check_null) (*_exp++).type=enull;
        }
        lexer();
        exp00(0);
        if ((*ob).tloc.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tloc.len1; }
        if ((*ob).tloc.len2>-1) {
          if (piece!=p_comma) c_error(3,130);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tloc.len2; }
          if ((*ob).tloc.len3>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).tloc.len3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).tloc.len2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).tloc.len1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
        (*_exp).type=eoper; (*_exp++).token=p_add;
      } break;

    case tbloc:
    case tpblo:
    case tpclo:
      (*_exp).type=econs; (*_exp++).value=(*o).bloc.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if ((*ob).type==tpblo || (*ob).type==tpcgl) {
        if (piece!=p_lbracket) break;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
        if (check_null) (*_exp++).type=enull;
      }
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if ((*ob).bloc.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bloc.len1; }
        if ((*ob).bloc.len2>-1) {
          if (piece!=p_comma) c_error(3,130);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bloc.len2; }
          if ((*ob).bloc.len3>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).bloc.len3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).bloc.len2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).bloc.len1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).type=econs; (*_exp++).value=0;
      } struct_pointer=p_pointerbyte; break;

    case twloc:
    case tpwlo:
      (*_exp).type=econs; (*_exp++).value=(*o).wloc.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if ((*ob).type==tpwlo) {
        if (piece!=p_lbracket) break;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
        if (check_null) (*_exp++).type=enull;
      }
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if ((*ob).wloc.len1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wloc.len1; }
        if ((*ob).wloc.len2>-1) {
          if (piece!=p_comma) c_error(3,130);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wloc.len2; }
          if ((*ob).wloc.len3>-1) {
            if (piece!=p_comma) c_error(3,130);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).wloc.len3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).wloc.len2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).wloc.len1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
      } else { // mi_byte ­ mi_byte[0]
        (*_exp).type=econs; (*_exp++).value=0;
      } struct_pointer=p_pointerword; break;

    case tcloc:
      (*_exp).type=estring; (*_exp++).value=(*o).cloc.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).cloc.totalen; }
        if (piece!=p_rbracket) c_error(3,26);
        lexer();
        struct_pointer=p_pointerchar;
      } else {
        factor_type=2;
        if (expression_type==1) {
          (*_exp).type=econs; (*_exp++).value=0;
          struct_pointer=p_pointerchar;
        } else {
          if (expression_type==-1) { (*_exp).type=eoper; (*_exp++).token=p_strlen; }
          struct_pointer=p_string;
        }
      } break;

    case tsloc:
    case tpslo:
      (*_exp).type=econs; (*_exp++).value=(*o).sloc.offset; ob=o;
      (*_exp).type=eoper; (*_exp++).token=p_add;
      member=NULL; lexer();
      if ((*ob).type==tpslo) {
        if (piece!=p_lbracket && piece!=p_point) break;
        (*_exp).type=eoper; (*_exp++).token=p_pointer;
        if (check_null) (*_exp++).type=enull;
      }
      if (piece==p_lbracket) {
        lexer();
        exp00(0);
        if ((*ob).sloc.items1>-1) if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sloc.items1; }
        if ((*ob).sloc.items2>-1) {
          if (piece!=p_comma) c_error(3,131);
          lexer();
          exp00(0);
          if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sloc.items2; }
          if ((*ob).sloc.items3>-1) {
            if (piece!=p_comma) c_error(3,131);
            lexer();
            exp00(0);
            if (check_range) { (*_exp).type=erange; (*_exp++).value=(*ob).sloc.items3; }
            (*_exp).type=econs; (*_exp++).value=(*ob).sloc.items2+1;
            (*_exp).type=eoper; (*_exp++).token=p_mul;
            (*_exp).type=eoper; (*_exp++).token=p_add;
          }
          (*_exp).type=econs; (*_exp++).value=(*ob).sloc.items1+1;
          (*_exp).type=eoper; (*_exp++).token=p_mul;
          (*_exp).type=eoper; (*_exp++).token=p_add;
        }
        if (piece!=p_rbracket) c_error(3,26); lexer();
        if ((*ob).type==tpslo) {
          (*_exp).type=econs; (*_exp++).value=(*((*ob).pslo.ostruct)).sloc.len_item;
        } else {
          (*_exp).type=econs; (*_exp++).value=(*ob).sloc.len_item;
        }
        (*_exp).type=eoper; (*_exp++).token=p_mul;
        (*_exp).type=eoper; (*_exp++).token=p_add;
      }
      if (piece==p_point) {
        if ((*ob).type==tpslo) member=(*ob).pslo.ostruct; else member=ob;
        lexer(); factor_struct();
      } break;

    default: c_error(3,58); break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Write the object table file
//═════════════════════════════════════════════════════════════════════════════

#ifdef listings

void l_objects (void) {
  FILE * sta;
  int n,o,l;

  sta=fopen("div.tab","wb");

  fprintf(sta,"■ Objects of %s - DIV Version " version "\n\n",file_in);

  for (n=1;n<=8;n++) fprintf(sta,"[%u]: %u\n",n,mem[n]); fprintf(sta,"\n");

  for (n=0;n<obj_cnt;n++) {
    switch(obj[n].type) {
      case tnone: fprintf(sta,"%5u\ttnone: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        break;
      case tcons: fprintf(sta,"%5u\ttcons: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\tvalue=%u\n",obj[n].cons.value);
        break;
      case tvglo: fprintf(sta,"%5u\ttvglo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].vglo.offset);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        else fprintf(sta,"\tvalue=%u\n",mem[o]);
        break;
      case ttglo: fprintf(sta,"%5u\tttglo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpbgl: case tpwgl: case tpcgl:
      case tpigl: fprintf(sta,"%5u\ttp?gl: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tsglo: fprintf(sta,"%5u\ttsglo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sglo.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sglo.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sglo.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sglo.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sglo.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sglo.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpsgl: fprintf(sta,"%5u\ttpsgl: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sglo.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sglo.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sglo.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sglo.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sglo.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sglo.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tvloc: fprintf(sta,"%5u\ttvloc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].vloc.offset);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        else fprintf(sta,"\tvalue=%u\n",loc[o]);
        break;
      case ttloc: fprintf(sta,"%5u\tttloc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tloc.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tloc.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tloc.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tloc.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tloc.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpblo: case tpwlo: case tpclo:
      case tpilo: fprintf(sta,"%5u\ttp?lo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].tloc.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].tloc.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].tloc.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].tloc.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].tloc.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tsloc: fprintf(sta,"%5u\ttsloc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sloc.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sloc.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sloc.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sloc.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sloc.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sloc.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tpslo: fprintf(sta,"%5u\ttpslo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].sloc.offset);
        fprintf(sta,"\titems1=%u\n",obj[n].sloc.items1);
        fprintf(sta,"\titems2=%u\n",obj[n].sloc.items2);
        fprintf(sta,"\titems3=%u\n",obj[n].sloc.items3);
        fprintf(sta,"\ttotalitems=%u\n",l=obj[n].sloc.totalitems);
        fprintf(sta,"\tlen_item=%u\n",obj[n].sloc.len_item);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tproc: fprintf(sta,"%5u\ttproc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\ttype=%u\n",(memptrsize)(obj[n].proc.block));
        fprintf(sta,"\toffset=%u\n",obj[n].proc.offset);
        fprintf(sta,"\tnum_params=%u\n",obj[n].proc.num_params);
        break;
      case tfunc: fprintf(sta,"%5u\ttfunc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\tcode=%u\n",obj[n].func.code);
        fprintf(sta,"\tnum_params=%u\n",obj[n].func.num_params);
        break;
      case tfext: fprintf(sta,"%5u\ttfext: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\tcode=%u\n",obj[n].fext.code);
        fprintf(sta,"\tnum_params=%u\n",obj[n].fext.num_params);
        break;
      case tbglo: fprintf(sta,"%5u\ttbglo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case tbloc: fprintf(sta,"%5u\ttbloc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case twglo: fprintf(sta,"%5u\ttwglo: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
      case twloc: fprintf(sta,"%5u\ttwloc: %s",n,obj[n].name);
        if (obj[n].used) fprintf(sta," (*) used\n"); else fprintf(sta,"\n");
        fprintf(sta,"\toffset=%u\n",o=obj[n].bglo.offset);
        fprintf(sta,"\tlen1=%u\n",obj[n].bglo.len1);
        fprintf(sta,"\tlen2=%u\n",obj[n].bglo.len2);
        fprintf(sta,"\tlen3=%u\n",obj[n].bglo.len3);
        fprintf(sta,"\ttotalen=%u\n",obj[n].bglo.totalen);
        if (obj[n].member) fprintf(sta,"\tmember of %s\n",(*obj[n].member).name);
        break;
    }
    fprintf(sta,"\tblock(%u), previous(%u)\n",obj[n].block,obj[n].previous);
    fprintf(sta,"\n");
  }

  fclose(sta);
}

#endif

//═════════════════════════════════════════════════════════════════════════════
//      Write the file with debugging information
//═════════════════════════════════════════════════════════════════════════════

// Careful, writing items1,items2 and items3 is still missing (or len1,len2,len3 for tables)

void save_dbg(void) {
  FILE * sta;
  int n;

  struct {
    int type;
    int name;
    int block;
    int member;
    int v0,v1,v2,v3,v4,v5;
  } ob;

  sta=fopen("system/exec.dbg","wb");

  fwrite(&obj_cnt,4,1,sta);
  fwrite(&predefined_obj_count,4,1,sta);
  n=(memptrsize)&obj[0];
  fwrite(&n,4,1,sta);
  n=sizeof(struct object);
  fwrite(&n,4,1,sta);

  for (n=0;n<obj_cnt;n++) {
    ob.type=(memptrsize)obj[n].type;
    ob.name=(memptrsize)obj[n].name-(memptrsize)vnames;
    ob.block=(memptrsize)obj[n].block;
    ob.member=(memptrsize)obj[n].member;
    ob.v0=(memptrsize)obj[n].sglo.offset;
    ob.v1=(memptrsize)obj[n].sglo.len_item;
    ob.v2=(memptrsize)obj[n].sglo.totalitems;
    ob.v3=(memptrsize)obj[n].sglo.items1;
    ob.v4=(memptrsize)obj[n].sglo.items2;
    ob.v5=(memptrsize)obj[n].sglo.items3;
    if (obj[n].type==tpsgl || obj[n].type==tpslo) ob.v1=(ob.v1-(memptrsize)&obj[0])/sizeof(struct object);
    // NOTE ! objects cannot be added here (see use of &obj[0] and sizeof(struct object))
    fwrite(&ob,sizeof(ob),1,sta);
  }

  n=(memptrsize)ivnames.b-(memptrsize)vnames;
  fwrite(&n,4,1,sta);

  fwrite(vnames,1,n,sta);

  fclose(sta);
}

//═════════════════════════════════════════════════════════════════════════════
//    Write the execution parameters for the interpreter
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
//      Write the assembler file
//═════════════════════════════════════════════════════════════════════════════

#ifdef listings

void l_assembler (void) {

  FILE * sta;
  int i=mem[1];

  sta=fopen("div.eml","wt");

  fprintf(sta,"■ EML code of %s - DIV Version " version "\n",file_in);

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
//  Function to locate and open a file (pal,fpg,fnt,...)
//  This function must follow the same algorithm in F.CPP and DIVC.CPP
//════════════════════════════════════════════════════════════════════════════
FILE *__fpopen (byte *file, char *mode) {
	
	char fprgpath[_MAX_PATH*2];
	FILE *f;

	strcpy(fprgpath,(char *)&type[8]);
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
//      Compile dialog
//═════════════════════════════════════════════════════════════════════════════

uint8_t e_msg[32];
int compiled=0;

void compile1(void) {
  _show_items();
  wwrite(v.ptr,v.w/big2,v.h/big2,3,12,0,texts[206],c3);
  wwrite(v.ptr,v.w/big2,v.h/big2,6+text_len(texts[206]),12,0,window[v_window+1].title,c4);
}

extern uint8_t cerror[128];
void get_error(int n);

void compile2(void) {
  if (compiled==0) {
    compiled=1; mouse_graph=3; error_number=-1;
    comp();
    if (error_number>=0) {
      //      strcpy(e_msg,texts[207]);
      //      itoa(error_number,e_msg+strlen(e_msg),10);
//      if (error_number>=10) {
        get_error(500+error_number);
      //        strcat(e_msg,texts[208]);
      //        itoa(error_line,e_msg+strlen(e_msg),10);
//      }
      //      strcat(e_msg,".");
      //      compilation_message(e_msg);
      compilation_message(cerror);
    } else {
      compilation_message(texts[202]);
      if (run_program) end_dialog=1;
    }
    mouse_graph=1; v.blit=1;
  }
  _process_items();
  if (v.active_item==0) end_dialog=1;
  else if (v.active_item==1) { v_help=1; end_dialog=1; }
}

void compile0(void) {
  v.type=1; v.w=300; v.h=46;
  switch (run_program) {
    case 0: v.title=texts[200]; break;
    case 1: v.title=texts[235]; break;
    case 2: v.title=texts[236]; break;
    case 3: v.title=texts[241]; break;
  }
  v.paint_handler=compile1;
  v.click_handler=compile2;
  _button(100,7,v.h-14,0);
  _button(125,v.w-8,v.h-14,2);
  compiled=0; v_help=0;
}

void compile_program(void) {
  dialog(compile0);
  read_key(); scan_code=0; ascii=0; mouse_b=0;
}

//═════════════════════════════════════════════════════════════════════════════
//  Code generation functions
//═════════════════════════════════════════════════════════════════════════════

// void g1(int op) { mem[imem++]=op; }
// void g2(int op, int pa) { mem[imem++]=op; mem[imem++]=pa; }

//═════════════════════════════════════════════════════════════════════════════
//  Peephole optimization of EML intermediate code
//═════════════════════════════════════════════════════════════════════════════

// *** NOTE!!! *** maybe "dir" and that absurd check can be removed ...

//struct {      // Optimization peephole, "sight"
//  int dir;    // Address
//  int param;  // Tells the number of parameters of the instruction
//  int op;     // Opcode
//} code[16];   // code[15] must always hold the last generated instruction

void g1(int op) {
  if (optimize) gen(0,op,0); else mem[imem++]=op;
}

void g2(int op, int pa) {
  if (optimize) gen(1,op,pa); else { mem[imem++]=op; mem[imem++]=pa; }
}

int optimized;

void gen(int param, int op, int pa) {
//  int n;

  optimized=0;

  switch(op) {
    case lcar:
      if (code[15].op==lcar) {
        code[15].op=mem[imem-2]=lcar2;
        code[15].param=2;
        mem[imem++]=pa; optimized=1;
      } else if (code[15].op==lcar2) {
        code[15].op=mem[imem-3]=lcar3;
        code[15].param=3;
        mem[imem++]=pa; optimized=1;
      } else if (code[15].op==lcar3) {
        code[15].op=mem[imem-4]=lcar4;
        code[15].param=4;
        mem[imem++]=pa; optimized=1;
      } break;
    case lasp:
      if (code[15].op==lasi) {
        if (code[14].op==lcar) {
          remove_code(1);
          code[15].op=mem[imem-2]=lcarasiasp;
        } else {
          code[15].op=mem[imem-1]=lasiasp;
          optimized=1;
        }
      } else if (code[15].op==lfun) {
        code[15].op=mem[imem-2]=lfunasp;
        optimized=1;
      } break;
    case laid:
      if (code[15].op==lcar) {
        code[15].op=mem[imem-2]=lcaraid;
        optimized=1;
      } break;
    case lptr:
      if (code[15].op==lcar) {
        code[15].op=mem[imem-2]=lcarptr;
        optimized=1;
      } else if (code[15].op==lcaradd) {
        code[15].op=mem[imem-2]=lcaraddptr;
        optimized=1;
      } else if (code[15].op==laid) {
        code[15].op=mem[imem-1]=laidptr;
        optimized=1;
      } else if (code[15].op==ladd) {
        code[15].op=mem[imem-1]=laddptr;
        optimized=1;
      } else if (code[15].op==lcaraid) {
        code[15].op=mem[imem-2]=lcaraidptr;
        optimized=1;
      } break;
    case lcpa:
      if (code[15].op==lcaraid) {
        code[15].op=mem[imem-2]=lcaraidcpa;
        optimized=1;
      } break;
    case ladd:
      if (code[15].op==lcar2){
        code[15].op=mem[imem-3]=lcar;
        code[15].param=1;
        mem[imem-2]+=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar3){
        code[15].op=mem[imem-4]=lcar2;
        code[15].param=2;
        mem[imem-2]+=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar4){
        code[15].op=mem[imem-5]=lcar3;
        code[15].param=3;
        mem[imem-2]+=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar){
        if (mem[imem-1]==0) remove_code(1); else {
          code[15].op=mem[imem-2]=lcaradd;
          optimized=1;
        }
      } else if (code[15].op==lcarmul){
        code[15].op=mem[imem-2]=lcarmuladd;
        optimized=1;
      } break;
    case lmul:
      if (code[15].op==lcar2){
        code[15].op=mem[imem-3]=lcar;
        code[15].param=1;
        mem[imem-2]*=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar3){
        code[15].op=mem[imem-4]=lcar2;
        code[15].param=2;
        mem[imem-2]*=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar4){
        code[15].op=mem[imem-5]=lcar3;
        code[15].param=3;
        mem[imem-2]*=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar){
        if (mem[imem-1]==1) remove_code(1); else {
          code[15].op=mem[imem-2]=lcarmul;
          optimized=1;
        }
      } break;
    case lsub:
      if (code[15].op==lcar2){
        code[15].op=mem[imem-3]=lcar;
        code[15].param=1;
        mem[imem-2]-=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar3){
        code[15].op=mem[imem-4]=lcar2;
        code[15].param=2;
        mem[imem-2]-=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar4){
        code[15].op=mem[imem-5]=lcar3;
        code[15].param=3;
        mem[imem-2]-=mem[imem-1];
        imem--; optimized=1;
      } else if (code[15].op==lcar){
        if (mem[imem-1]==0) remove_code(1); else {
          code[15].op=mem[imem-2]=lcarsub;
          optimized=1;
        }
      } break;
    case ldiv:
      if (code[15].op==lcar2){
        if (mem[imem-1]!=0) {
          code[15].op=mem[imem-3]=lcar;
          code[15].param=1;
          mem[imem-2]/=mem[imem-1];
          imem--; optimized=1;
        }
      } else if (code[15].op==lcar3){
        if (mem[imem-1]!=0) {
          code[15].op=mem[imem-4]=lcar2;
          code[15].param=2;
          mem[imem-2]/=mem[imem-1];
          imem--; optimized=1;
        }
      } else if (code[15].op==lcar4){
        if (mem[imem-1]!=0) {
          code[15].op=mem[imem-5]=lcar3;
          code[15].param=3;
          mem[imem-2]/=mem[imem-1];
          imem--; optimized=1;
        }
      } else if (code[15].op==lcar){
        if (mem[imem-1]==1) remove_code(1);
        else if (mem[imem-1]!=0) {
          code[15].op=mem[imem-2]=lcardiv; // A cardiv will never be "cardiv 0"
          optimized=1;
        }
      } break;
    case lneg:
      if (code[15].op==lcar || code[15].op==lcar2 || code[15].op==lcar3 || code[15].op==lcar4) {
        mem[imem-1]=-mem[imem-1]; optimized=1;
      }
      break;
    case lnot:
      if (code[15].op==lcar || code[15].op==lcar2 || code[15].op==lcar3 || code[15].op==lcar4) {
        mem[imem-1]=mem[imem-1]^-1; optimized=1;
      }
      break;
  }

  if (!optimized) {
    if (imem-code[15].dir>2) delete_code();
    add_code(imem,param,op);
    if (param) { mem[imem++]=op; mem[imem++]=pa; } else mem[imem++]=op;
  }

}

void remove_code(int i) {
  int n;
  optimized=1;
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
//      Precompilation
//═════════════════════════════════════════════════════════════════════════════

void plexer(void) {
  byte ** ptr, * _ivnames, h, * _source=source;
  struct lex_ele * e;

  if (!comment) {old_line=line_num; old_ierror=ierror; old_ierror_end=ierror_end;}

  lex_scan: ierror=_source;
  switch ((memptrsize)lex_case[*_source]) { // Pointer to a lex_ele or l_???

    case l_err:
      if (comment) { piece=p_rem; _source++; } else c_error(0,10); break;

    case l_cr :
      line_num++; if ((*++_source)==lf) { _source++; last_line=_source; goto lex_scan; }
      piece=p_last; break; // eof

    case l_id :
      if (comment) { piece=p_rem; _source++; break; }
      _ivnames=ivnames.b; *ivnames.p++=0; *ivnames.p++=0; h=0;
      while (*ivnames.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*ivnames.b++);
      ivnames.b++; _source--; if (ivnames.b-vnames>max_obj*long_med_id) c_error(0,100);
      ptr=&vhash[h];
      while (*ptr && strcmp((char *)(ptr+2),(char *)(_ivnames+ptr8))) ptr=(byte **)*ptr;
      if (!strcmp((char *)(ptr+2),(char *)_ivnames+ptr8)) { // id found
        ivnames.b=_ivnames; // takes it out of vnames
        piece=(memptrsize)*(ptr+1);
        if (piece<256 && piece>=0) { // reserved word (token)
          if (piece==p_rem) { while (*_source!=cr) _source++; goto lex_scan; }
        } else { // object (id previous)
          piece=p_id;
        }
      } else {
        ivnames.b=_ivnames; // takes it out of vnames
        piece=p_id; // new id
      } break;

    case l_spc:
      while ((*++_source)==' '); goto lex_scan;

    case l_lit:
      if (comment) { piece=p_rem; _source++; break; }
      piece=p_lit; h=*_source; _ivnames=ivnames.b; // Literal between two h
      do { if (*++_source==cr) c_error(0,11);
           if (*_source==h)
             if (*(_source+1)==h) *_ivnames=*++_source; else *_ivnames=0;
           else *_ivnames=*_source;
      } while (*_ivnames++); _source++;
      texts_length+=(strlen((char *)ivnames.b)+ptr4)/4;
      ivnames.b=_ivnames; // takes it out of vnames
      break;

    case l_num:
      if (comment) { piece=p_rem; _source++; break; }
      piece=p_num; piece_num=0;
      if (*_source=='0' && lower[*(_source+1)]=='x') {
        _source+=2;
        while ((memptrsize)lex_case[*_source]==l_num ||
               (lower[*_source]>='a' && lower[*_source]<='f')) {
          if ((memptrsize)lex_case[*_source]==l_num)
            piece_num=piece_num*16+*_source++-0x30;
          else piece_num=piece_num*16+lower[*_source++]-'a'+10;
        }
      } else do {
        piece_num=piece_num*10+*_source++-0x30;
      } while ((memptrsize)lex_case[*_source]==l_num);
      break;

    default: // pointer to a lex_ele
      e=lex_case[*_source++]; _ivnames=_source; piece=(*e).token;
      while (e=(*e).next) {
        while (*_source!=(*e).character && (*e).alternative) e=(*e).alternative;
        if (*_source++==(*e).character && (*e).token) {
          piece=(*e).token; _ivnames=_source;
        }
      } _source=_ivnames;

      if (piece==p_rem && !comment) {
        while (*_source!=cr) _source++;
        goto lex_scan;
      }

      if (piece==p_start_rem) {
        comment++;
        do {
           source=_source; plexer(); _source=source;
        } while (piece!=p_end_rem);
        comment--;
        goto lex_scan;
      }

      if (piece==p_last) if (comment) c_error(0,55); else c_error(0,12);
      break;

  } source=_source; ierror_end=_source-1;
}

void psyntactic(void) {
  byte * _ivnames=ivnames.b;

  texts_length=0;

  do {
    plexer();
  } while (piece!=p_last);

  ivnames.b=_ivnames;
  source=source_ptr;
  _source=source;
  comment=0;

}
