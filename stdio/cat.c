#include "../stdio.h"
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

int main(int argc, char** argv) {
    init();

    printf("debut %s %s %s\n", "de lecture", "et ecriture", "de lignes");
    char buf[BUFSIZ];

    gets(buf);
    puts(buf);

    putc('a', stdout);
    putc('b', stdout);
    putc('c', stdout);
    putc('d', stdout);
    putc('\n', stdout);

    printf("affichage %s %s\n", "de", "ls");
    FILE* fp;
    fp = popen("ls", "w");
    if (fp == NULL) {
        write(2, "null\n", strlen("null\n"));
    }
    pclose(fp);

    return 0;
}
