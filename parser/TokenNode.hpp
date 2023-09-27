//
// Created by hiccup on 2023/9/22.
//

#pragma once

#include "ASTNode.hpp"
#include "../lexer/Lexer.h"
#include <utility>
#include <vector>
#include <string>

class TokenNode : public ASTNode {
private:
    std::string value;
    int lineNum;
    TokenType type;
public:
    TokenNode(Token token) :
            value(std::move(token.value)), type(token.type), lineNum(token.lineNum) {}
};


