
#include "../stdio.h"

extern int _filbuf(FILE * f){
    	int n,c;
	//todo faire les controles.verif si il y a un buffer.si pas buffer allouer un buffer.verif si le fichier est ouvert en lecture.(ne pas faire pour le moment)
	n=read(f->_file,&c,1);
	f->_cnt=0;
	return n?c:EOF;

}

extern int _flsbuf(unsigned char c, FILE *f){
    
}