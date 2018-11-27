#ifndef DECOMPILER_LOAD_H
#define DECOMPILER_LOAD_H

void loadIncludeAndData(std::ifstream &fin, std::ofstream &fout);

std::string genRandomString(int len);
std::string writeTab(int count);

#endif //DECOMPILER_LOAD_H
