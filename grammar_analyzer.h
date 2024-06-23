#ifndef GRAMMAR_ANALYZER_H
#define GRAMMAR_ANALYZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_set>

// 定义一个变量单元类
class VarUnit 
{
public:
    std::string vName; // 变量名
    std::string vProc; // 所属过程
    size_t vKind; // 0为变量，1为形参
    std::string vType; // 变量类型
    size_t vLev; // 变量层次
    size_t vAdr; // 变量在表中的位置

    // 构造函数
    VarUnit(const std::string& name, const std::string& proc, size_t kind, const std::string& type, size_t lev, size_t adr)
        :vName(name)
        ,vProc(proc)
        ,vKind(kind)
        ,vType(type)
        ,vLev(lev)
        ,vAdr(adr)
    {}

    // 变量单元的输出函数
    void printer(const std::string& output_path) const 
    {
        // 使用fstream来打开并写入文件
        std::ofstream s(output_path, std::ios::app); 
        if (!s.is_open()) 
        {
            std::cerr << "Failed to open file: " << output_path << '\n';
            return;
        }
        // 使用iomanip库来格式化输出
        s << std::left
          << std::setw(10) << vName
          << std::setw(10) << vProc
          << std::setw(10) << vKind
          << std::setw(10) << vType
          << std::setw(10) << vLev
          << std::setw(10) << vAdr << '\n';
    }
};

// 定义一个过程单元类
class  ProUnit 
{
public:
    std::string pName; // 过程名
    std::string pType; // 过程类型
    size_t pLev; // 过程层次
    size_t fAdr; // 第一个变量在变量表中的位置
    size_t lAdr; // 最后一个变量在变量表中的位置

    // 构造函数
    ProUnit(const std::string& name, const std::string& type, size_t lev, size_t fadr, size_t ladr)
        :pName(name)
        ,pType(type)
        ,pLev(lev)
        ,fAdr(fadr)
        ,lAdr(ladr)
    {}

    // 过程单元输出函数
    void printer(const std::string& output_path) const 
    {
        std::ofstream s(output_path, std::ios::app);
        if (!s.is_open()) 
        {
            std::cerr << "Failed to open file: " << output_path << '\n';
            return;
        }
        s << std::left
          << std::setw(10) << pName
          << std::setw(10) << pType
          << std::setw(10) << pLev
          << std::setw(10) << fAdr
          << std::setw(10) << lAdr << '\n';
    }
};

// 定义一个语法分析器类
class GrammarAnalyzer 
{
private:
    std::vector<std::pair<std::string, std::string>> tokenList; //  词法分析所得的词法单元列表
    size_t listCurrent; // 当前位置
    size_t lineCurrent; // 当前行
    size_t tokenListLength; // 列表长度，判断是否越界
    std::string errorFile; // 错误文件路径
    std::vector<VarUnit> varList; // 变量列表
    std::vector<ProUnit> proList; // 过程列表
    size_t currentLevel; // 当前嵌套层级

public:
    // 构造函数
    GrammarAnalyzer(const std::vector<std::pair<std::string, std::string>>& tokenList, 
        const std::string& errorFile)
        :tokenList(tokenList)
        ,tokenListLength(tokenList.size())
        ,listCurrent(0)
        ,lineCurrent(1)
        ,errorFile(errorFile)
        ,currentLevel(0)
    {}

    // 处理移进的方法
    void advance()
    {
        if (listCurrent < tokenListLength) 
        {
            /*std::cout << tokenList[listCurrent].first << std::endl;*/
            ++listCurrent;
            if (tokenList[listCurrent].first == "EOLN")
            { // 换行
                ++listCurrent;
                ++lineCurrent;
            }
            // 用作调试
            /*std::cout << "listCurrent: " << listCurrent
                << ", Token: " << tokenList[listCurrent].first
                << ", lineCurrent: " << lineCurrent << std::endl;*/
        }
    }

    // 处理报错的方法
    void error(const std::string& errorCode, const std::string& symbol)
    {
        std::ofstream errFile(errorFile, std::ios::app);
        if (!errFile.is_open())
        {
            std::cerr << "Unable to open error file." << std::endl;
            return;
        }

        if (errorCode == "symbol_not_found")
        {
            errFile << "***" << lineCurrent << ": " << symbol << " not found." << "\n";
        }
        else if (errorCode == "symbol_not_match")
        {
            errFile << "***" << lineCurrent << ": " << symbol << " not matched." << "\n";
        }
        else if (errorCode == "symbol_not_defined")
        {
            errFile << "***" << lineCurrent << ": " << symbol << " not defined." << "\n";
        }
        errFile.close();
    }

    // 解析程序
    void parseProgram()
    {
        parseSubProgram();
    }

    // 解析分程序
    void parseSubProgram()
    {
        if (tokenList[listCurrent].first == "begin") 
        {
            /*std::cout << listCurrent << std::endl;*/
            advance();
            size_t lAdr = 0;
            parseDeclarationList(lAdr); // 解析说明语句表
            if (tokenList[listCurrent].first == ";") 
            {
                advance();
                parseExecutionStatementList(); // 解析执行语句表
                if (tokenList[listCurrent].first == "end") 
                {
                    advance();
                }
            }
            else 
            {
                error("symbol_not_found", ";");
            }
        }
        else
        {
            error("symbol_not_found", "begin");
        }
    }

    // 解析说明语句表
    void parseDeclarationList(size_t& lAdr) 
    {
        parseDeclaration(lAdr);
        parseDeclarationListPrime(lAdr);
    }

    // 处理左递归
    void parseDeclarationListPrime(size_t& lAdr) 
    {
        if ((listCurrent + 1 < tokenListLength && tokenList[listCurrent + 1].first == "integer") ||
            (listCurrent + 2 < tokenListLength && tokenList[listCurrent + 1].first == "EOLN" && 
                tokenList[listCurrent + 2].first == "integer")) 
        {
            advance(); // 读入一个分号
            parseDeclaration(lAdr);
            parseDeclarationListPrime(lAdr);
        }
    }

    // 解析说明语句
    void parseDeclaration(size_t& lAdr) 
    {
        if (tokenList[listCurrent].second == "03")
        {
            // <说明语句>→<变量说明>│<函数说明>
            if (tokenList[listCurrent + 1].second == "10" ||
                (tokenList[listCurrent + 1].first == "EOLN" && tokenList[listCurrent + 2].second == "10"))
            { // 变量说明
                parseVariableDeclaration(lAdr);
            }
            else if (tokenList[listCurrent + 1].first == "function" ||
                (tokenList[listCurrent + 1].first == "EOLN" && tokenList[listCurrent + 2].first == "function"))
            { // 函数说明
                parseFunctionDeclaration();
            }
            else
            {
                error("symbol_not_found", "variable or function");
            }
        }
        else
        {
            error("symbol_not_found", "variable or function");
        }
        
    }

    // 解析变量说明语句
    void parseVariableDeclaration(size_t& lAdr) 
    {
        // <变量说明>→integer <变量>
        if (tokenList[listCurrent].first == "integer") 
        {
            advance();
            // 登记变量
            // 根据当前的过程和等级创建新的变量单元
            std::string vName = tokenList[listCurrent].first;
            std::string vProc = "main";
            if (!proList.empty())
            {
                vProc = currentLevel > 0 ? proList.back().pName : "main";
            }
            size_t vKind = 0; // 变量的种类, 0表示变量
            std::string vType = "integer"; // 变量的类型
            size_t vLev = currentLevel; // 变量的层级
            size_t vAdr = varList.size(); // 变量在列表中的地址
            lAdr = vAdr; // 更新当前过程最后一个变量位置
           
            varList.push_back(VarUnit(vName, vProc, vKind, vType, vLev, vAdr));
            advance();
            if (tokenList[listCurrent].second != "23")
            {
                error("symbol_not_found", "variable or function");
                exit(-1);
            }
        }
    }

    // 解析函数说明语句
    void parseFunctionDeclaration() 
    {
        // <函数说明>→integer function <标识符>（<参数>）；<函数体>
        std::string pName; // 过程名
        std::string pType; // 过程类型
        size_t pLev; // 过程层次
        size_t fAdr = 0; // 第一个变量在变量表中的位置
        size_t lAdr = 0; // 最后一个变量在变量表中的位置

        if (tokenList[listCurrent].first == "integer") 
        {
            pType = tokenList[listCurrent].first;
            advance();
            if (tokenList[listCurrent].first == "function") 
            {
                advance();
                if (tokenList[listCurrent].second == "10") 
                {
                    pName = tokenList[listCurrent].first;
                    advance();
                    if (tokenList[listCurrent].first == "(") 
                    {
                        advance();
                        currentLevel++;
                        pLev = currentLevel;
                        parseParameter(pName, fAdr); // 处理参数
                        if (tokenList[listCurrent].first == ")") 
                        {
                            advance();
                            if (tokenList[listCurrent].first == ";") 
                            {
                                advance();
                                // 登记过程信息
                                proList.push_back(ProUnit(pName, pType, pLev, fAdr, 0));
                                parseFunctionBody(lAdr); // 处理函数体
                                // 更新过程信息
                                proList.back().lAdr = lAdr;
                                currentLevel--;
                            }
                            else 
                            {
                                error("symbol_not_found", ";");
                            }
                        }
                        else 
                        {
                            error("symbol_not_found", ")");
                        }
                    }
                    else 
                    {
                        error("symbol_not_found", "(");
                    }
                }
                else 
                {
                    error("symbol_not_defined", "functionName");
                }
            }
            else 
            {
                error("symbol_not_found", "function");
            }
        }
    }

    // 解析函数参数
    void parseParameter(const std::string& pName, size_t& fAdr) 
    {
        // <参数>→<变量>
        std::string vName = tokenList[listCurrent].first;
        std::string vProc = pName;
        size_t vKind = 1; // 形参
        std::string vType = "integer";
        size_t vLev = currentLevel;
        size_t vAdr = varList.size();
        fAdr = vAdr;
        varList.push_back(VarUnit(vName, vProc, vKind, vType, vLev, vAdr));
        advance();
    }

    // 解析函数体
    void parseFunctionBody(size_t& lAdr) 
    {
        // <函数体>→begin <说明语句表>；<执行语句表> end
        if (tokenList[listCurrent].first == "begin") 
        {
            advance();
            parseDeclarationList(lAdr);
            if (tokenList[listCurrent].first == ";") 
            {
                advance();
                parseExecutionStatementList();
                if (tokenList[listCurrent].first == "end") 
                {
                    advance();
                }
                else 
                {
                    error("symbol_not_found", "end");
                }
            }
            else 
            {
                error("symbol_not_found", ";");
            }
        }
        else 
        {
            error("symbol_not_found", "begin");
        }
    }

    // 解析执行语句表
    void parseExecutionStatementList() 
    {
        // <执行语句表>→<执行语句>│<执行语句表>；<执行语句>
        parseExecutionStatement();
        parseExecutionStatementListPrime();
    }

    // 处理左递归
    void parseExecutionStatementListPrime() 
    {
        // 检测是否有分号继续执行语句
        if (tokenList[listCurrent].first == ";") 
        {
            advance(); // advance
            parseExecutionStatement();
            parseExecutionStatementListPrime();
        }
    }

    // 解析执行语句
    void parseExecutionStatement() 
    {
        // <执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>
        if (tokenList[listCurrent].first == "read") 
        {
            advance();
            parseReadStatement();
        }
        else if (tokenList[listCurrent].first == "write") 
        {
            advance();
            parseWriteStatement();
        }
        else if (tokenList[listCurrent].second == "10") 
        {
            // 变量名的标记通常解析为赋值语句
            parseAssignmentStatement();
        }
        else if (tokenList[listCurrent].first == "if") 
        {
            parseConditionStatement();
        }
        else
        {
            error("symbol_not_match", "execution statement");
        }
    }

    // 解析读语句
    void parseReadStatement() 
    {
        // 读语句
        if (tokenList[listCurrent].first == "(") 
        {
            advance();
            if (tokenList[listCurrent].second == "10") 
            {
                advance();
                if (tokenList[listCurrent].first == ")") 
                {
                    advance();
                }
                else 
                {
                    error("symbol_not_found", ")");
                }
            }
            else 
            {
                error("symbol_not_found", "variable");
            }
        }
        else 
        {
            error("symbol_not_found", "(");
        }
    }

    // 解析写语句
    void parseWriteStatement() 
    {
        // 写语句
        if (tokenList[listCurrent].first == "(") 
        {
            advance();
            if (tokenList[listCurrent].second == "10") 
            {
                advance();
                if (tokenList[listCurrent].first == ")") 
                {
                    advance();
                }
                else 
                {
                    error("symbol_not_found", ")");
                }
            }
            else 
            {
                error("symbol_not_found", "variable");
            }
        }
        else 
        {
            error("symbol_not_found", "(");
        }
    }

    // 查找变量名表，返回布尔值表示成功或失败
    bool doesVarExist(const std::string& varName) 
    {
        for (const auto& var : varList) 
        {
            if (var.vName == varName) 
            {
                return true; // 找到了变量
            }
        }
        return false; // 没找到变量
    }

    // 查找过程名表，返回布尔值表示成功或失败
    bool doesProExist(const std::string& proName) 
    {
        for (const auto& pro : proList) 
        {
            if (pro.pName == proName) 
            {
                return true; // 找到了
            }
        }
        return false; // 没找到
    }

    // 解析赋值语句
    void parseAssignmentStatement() 
    {
        // 赋值语句，假设识别标识符后跟随 ':=' 操作符
        if (tokenList[listCurrent].second == "10") 
        {
            if (!doesVarExist(tokenList[listCurrent].first)) 
            { // 如果变量名不存在
                if (!doesProExist(tokenList[listCurrent].first)) 
                { // 如果变量名不存在，并且过程名也不存在
                    error("symbol_not_defined", "variable/process " + tokenList[listCurrent].first);
                }
            }
           
            advance();
            if (tokenList[listCurrent].first == ":=") 
            {
                advance();
                parseArithmeticExpression();
            }
            else 
            {
                error("symbol_not_found", ":=");
            }
        }
        else 
        {
            error("symbol_not_found", "variable");
        }
    }

    // 解析条件语句
    void parseConditionStatement() 
    {
        // 条件语句
        if (tokenList[listCurrent].first == "if") 
        {
            advance();
            parseConditionExpression();
            if (tokenList[listCurrent].first == "then") 
            {
                advance();
                parseExecutionStatement();
                if (tokenList[listCurrent].first == "else") 
                {
                    advance();
                    parseExecutionStatement();
                }
            }
            else 
            {
                error("symbol_not_found", "then");
            }
        }
        else 
        {
            error("symbol_not_found", "if");
        }
    }

    // 解析条件表达式
    void parseConditionExpression() 
    {
        // <条件表达式>→<算术表达式><关系运算符><算术表达式>
        parseArithmeticExpression();
        std::unordered_set<std::string> relationOpSet = {"<", "<=", ">", ">=", "=", "<>"};
        if (relationOpSet.find(tokenList[listCurrent].first) != relationOpSet.end()) 
        {
            advance();
            parseArithmeticExpression();
        }
        else 
        {
            error("symbol_not_found", "relational operator");
        }
    }

    // 解析算术表达式
    void parseArithmeticExpression() 
    {
        // <算术表达式>→<算术表达式>-<项>|<项>
        parseTerm();
        parseArithmeticExpressionPrime();
    }

    // 处理左递归
    void parseArithmeticExpressionPrime() 
    {
        if (tokenList[listCurrent].first == "-") 
        {
            advance();
            parseTerm();
            parseArithmeticExpressionPrime();
        }
        // else 空串，不做任何操作
    }

    // 解析项
    void parseTerm() 
    {
        // <项>→<项>*<因子>|<因子>
        parseFactor();
        parseTermPrime();
    }

    // 处理左递归
    void parseTermPrime() 
    {
        if (tokenList[listCurrent].first == "*") 
        {
            advance();
            parseFactor();
            parseTermPrime();
        }
        // else 空串，不做任何操作
    }

    // 解析因子
    void parseFactor() 
    {
        // <因子>→<变量>|<常数>|<函数调用>
        if (tokenList[listCurrent].second == "10") 
        {
            // 处理变量或函数调用
            if (listCurrent + 1 < tokenList.size() && tokenList[listCurrent + 1].first == "(") 
            {
                // 函数调用
                advance();
                parseFunctionCall();
            }
            else 
            {
                // 变量
                advance();
            }
        }
        else if (tokenList[listCurrent].second == "11") 
        {
            // 处理常量
            advance();
        }
    }

    // 解析函数调用
    void parseFunctionCall() 
    {
        // 函数调用
        if (tokenList[listCurrent].first == "(") 
        {
            advance();
            parseArithmeticExpression();
            if (tokenList[listCurrent].first == ")") 
            {
                advance();
            }
            else 
            {
                error("symbol_not_found", ")");
            }
        }
        else 
        {
            error("symbol_not_found", "(");
        }
    }

    // 输出到文件
    void printFiles(const std::string& varPath, const std::string& proPath) 
    {
        for (const auto& var : varList) 
        {
            var.printer(varPath);
        }
        for (const auto& proc : proList) 
        {
            proc.printer(proPath);
        }
    }
};
#endif
