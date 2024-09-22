//-----------------------------------------------------------<include LIBs>
#include "MyLS.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                                Macros                                 */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
//escape code to make the string coloured
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"
#define BLUE        "\033[1;34m"
#define MAGENTA     "\033[1;35m"
#define CYAN        "\033[1;36m"
#define BUFFER_MAX  150
//=========================================================================

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                              User define                              */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
typedef enum SetMyLSFormat
{
    OrdinaryFormat,
    OneEntryPerLine,
    LongFormat
};

typedef enum SetMyLSSorting
{
    AlphaBetaSorting,
    ModificationTimeSorting,
    AccessTimeSorting,
    ChangeTimeSorting,
    DirectoryOrderSorting
};

typedef enum AdditionalFeatures
{
    EnableInode = 1,
    ShowDotFilesEnable,
    ListDirectoryOnly
};

struct S_MyLSFunctionality
{
    int formatType;
    int SortType;
    int ShowInode;
    int AdditionalFiles; /*Show files starts with ., in other meaning show hidden files*/
    int ShowDirectoryOnly; /*Show the working directory only*/

};
//=========================================================================

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                              Global variables                         */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
struct S_MyLSFunctionality G_MyLSFunctionality = {0, 0, 0, 0, 0};
struct dirent *entry[150];
char *Parr[10] = {0}; /*it is not mandatory to use dynamic allocation*/ 
int ArgNum = 0;
int ArgLocator = 0, Indicator = 0;
int iterateNum = 0;
int NumOfEntries = 0;
int EntriesLocator = 0;
char arr[BUFFER_MAX];
long long total = 0;
//=========================================================================

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                            Services function                          */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static char* ReturnFileType(int FileMode)
{
   if ((FileMode &  0170000) == 0010000) 
   {
        return "Named Pipe";

   } else if ((FileMode &  0170000) == 0020000) 
   {
        return "Character Special file";

   } else if ((FileMode &  0170000) == 0040000) 
   {
        return "Directory";

   } else if ((FileMode &  0170000) == 0060000) 
   {
        return "Block Special file";

   } else if ((FileMode &  0170000) == 0100000) 
   {
        return "Regular file";

   } else if ((FileMode &  0170000) == 0120000) 
   {
        return "Symbolic Link";

   } else if ((FileMode &  0170000) == 0140000) 
   {
        return "Socket";

   } else
   {
        return "Unknwon mode";
   }

}

static char* get_User(int UID)
{
    struct passwd * pwd = getpwuid(UID);

    return pwd->pw_name;
}

static char* get_Group(int GID)
{
   struct group * grp = getgrgid(GID);

   return grp->gr_name;
}

static char * return_permission(int mode, char* permission)
{
    struct stat fileState;
    char path[250];
    int i = 0;

   if ((mode &  0170000) == 0010000) 
   {
        permission[0] = 'p';

   } else if ((mode &  0170000) == 0020000) 
   {
        permission[0] = 'c';

   } else if ((mode &  0170000) == 0040000) 
   {
        permission[0] = 'd';

   } else if ((mode &  0170000) == 0060000) 
   {
        permission[0] = 'b';

   } else if ((mode &  0170000) == 0100000) 
   {
        permission[0] = '-';

   } else if ((mode &  0170000) == 0120000) 
   {
        permission[0] = 'l';

   } else if ((mode &  0170000) == 0140000) 
   {
        permission[0] = 's';
   }

    //owner  permissions
    if((mode & 0000400) == 0000400) permission[1] = 'r';
    if((mode & 0000200) == 0000200) permission[2] = 'w';
    if((mode & 0000100) == 0000100) permission[3] = 'x';
    
    //group permissions
    if((mode & 0000040) == 0000040) permission[4] = 'r';
    if((mode & 0000020) == 0000020) permission[5] = 'w';
    if((mode & 0000010) == 0000010) permission[6] = 'x';
    
    //others  permissions
    if((mode & 0000004) == 0000004) permission[7] = 'r';
    if((mode & 0000002) == 0000002) permission[8] = 'w';
    if((mode & 0000001) == 0000001) permission[9] = 'x';
    
    //special  permissions
    if((mode & 0004000) == 0004000) permission[3] = 's';
    if((mode & 0002000) == 0002000) permission[6] = 's';
    if((mode & 0001000) == 0001000) permission[9] = 't';

    while ( i < 10 )
    {
        if ( permission[i] == 0 )
        {
            permission[i] = '-';
        }

        ++i;
    }

    return &permission[0];
}

static int Comparing_FUNC(void *ptr1, void *ptr2)
{
    struct stat file_stat1;
    struct stat file_stat2;
    struct dirent *entry1 = *(unsigned long*)ptr1;
    struct dirent *entry2 = *(unsigned long*)ptr2;
    char path1[250];
    char path2[250];


    if (G_MyLSFunctionality.SortType == AlphaBetaSorting )
    {
        if ( strcmp(".", entry1->d_name) == 0 )
        {
            return -1;

        } else if ( strcmp(".", entry2->d_name) == 0 ) 
        {
            return 1;

        } else if ( strcmp("..", entry1->d_name) == 0 )
        {
            return -1;

        } else if ( strcmp("..", entry2->d_name) == 0 )
        {
            return 1;
        }

        if ( entry1->d_name[0] == '.' && entry2->d_name[0] == '.' )
        {
            return tolower(entry1->d_name[1]) > tolower(entry2->d_name[1]) ? 1 : tolower(entry1->d_name[1]) < tolower(entry2->d_name[1]) ? -1 : 0;
        
        } else if ( entry1->d_name[0] == '.' )
        {
            return tolower(entry1->d_name[1]) > tolower(entry2->d_name[0]) ? 1 : tolower(entry1->d_name[1]) < tolower(entry2->d_name[0]) ? -1 : 0;

        } else if ( entry2->d_name[0] == '.' )
        {
            return tolower(entry1->d_name[0]) > tolower(entry2->d_name[1]) ? 1 : tolower(entry1->d_name[0]) < tolower(entry2->d_name[1]) ? -1 : 0;
        }

        return tolower(entry1->d_name[0]) > tolower(entry2->d_name[0]) ? 1 : tolower(entry1->d_name[0]) < tolower(entry2->d_name[0]) ? -1 : 0;
    } 
    
    snprintf(path1, sizeof(path1), "%s/%s", Parr[ArgLocator], entry1->d_name);
    snprintf(path2, sizeof(path2), "%s/%s", Parr[ArgLocator], entry2->d_name);

    if ( lstat(path1, &file_stat1) == -1 )
    {
        perror("lstat:");
        exit(EXIT_FAILURE);
    }

    if ( lstat(path2, &file_stat2) == -1 )
    {
        perror("lstat:");
        exit(EXIT_FAILURE);
    }

    if ( G_MyLSFunctionality.SortType == ModificationTimeSorting )
    {
        return file_stat1.st_mtime > file_stat2.st_mtime ? -1 : file_stat1.st_mtime < file_stat2.st_mtime ? 1 : 0;

    } else if ( G_MyLSFunctionality.SortType == AccessTimeSorting )
    {
        return file_stat1.st_atime > file_stat2.st_atime ? -1 : file_stat1.st_atime < file_stat2.st_atime ? 1 : 0;

    } else if ( G_MyLSFunctionality.SortType == ChangeTimeSorting )
    {
        return file_stat1.st_ctime > file_stat2.st_ctime ? -1 : file_stat1.st_ctime < file_stat2.st_ctime ? 1 : 0;
    }
}

static void store_entries()
{
    DIR *dp;
    char path[250];
    struct stat file_stat;
    struct dirent *Temp;

    dp = opendir(Parr[ArgLocator]);

    if ( G_MyLSFunctionality.AdditionalFiles == ShowDotFilesEnable )
    {
        while ( ( Temp = readdir(dp) ) != NULL )
        {
            if (G_MyLSFunctionality.formatType == LongFormat)
            {
                entry[NumOfEntries] = (struct dirent *)malloc(sizeof(struct dirent));

                memcpy(entry[NumOfEntries], Temp, sizeof(struct dirent));

                if ( strcmp(".", entry[NumOfEntries]->d_name) || strcmp("..", entry[NumOfEntries]->d_name) )
                {
                    snprintf(path, sizeof(path), "%s/%s", Parr[ArgLocator], entry[NumOfEntries]->d_name);

                    if ( lstat(path, &file_stat) == -1 )
                    {
                        perror("lstat:");
                        exit(EXIT_FAILURE);
                    }   

                    total += file_stat.st_blocks;
                }
                            
                ++NumOfEntries;
            }
        }
    
    } else if (G_MyLSFunctionality.ShowDirectoryOnly == ListDirectoryOnly)
    {
        while ( ( Temp = readdir(dp) ) != NULL )
        {
            if (strcmp( Temp->d_name, ".") == 0 )
            {
                entry[NumOfEntries] = (struct dirent *)malloc(sizeof(struct dirent));
                
                memcpy(entry[NumOfEntries], Temp, sizeof(struct dirent));

                ++NumOfEntries;
            }
        }
        
    } else
    {
        while ( ( Temp = readdir(dp) ) != NULL )
        {
            if (Temp->d_name[0] != '.')
            {
                entry[NumOfEntries] = (struct dirent *)malloc(sizeof(struct dirent));
                
                memcpy(entry[NumOfEntries], Temp, sizeof(struct dirent));

                if (G_MyLSFunctionality.formatType == LongFormat)
                {
                    snprintf(path, sizeof(path), "%s/%s", Parr[ArgLocator], entry[NumOfEntries]->d_name);

                    if ( lstat(path, &file_stat) == -1 )
                    {
                        perror("lstat:");
                        exit(EXIT_FAILURE);
                    }   

                    total += file_stat.st_blocks;
                }

                ++NumOfEntries;
            }
        }
    }
    closedir(dp);
}

static void print_AnotherFormat()
{
    struct stat fileState;
    char path[250];
    char *TypeOfFile = NULL;

    while (EntriesLocator < NumOfEntries)
    {
        snprintf(path, sizeof(path), "%s/%s", Parr[ArgLocator], entry[EntriesLocator]->d_name);

        if ( lstat(path, &fileState) == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        //Figure out file type
        TypeOfFile = ReturnFileType(fileState.st_mode);

        if ( ( G_MyLSFunctionality.formatType == OrdinaryFormat && strcmp(TypeOfFile, "Symbolic Link") ) || G_MyLSFunctionality.formatType == OneEntryPerLine )
        {
            if ( G_MyLSFunctionality.ShowInode == EnableInode )
            {
                printf("%-7lu ", fileState.st_ino);
            }

            if ( strcmp(TypeOfFile, "Symbolic Link") == 0 )
            {
                printf("%s%-18s\033[0m", CYAN, entry[EntriesLocator]->d_name );
  
            } else if ( strcmp(TypeOfFile, "Directory") == 0 )
            {
                printf("%s%-18s\033[0m", BLUE, entry[EntriesLocator]->d_name );

            } else if ( fileState.st_mode & 0b1)
            {
                printf("%s%-18s\033[0m", GREEN, entry[EntriesLocator]->d_name );

            } else
            {
                printf("%-18s", entry[EntriesLocator]->d_name);
            }

            if ( G_MyLSFunctionality.formatType == OneEntryPerLine )
            {
                printf("\n");
            }
        }
        
        ++EntriesLocator;
    }

    if ( ArgLocator < ( ArgNum - 1 ) || ArgNum == 1 )
    {
        printf("\n");
        EntriesLocator = 0;
        NumOfEntries = 0;
        total = 0;
    }
}

static void print_LongFormat()
{
    char permission[20] = {0};
    char *Timestring = NULL;
    char *TypeOfFile = NULL;
    char Text_Buffer[1000];
    struct stat fileState;
    char path[250];
    int mode = 0, i = 0;
    char buff[100];
    int len = 0;

    //print total
    if (total)
    {
        //Will print the directory text or not, depend on there is multidirectories or not
        if ( ArgNum != 1 ) 
        {
            if (Indicator)
            {
                printf(".:\n");

                Indicator = 0;

            } else
            {
                printf("%s:\n", Parr[ArgLocator]);
            }
        }
        printf("total %lu\n", total / 2);
    }

    while ( EntriesLocator < NumOfEntries )
    {
        snprintf(path, sizeof(path), "%s/%s", Parr[ArgLocator], entry[EntriesLocator]->d_name);

        if ( lstat(path, &fileState) == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        //adjust time before printing
        Timestring = ctime(&(fileState.st_mtime));
        Timestring[strlen(Timestring) - 1] = '\0';

        //Figure out file type
        TypeOfFile = ReturnFileType(fileState.st_mode);

        if ( G_MyLSFunctionality.ShowInode == EnableInode )
        {
            snprintf(Text_Buffer, 1000, "%-7lu %s %3d %9s %8s %5lu %20s ", 
                    fileState.st_ino,
                    return_permission(fileState.st_mode, permission),
                    fileState.st_nlink,
                    get_User(fileState.st_uid),
                    get_Group(fileState.st_gid),
                    fileState.st_size,
                    Timestring);
        } else
        {
            snprintf(Text_Buffer, 1000, "%s %3d %9s %8s %5lu %20s ", 
                    return_permission(fileState.st_mode, permission),
                    fileState.st_nlink,
                    get_User(fileState.st_uid),
                    get_Group(fileState.st_gid),
                    fileState.st_size,
                    Timestring);
        }

        if ( strcmp(TypeOfFile, "Directory") == 0 )
        {
            printf("%s%s%s\033[0m\n", Text_Buffer, BLUE, entry[EntriesLocator]->d_name);

        } else if ( strcmp(TypeOfFile, "Symbolic Link") == 0 )
        {
            if ( (len = readlink(path, buff, sizeof(buff) - 1) ) == -1 )
            {
                perror("readlink");
                exit(EXIT_FAILURE);
            }

            buff[len] = '\0';

            printf("%s%s%s\033[0m -> %s%s\033[0m\n", Text_Buffer, CYAN, entry[EntriesLocator]->d_name, BLUE, buff);

        } else if ( fileState.st_mode & 0b1)
        {
            printf("%s%s%s\033[0m\n", Text_Buffer, GREEN , entry[EntriesLocator]->d_name);

        } else
        {
            printf("%s%s\n", Text_Buffer, entry[EntriesLocator]->d_name);
        }

        ++EntriesLocator;

        memset(permission, 0, sizeof(permission));
    }

    if ( ArgLocator < ArgNum - 1 )
    {
        printf("\n");
        EntriesLocator = 0;
        NumOfEntries = 0;
        total = 0;
    }
}

//=========================================================================

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                          Function implementation                      */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
void SetMyLSFeature(char option)
{
    if ( option == 't' )
    {
        G_MyLSFunctionality.SortType = ModificationTimeSorting;

    } else if ( option == 'u' )
    {
        G_MyLSFunctionality.SortType = AccessTimeSorting;

    } else if ( option == 'c' )
    {
        G_MyLSFunctionality.SortType = ChangeTimeSorting;

    } else if ( option == 'f' )
    {
        G_MyLSFunctionality.SortType = DirectoryOrderSorting;

        G_MyLSFunctionality.AdditionalFiles = ShowDotFilesEnable;

    } else if ( option == '1' )
    {
        if ( G_MyLSFunctionality.formatType != LongFormat )
        {
            G_MyLSFunctionality.formatType = OneEntryPerLine;
        }

    } else if ( option == 'l' )
    {
        G_MyLSFunctionality.formatType = LongFormat;

    } else if ( option == 'i' )
    {
        G_MyLSFunctionality.ShowInode = EnableInode;

    } else if ( option == 'a' )
    {
        G_MyLSFunctionality.AdditionalFiles = ShowDotFilesEnable;

    } else if ( option == 'd' )
    {
        G_MyLSFunctionality.ShowDirectoryOnly = ListDirectoryOnly;
    }
}

void MyLS_Core()
{
    if ( ArgNum == 0 || strcmp(Parr[0], ".") == 0 ) /*i will handle case there is just one .*/
    {
        if ( getcwd(arr, BUFFER_MAX) == -1 )
        {
            perror("getcwd:");
            exit(EXIT_FAILURE);
        }

        Parr[0] = arr;
        ++Indicator;

        if ( ArgNum == 0 )
        {
            ++ArgNum;
        }
    }

    while ( ArgLocator < ArgNum )
    {
        //1. store entries in array.
        store_entries();

        //2. sort entries
        if ( G_MyLSFunctionality.SortType != DirectoryOrderSorting )
        {
            qsort(entry, NumOfEntries, sizeof(struct dirent *), Comparing_FUNC);
        }

        if ( G_MyLSFunctionality.formatType == LongFormat )
        {
            //Printing start node (if any), permission for user, group and owner, owner username, group username, size, time and file name
            print_LongFormat();

        } else if ( G_MyLSFunctionality.formatType == OrdinaryFormat || G_MyLSFunctionality.formatType == OneEntryPerLine )
        {
            //printing Ordinary format
            print_AnotherFormat();
        }

        ++ArgLocator;
    }
}
//===============================================================================================================================