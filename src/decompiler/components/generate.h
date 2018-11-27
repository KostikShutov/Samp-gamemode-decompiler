#ifndef DECOMPILER_GENERATE_H
#define DECOMPILER_GENERATE_H

extern std::map <std::string, int> natives, labels;
extern std::vector<std::string> params;
extern bool proc_checker;
extern bool case_checker;

void generateCode(std::ifstream &fin, std::ofstream &fout);

void writeCloseBracket(std::ofstream &fout);
std::string writeTab(int count);
std::string genRandomString(int len);
void parseLine(std::string input);

#endif //DECOMPILER_GENERATE_H
