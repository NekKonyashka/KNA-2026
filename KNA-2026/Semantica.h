#pragma once
#include "LT.h"
#include "IT.h"

namespace SEM {
	void CheckUsage(LT::LexTable& l);
	void CheckRepeated(IT::IdTable& i);
	void CheckLeftOperands(LT::LexTable& lt, IT::IdTable& it);
	void CheckFunctionCalls(LT::LexTable& lt, IT::IdTable& it);
}