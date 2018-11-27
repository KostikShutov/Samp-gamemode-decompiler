#include <iostream>
#include <fstream>

#include "parse.h"
#include <sys/stat.h>

extern "C" int disassemble(char *mod_name_amx, const char* mod_name);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc != 2)
    {
        std::cout << "Usage: decompiler [*.amx]" << std::endl;
        return 1;
    }

    std::string mod_name = argv[1];
    mod_name.resize(mod_name.size() - 4);
    mkdir(mod_name.c_str(), 0777);

    disassemble(argv[1], mod_name.c_str());

    decompile(mod_name);
    return 0;
}