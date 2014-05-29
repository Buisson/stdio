#include "../stdio.h"
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

extern int _filbuf(FILE * f) {
    int n, c;
    char char_read = EOF;
    //todo faire les controles.verif si il y a un buffer.si pas buffer allouer un buffer.verif si le fichier est ouvert en lecture.(ne pas faire pour le moment)
    if (!f->_base) {
        //f->_bufsiz=BUFSIZ;
        puts("dans le if");
        f->_base = malloc(f->_bufsiz);
    } else if (((int) f->_cnt) > 0) {
        /*
         * il reste des caracteres a lire dans le buffer
         * donc on retourne ce caractere.
         */
        --f->_cnt;
        char_read = *f->_ptr++;
        return char_read;
    }

    /*
     * Remplis le buffer avec les 1024(ou taille du buffer) prochain caractere du fichier.
     */
    if (f->_bufsiz) {
        f->_cnt = read(f->_file, (char *) (f->_ptr = f->_base), f->_bufsiz);
    } else { //Si la taille du buffer est mauvaise (par exemple une valeur negative).
        f->_cnt = 0;
    }

    //Verif du f->cnt :
    if (f->_cnt < 0) { /* valeur negative -> erreur. */
        f->_flag |= _IOERR;
    } else if (f->_cnt == 0) { /* 0 indique un EOF */
        f->_flag |= _IOEOF;
    } else { /* sinon une bonne donnée a été lu. */
        f->_flag &= _IOEOF;
        f->_cnt--;
        char_read = *f->_ptr++;
    }


    return char_read;

}

extern int _flsbuf(unsigned char c, FILE *f) {
    int count;
    char tmp[1];

    if (f->_bufsiz <= 1) { // Si pas de buffer.
        f->_cnt = 0;
        tmp[0] = c;
        return (int) c;
    }

    if (!(f->_base)) { //pas de buffer encore aloué. On en aloue un.
        f->_ptr = f->_base = malloc(f->_bufsiz);
        f->_cnt = f->_bufsiz;

    }

    if (f->_ptr == f->_base + f->_bufsiz) { //Si le buffer est plein
        f->_ptr = f->_base;
        f->_cnt = f->_bufsiz;
    }

    if (f->_flag & _IOWRT || f->_flag & _IORW) { //si le fichier a les droit en ecriture alors on peut copier le caracter dans le buffer.
        *f->_ptr++ = c;
        f->_cnt--;
    }

}

FILE *fopen(const char *path, const char *mode) {
    FILE* f;
    f->_file = creat(path, mode);
    // f->_flag=mode;
    _filbuf(f);

    return f;
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
    stream->_flag = mode;
    stream->_bufsiz = size;
    return 0;

}

int printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int numberChar = fprintf(stdout, format, ap);
    va_end(ap);
    return numberChar;

}

int fprintf(FILE *stream, const char *format, ...) {
    int sizeOfString = strlen(format);
    va_list ap;
    va_start(ap, format);
    char* argument;
    for (argument = va_arg(ap, char*); argument != NULL; argument = va_arg(ap, char*)) {
        sizeOfString += strlen(va_arg(ap, char*));
    }
    char str[(strlen(format) + sizeOfString)];
    sprintf(str, format, ap);
    va_end(ap);
    return fputs(str, stream);
}

int sprintf(char *str, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    char* argument;
    int nbArguments = 0;
    int sizeOfString = strlen(format);

    for (argument = va_arg(ap, char*); argument; argument = va_arg(ap, char*)) {
        nbArguments++;
        sizeOfString += strlen(va_arg(ap, char*));
    }
    va_end(ap);

    regmatch_t match[nbArguments];
    char tmp[strlen(format) + sizeOfString];
    strcpy(tmp, format);
    str = "";


    regex_t regex;
    int reti;

    /* Compile regular expression */
    reti = regcomp(&regex, "^%[[:alnum:]]", 0);
    if (reti) {
        return 0;
    }

    /* Execute regular expression */
    reti = regexec(&regex, str, 0, match, 0);
    if (!(!reti || reti == REG_NOMATCH)) {
        return 0;
    }

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);

    va_start(ap, format);
    int currentDecalageOffset = 0;
    int currentArgument = 0;
    for (argument = va_arg(ap, char*); argument != NULL; argument = va_arg(ap, char*)) {
        strncat(str, tmp, match[currentArgument].rm_eo - 1);
        strcat(str, argument);
        *tmp += (match[currentArgument].rm_so - currentDecalageOffset);
        currentDecalageOffset += match[currentArgument].rm_so;
        currentArgument++;
    }

    return strlen(str);

}

int fputc(int c, FILE *stream) {
    char character[1];
    character[0] = c;
    return fputs(character, stream);
}

int fputs(const char *s, FILE *stream) {
    //TODO
    strcat(stream->_ptr, s);
    return strlen(s);
}

int puts(const char *s) {
    return fputs(s, stdout);
}

int fclose(FILE *fp) {
    fflush(fp);
    return close(fp->_file);
}

int fflush(FILE *stream) {
    if (stream == NULL) {
        for (int i = 0; &_IOB[i] != NULL; i++) {
            if (write((&_IOB[i])->_file, (&_IOB[i])->_base, (&_IOB[i])->_bufsiz) == -1) {
                return -1;
            }
            (&_IOB[i])->_ptr = NULL;
            (&_IOB[i])->_base = NULL;
            _filbuf(&_IOB[i]);
        }
    } else {
        if (write(stream->_file, stream->_base, stream->_bufsiz) == -1) {
            return -1;
        }
        stream->_ptr = NULL;
        stream->_base = NULL;
        _filbuf(stream);
        return 0;
    }
}
