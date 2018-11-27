#ifndef DECOMPILER_ANALYZE_H
#define DECOMPILER_ANALYZE_H

extern std::map <std::string, int> natives, labels;
extern std::vector<std::string> params;
extern bool proc_checker;

void analyzeCode(std::ifstream &fin, std::ofstream &fout);
std::string writeTab(int count);
void parseLine(std::string input);

#endif //DECOMPILER_ANALYZE_H
