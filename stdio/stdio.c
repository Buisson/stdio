#include "../stdio.h"
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NUMBER_OF_FILE 5000
struct _iobuf _IOB[NUMBER_OF_FILE];
char errmsg[500];
char *tmp_file[NUMBER_OF_FILE];
void _allocate(FILE *);
void init();

//Cette fonction est appellé lors de la fermeture du programme, ferme tous les files descriptor et free la memoire
//(particulierement utile pour l'implementation de la fonction tmpfile )

static void on_exit() {

    for (int i = NUMBER_OF_FILE; i >= 0; i--) {

        if (fclose(&_IOB[i]) == -1) {
            continue;
        }
        if ((&_IOB[i])->_flag & _IOMYBUF) {
            free((&_IOB[i])->_base);
        }
        (_IOB[i]._base) = NULL;
        (_IOB[i]._ptr) = NULL;
    }

}

//Cette fonction doit etre appellé a chaque debut de programme, il y a certainement un moyen de l'inclure
//automatique avec gcc, mais pour notre test d'implementation de stdio.h, il sera inclut manuellement dans le main

void init() {

    FILE * f0 = malloc(sizeof (FILE));
    f0->_file = 0;
    f0->_flag = _IOMYBUF | _IOREAD | _IONBF;
    _allocate(f0);
    _IOB[0] = *f0;

    FILE * f1 = malloc(sizeof (FILE));
    f1->_file = 1;
    f1->_flag = _IOMYBUF | _IOWRT | _IOLBF;
    _allocate(f1);
    _IOB[1] = *f1;

    FILE * f2 = malloc(sizeof (FILE));
    f2->_file = 2;
    f2->_flag = _IOMYBUF | _IOWRT | _IONBF;
    _allocate(f2);
    _IOB[2] = *f2;

    if (atexit(on_exit)) {
        fprintf(stderr, "Failed to register the call back\n");

    }
}

//Vide le buffer dans le fichier

int _flsbuf(unsigned char c, FILE* stream) {

    if (stream == NULL) {
        for (int i = 0; &_IOB[i] != NULL; i++) {
            if ((&_IOB[i])->_flag & _IOWRT || (&_IOB[i])->_flag & _IORW) {
                if (write((&_IOB[i])->_file, (&_IOB[i])->_base, (&_IOB[i])->_bufsiz) == -1) {
                    perror("stdio.c:");
                    (&_IOB[i])->_flag |= _IOERR;
                    return -1;
                }
            }

            for (int i = 0; i < (&_IOB[i])->_bufsiz; i++) {
                (&_IOB[i])->_base[i] = '\0';
            }
            (&_IOB[i])->_ptr = (&_IOB[i])->_base;
            (&_IOB[i])->_cnt = (&_IOB[i])->_bufsiz;

        }
    } else {
        if (stream->_flag & _IOWRT || stream->_flag & _IORW) {
            if (write(stream->_file, stream->_base, stream->_bufsiz) == -1) {
                perror("stdio.c:");
                stream->_flag |= _IOERR;
                return -1;
            }
        }

        for (int i = 0; i < stream->_bufsiz; i++) {
            stream->_base[i] = '\0';
        }
        stream->_ptr = stream->_base;
        stream->_cnt = stream->_bufsiz;
        return 0;
    }
}

//Malloc de _base pour preparer le buffer a etre rempli

void _allocate(FILE * f) {

    if (f->_flag & _IOWRT) {
        f->_cnt = BUFSIZ;
    } else {
        f->_cnt = 0;
    }

    //todo faire les controles.verif si il y a un buffer.si pas buffer allouer un buffer.verif si le fichier est ouvert en lecture.(ne pas faire pour le moment)
    if (!f->_base) {

        f->_bufsiz = BUFSIZ;
        f->_base = malloc(sizeof (char)*f->_bufsiz);
        f->_ptr = f->_base;
    } else {
        f->_ptr = f->_base;
    }
}

//notre fopen ne prend pas en charge le mode 'b' pour binaire, etant donnée que ce parametre
//n'est de toute maniere pas pris en compte sous linux, et n'est meme pas posix. (cf: man fopen)

FILE *fopen(const char *path, const char *mode) {
    FILE * f = malloc(sizeof (FILE));
    int flag;

    if (!strcmp(mode, "r")) {
        f->_flag = _IOREAD;
        flag = O_RDONLY;
    } else if (!strcmp(mode, "r+")) {
        f->_flag = (_IORW);
        flag = O_RDWR;
    } else if (!strcmp(mode, "w")) {
        f->_flag = (_IOWRT);
        flag = O_WRONLY | O_TRUNC | O_CREAT;
    } else if (!strcmp(mode, "w+")) {
        f->_flag = (_IORW);
        flag = O_RDWR | O_CREAT;
    } else if (!strcmp(mode, "a")) {
        f->_flag = (_IOWRT | _IOEOF);
        flag = O_APPEND | O_WRONLY;

    } else if (!strcmp(mode, "a+")) {
        f->_flag = (_IORW | _IOEOF);
        flag = O_APPEND | O_RDWR;
    } else {
        f->_flag = (_IOERR);
        snprintf(errmsg, 500, "Bad mode, die.\n");
        write(2, errmsg, strlen(errmsg));
    }

    if ((f->_file = open(path, flag, 0666)) == -1) {
        snprintf(errmsg, 500, "Open fail, die.\n");
        write(2, errmsg, strlen(errmsg));
        f->_flag |= _IOERR;
        return NULL;
    }

    _allocate(f);
    if (!(f->_flag & _IOERR)) {
        _IOB[(int) f->_file] = *f;
        return f;
    } else {
        return NULL;
    }
}

void setbuf(FILE *stream, char *buf) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}

int setvbuf(FILE *stream, char *buf, int mode, int size) {
    if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF) {
        return -1;
    }
    stream->_ptr = buf;
    stream->_base = buf;
    stream->_flag &= ~_IOFBF;
    stream->_flag &= ~_IOLBF;
    stream->_flag &= ~_IONBF;
    stream->_flag |= mode;
    stream->_bufsiz = size;
    return 0;

}

int printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    char * arguments[count_arg(format)];
    for (int i = 0; i < count_arg(format); i++) {
        arguments[i] = va_arg(ap, char*);
    }
    va_end(ap);

    int numberChar = fprintf2(stdout, format, arguments);
    return numberChar;

}


//compte le nombre d'occurrence de "%[[:alpha:]]", utile pour l'implementation de sprintf, fprintf,printf
int count_arg(const char *str);

int count_arg(const char *str) {
    regex_t regex;
    int reti;
    regmatch_t match[5000];
    int numberMatch = 0;
    char * tmp = malloc(sizeof (str) * strlen(str));
    strcpy(tmp, str);
    char * tmpPointer = tmp;
    /* Compile regular expression */
    reti = regcomp(&regex, "(%[[:alpha:]])", REG_ICASE | REG_EXTENDED);
    if (reti) {
        snprintf(errmsg, 500, "Could not compile regex\n");
        write(2, errmsg, strlen(errmsg));
        exit(1);
    }
    do {

        /* Execute regular expression */
        reti = regexec(&regex, tmp, 5000, match, 0);
        if (!reti && match[0].rm_eo != -1) {
            numberMatch++;
            tmp = tmp + match[0].rm_eo;
        } else if (reti == REG_NOMATCH) {
            //   write(2, "no match", strlen("no match"));
            break;
        } else {
            exit(-1);
        }

    } while (!reti && match[0].rm_eo != -1);
    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);

    free(tmpPointer);
    return numberMatch;
}

/*
 Je voulais passer les parametres "..." aux autres fonctions, mais toute les recherches que j'ai
 * fait m'ont amené à soit "c'est impossible", soit "il faut activer une extension gcc pour c++ pour le faire"
 * donc vu que je ne savais pas comment faire, les fonctions nommé du type ".+\d" contiennent le code copier coller de mes autres fonctions
 */

int fprintf2(FILE *stream, const char *format, char* arguments[]) {
    int sizeOfString = strlen(format);
    char* argument;
    for (int i = 0; i < count_arg(format); i++) {
        sizeOfString += strlen(arguments[i]);
    }
    char str[(strlen(format) + sizeOfString)];
    sprintf2(str, format, arguments);
    return fputs(str, stream);
}

int fprintf(FILE *stream, const char *format, ...) {
    int sizeOfString = strlen(format);
    va_list ap;
    va_start(ap, format);
    char* argument;
    char* arguments[count_arg(format)];
    for (int i = 0; i < count_arg(format); i++) {
        argument = va_arg(ap, char*);
        arguments[i] = argument;
        sizeOfString += strlen(argument);
    }
    va_end(ap);

    char str[(strlen(format) + sizeOfString)];
    sprintf2(str, format, arguments);
    // write(2, str, strlen(str));
    return fputs(str, stream);
}

//Dans l'etat actuel, sprintf fonctionne uniquement si ses parametres sont des char * (%s), pas d'autres
//type disponible actuellement

int sprintf(char *str, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    char* argument;
    char* arguments[count_arg(format)];

    int sizeOfString = strlen(format);

    for (int i = 0; i < count_arg(format); i++) {
        argument = va_arg(ap, char*);
        arguments[i] = argument;
        sizeOfString += strlen(argument);
        write(2, argument, strlen(argument));
    }

    va_end(ap);

    int nbArguments = count_arg(format);
    regmatch_t match[nbArguments];
    char *tmp = malloc(sizeof (char) * (strlen(format) + sizeOfString));
    char *tmpPointer = tmp;
    strcpy(tmp, format);
    strcpy(str, "");

    regex_t regex;
    int reti;

    /* Compile regular expression */
    reti = regcomp(&regex, "%[[:alpha:]]", REG_ICASE | REG_EXTENDED);
    if (reti) {
        char * err = "Could not compile regex\n";
        write(2, err, strlen(err));
        exit(1);
    }

    int i = 0;
    for (; i < count_arg(format); i++) {
        argument = arguments[i];
        reti = regexec(&regex, tmp, nbArguments, match, 0);
        strncat(str, tmp, match[0].rm_so);
        strcat(str, argument);
        tmp = (tmp + match[0].rm_eo);
    }
    strcat(str, tmp);
    regfree(&regex);
    free(tmpPointer);
    return strlen(str);

}

int sprintf2(char *str, const char *format, char * arguments[]) {
    int sizeOfString = strlen(format);
    for (int i = 0; i < count_arg(format); i++) {
        sizeOfString += strlen(arguments[i]);
    }

    int nbArguments = count_arg(format);
    regmatch_t match[nbArguments];
    char *tmp = malloc(sizeof (char) * (strlen(format) + sizeOfString));
    char *tmpPointer = tmp;
    strcpy(tmp, format);
    strcpy(str, "");

    regex_t regex;
    int reti;

    /* Compile regular expression */
    reti = regcomp(&regex, "%[[:alpha:]]", REG_ICASE | REG_EXTENDED);
    if (reti) {
        char * err = "Could not compile regex\n";
        write(2, err, strlen(err));
        exit(1);
    }

    char * argument;
    int i = 0;
    for (; i < count_arg(format); i++) {
        argument = arguments[i];
        reti = regexec(&regex, tmp, nbArguments, match, 0);
        strncat(str, tmp, match[0].rm_so);
        strcat(str, argument);
        tmp = (tmp + match[0].rm_eo);
    }
    strcat(str, tmp);
    regfree(&regex);
    free(tmpPointer);
    return strlen(str);
}

int fputc(int c, FILE *stream) {
    if (&(stream->_file) == NULL) {
        snprintf(errmsg, 500, "file descriptor is closed, die.\n");
        write(2, errmsg, strlen(errmsg));
        exit(-1);
    }

    if (stream->_flag & _IOREAD) {
        snprintf(errmsg, 500, "cannot write on read-only file, die.\n");
        write(2, errmsg, strlen(errmsg));
        exit(-1);
    }
    if (stream->_cnt == 0) {
        _flsbuf('\0', stream);
    }
    stream->_ptr[0] = c;
    stream->_cnt--;
    stream->_ptr++;
    if ((stream->_flag & _IOLBF && c == '\n') || stream->_flag & _IONBF) {
        _flsbuf('\0', stream);
    }
    return 1;
}

int fputs(const char *s, FILE *stream) {
    for (int i = 0; i < strlen(s); i++) {

        fputc(s[i], stream);
    }

    return strlen(s);

}

int puts(const char *s) {
    return fputs(s, stdout);
}

int _filbuf(FILE *stream) {
    if (ferror(stream)) {
        return -1;
    }
    stream->_cnt = read(stream->_file, stream->_base, BUFSIZ);
    stream->_ptr = stream->_base;
    if (stream->_cnt == -1) {
        perror("stdio.c:");
        stream->_flag |= _IOERR;
        return -1;
    }

    return 0;
}

int fgetc(FILE *stream) {
    if (stream->_cnt == 0) {
        _flsbuf('\0', stream);
        if (_filbuf(stream) == -1) {
            return -1;
        }
    }
    stream->_cnt--;
    return (int) *(stream->_ptr++);
}

char *fgets(char *s, int size, FILE *stream) {
    if (size < 0) {
        snprintf(errmsg, 500, "size cannot be less than zero, die.\n");
        write(2, errmsg, strlen(errmsg));
        return NULL;
    }
    char character;
    int index = 0;
    do {
        character = (unsigned char) fgetc(stream);
        if (character == -1) {
            return NULL;
        }
        if (character != EOF) {
            s[index] = character;
            index++;
        }
    } while (character != '\n' && character != EOF && --size != 0);
    s[index] = '\0';
    return s;
}

char *gets(char *s) {
    return fgets(s, 0, stdin);
}

int fclose(FILE *fp) {
    fflush(fp);
    int fd = fp->_file;
    if (tmp_file[fd] != NULL) {
        if (unlink(tmp_file[fd]) == -1) {
            perror("stdio");
        }
        free(tmp_file[fd]);
    }
    if (close(fp->_file) == -1) {
        return -1;
    }



    return 0;

}

int fflush(FILE *stream) {

    return _flsbuf('\0', stream);
}

FILE * fdopen(int fd, const char *mode) {
    if (&_IOB[fd] != NULL) {
        FILE* f = &_IOB[fd];
        f->_file = fd;
        _allocate(f);
        return f; //fopen(f, mode); //voir si sa plante ...
    } else {

        FILE f;
        (&f)->_base = malloc(sizeof (char)*BUFSIZ);
        (&f)->_ptr = (&f)->_base;
        (&f)->_file = fd;
        (&f)->_flag |= _IOMYBUF;

        _allocate(&f);

        return &f;
    }

}

FILE * freopen(const char *path, const char *mode, FILE * stream) {
    FILE* ret = fopen(path, mode);
    ret->_file = stream->_file;

    return ret;
}

FILE * popen(const char *command, const char *type) {
    int p[2], pid;
    int end_parent, end_child;

    if (!strcmp(type, "r")) {
        end_parent = 0; //READ
        end_child = 1; //WRITE;
    } else if (!strcmp(type, "w")) {
        end_parent = 1; //WRITE
        end_child = 0; //READ
    } else {
        return NULL;
    }

    if (pipe(p) == -1) {
        return NULL; //return null if pipe can't be create.
    }
    if ((pid = fork()) == -1) {
        close(p[0]);
        close(p[1]);
        return NULL; //Return null and close the pipe if fork fail.
    }

    //parent
    if (pid > 0) {
        if (close(p[end_child]) == -1) {
            return NULL;
        }
        return fdopen(p[end_parent], type);
    }

    //child
    if (close(p[end_parent]) == -1) {
        exit(1);
    }

    if (dup2(p[end_child], end_child) == -1) {
        exit(1);
    }
    if (close(p[end_child]) == -1) {

        exit(1);
    }
    execl("/bin/sh", "sh", "-c", command, NULL);
    exit(1);

}

int pclose(FILE * stream) {
    int fd;
    fd = stream->_file;
    sleep(2); //Wait for the process ending.
    if (close(fd) == -1) {
        return -1;
    } else {

        return 1;
    }

}

//Ne fonctionne pas, le fichier est correctement cree et ouvert, mais n'est pas supprimer lors
//de la fermeture du programme, pour une raison que j'ignore

FILE * tmpfile(void) {
    FILE * random = fopen("/dev/urandom", "r");
    char * uuid = malloc(sizeof (char) * 16);
    uuid[15] = '\0';
    int indexUuid = 0;
    char character;

    for (character = fgetc(random); indexUuid < 15; character = fgetc(random)) {
        if (((int) character > 64 && (int) character < 91) || ((int) character > 96 && (int) character < 124)) {
            uuid[indexUuid] = character;
            indexUuid++;
        }
    }
    fclose(random);
    FILE * randomFile = fopen(uuid, "w+");
    tmp_file[(int) randomFile->_file] = uuid;
    return randomFile;
}


