#ifndef __MYLS__ /*Header protection*/
#define __MYLS__
//===========================================<include LIBs>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                          Function prototype                           */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
void MyLS_Core();
void SetMyLSFeature(char option);
//=========================================================================

#endif