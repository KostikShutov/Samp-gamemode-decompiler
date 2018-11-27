#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "sc.h"
#include "../amx/amxdbg.h"

#include "funcs.h"

FILE *fpamx, *fpfunc;
AMX_HEADER amxhdr;
int dbgloaded;
AMX_DBG amxdbg;

OPCODE opcodelist[] = {
        {  0, "???",        parm0 },
        {  1, "load.pri",   parm1 },
        {  2, "load.alt",   parm1 },
        {  3, "load.s.pri", parm1 },
        {  4, "load.s.alt", parm1 },
        {  5, "lref.pri",   parm1 },
        {  6, "lref.alt",   parm1 },
        {  7, "lref.s.pri", parm1 },
        {  8, "lref.s.alt", parm1 },
        {  9, "load.i",     parm0 },
        { 10, "lodb.i",     parm1 },
        { 11, "const.pri",  const_pri_object },
        { 12, "const.alt",  parm1 },
        { 13, "addr.pri",   parm1 },
        { 14, "addr.alt",   parm1 },
        { 15, "stor.pri",   parm1 },
        { 16, "stor.alt",   parm1 },
        { 17, "stor.s.pri", parm1 },
        { 18, "stor.s.alt", parm1 },
        { 19, "sref.pri",   parm1 },
        { 20, "sref.alt",   parm1 },
        { 21, "sref.s.pri", parm1 },
        { 22, "sref.s.alt", parm1 },
        { 23, "stor.i",     parm0 },
        { 24, "strb.i",     parm1 },
        { 25, "lidx",       parm0 },
        { 26, "lidx.b",     parm1 },
        { 27, "idxaddr",    parm0 },
        { 28, "idxaddr.b",  parm1 },
        { 29, "align.pri",  parm1 },
        { 30, "align.alt",  parm1 },
        { 31, "lctrl",      parm1 },
        { 32, "sctrl",      parm1 },
        { 33, "move.pri",   parm0 },
        { 34, "move.alt",   parm0 },
        { 35, "xchg",       parm0 },
        { 36, "push.pri",   parm0 },
        { 37, "push.alt",   parm0 },
        { 38, "push.r",     parm1 },  /* obsolete (never generated) */
        { 39, "push.c",     parm1 },
        { 40, "push",       parm1 },
        { 41, "push.s",     parm1 },
        { 42, "pop.pri",    parm0 },
        { 43, "pop.alt",    parm0 },
        { 44, "stack",      parm1 },
        { 45, "heap",       parm1 },
        { 46, "proc",       do_proc },
        { 47, "ret",        parm0 },
        { 48, "retn",       parm0 },
        { 49, "call",       do_call },
        { 50, "call.pri",   parm0 },
        { 51, "jump",       do_jump },
        { 52, "jrel",       parm1 },  /* same as jump, since version 10 */
        { 53, "jzer",       do_jump },
        { 54, "jnz",        do_jump },
        { 55, "jeq",        do_jump },
        { 56, "jneq",       do_jump },
        { 57, "jless",      do_jump },
        { 58, "jleq",       do_jump },
        { 59, "jgrtr",      do_jump },
        { 60, "jgeq",       do_jump },
        { 61, "jsless",     do_jump },
        { 62, "jsleq",      do_jump },
        { 63, "jsgrtr",     do_jump },
        { 64, "jsgeq",      do_jump },
        { 65, "shl",        parm0 },
        { 66, "shr",        parm0 },
        { 67, "sshr",       parm0 },
        { 68, "shl.c.pri",  parm1 },
        { 69, "shl.c.alt",  parm1 },
        { 70, "shr.c.pri",  parm1 },
        { 71, "shr.c.alt",  parm1 },
        { 72, "smul",       parm0 },
        { 73, "sdiv",       parm0 },
        { 74, "sdiv.alt",   parm0 },
        { 75, "umul",       parm0 },
        { 76, "udiv",       parm0 },
        { 77, "udiv.alt",   parm0 },
        { 78, "add",        parm0 },
        { 79, "sub",        parm0 },
        { 80, "sub.alt",    parm0 },
        { 81, "and",        parm0 },
        { 82, "or",         parm0 },
        { 83, "xor",        parm0 },
        { 84, "not",        parm0 },
        { 85, "neg",        parm0 },
        { 86, "invert",     parm0 },
        { 87, "add.c",      parm1 },
        { 88, "smul.c",     parm1 },
        { 89, "zero.pri",   zero_pri_object },
        { 90, "zero.alt",   parm0 },
        { 91, "zero",       parm1 },
        { 92, "zero.s",     parm1 },
        { 93, "sign.pri",   parm0 },
        { 94, "sign.alt",   parm0 },
        { 95, "eq",         parm0 },
        { 96, "neq",        parm0 },
        { 97, "less",       parm0 },
        { 98, "leq",        parm0 },
        { 99, "grtr",       parm0 },
        {100, "geq",        parm0 },
        {101, "sless",      parm0 },
        {102, "sleq",       parm0 },
        {103, "sgrtr",      parm0 },
        {104, "sgeq",       parm0 },
        {105, "eq.c.pri",   parm1 },
        {106, "eq.c.alt",   parm1 },
        {107, "inc.pri",    parm0 },
        {108, "inc.alt",    parm0 },
        {109, "inc",        parm1 },
        {110, "inc.s",      parm1 },
        {111, "inc.i",      parm0 },
        {112, "dec.pri",    parm0 },
        {113, "dec.alt",    parm0 },
        {114, "dec",        parm1 },
        {115, "dec.s",      parm1 },
        {116, "dec.i",      parm0 },
        {117, "movs",       parm1 },
        {118, "cmps",       parm1 },
        {119, "fill",       parm1 },
        {120, "halt",       parm1 },
        {121, "bounds",     parm1 },
        {122, "sysreq.pri", parm0 },
        {123, "sysreq.c",   do_sysreq },
        {124, "file",       do_file },
        {125, "line",       parm2 },
        {126, "symbol",     do_symbol },
        {127, "srange",     parm2 },        /* version 1 */
        {128, "jump.pri",   parm0 },        /* version 1 */
        {129, "switch",     do_switch },    /* version 1 */
        {130, "casetbl",    casetbl },      /* version 1 */
        {131, "swap.pri",   parm0 },        /* version 4 */
        {132, "swap.alt",   parm0 },        /* version 4 */
        {133, "push.adr",   parm1 },        /* version 4 */
        {134, "nop",        parm0 },        /* version 6 */
        {135, "sysreq.n",   parm2 },        /* version 9 (replaces SYSREQ.d from earlier version) */
        {136, "symtag",     parm1 },        /* version 7 */
        {137, "break",      parm0 },        /* version 8 */
        {138, "push2.c",    parm2 },        /* version 9 */
        {139, "push2",      parm2 },        /* version 9 */
        {140, "push2.s",    parm2 },        /* version 9 */
        {141, "push2.adr",  parm2 },        /* version 9 */
        {142, "push3.c",    parm3 },        /* version 9 */
        {143, "push3",      parm3 },        /* version 9 */
        {144, "push3.s",    parm3 },        /* version 9 */
        {145, "push3.adr",  parm3 },        /* version 9 */
        {146, "push4.c",    parm4 },        /* version 9 */
        {147, "push4",      parm4 },        /* version 9 */
        {148, "push4.s",    parm4 },        /* version 9 */
        {149, "push4.adr",  parm4 },        /* version 9 */
        {150, "push5.c",    parm5 },        /* version 9 */
        {151, "push5",      parm5 },        /* version 9 */
        {152, "push5.s",    parm5 },        /* version 9 */
        {153, "push5.adr",  parm5 },        /* version 9 */
        {154, "load.both",  parm2 },        /* version 9 */
        {155, "load.s.both",parm2 },        /* version 9 */
        {156, "const",      parm2 },        /* version 9 */
        {157, "const.s",    parm2 },        /* version 9 */
};

void print_opcode(FILE *ftxt,cell opcode,cell cip)
{
    fprintf(ftxt,"%08"PRIxC" %s ",cip,opcodelist[(int)(opcode & 0x0000ffff)].name);
}

void print_funcname(FILE *ftxt,cell address)
{
    int idx,numpublics;
    AMX_FUNCSTUBNT func;
    char name[sNAMEMAX+1];
    size_t namelen=0;
    const char *dbgname;

    /* first look up the address in the debug info and, if failed, find it
	* in the public function table */
    if (dbgloaded && dbg_LookupFunction(&amxdbg,address,&dbgname)==AMX_ERR_NONE) {
        strncpy(name,dbgname,sizeof name);
    } else {
        numpublics=(amxhdr.natives-amxhdr.publics)/sizeof(AMX_FUNCSTUBNT);
        fseek(fpamx,amxhdr.publics,SEEK_SET);
        for (idx=0; idx<numpublics; idx++) {
            if (fread(&func,sizeof func,1,fpamx)==0)
                break;
            if (func.address==address) {
                fseek(fpamx,func.nameofs,SEEK_SET);
                namelen=fread(name,1,sizeof name,fpamx);
                break;
            } /* if */
        } /* for */
    } /* if */
    if (namelen>0)
        fprintf(ftxt,"%s ",name);
}

cell zero_pri_object(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"\n");
    return 1;
}

cell const_pri_object(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" \n",*params);
    return 2;
}

cell parm0(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"\n");
    return 1;
}

cell parm1(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" \n",*params);
    return 2;
}

cell parm2(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" %08"PRIxC" \n",params[0],params[1]);
    return 3;
}

cell parm3(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" %08"PRIxC" %08"PRIxC" \n",
            params[0],params[1],params[2]);
    return 4;
}

cell parm4(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" %08"PRIxC" %08"PRIxC" %08"PRIxC" \n",
            params[0],params[1],params[2],params[3]);
    return 5;
}

cell parm5(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" %08"PRIxC" %08"PRIxC" %08"PRIxC" %08"PRIxC" \n",
            params[0],params[1],params[2],params[3],params[4]);
    return 6;
}

cell do_proc(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    print_funcname(ftxt,cip);
    fprintf(ftxt,"\n");
    return 1;
}

cell do_call(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" ",*params);
    print_funcname(ftxt,*params);
    fputs("\n",ftxt);
    return 2;
}

cell do_jump(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" \n",*params);
    return 2;
}

cell do_sysreq(FILE *ftxt, const cell *params, cell opcode, cell cip)
{
    int idx, numnatives, nameoffset;
    AMX_FUNCSTUBNT func;
    char name[sNAMEMAX + 1];
    size_t namelen = 0;

    nameoffset = -1;
    numnatives = (amxhdr.libraries - amxhdr.natives) / sizeof(AMX_FUNCSTUBNT);
    fseek(fpamx, amxhdr.natives, SEEK_SET);
    for (idx = 0; idx < numnatives && nameoffset < 0; idx++)
    {
        if (fread(&func, sizeof func, 1, fpamx) == 0)
            break;
        if (idx == *params)
            nameoffset = func.nameofs;
    }
    if (nameoffset >= 0)
    {
        fseek(fpamx, nameoffset, SEEK_SET);
        namelen=fread(name, 1, sNAMEMAX + 1, fpamx);
    }

    print_opcode(ftxt, opcode, cip);
    fprintf(ftxt, "%08"PRIxC, *params);
    if (namelen > 0)
    {
        fprintf(ftxt, " %s", name);
    }
    fprintf(ftxt, " \n");
    return 2;
}

cell do_switch(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    print_opcode(ftxt,opcode,cip);
    fprintf(ftxt,"%08"PRIxC" \n",*params);
    return 2;
}

cell casetbl(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    cell num;
    int idx;

    print_opcode(ftxt,opcode,cip);
    num=params[0]+1;
    fprintf(ftxt,"%08"PRIxC" %08"PRIxC" \n",params[0],params[1]);
    for (idx=1; idx<num; idx++)
    {
        print_opcode(ftxt,opcode,cip);
        fprintf(ftxt,"%08"PRIxC" %08"PRIxC" \n", params[2*idx],params[2*idx+1]);
    }

    return 2*num+1;
}

cell do_file(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    assert(0);
    return 0;
}

cell do_symbol(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
    assert(0);
    return 0;
}

void expand(unsigned char *code, long codesize, long memsize)
{
    ucell c;
    struct
    {
        long memloc;
        ucell c;
    } spare[AMX_COMPACTMARGIN];
    int sh = 0, st = 0, sc = 0;
    int shift;
    assert(memsize % sizeof(cell) == 0);
    while (codesize > 0)
    {
        c = 0;
        shift = 0;
        do
        {
            codesize--;
            assert(shift < 8 * sizeof(cell));
            assert(shift > 0 || (code[(size_t)codesize] & 0x80) == 0);
            c |= (ucell)(code[(size_t)codesize] & 0x7f) << shift;
            shift += 7;
        }
        while (codesize > 0 && (code[(size_t)codesize - 1] & 0x80) != 0);

        if ((code[(size_t)codesize] & 0x40) != 0)
        {
            while (shift < (int)(8 * sizeof(cell)))
            {
                c |= (ucell)0xff << shift;
                shift += 8;
            }
        }

        while (sc && (spare[sh].memloc > codesize))
        {
            *(ucell *)(code + (int)spare[sh].memloc) = spare[sh].c;
            sh = (sh + 1) % AMX_COMPACTMARGIN;
            sc--;
        }
        memsize -= sizeof(cell);
        assert(memsize >= 0);
        if ((memsize > codesize) || ((memsize == codesize) && (memsize == 0)))
        {
            *(ucell *)(code + (size_t)memsize) = c;
        }
        else
        {
            assert(sc < AMX_COMPACTMARGIN);
            spare[st].memloc = memsize;
            spare[st].c = c;
            st = (st + 1) % AMX_COMPACTMARGIN;
            sc++;
        }
    }
    assert(memsize == 0);
}

char addchars(cell value, int pos)
{
    for (int i = 0, v; i < sizeof(cell); i++)
    {
        v = (value >> 8 * (sizeof(cell) - 1)) & 0xff;
        value <<= 8;
        if(v >= 32) return (char)v;
    }
    return ' ';
}