/* 
 * File:   main.c
 * Author: ca309567
 *
 * Created on 27 mai 2014, 16:40
 */


#include "../stdio.h"


/*
 * 
 */
int main(int argc, char** argv) {
    
    char c;
    FILE* f;
    f->_file=0;
    f->_base="clavier";
    f->_bufsiz=1;
    
    
   
    while(read(0,&c,1)){
        _filbuf(f);
	write(0,&c,1);
    }
    return 0;
}

