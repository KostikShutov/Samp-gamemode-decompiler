#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include "components/analyze.h"
#include "components/generate.h"
#include "components/load.h"
#include "parse.h"

std::map <std::string, int> natives, labels;
std::vector<std::string> params;
bool proc_checker = false;
bool case_checker = false;

void decompile(std::string modname)
{
    std::ifstream fin_data(modname + "/data_" + modname + ".txt");
    std::ifstream fin_source(modname + "/source_" + modname + ".txt");
    std::ofstream fout_pwn(modname + "/" + modname + ".pwn");

    loadIncludeAndData(fin_data, fout_pwn);
    analyzeCode(fin_source, fout_pwn);
    //
    fin_source.close();
    fin_source.open(modname + "/source_" + modname + ".txt");
    //
    generateCode(fin_source, fout_pwn);

    fin_data.close();
    fin_source.close();
    fout_pwn.close();
}

void writeCloseBracket(std::ofstream &fout)
{
    if(proc_checker)
    {
        proc_checker = false;
        fout << "}" << std::endl << std::endl;
    }
}

std::string writeTab(int count)
{
    std::string result;

    for(int i = 0; i < count; i++)
    {
        result += "\t";
    }

    return result;
}

std::string genRandomString(int len)
{
    std::string alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;

    for (int i = 0; i < len; i++)
    {
        result += alphanum[rand() % (alphanum.length() - 1)];
    }

    return result;
}

void parseLine(std::string input)
{
    std::string buffer;
    std::stringstream output;
    output << input;

    while(output >> buffer)
    {
        params.push_back(buffer);
    }
}