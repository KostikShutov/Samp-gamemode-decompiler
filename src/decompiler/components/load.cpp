#include <map>
#include <fstream>
#include <vector>

#include "load.h"

void loadIncludeAndData(std::ifstream &fin, std::ofstream &fout)
{
    fout << "#include <a_samp>" << std::endl << std::endl;

    fout << "#pragma warning disable 202" << std::endl
         << "#pragma warning disable 215" << std::endl << std::endl;

    std::string line;
    std::string rnd = genRandomString(10);

    fout << "forward LOAD_DATA_SEGMENT_" << rnd <<"();" << std::endl
         << "public LOAD_DATA_SEGMENT_" << rnd <<"()" << std::endl
         << "{" << std::endl;

    while (!fin.eof())
    {
        getline(fin, line);

        if(line == "") break;

        fout << writeTab(1) <<"\"" << line.substr(14, line.length() - 15) << "\";" << std::endl;
    }

    fout << "}" << std::endl << std::endl;

    fin.close();
}