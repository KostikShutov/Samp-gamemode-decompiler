#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc.h"
#include "../amx/amxdbg.h"

#include "funcs.h"

extern FILE *fpamx;
extern AMX_HEADER amxhdr;
extern int dbgloaded;
extern AMX_DBG amxdbg;

extern OPCODE opcodelist[];

int disassemble(char *mod_name_amx, const char* mod_name)
{
    char name[FILENAME_MAX], data[FILENAME_MAX];
    FILE *fplist, *fpdata;

    strcpy(name, mod_name);
    strcat(name, "/source_");
    strcat(name, mod_name);
    strcat(name, ".txt");

    strcpy(data, mod_name);
    strcat(data, "/data_");
    strcat(data, mod_name);
    strcat(data, ".txt");

    if ((fpamx = fopen(mod_name_amx, "rb")) == NULL)
    {
        printf("Unable to open input file \"%s\"\n", mod_name_amx);
        return 1;
    }
    if ((fplist = fopen(name, "wt")) == NULL)
    {
        printf("Unable to create output file \"%s\"\n", name);
        return 1;
    }
    if ((fpdata = fopen(data, "wt")) == NULL)
    {
        printf("Unable to create output file \"%s\"\n", data);
        return 1;
    }

    fseek(fpamx, 0, SEEK_SET);
    if (fread(&amxhdr, sizeof amxhdr, 1, fpamx) == 0)
    {
        printf("Unable to read AMX header: %s\n", feof(fpamx) ? "End of file reached" : strerror(errno));
        return 1;
    }
    if (amxhdr.magic != AMX_MAGIC)
    {
        printf("Not a valid AMX file\n");
        return 1;
    }
    int codesize = amxhdr.hea - amxhdr.cod;


    cell *code;
    if ((code = malloc(codesize)) == NULL)
    {
        printf("Insufficient memory: need %d bytes\n", codesize);
        return 1;
    }

    fseek(fpamx, amxhdr.cod, SEEK_SET);
    if ((int32_t)fread(code, 1, codesize, fpamx) < amxhdr.size-amxhdr.cod)
    {
        printf("Unable to read code: %s\n", feof(fpamx) ? "End of file reached" : strerror(errno));
        return 1;
    }
    if ((amxhdr.flags & AMX_FLAG_COMPACT) != 0)
        expand((unsigned char *)code,amxhdr.size-amxhdr.cod,amxhdr.hea-amxhdr.cod);

    char line[sLINEMAX], sym;
    FILE *fpsrc;
    int count, i, j;
    cell *cip, tmp;
    OPCODE_PROC func;
    const char *filename;
    long nline, nprevline;

    //Print code
    cip = code;
    codesize = amxhdr.dat-amxhdr.cod;
    nprevline = -1;
    while (((unsigned char*)cip - (unsigned char*)code) < codesize)
    {
        if (dbgloaded)
        {
            dbg_LookupFile(&amxdbg, (cell)(cip-code)*sizeof(cell), &filename);
            dbg_LookupLine(&amxdbg, (cell)(cip-code)*sizeof(cell), &nline);
            if (filename != NULL && nline != nprevline)
            {
                fpsrc = fopen(filename, "r");
                if (fpsrc != NULL)
                {
                    for (i = 0; i <= nline; i++)
                    {
                        if (fgets(line, sizeof(line), fpsrc) == NULL)
                            break;
                        for (j=0; line[j] <= ' ' && line[j] != '\0'; j++)
                            continue;
                        if (line[j] != '\0' && i > nprevline)
                            fputs(line, fplist);
                    }
                    fclose(fpsrc);
                }
                nprevline = nline;
            }
        }
        func = opcodelist[(int)(*cip&0x0000ffff)].func;
        int flag = 0;

        if(opcodelist[(int)(*cip&0x0000ffff)].name == "sysreq.c")
        {
            flag = 1;
        }
        /*if(flag == 0) cip += func(fplist, cip+1, *cip, (cell)(cip-code)*sizeof(cell));
        else cip += func(fpfunc, cip+1, *cip, (cell)(cip-code)*sizeof(cell));*/
        cip += func(fplist, cip+1, *cip, (cell)(cip-code)*sizeof(cell));
        flag = 0;
    }

    //Print data
    cip = (cell*)((unsigned char*)code + (amxhdr.dat - amxhdr.cod));
    codesize = amxhdr.hea - amxhdr.cod;
    count = 0;
    int flag = 0;
    int antispace = 1;
    fprintf(fpdata, "[00000000] >> ");
    while (((unsigned char*)cip - (unsigned char*)code) < codesize)
    {
        if (count == 0)
            tmp = (cell)((cip-code) * sizeof(cell) - (amxhdr.dat - amxhdr.cod));
        if(flag == 1)
        {
            fprintf(fpdata,"[%08"PRIxC"] >> ", tmp + count * 4);
            flag = 0;
        }
        if((int)*cip == 0)
        {
            flag = 1;
            fprintf(fpdata,"\n");
            antispace = 0;
        }
        sym = addchars(*cip, count);
        if(antispace)
        {
            fprintf(fpdata, "%c", sym);
        }
        antispace = 1;
        count = (count + 1) % 4;
        cip++;
    }

    if (dbgloaded) dbg_FreeInfo(&amxdbg);

    free(code);
    fclose(fpamx);
    fclose(fplist);
    fclose(fpdata);
    return 0;
}