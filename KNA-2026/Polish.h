#pragma once
#include "LT.h"
#include "IT.h"

namespace POLISH {
	static int GetPrioritet(char op);
	bool PolishNotation(int pos, LT::LexTable& lex, IT::IdTable& it);
	bool CheckExpressions(LT::LexTable& lex, IT::IdTable& it);
}