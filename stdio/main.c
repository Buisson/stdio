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
    // FILE* f = fopen("toto.txt", "w");
    //tracer(f);
    //fputs("cannot write on read-only file, die.\n", stderr);

    /*
        char sentence [256];
        sentence[0] = 'C';
        sentence[1] = 'C';
        sentence[2] = '\0';
        fputs(sentence, f);
        fflush(f);
        fclose(f);
     */
        char buf[BUFSIZ];
        FILE* fp = malloc(sizeof(FILE));
        fp = popen("ls", "w");
        tracer(fp);
        if (fp == NULL) {
            write(2, "null\n", strlen("null\n"));
        }
        while (fgets(buf, BUFSIZ, fp) != NULL) {
            fputs(buf, stdout); //SEGFAULT ....

        }
    
    //tracer(fp);
    
    /*
    while (1) {
        char buf[BUFSIZ];
        fgets(buf, BUFSIZ, stdin);
        fputs(buf, stdout); //SEGFAULT ....
        //  for (int i = 0; i < BUFSIZ; i++) {
        //     buf[i] = '\0';
        //}

    }*/


    return 0;
}

