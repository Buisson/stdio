#include "../stdio.h"
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

struct _iobuf _IOB[5000];

void init() {
    (&_IOB[0])->_file = 0;
    (&_IOB[0])->_flag = _IOMYBUF | _IOREAD;
    _filbuf(&(_IOB[0]));

    (&_IOB[1])->_file = 1;
    (&_IOB[1])->_flag = _IOMYBUF | _IOWRT;
    _filbuf(&(_IOB[1]));

    (&_IOB[2])->_file = 2;
    (&_IOB[2])->_flag = _IOMYBUF | _IOWRT;
    _filbuf(&(_IOB[2]));
}

int _filbuf(FILE * f) {

    f->_cnt = BUFSIZ;
    //todo faire les controles.verif si il y a un buffer.si pas buffer allouer un buffer.verif si le fichier est ouvert en lecture.(ne pas faire pour le moment)
    if (!f->_base) {
        f->_bufsiz = BUFSIZ;
        f->_base = malloc(sizeof (char)*f->_bufsiz);
        f->_ptr = f->_base;
        return 1;
    } else {
        f->_ptr = f->_base;
        return 0;
    }


}

FILE *fopen(const char *path, const char *mode) {
    FILE* f = malloc(sizeof (FILE));
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
        write(2, "Bad mode, die.\n", strlen("Bad mode, die.\n"));
    }

    if ((f->_file = open(path, flag)) == -1) {
        write(2, "Open fail, die.\n", strlen("Open fail, die.\n"));
        exit(-1);
    }

    _filbuf(f);
    if (!(f->_flag & _IOERR)) {
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
    stream->_flag |= mode;
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


int count_arg(const char *str);

int count_arg(const char *str) {
    regex_t regex;
    int reti;
    regmatch_t match[5000];

    /* Compile regular expression */
    reti = regcomp(&regex, "%[[:alpha:]]", REG_ICASE | REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    /* Execute regular expression */
    reti = regexec(&regex, str, 5000, match, 0);
    if (!reti) {
        write(2, "match", strlen("match"));
    } else if (reti == REG_NOMATCH) {
        write(2, "no match", strlen("no match"));
    } else {
        exit(-1);
    }

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);
    int i;
    for (i = 0; match[i].rm_eo != -1; i++);


    char buffer[500];
    snprintf(buffer, 500, "\nnombre de match: %d\n\n", i);
    write(2, buffer, strlen(buffer));

    return i;
}

int fprintf(FILE *stream, const char *format, ...) {
    int sizeOfString = strlen(format);
    va_list ap;
    va_start(ap, format);
    char* argument;
    for (int i = 0; i < count_arg(format); i++) {
        argument = va_arg(ap, char*);

        char buffer[500];
        snprintf(buffer, 500, "\nfprintf argument: %s\n\n", argument);
        write(2, buffer, strlen(buffer));

        sizeOfString += strlen(argument);
    }
    char str[(strlen(format) + sizeOfString)];
    sprintf(str, format, ap);
    va_end(ap);
    write(2, str, strlen(str));
    return fputs(str, stream);
}

int sprintf(char *str, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    char* argument;
    int nbArguments = count_arg(format);
    int sizeOfString = strlen(format);

    for (int i = 0; i < count_arg(format); i++) {
        argument = va_arg(ap, char*);
        sizeOfString += strlen(argument);
        write(2, argument, strlen(argument));
    }

    va_end(ap);

    regmatch_t match[nbArguments];
    char tmp[strlen(format) + sizeOfString];
    strcpy(tmp, format);
    str = "";


    regex_t regex;
    int reti;

    /* Compile regular expression */
    reti = regcomp(&regex, "%[[:alpha:]]", REG_ICASE | REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    /* Execute regular expression */
    reti = regexec(&regex, tmp, nbArguments, match, 0);
    if (!reti) {
        write(2, "Match", 5);
    } else if (reti == REG_NOMATCH) {
        write(2, "No Match", 8);
    }

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);

    va_start(ap, format);
    int currentDecalageOffset = 0;
    int currentArgument = 0;


    for (int i = 0; i < count_arg(format); i++) {
        argument = va_arg(ap, char*);
        strncat(str, tmp, match[currentArgument].rm_eo - 2);
        write(2, argument, strlen(argument));
        strcat(str, argument);
        *tmp += (match[currentArgument].rm_so - currentDecalageOffset);
        currentDecalageOffset += match[currentArgument].rm_so;
        currentArgument++;
    }

    return strlen(str);

}

int fputc(int c, FILE *stream) {
    if (&(stream->_file) == NULL) {
        write(2, "file descriptor is closed, die.\n", strlen("file descriptor is closed, die.\n"));
        exit(-1);
    }
    tracer(stream);

    if (stream->_flag & _IOREAD) {
        write(2, "cannot write on read-only file, die.\n", strlen("cannot write on read-only file, die.\n"));
        exit(-1);
    }
    if (stream->_cnt == 0) {
        fflush(stream);
        _filbuf(stream);
    }
    stream->_ptr[0] = c;
    stream->_cnt--;
    stream->_ptr++;
    if ((stream->_flag & _IOLBF && c == '\n') || stream->_flag & _IONBF) {
        fflush(stream);
        _filbuf(stream);
    }
    tracer(stream);
    return 1;
}

int fputs(const char *s, FILE *stream) {

    write(2, s, strlen(s));
    for (int i = 0; i < strlen(s); i++) {
        fputc(s[i], stream);
    }
    return strlen(s);

}

int puts(const char *s) {
    return fputs(s, stdout);
}

void filbuf(FILE *stream) {
    stream->_cnt = read(stream->_file, stream->_base, BUFSIZ);
    stream->_ptr = stream->_base;
    if (stream->_cnt == -1) {
        fputs("read file error, die.\n", stderr);
        exit(-1);
    }

}

int fgetc(FILE *stream) {
    tracer(stream);
    if (stream->_cnt == 0) {
        filbuf(stream);
    }
    stream->_cnt--;
    return (int) *(stream->_ptr++);
}

char *fgets(char *s, int size, FILE *stream) {
    if (size < 0) {
        fputs("size cannot be less than zero, die.\n", stderr);
        return NULL;
        exit(-1);
    }
    char character;
    do {
        character = (unsigned char) fgetc(stream);
        if (character != EOF) {
            s[strlen(s)] = character;
        }
    } while (character != '\n' && character != EOF && --size != 0);
    return s;
}

char *gets(char *s) {
    return fgets(s, 0, stdin);
}

int fclose(FILE *fp) {
    fflush(fp);
    return close(fp->_file);
}

int fflush(FILE *stream) {
    if (stream == NULL) {
        for (int i = 0; &_IOB[i] != NULL; i++) {
            if (stream->_flag & _IOWRT || stream->_flag & _IORW) {
                if (write((&_IOB[i])->_file, (&_IOB[i])->_base, (&_IOB[i])->_bufsiz) == -1) {
                    return -1;
                }
            }

            if ((&_IOB[i])->_flag & _IOMYBUF) {
                free((&_IOB[i])->_base);
            }
            (&_IOB[i])->_ptr = NULL;
            (&_IOB[i])->_base = NULL;
            _filbuf(&_IOB[i]);
        }
    } else {
        if (stream->_flag & _IOWRT || stream->_flag & _IORW) {
            if (write(stream->_file, stream->_base, stream->_bufsiz) == -1) {
                return -1;
            }
        }
        if (stream->_flag & _IOMYBUF) {
            free(stream->_base);
        }
        stream->_ptr = NULL;
        stream->_base = NULL;
        _filbuf(stream);

        return 0;
    }
}

FILE * fdopen(int fd, const char *mode) {
    if (&_IOB[fd] != NULL) {
        FILE* f = &_IOB[fd];
        return fopen(f, mode); //voir si sa plante ...
    } else {
        return NULL;
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

    if (close(fd) == -1) {
        return -1;
    } else {
        return 1;
    }

}

FILE * tmpfile(void) {
    FILE* f = malloc(sizeof (FILE));
    f->_base = malloc(sizeof (char)*BUFSIZ);
    f->_file = strlen(_IOB);
    filbuf(f);
    //_IOB[strlen(_IOB)]=f;
    return f;
}


