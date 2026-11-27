#pragma once
#include <vector>
#include "Error.h"
#include "LT.h"
#include "IT.h"

namespace SEM {

	struct Diagnostics {
		std::vector<Error::ERROR> errors;
		std::vector<Error::ERROR> warnings;
	};


	void CollectSignatures(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d);	// сигнатуры блоков, main
	void CheckRepeated(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d);		// дубли в таблице идентификаторов
	void CheckUsage(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d);			// использование до объ€влени€, повторное объ€вление


	void CheckLeftOperands(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d);	// цель := и capture
	void CheckTypes(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d);			// типы выражений, константы, ret, число повторений
	void CheckWarnings(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d);		// 800, 801

	void Finish(Diagnostics& d);
}
