#pragma once
#include "LT.h"
#include "IT.h"

namespace SEM {
	void CollectSignatures(LT::LexTable& lt, IT::IdTable& it);
	void CheckRepeated(LT::LexTable& lt, IT::IdTable& it);
	void CheckUsage(LT::LexTable& lt, IT::IdTable& it);       
	void CheckLeftOperands(LT::LexTable& lt, IT::IdTable& it);
	void CheckFunctionCalls(LT::LexTable& lt, IT::IdTable& it); 
	void CheckTypes(LT::LexTable& lt, IT::IdTable& it);         
}