/*
 * File:   main.c
 * Author: ca309567
 *
 * Created on 27 mai 2014, 16:40
 */


#include "../stdio.h"
#include <unistd.h>
#include <string.h>

void tracer(FILE *f) {
    char buffer[500];
    snprintf(buffer, 500, "Trace FILE %p\n", f);
    write(2, buffer, strlen(buffer));
    snprintf(buffer, 500,
            "\tcnt: %d, ptr: %p, base: %p, bufsiz: %d, flag:%x, file:%d\n\n",
            f->_cnt, f->_ptr, f->_base, f->_bufsiz, f->_flag, f->_file);
    write(2, buffer, strlen(buffer));
}

/*
 *
 */
int main(int argc, char** argv) {
    init();
<<<<<<< HEAD
    FILE* f = fopen("toto.txt", "w");
=======
    FILE* f = fopen("toto.txt", "w+");
    tracer(f);
>>>>>>> 4d3b317bc8afa4db6c27e64fc0b6f185aca6342f
    //fputs("cannot write on read-only file, die.\n", stderr);


    char sentence [256];
    sentence[0] = 'C';
    sentence[1] = 'C';
    sentence[2] = '\0';
    tracer(f);

    fputs(sentence, f);
    fflush(f);
    fclose(f);

    // printf("bite %s \n", "coucou");
    // fflush(stdout);
    //execlp("cat", "cat");
    //char c;
    //FILE* f = malloc(sizeof (FILE)); //Fuck it.
    //puts("coucou");
    //_filbuf(f);
    //puts("bug?");



    return 0;
}

