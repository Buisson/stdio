/*
 * File:   main.c
 * Author: ca309567
 *
 * Created on 27 mai 2014, 16:40
 */


#include "../stdio.h"
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>


#define MAXBUFF 1024
#define PATH_MAX 1024

void tracer(FILE *f) {
    char buffer[500];
    snprintf(buffer, 500, "Trace FILE %p\n", f);
    write(2, buffer, strlen(buffer));
    snprintf(buffer, 500,
            "\tcnt: %d, ptr: %p, base: %p, bufsiz: %d, flag:%x, file:%d\n\n",
            f->_cnt, f->_ptr, f->_base, f->_bufsiz, f->_flag, f->_file);
    write(2, buffer, strlen(buffer));
}

int is_dir(char *f) {
    struct stat buff;

    return 0; //((stat(f, &buff) == 0) && buff.st_mode &S_IFDIR);
}

void usage(void) {
    //  fprintf(stderr, "Usage: cp f1 f2; or: cp  f1 ... fn d\n");
    exit(1);
}

char* basename(char *f) {
    char *p = strrchr(f, '/');

    return p ? (p + 1) : f;
}

int same_files(char *f, char *g) {
    struct stat fs, gs;

    if (stat(f, &fs) || stat(g, &gs)) return 0;
    /* Les deux fichiers existent. Verifier s'ils sont identiques */
    return (fs.st_dev == gs.st_dev && fs.st_ino == gs.st_ino);
}

void copy_file(char *f, char *g)
/* Copie de f sur g. les deux paramètres sont des noms de fichiers */ {
    int fd, gd, n;
    char buff[MAXBUFF];

    /* Essayer d'ouvrir les fichiers f et g */
    if ((fd = open(f, O_RDONLY)) == -1 ||
            (gd = open(g, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        perror((fd == -1) ? f : g);
        exit(1);
    }

    /* Les fichiers sont ouverts; copier */
    while ((n = read(fd, buff, MAXBUFF)) > 0) {
        if (write(gd, buff, n) < n) {
            perror("cp");
            exit(1);
        }
    }

    if (n < 0) {
        perror("cp");
        exit(1);
    }

    /* Fermeture des fichiers */
    close(fd);
    close(gd);
}

void copy(char *s, char *d) {
    char dest[PATH_MAX];

    if (is_dir(s)) {
        fprintf(stderr, "cp: %s: Is a directory (not copied).\n", s);
        exit(1);
    }

    if (is_dir(d)) {
        sprintf(dest, "%s/%s", d, basename(s));
        d = dest;
    }

    if (same_files(s, d)) {
        fprintf(stderr, "cp: %s and %s are identical (not copied).\n", s, d);
        exit(1);
    }

    copy_file(s, d);
}

/*
 *
 
int main(int argc, char** argv) {
    init();

    char *dest;

    ///* Vérifier qu'il y assez d'arguments
    if (argc <= 2) usage();

    ///* Si plus de deux arguments vérifier que le dernier est un répertoire
    dest = argv[--argc];
    if (argc > 2 && !is_dir(dest)) usage();

    ///* C'est bon, on peut  copier
    while (--argc)
        copy(*++argv, dest);

    char buf[BUFSIZ];
    FILE* fp;
    fp = popen("ls", "w");
    if (fp == NULL) {
        write(2, "null\n", strlen("null\n"));
    }
    pclose(fp);
    return 0;
}
 */