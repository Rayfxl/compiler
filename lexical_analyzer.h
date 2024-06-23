#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>

// �����ַ����
enum class CharType {
    SPACE, // �ո�
    LETTER, // ��ĸ
    DIGIT, // ����
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

// �����������
enum class ErrorType {
    INVALID_SYMBOL, // ���Ϸ��ı�ʶ��
    IDENTIFIER_TOO_LONG, // ��ʶ������
    MISSING_EQUAL_AFTER_COLON, // ð�Ų�ƥ��
};

// ����״̬��
enum class State {
    INITIAL, // ��ʼ̬
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

// �����ֱ���ձ�
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

// ��ʼ���ַ����Ͳ��ұ��ñ�����ȷ�������ַ�������
void initcharTypeTable();

// ��ʼ���ֱ���ձ�
void initTokenTable();

// ��鲢�����ַ�c������
CharType check(char c);

// ��������
void error(ErrorType type, std::ofstream& outErrorFile);

// ��ʽ������ַ���
std::string format(const std::string& key, const std::string& type);

// �����
void handleWord(std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile);

// ����ո�
void handleSpace(std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// ������ĸ
void handleLetter(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// ��������
void handleDigit(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// ���� =
void handleEqual(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// ͨ�ô�������������handleWord
void normalhandle(CharType type, char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// ���� >
void handleGreaterThan(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState);

// ������
void handleNewLine(char c, std::string& word, std::ifstream& infile, std::ofstream& outTargetFile, std::ofstream& outErrorFile);

// ����.dyd�ļ�
void generateDydFile(std::string sourceFileName);

// �ʷ��������غ���
int lexical_analyzer(std::string sourceFileName);

#endif
