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
int ArgLocator = 0;
int iterateNum = 0;
int NumOfEntries = 0;
int EntriesLocator = 0;
long long total = 0;
//=========================================================================

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*                            Services function                          */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
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

static char * print_permission(char *permission)
{
    struct stat fileState;
    char path[250];
    int mode = 0, i = 0;

    snprintf(path, sizeof(path), "%s/%s", Parr[ArgLocator], entry[EntriesLocator]->d_name);

    if ( lstat(path, &fileState) == -1)
    {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    mode =  fileState.st_mode;

    //owner  permissions
    if((mode & 0000400) == 0000400) permission[0] = 'r';
    if((mode & 0000200) == 0000200) permission[1] = 'w';
    if((mode & 0000100) == 0000100) permission[2] = 'x';
    
    //group permissions
    if((mode & 0000040) == 0000040) permission[3] = 'r';
    if((mode & 0000020) == 0000020) permission[4] = 'w';
    if((mode & 0000010) == 0000010) permission[5] = 'x';
    
    //others  permissions
    if((mode & 0000004) == 0000004) permission[6] = 'r';
    if((mode & 0000002) == 0000002) permission[7] = 'w';
    if((mode & 0000001) == 0000001) permission[8] = 'x';
    
    //special  permissions
    if((mode & 0004000) == 0004000) permission[2] = 's';
    if((mode & 0002000) == 0002000) permission[5] = 's';
    if((mode & 0001000) == 0001000) permission[8] = 't';

    while ( i < 9 )
    {
        if ( permission[i] == 0 )
        {
            permission[i] = '-';
        }

        ++i;
    }

    return &permission[0];
}

static void Comparing_FUNC(struct dirent *entry1, struct dirent *entry2)
{
    struct stat file_stat1;
    struct stat file_stat2;
    char path1[250];
    char path2[250];


    if (G_MyLSFunctionality.SortType == AlphaBetaSorting )
    {
        return tolower(entry1->d_name[0]) > tolower(entry2->d_name[0]) ? -1 : tolower(entry1->d_name[0]) < tolower(entry2->d_name[0]) ? 1 : 0;
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

    NumOfEntries = 0;
    dp = opendir(Parr[iterateNum]);

    if ( G_MyLSFunctionality.AdditionalFiles == ShowDotFilesEnable )
    {
        while ( ( entry[NumOfEntries] = readdir(dp) ) != NULL )
        {
            if ( strcmp(".", entry[NumOfEntries]->d_name) || strcmp("..", entry[NumOfEntries]->d_name) )
            {
                snprintf(path, sizeof(path), "%s/%s", Parr[EntriesLocator], entry[NumOfEntries]->d_name);

                if ( lstat(path, &file_stat) == -1 )
                {
                    perror("lstat:");
                    exit(EXIT_FAILURE);
                }

                total += file_stat.st_blocks;
            }

            ++NumOfEntries;
        }
    
    } else
    {
        while ( ( entry[NumOfEntries] = readdir(dp) ) != NULL )
        {
            if ( entry[NumOfEntries]->d_name[0] != '.' )
            {
                entry[NumOfEntries] = NULL;

            } else
            {
                ++NumOfEntries;
            }
        }
    }

    closedir(dp);
}

static void SortEntries()
{
    qsort(entry, NumOfEntries, sizeof(entry), Comparing_FUNC);
}

static void print_OrdinaryFormat()
{

}
static void print_OneEntryPerLine()
{
    // if ( G_MyLSFunctionality.ShowInode == EnableInode )
    // {
    //     char *Format1 = "";


    // } else
    // {
    //     char *Format2 = "";
    // }
}

static void print_LongFormat()
{
    char *permission[15] = {NULL};
    char *Timestring = NULL;
    char *TypeOfFile = NULL;
    char Text_Buffer[500];
    struct stat fileState;
    char path[250];
    int mode = 0, i = 0;
    char buff[100];

    //Will print the directory text or not, depend on there is multidirectories or not
    if ( ArgNum != 1 ) 
    {
        printf("%s:\n", Parr[ArgLocator]);
    }

    //print total
    printf("%lu\n", total);


    while ( EntriesLocator < NumOfEntries )
    {
        snprintf(path, sizeof(path), "%s/%s", Parr[ArgLocator], entry[EntriesLocator]->d_name);

        if ( lstat(path, &fileState) == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        //adjust time before printing
        Timestring = ctime(fileState.st_mtime);
        Timestring[strlen(Timestring) - 1] = '\0';

        //Figure out file type
        TypeOfFile = (fileState.st_mode);

        if ( G_MyLSFunctionality.ShowInode == EnableInode )
        {
            snprintf(Text_Buffer, 500, "%15lu %15s %3d %10s %10s %10lu %25s ", 
                    fileState.st_ino,
                    print_permission(permission),
                    fileState.st_nlink,
                    get_User(fileState.st_uid),
                    get_Group(fileState.st_gid),
                    fileState.st_size,
                    Timestring);
        } else
        {
            snprintf(Text_Buffer, 500, "%15s %3d %10s %10s %10lu %25s ", 
                    print_permission(permission),
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
            if ( (readlink(path, buff, sizeof(buff) - 1) ) == -1 )
            {
                perror("readlink");
                exit(EXIT_FAILURE);
            }

            printf("%s%s%s%s\033[0m -> %s%s\033[0m\n", Text_Buffer, CYAN, entry[EntriesLocator]->d_name, BLUE, buff);

        } else
        {
            printf("%s%s\n", Text_Buffer, entry[EntriesLocator]->d_name);
        }

        ++EntriesLocator;
    }

    EntriesLocator = 0;
    total = 0;
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

        G_MyLSFunctionality.ShowDirectoryOnly = ShowDotFilesEnable;

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
        G_MyLSFunctionality.ShowDirectoryOnly = ShowDotFilesEnable;

    } else if ( option == 'd' )
    {
        G_MyLSFunctionality.ShowDirectoryOnly = ListDirectoryOnly;
    }
}

void MyLS_Core()
{
    char arr[BUFFER_MAX];
    int flag = 0;

    if ( ArgNum == 0 )
    {
        if ( getcwd(arr, BUFFER_MAX) == -1 )
        {
            perror("getcwd:");
            exit(EXIT_FAILURE);
        }

        Parr[0] = arr;

        if ( G_MyLSFunctionality.formatType != LongFormat )
        {
            G_MyLSFunctionality.formatType = OneEntryPerLine;
        }

        ++ArgNum;
        ++flag;
    }

    while ( ArgLocator < ArgNum )
    {
        //1. store entries in array.
        if ( G_MyLSFunctionality.ShowDirectoryOnly != ListDirectoryOnly )
        {
            store_entries();
        }

        //2. sort entries
        if ( G_MyLSFunctionality.SortType != DirectoryOrderSorting )
        {
            SortEntries();
        }

        ++ArgLocator;
    }

    if ( G_MyLSFunctionality.formatType == LongFormat )
    {
        //Printing start node (if any), permission for user, group and owner, owner username, group username, size, time and file name
        print_LongFormat();

    } else if ( G_MyLSFunctionality.formatType == print_OrdinaryFormat )
    {
        //printing Ordinary format
        print_OrdinaryFormat();

    } else if ( G_MyLSFunctionality.formatType == OneEntryPerLine )
    {
        //printing OneEntryPerLine format
        print_OneEntryPerLine();
    }
}
//===============================================================================================================================