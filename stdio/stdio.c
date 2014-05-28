#include "../stdio.h"

extern int _filbuf(FILE * f){
    	int n,c;
        char char_read = EOF;
	//todo faire les controles.verif si il y a un buffer.si pas buffer allouer un buffer.verif si le fichier est ouvert en lecture.(ne pas faire pour le moment)
	if(!f->_base){
            //f->_bufsiz=BUFSIZ;
            puts("dans le if");
            f->_base=malloc(f->_bufsiz);
        } else if (((int)f->_cnt) > 0) {
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
            f->_cnt = read(f->_file, (char *)(f->_ptr = f->_base), f->_bufsiz);
        } else { //Si la taille du buffer est mauvaise (par exemple une valeur negative).
            f->_cnt = 0; 
        }
        
        //Verif du f->cnt :
        if (f->_cnt < 0) {		/* valeur negative -> erreur. */
            f->_flag |= _IOERR;
        } else if (f->_cnt==0) {	/* 0 indique un EOF */
            f->_flag |= _IOEOF;
        } else {			/* sinon une bonne donnée a été lu. */
            f->_flag &= _IOEOF;
            f->_cnt--;
            char_read = *f->_ptr++;
        }
        
        
	return char_read;
        
}


extern int _flsbuf(unsigned char c, FILE *f){
    int count;
    char tmp[1];

    if (f->_bufsiz <= 1) { // Si pas de buffer.
        f->_cnt = 0;
        tmp[0] = c;
	return (int)c;
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

FILE *fopen(const char *path, const char *mode){
    FILE* f;
    f->_file = creat(path, mode);
   // f->_flag=mode;
    _filbuf(f);
    
    return f;
}