//
// Created by hiccup on 2023/9/18.
//
#include <bits/stdc++.h>
#include "parser/Parser.hpp"
#include "parser/Exception.hpp"
#include "lexer/Lexer.h"
#include "Config.hpp"
using namespace std;

bool PARSER_DISPLAY = true;
bool PARSER_SWITCH = true;

std::ofstream outfile("./output.txt");

void printString(const std::string& str) {
    if (PARSER_DISPLAY && PARSER_SWITCH) {
        outfile << str << std::endl;
    }
}

map<Exception, char> exceptionToString = {
        {Exception::INVALID_CHARACTER,       'a'},
        {Exception::REDEFINED_IDENT,         'b'},
        {Exception::UNDEFINED_IDENT,         'c'},
        {Exception::PARAMS_NUM_UNMATCHED,    'd'},
        {Exception::PARAM_TYPE_UNMATCHED,    'e'},
        {Exception::VOID_RETURN_INT,         'f'},
        {Exception::INT_RETURN_LACKED,       'g'},
        {Exception::CONST_ASSIGNED,          'h'},
        {Exception::SEMICN_LACKED,           'i'},
        {Exception::RPARENT_LACKED,          'j'},
        {Exception::RBRACK_LACKED,           'k'},
        {Exception::FORMAT_CHAR_UNMATCHED,   'l'},
        {Exception::BREAK_CONTINUE_OUT_LOOP, 'm'},
        {Exception::UNDEFINED_ERROR,         'n'}
};

int main() {

    std::ifstream infile("testfile.txt");
    std::string code((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());


    Lexer lexer(code);
    std::vector<Token> tokens = lexer.analyze();

    TokenStream tokenStream(tokens);
    Parser parser(tokenStream);

    auto AST = parser.parseCompUnit();

    AST.checkError(make_shared<ErrorCtx>(), make_shared<ErrorRet>());

    return 0;
}