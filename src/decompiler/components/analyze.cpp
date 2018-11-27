#include <map>
#include <fstream>
#include <vector>

#include "analyze.h"

int counter = 0;

void opcodeAnalyzeFunc_1(std::ofstream& fout)
{
    if(!natives.count(params[3]))
    {
        natives.insert(std::pair<std::string, int>(params[3], 0));

        fout << "forward " << "F" << counter <<"_" << params[3] << "();" << std::endl
             << "public " << "F" << counter << "_" << params[3] << "()" << std::endl
             << "{" << std::endl
             << writeTab(1) << params[3] << "();" << std::endl
             << "}" << std::endl << std::endl;

        counter++;
    }
}

void opcodeAnalyzeFunc_2(std::ofstream& fout)
{
    labels[params[2]]++;
}

void opcodeAnalyzeFunc_3(std::ofstream& fout)
{
    labels[params[3]]++;
}

std::map<std::string, void(*)(std::ofstream&)> opcode_analyze_list =
{
        {"sysreq.c", opcodeAnalyzeFunc_1},
        {"jump", opcodeAnalyzeFunc_2},
        {"jzer", opcodeAnalyzeFunc_2},
        {"jnz", opcodeAnalyzeFunc_2},
        {"jeq", opcodeAnalyzeFunc_2},
        {"jneq", opcodeAnalyzeFunc_2},
        {"jsless", opcodeAnalyzeFunc_2},
        {"jsleq", opcodeAnalyzeFunc_2},
        {"jsgrtr", opcodeAnalyzeFunc_2},
        {"jsgeq", opcodeAnalyzeFunc_2},
        {"casetbl", opcodeAnalyzeFunc_3}
};

void analyzeCode(std::ifstream &fin, std::ofstream &fout)
{
    std::string line;

    while (!fin.eof())
    {
        getline(fin, line);

        parseLine(line);

        std::map<std::string, void(*)(std::ofstream&)>::iterator it;
        it = opcode_analyze_list.find(params[1]);

        if(it != opcode_analyze_list.end())
        {
            it->second(fout);
        }

        params.clear();
    }
}