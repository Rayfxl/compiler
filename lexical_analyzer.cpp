#include <string>
#include <unordered_map>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <array>
#include "lexical_analyzer.h"

std::string targetFileName;
std::string errorFileName;
std::unordered_map<std::string, std::string> tokentable;
std::array<CharType, 128> charTypeTable = {};
int currentline = 1;
const int KEY_FORMAT_LENGTH = 16;

void initTokenTable() {
	tokentable["begin"] = TokenType::BEGIN;
	tokentable["end"] = TokenType::END;
	tokentable["integer"] = TokenType::INTEGER;
	tokentable["if"] = TokenType::IF;
	tokentable["then"] = TokenType::THEN;
	tokentable["else"] = TokenType::ELSE;
	tokentable["function"] = TokenType::FUNCTION;
	tokentable["read"] = TokenType::READ;
	tokentable["write"] = TokenType::WRITE;
	tokentable["identifier"] = TokenType::IDENTIFIER;
	tokentable["constant"] = TokenType::CONSTANT;
	tokentable["="] = TokenType::EQUALS;
	tokentable["<>"] = TokenType::NOT_EQUALS;
	tokentable["<="] = TokenType::LESS_OR_EQUALS;
	tokentable["<"] = TokenType::LESS;
	tokentable[">="] = TokenType::GREATER_OR_EQUALS;
	tokentable[">"] = TokenType::GREATER;
	tokentable["-"] = TokenType::MINUS;
	tokentable["*"] = TokenType::MULTIPLY;
	tokentable[":="] = TokenType::ASSIGN;
	tokentable["("] = TokenType::OPEN_PAREN;
	tokentable[")"] = TokenType::CLOSE_PAREN;
	tokentable[";"] = TokenType::SEMICOLON;
	tokentable["EOLN"] = TokenType::EOLN;
	tokentable["EOF"] = TokenType::END_OF_FILE;
}

void initcharTypeTable() {
    // 设置每个特殊字符对应的CharType
    charTypeTable[' '] = CharType::SPACE;
    charTypeTable['='] = CharType::EQUALS_SIGN;
    charTypeTable['-'] = CharType::MINUS_SIGN;
    charTypeTable['*'] = CharType::MULTIPLY_SIGN;
    charTypeTable['('] = CharType::LEFT_PAREN;
    charTypeTable[')'] = CharType::RIGHT_PAREN;
    charTypeTable['<'] = CharType::LESS_THAN;
    charTypeTable['>'] = CharType::GREATER_THAN;
    charTypeTable[':'] = CharType::COLON;
    charTypeTable[';'] = CharType::SEMICOLON;
    charTypeTable['\r'] = CharType::NEW_LINE;
    charTypeTable['\n'] = CharType::NEW_LINE;
    for (char c = 'a'; c <= 'z'; ++c) charTypeTable[c] = CharType::LETTER;
    for (char c = 'A'; c <= 'Z'; ++c) charTypeTable[c] = CharType::LETTER;
    for (char c = '0'; c <= '9'; ++c) charTypeTable[c] = CharType::DIGIT;
}

CharType check(char c) {
    return charTypeTable[c];
}

void error(ErrorType type, std::ofstream& outErrorFile) {
    switch (type) {
        case ErrorType::INVALID_SYMBOL: {
            outErrorFile << "***LINE:" + std::to_string(currentline) + "  Invalid symbol." << std::endl;
            break;
        }
        case ErrorType::IDENTIFIER_TOO_LONG: {
            outErrorFile << "***LINE:" + std::to_string(currentline) + "  Identifier is too long." << std::endl;
            break;
        }
        case ErrorType::MISSING_EQUAL_AFTER_COLON: {
            outErrorFile << "***LINE:" + std::to_string(currentline) + "  miss '=' after ':'." << std::endl;
            break;
        }
    }
}

std::string format(const std::string& key, const std::string& type) {
    std::stringstream ss;
    ss << std::setw(KEY_FORMAT_LENGTH) << std::right << key << " " << type;
    return ss.str();
}

void handleWord(std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile) {
    if (word.length() == 0) return;
    if (word.length() > KEY_FORMAT_LENGTH) {
        error(ErrorType::IDENTIFIER_TOO_LONG, outErrorFile);
        word.clear();
        return;
    }
    auto it = tokentable.find(word);
    if (it != tokentable.end()) {
        std::string key = it->first;
        std::string type = it->second;
        if (key.compare("EOF") == 0) {
            outTargetFile << format(key, type);
        } else {
            outTargetFile << format(key, type) << std::endl;
        }
    } else if (word[0] != ':') {
        std::string type = tokentable.find("identifier")->second;
        std::string key = word;
        outTargetFile << format(key, type) << std::endl;
    } else {
        error(ErrorType::MISSING_EQUAL_AFTER_COLON, outErrorFile);
    }
    word.clear();
}

void handleSpace(std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState) {
    if (currentState != State::INITIAL) {
        handleWord(word, outTargetFile, outErrorFile);
        currentState = State::INITIAL;
    }
}

void handleLetter(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState) {
    if (currentState == State::IN_NUMBER) {
        error(ErrorType::INVALID_SYMBOL, outErrorFile);
        word.clear();
        currentState = State::INITIAL;
    } else if (currentState != State::INITIAL && currentState != State::IN_WORD) {
        handleWord(word, outTargetFile, outErrorFile);
    }
    word += c;
    currentState = State::IN_WORD;
}

void handleDigit(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState) {
    if (currentState == State::INITIAL) {
        word += c;
        currentState = State::IN_NUMBER;
    } else if (currentState == State::IN_NUMBER || currentState == State::IN_WORD) {
        word += c;
    } else {
        handleWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = State::IN_NUMBER;
    }
}

void handleEqual(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState) {
    if (currentState != State::INITIAL && currentState != State::AFTER_LESS_THAN
        && currentState != State::AFTER_GREATER_THAN && currentState != State::AFTER_COLON) {
        handleWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = State::AFTER_EQUALS;
    } else {
        word += c;
        handleWord(word, outTargetFile, outErrorFile);
        currentState = State::INITIAL;
    }
}

void normalhandle(CharType type, char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState) {
    handleWord(word, outTargetFile, outErrorFile);
    word += c;
    if (type == CharType::MINUS_SIGN) {
        currentState = State::AFTER_MINUS;
    } else if (type == CharType::MULTIPLY_SIGN) {
        currentState = State::AFTER_MULTIPLY;
    } else if (type == CharType::LEFT_PAREN) {
        currentState = State::AFTER_LEFT_PAREN;
    } else if (type == CharType::RIGHT_PAREN) {
        currentState = State::AFTER_RIGHT_PAREN;
    } else if (type == CharType::LESS_THAN) {
        currentState = State::AFTER_LESS_THAN;
    } else if (type == CharType::COLON) {
        currentState = State::AFTER_COLON;
    } else if (type == CharType::SEMICOLON){
        currentState = State::AFTER_SEMICOLON;
    }
}

void handleGreaterThan(char c, std::string& word, std::ofstream& outTargetFile, std::ofstream& outErrorFile, State& currentState) {
    if (currentState == State::AFTER_LESS_THAN) {
        word += c;
        handleWord(word, outTargetFile, outErrorFile);
        currentState = State::INITIAL;
    }
    else {
        handleWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = State::AFTER_GREATER_THAN;
    }
}

void handleNewLine(char c, std::string& word, std::ifstream& infile, std::ofstream& outTargetFile, std::ofstream& outErrorFile) {
    handleWord(word, outTargetFile, outErrorFile);
    word = "EOLN";
    handleWord(word, outTargetFile, outErrorFile);
    currentline++;
    if (c == '\r')
        infile >> std::noskipws >> c;
}

void generateDydFile(std::string sourceFileName) {
    std::ifstream infile;
    infile.open(sourceFileName.data(), std::ios::binary);
    assert(infile.is_open());
    
    std::ofstream outTargetFile, outErrorFile;
    outTargetFile.open(targetFileName.data(), std::ios::out);
    outErrorFile.open(errorFileName.data(), std::ios::out);
    assert(outTargetFile.is_open());
    assert(outErrorFile.is_open());

    std::string word;
    char c;
    State currentState = State::INITIAL;

    while (infile >> std::noskipws >> c) {
        CharType type = check(c);
        switch (type) {
            case CharType::SPACE: {
                handleSpace(word, outTargetFile, outErrorFile, currentState);
                break;
            }
            case CharType::LETTER: {
                handleLetter(c, word, outTargetFile, outErrorFile, currentState);
                break;
            }
            case CharType::DIGIT: {
                handleDigit(c, word, outTargetFile, outErrorFile, currentState);
                break;
            }
            case CharType::EQUALS_SIGN: {
                handleEqual(c, word, outTargetFile, outErrorFile, currentState);
                break;
            }
            case CharType::MINUS_SIGN:
            case CharType::MULTIPLY_SIGN:
            case CharType::LEFT_PAREN:
            case CharType::RIGHT_PAREN:
            case CharType::LESS_THAN:
            case CharType::COLON:
            case CharType::SEMICOLON: {
                normalhandle(type, c, word, outTargetFile, outErrorFile, currentState);
                break;
            }
            case CharType::GREATER_THAN: {
                handleGreaterThan(c, word, outTargetFile, outErrorFile, currentState);
                break;
            }
            case CharType::NEW_LINE: {
                handleNewLine(c, word, infile, outTargetFile, outErrorFile);
                break;
            }
            case CharType::OTHERS: {
                error(ErrorType::INVALID_SYMBOL, outErrorFile);
                break;
            }
        }
    }
    word = "EOF";
    handleWord(word, outTargetFile, outErrorFile);
    infile.close();
    outTargetFile.close();
    outErrorFile.close();
}

int lexical_analyzer(std::string sourceFileName) {
	targetFileName = sourceFileName.substr(0, sourceFileName.find_last_of(".")) + ".dyd";
	errorFileName = "lexicalError.err";

	initTokenTable();
    initcharTypeTable();
	generateDydFile(sourceFileName);

	return 0;
}