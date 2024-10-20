#ifndef __DINAMIC_STRING__
#define __DINAMIC_STRING__

#define dstring char*
#define TOCHAR (char*)
#define UNINT unsigned int

dstring dstr_init(void);
void dstr_addc(dstring *dstr, char c);
void dstr_fputs(dstring *dstr);
void dstr_end(dstring *dstr);

#endif
