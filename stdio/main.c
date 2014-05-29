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

