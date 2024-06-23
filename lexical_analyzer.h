#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>

// 定义字符类别
enum class CharType {
    SPACE, // 空格
    LETTER, // 字母
    DIGIT, // 数字
    EQUALS_SIGN, // =
    MINUS_SIGN, // -
    MULTIPLY_SIGN, // *
    LEFT_PAREN, // (
    RIGHT_PAREN, // )
    LESS_THAN, // <
    GREATER_THAN, // >
    COLON, // :
    SEMICOLON, // ;
    NEW_LINE, // \n \r
    OTHERS,
};

// 定义错误类型
enum class ErrorType {
    INVALID_SYMBOL, // 不合法的标识符
    IDENTIFIER_TOO_LONG, // 标识符过长
    MISSING_EQUAL_AFTER_COLON, // 冒号不匹配
};

// 定义状态集
enum class State {
    INITIAL, // 初始态
    IN_WORD,
    IN_NUMBER,
    AFTER_EQUALS,
    AFTER_MINUS,
    AFTER_MULTIPLY,
    AFTER_LEFT_PAREN,
    AFTER_RIGHT_PAREN,
    AFTER_LESS_THAN,
    AFTER_GREATER_THAN,
    AFTER_COLON,
    AFTER_SEMICOLON,
};

// 定义种别对照表
namespace TokenType {
    const std::string BEGIN = "01";
    const std::string END = "02";
    const std::string INTEGER = "03";
    const std::string IF = "04";
    const std::string THEN = "05";
    const std::string ELSE = "06";
    const std::string FUNCTION = "07";
    const std::string READ = "08";
    const std::string WRITE = "09";
    const std::string IDENTIFIER = "10";
    const std::string CONSTANT = "11";
    const std::string EQUALS = "12";
    const std::string NOT_EQUALS = "13";
    const std::string LESS_OR_EQUALS = "14";
    const std::string LESS = "15";
    const std::string GREATER_OR_EQUALS = "16";
    const std::string GREATER = "17";
    const std::string MINUS = "18";
    const std::string MULTIPLY = "19";
    const std::string ASSIGN = "20";
    const std::string OPEN_PAREN = "21";
    const std::string CLOSE_PAREN = "22";
    const std::string SEMICOLON = "23";
    const std::string EOLN = "24";
    const std::string END_OF_FILE = "25";
};

// 初始化字符类型查找表，该表用于确定给定字符的类型
void initcharTypeTable();

// 初始化种别对照表
void initTokenTable();

// 检查并返回字符c的类型
CharType check(char c);

// 错误处理函数
void error(ErrorType type, std::ofstream& outErrorFile);

// 格式化输出字符串
std::string format(const std::string& key, const std::string& type);

// 处理词
void handleWord(std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile);

// 处理空格
void handleSpace(std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// 处理字母
void handleLetter(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// 处理数字
void handleDigit(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// 处理 =
void handleEqual(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// 通用处理函数，调用了handleWord
void normalhandle(CharType type, char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// 处理 >
void handleGreaterThan(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// 处理换行
void handleNewLine(char c, std::string& word, std::ifstream& infile, std::ofstream& outTargetFile, std::ofstream& outErrorFile);

// 生成.dyd文件
void generateDydFile(std::string sourceFileName);

// 词法分析主控函数
int lexical_analyzer(std::string sourceFileName);

#endif
