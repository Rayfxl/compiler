#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "lexical_analyzer.h"
#include "grammar_analyzer.h"

int main(int argc, char* argv[]) 
{
    if (argc != 2) 
    {
        std::cout << "You should enter the name of source code." << std::endl;
    }

    // 词法分析
    std::string sourceFileName = argv[1];
    lexical_analyzer(sourceFileName);

    // 语法分析
    std::string varPath = "variableList.var";
    std::string proPath = "processList.pro";
    std::string errFile = "grammarError.err";
    std::string inputFile = "test.dyd";
    // 存放.dyd的数据结构
    std::vector<std::pair<std::string, std::string>> tokenList;
    std::string temp;
    std::ifstream input(inputFile);

    // 读取.dyd文件
    if (input.is_open()) 
    {
        while (getline(input, temp)) 
        {
            std::istringstream iss(temp);
            std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)),
                std::istream_iterator<std::string>());
            for (size_t i = 0; i < tokens.size(); i += 2) 
            {
                if (i + 1 < tokens.size()) 
                {
                    tokenList.push_back({ tokens[i], tokens[i + 1] });
                }
                else 
                {
                    std::cerr << "Token list has an odd number of elements." << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
        input.close();
    }
    else 
    {
        std::cerr << "Could not open the file - '" << inputFile << "'" << std::endl;
        return EXIT_FAILURE;
    }

    // 初始化语法分析器对象
    GrammarAnalyzer analyzer(tokenList, errFile);
    analyzer.parseProgram(); // 开始分析

    // 输出文件
    analyzer.printFiles(varPath, proPath);

    system("pause");
    return 0;
}