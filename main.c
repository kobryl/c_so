#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char** argv) {
    DIR *pDIR;
    struct dirent *pDirEnt;
    char* name;
    pDIR = opendir(".");

    if(pDIR == NULL) {
        fprintf(stderr, "%s %d: opendir() failed (%s)\n",
                __FILE__,__LINE__, strerror(errno));
        exit(-1);
    }

    pDirEnt = readdir(pDIR);
    while(pDirEnt != NULL){
        struct stat st;
        stat(pDirEnt->d_name, &st);
        long int size = st.st_size;
        struct passwd* user = getpwuid(st.st_uid);
        struct group* group = getgrgid(st.st_gid);
        unsigned long int nlinks = st.st_nlink;
        long int date = st.st_mtime;
        char* mod = "TBD";
        char* t = ctime(&date);
        char type;
        switch (pDirEnt->d_type) {
            case DT_DIR:
                type = 'd';
                break;
            case DT_BLK:
                type = 'b';
                break;
            case DT_CHR:
                type = 'c';
                break;
            case DT_LNK:
                type = 'l';
                break;
            case DT_FIFO:
                type = 'p';
                break;
            case DT_SOCK:
                type = 's';
                break;
            default:
                type = '-';
                break;
        }
        printf("%s %lu %s %s %ld %s %s\n", mod, nlinks, user->pw_name, group->gr_name, size, strtok(t, "\n"), pDirEnt->d_name);
        //		printf("%s %i\n", pDirEnt->d_name, st.st_size);
        pDirEnt = readdir(pDIR);
    }
    closedir(pDIR);
    return 0;
}