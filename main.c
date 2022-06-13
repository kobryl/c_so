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

#define BUF_SIZE 255

void ls(const char* name) {
    DIR *pDIR;
    char** dirs = NULL;
    int currentDirIndex = 0;
    struct dirent *pDirEnt;
    pDIR = opendir(name);

    if(pDIR == NULL) {
        fprintf(stderr, "%s %d: opendir() failed (%s)\n",
                __FILE__,__LINE__, strerror(errno));
        exit(-1);
    }

    pDirEnt = readdir(pDIR);
    while(pDirEnt != NULL){
        struct stat st;
        if (pDirEnt->d_name[0] == '.') {
            pDirEnt = readdir(pDIR);
        }
        else {
            char filename[BUF_SIZE];
            strcpy(filename, name);
            strcat(filename, "/");
            strcat(filename, pDirEnt->d_name);
            stat(filename, &st);
            long int size = st.st_size;
            struct passwd *user = getpwuid(st.st_uid);
            struct group *group = getgrgid(st.st_gid);
            unsigned long int nlinks = st.st_nlink;
            long int date = st.st_mtime;
            char *t = ctime(&date);
            char type;
            char *mod = malloc(sizeof(char) * 9 + 1);
            mode_t mode = st.st_mode;
            mod[0] = (mode & S_IRUSR) ? 'r' : '-';
            mod[1] = (mode & S_IWUSR) ? 'w' : '-';
            mod[2] = (mode & S_IXUSR) ? 'x' : '-';
            mod[3] = (mode & S_IRGRP) ? 'r' : '-';
            mod[4] = (mode & S_IWGRP) ? 'w' : '-';
            mod[5] = (mode & S_IXGRP) ? 'x' : '-';
            mod[6] = (mode & S_IROTH) ? 'r' : '-';
            mod[7] = (mode & S_IWOTH) ? 'w' : '-';
            mod[8] = (mode & S_IXOTH) ? 'x' : '-';
            mod[9] = '\0';
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
            printf("%c%s %lu %s %s %ld %s %s\n", type, mod, nlinks, user->pw_name, group->gr_name, size,
                   strtok(t, "\n"), pDirEnt->d_name);
            //		printf("%s %i\n", pDirEnt->d_name, st.st_size);
            pDirEnt = readdir(pDIR);
            free(mod);
        }
    }
    rewinddir(pDIR);
    pDirEnt = readdir(pDIR);
    while(pDirEnt != NULL){
        struct stat st;
        if (pDirEnt->d_name[0] == '.') {
            pDirEnt = readdir(pDIR);
        }
        else {
            char filename[BUF_SIZE];
            strcpy(filename, name);
            strcat(filename, "/");
            strcat(filename, pDirEnt->d_name);
            stat(filename, &st);
            switch (pDirEnt->d_type) {
                case DT_DIR:
                    if (getuid() == st.st_uid || (st.st_mode & S_IROTH && st.st_mode & S_IXOTH)) {
                        printf("\n\n%s\n", filename);
                        ls(filename);
                    }
                    break;
            }
            //		printf("%s %i\n", pDirEnt->d_name, st.st_size);
            pDirEnt = readdir(pDIR);
        }
    }
    closedir(pDIR);
}

int main(int argc, char** argv) {
    DIR *pDIR;
    struct dirent *pDirEnt;
    char* name = ".";
    ls(name);
    return 0;
}