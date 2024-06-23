#ifndef GRAMMAR_ANALYZER_H
#define GRAMMAR_ANALYZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_set>

// ����һ��������Ԫ��
class VarUnit 
{
public:
    std::string vName; // ������
    std::string vProc; // ��������
    size_t vKind; // 0Ϊ������1Ϊ�β�
    std::string vType; // ��������
    size_t vLev; // �������
    size_t vAdr; // �����ڱ��е�λ��

    // ���캯��
    VarUnit(const std::string& name, const std::string& proc, size_t kind, const std::string& type, size_t lev, size_t adr)
        :vName(name)
        ,vProc(proc)
        ,vKind(kind)
        ,vType(type)
        ,vLev(lev)
        ,vAdr(adr)
    {}

    // ������Ԫ���������
    void printer(const std::string& output_path) const 
    {
        // ʹ��fstream���򿪲�д���ļ�
        std::ofstream s(output_path, std::ios::app); 
        if (!s.is_open()) 
        {
            std::cerr << "Failed to open file: " << output_path << '\n';
            return;
        }
        // ʹ��iomanip������ʽ�����
        s << std::left
          << std::setw(10) << vName
          << std::setw(10) << vProc
          << std::setw(10) << vKind
          << std::setw(10) << vType
          << std::setw(10) << vLev
          << std::setw(10) << vAdr << '\n';
    }
};

// ����һ�����̵�Ԫ��
class  ProUnit 
{
public:
    std::string pName; // ������
    std::string pType; // ��������
    size_t pLev; // ���̲��
    size_t fAdr; // ��һ�������ڱ������е�λ��
    size_t lAdr; // ���һ�������ڱ������е�λ��

    // ���캯��
    ProUnit(const std::string& name, const std::string& type, size_t lev, size_t fadr, size_t ladr)
        :pName(name)
        ,pType(type)
        ,pLev(lev)
        ,fAdr(fadr)
        ,lAdr(ladr)
    {}

    // ���̵�Ԫ�������
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

// ����һ���﷨��������
class GrammarAnalyzer 
{
private:
    std::vector<std::pair<std::string, std::string>> tokenList; //  �ʷ��������õĴʷ���Ԫ�б�
    size_t listCurrent; // ��ǰλ��
    size_t lineCurrent; // ��ǰ��
    size_t tokenListLength; // �б��ȣ��ж��Ƿ�Խ��
    std::string errorFile; // �����ļ�·��
    std::vector<VarUnit> varList; // �����б�
    std::vector<ProUnit> proList; // �����б�
    size_t currentLevel; // ��ǰǶ�ײ㼶

public:
    // ���캯��
    GrammarAnalyzer(const std::vector<std::pair<std::string, std::string>>& tokenList, 
        const std::string& errorFile)
        :tokenList(tokenList)
        ,tokenListLength(tokenList.size())
        ,listCurrent(0)
        ,lineCurrent(1)
        ,errorFile(errorFile)
        ,currentLevel(0)
    {}

    // �����ƽ��ķ���
    void advance()
    {
        if (listCurrent < tokenListLength) 
        {
            /*std::cout << tokenList[listCurrent].first << std::endl;*/
            ++listCurrent;
            if (tokenList[listCurrent].first == "EOLN")
            { // ����
                ++listCurrent;
                ++lineCurrent;
            }
            // ��������
            /*std::cout << "listCurrent: " << listCurrent
                << ", Token: " << tokenList[listCurrent].first
                << ", lineCurrent: " << lineCurrent << std::endl;*/
        }
    }

    // ������ķ���
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

    // ��������
    void parseProgram()
    {
        parseSubProgram();
    }

    // �����ֳ���
    void parseSubProgram()
    {
        if (tokenList[listCurrent].first == "begin") 
        {
            /*std::cout << listCurrent << std::endl;*/
            advance();
            size_t lAdr = 0;
            parseDeclarationList(lAdr); // ����˵������
            if (tokenList[listCurrent].first == ";") 
            {
                advance();
                parseExecutionStatementList(); // ����ִ������
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

    // ����˵������
    void parseDeclarationList(size_t& lAdr) 
    {
        parseDeclaration(lAdr);
        parseDeclarationListPrime(lAdr);
    }

    // ������ݹ�
    void parseDeclarationListPrime(size_t& lAdr) 
    {
        if ((listCurrent + 1 < tokenListLength && tokenList[listCurrent + 1].first == "integer") ||
            (listCurrent + 2 < tokenListLength && tokenList[listCurrent + 1].first == "EOLN" && 
                tokenList[listCurrent + 2].first == "integer")) 
        {
            advance(); // ����һ���ֺ�
            parseDeclaration(lAdr);
            parseDeclarationListPrime(lAdr);
        }
    }

    // ����˵�����
    void parseDeclaration(size_t& lAdr) 
    {
        if (tokenList[listCurrent].second == "03")
        {
            // <˵�����>��<����˵��>��<����˵��>
            if (tokenList[listCurrent + 1].second == "10" ||
                (tokenList[listCurrent + 1].first == "EOLN" && tokenList[listCurrent + 2].second == "10"))
            { // ����˵��
                parseVariableDeclaration(lAdr);
            }
            else if (tokenList[listCurrent + 1].first == "function" ||
                (tokenList[listCurrent + 1].first == "EOLN" && tokenList[listCurrent + 2].first == "function"))
            { // ����˵��
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

    // ��������˵�����
    void parseVariableDeclaration(size_t& lAdr) 
    {
        // <����˵��>��integer <����>
        if (tokenList[listCurrent].first == "integer") 
        {
            advance();
            // �ǼǱ���
            // ���ݵ�ǰ�Ĺ��̺͵ȼ������µı�����Ԫ
            std::string vName = tokenList[listCurrent].first;
            std::string vProc = "main";
            if (!proList.empty())
            {
                vProc = currentLevel > 0 ? proList.back().pName : "main";
            }
            size_t vKind = 0; // ����������, 0��ʾ����
            std::string vType = "integer"; // ����������
            size_t vLev = currentLevel; // �����Ĳ㼶
            size_t vAdr = varList.size(); // �������б��еĵ�ַ
            lAdr = vAdr; // ���µ�ǰ�������һ������λ��
           
            varList.push_back(VarUnit(vName, vProc, vKind, vType, vLev, vAdr));
            advance();
            if (tokenList[listCurrent].second != "23")
            {
                error("symbol_not_found", "variable or function");
                exit(-1);
            }
        }
    }

    // ��������˵�����
    void parseFunctionDeclaration() 
    {
        // <����˵��>��integer function <��ʶ��>��<����>����<������>
        std::string pName; // ������
        std::string pType; // ��������
        size_t pLev; // ���̲��
        size_t fAdr = 0; // ��һ�������ڱ������е�λ��
        size_t lAdr = 0; // ���һ�������ڱ������е�λ��

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
                        parseParameter(pName, fAdr); // �������
                        if (tokenList[listCurrent].first == ")") 
                        {
                            advance();
                            if (tokenList[listCurrent].first == ";") 
                            {
                                advance();
                                // �Ǽǹ�����Ϣ
                                proList.push_back(ProUnit(pName, pType, pLev, fAdr, 0));
                                parseFunctionBody(lAdr); // ��������
                                // ���¹�����Ϣ
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

    // ������������
    void parseParameter(const std::string& pName, size_t& fAdr) 
    {
        // <����>��<����>
        std::string vName = tokenList[listCurrent].first;
        std::string vProc = pName;
        size_t vKind = 1; // �β�
        std::string vType = "integer";
        size_t vLev = currentLevel;
        size_t vAdr = varList.size();
        fAdr = vAdr;
        varList.push_back(VarUnit(vName, vProc, vKind, vType, vLev, vAdr));
        advance();
    }

    // ����������
    void parseFunctionBody(size_t& lAdr) 
    {
        // <������>��begin <˵������>��<ִ������> end
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

    // ����ִ������
    void parseExecutionStatementList() 
    {
        // <ִ������>��<ִ�����>��<ִ������>��<ִ�����>
        parseExecutionStatement();
        parseExecutionStatementListPrime();
    }

    // ������ݹ�
    void parseExecutionStatementListPrime() 
    {
        // ����Ƿ��зֺż���ִ�����
        if (tokenList[listCurrent].first == ";") 
        {
            advance(); // advance
            parseExecutionStatement();
            parseExecutionStatementListPrime();
        }
    }

    // ����ִ�����
    void parseExecutionStatement() 
    {
        // <ִ�����>��<�����>��<д���>��<��ֵ���>��<�������>
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
            // �������ı��ͨ������Ϊ��ֵ���
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

    // ���������
    void parseReadStatement() 
    {
        // �����
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

    // ����д���
    void parseWriteStatement() 
    {
        // д���
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

    // ���ұ����������ز���ֵ��ʾ�ɹ���ʧ��
    bool doesVarExist(const std::string& varName) 
    {
        for (const auto& var : varList) 
        {
            if (var.vName == varName) 
            {
                return true; // �ҵ��˱���
            }
        }
        return false; // û�ҵ�����
    }

    // ���ҹ����������ز���ֵ��ʾ�ɹ���ʧ��
    bool doesProExist(const std::string& proName) 
    {
        for (const auto& pro : proList) 
        {
            if (pro.pName == proName) 
            {
                return true; // �ҵ���
            }
        }
        return false; // û�ҵ�
    }

    // ������ֵ���
    void parseAssignmentStatement() 
    {
        // ��ֵ��䣬����ʶ���ʶ������� ':=' ������
        if (tokenList[listCurrent].second == "10") 
        {
            if (!doesVarExist(tokenList[listCurrent].first)) 
            { // ���������������
                if (!doesProExist(tokenList[listCurrent].first)) 
                { // ��������������ڣ����ҹ�����Ҳ������
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

    // �����������
    void parseConditionStatement() 
    {
        // �������
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

    // �����������ʽ
    void parseConditionExpression() 
    {
        // <�������ʽ>��<�������ʽ><��ϵ�����><�������ʽ>
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

    // �����������ʽ
    void parseArithmeticExpression() 
    {
        // <�������ʽ>��<�������ʽ>-<��>|<��>
        parseTerm();
        parseArithmeticExpressionPrime();
    }

    // ������ݹ�
    void parseArithmeticExpressionPrime() 
    {
        if (tokenList[listCurrent].first == "-") 
        {
            advance();
            parseTerm();
            parseArithmeticExpressionPrime();
        }
        // else �մ��������κβ���
    }

    // ������
    void parseTerm() 
    {
        // <��>��<��>*<����>|<����>
        parseFactor();
        parseTermPrime();
    }

    // ������ݹ�
    void parseTermPrime() 
    {
        if (tokenList[listCurrent].first == "*") 
        {
            advance();
            parseFactor();
            parseTermPrime();
        }
        // else �մ��������κβ���
    }

    // ��������
    void parseFactor() 
    {
        // <����>��<����>|<����>|<��������>
        if (tokenList[listCurrent].second == "10") 
        {
            // ���������������
            if (listCurrent + 1 < tokenList.size() && tokenList[listCurrent + 1].first == "(") 
            {
                // ��������
                advance();
                parseFunctionCall();
            }
            else 
            {
                // ����
                advance();
            }
        }
        else if (tokenList[listCurrent].second == "11") 
        {
            // ������
            advance();
        }
    }

    // ������������
    void parseFunctionCall() 
    {
        // ��������
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

    // ������ļ�
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
