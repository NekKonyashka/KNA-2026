#pragma once
#include "In.h"
#include "IT.h"
#include "LT.h"

namespace LEX {

	struct lexer {
		LT::LexTable lextable;
		IT::IdTable idtable;
	};

	lexer LexAnaliz(LT::LexTable& lt, IT::IdTable& it, std::vector<std::pair<std::string, int>>& words);
}