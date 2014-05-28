/* 
 * File:   main.c
 * Author: ca309567
 *
 * Created on 27 mai 2014, 16:40
 */


#include "../stdio.h"
#include "stdio.c"

/*
 * 
 */
int main(int argc, char** argv) {
    
    char c;
    while(read(0,&c,1)){
	write(0,&c,1);
    }
    return 0;
}

