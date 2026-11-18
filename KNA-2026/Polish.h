#pragma once
#include "LT.h"
#include "IT.h"

namespace POLISH {
	bool PolishNotation(int pos, LT::LexTable& lex, IT::IdTable& it);	
	bool CheckExpressions(LT::LexTable& lex, IT::IdTable& it);			
}
