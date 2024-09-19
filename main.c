/* As it is clear ls's functionality depends on 

   1. parsing command line to figure out options.

   2. Sorting.

   3. format.

   4. number of colums.

   5. print multi directories.

   6. colour

*/

//---------------------------------------------<include LIBs>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "MyLS.h"
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                            Extern variables                           */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
extern char *Parr[10]; /*it is not mandatory to use dynamic allocation*/ 
extern int ArgNum;
//=========================================================================

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                              main Function                            */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
int main(int argc, char **argv)
{
    char opt = 0;
    int i = 0;

    //If there is error in number of arguments
    if ( argc < 0 ) /*i dont need to set statement argc < 1 because if there is not argument i will set the searching directort as the working directory*/
    {
        fprintf(stderr, "MyLS: Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }

    //Parsing command line to get num.
    //i will not handle case there is argument between two options
    while ( ( opt = getopt(argc, (char *const )argv, ":latucifd1") ) != -1 )
    {
        switch (opt) /*i can optimize this code by calling SetMyLSFeature() every time in loop after checking it is equal ? or :*/
        {
            case 't': SetMyLSFeature('t'); break;

            case 'u': SetMyLSFeature('u'); break;
            
            case 'c': SetMyLSFeature('c'); break;
            
            case 'f': SetMyLSFeature('f'); break;

            case 'l': SetMyLSFeature('l'); break;

            case 'a': SetMyLSFeature('a'); break;

            case 'i': SetMyLSFeature('i'); break;

            case '1': SetMyLSFeature('1'); break;

            case 'd': SetMyLSFeature('d'); break;
            
            case '?': fprintf(stderr, "MyLS: Bad option."); exit(EXIT_FAILURE); break;

            case ':': fprintf(stderr, "MyLS: Mission argument."); exit(EXIT_FAILURE); break;

            default: printf("MyLS: something werid with parsing options\n"); return -1;
        }
    }

    for (i = optind; i < argc; ++i)
    {
        Parr[ArgNum] = argv[i];
        ArgNum++;
    }

    //Call MyLS core to do the main functionality.
    MyLS_Core();

    return 0;
}
//=========================================================================

//===================================================================
//(_   _)( )                   ( )       ( )   ( )               ( )
//  | |  | |__    ____   ___   | |/ )     \ \_/ /  ___    _   _  | |
//  | |  |  _  \ / _  )/  _  \ | | <        \ /   /   \  ( ) ( ) | |
//  | |  | | | |( (_| || ( ) | | |\ \       | |  ( (_) ) | (_) | | |
//  (_)  (_) (_) \___ )(_) (_) (_) (_)      (_)   \___/   \___/  (_)