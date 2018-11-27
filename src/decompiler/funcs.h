#ifndef STRINGPARSER_FUNCS_H
#define STRINGPARSER_FUNCS_H

typedef cell (*OPCODE_PROC)(FILE *ftxt,const cell *params,cell opcode,cell cip);

typedef struct
{
    cell opcode;
    char *name;
    OPCODE_PROC func;
} OPCODE;

typedef union
{
    int i;
    float f;
} float_ieee754;

cell const_pri_object(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell zero_pri_object(FILE *ftxt,const cell *params,cell opcode,cell cip);

cell parm0(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm1(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm2(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm3(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm4(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm5(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_proc(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_call(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_jump(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_sysreq(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_switch(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell casetbl(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_file(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_symbol(FILE *ftxt,const cell *params,cell opcode,cell cip);

void expand(unsigned char *code, long codesize, long memsize);
char addchars(cell value, int pos);
int execute(char *mod_name_amx, const char* mod_name);

#endif //STRINGPARSER_FUNCS_H
