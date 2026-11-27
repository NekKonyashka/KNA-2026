#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "In.h"
#include "Error.h"
#include "Parm.h"
#include "LT.h"
#include "IT.h"
#include "Polish.h"

namespace MFST { struct Mfst; }

namespace Log {
	struct LOG {
		wchar_t logfile[PARM_MAX_SIZE];
		std::ofstream* stream;
	};

	LOG getlog(wchar_t logfile[]);
	void WriteLine(LOG log, char* c, ...);
	void WriteLine(LOG log, wchar_t* c, ...);
	void WriteLog(LOG log);
	void WriteParm(LOG log, Parm::PARM parm);
	void WriteIn(LOG log, In::IN in);
	void WriteError(LOG log, Error::ERROR error);
	void WriteWarning(LOG log, Error::ERROR warning);

	LOG getextra(LOG log, const wchar_t* ext);
	void WriteLexTable(LOG log, LT::LexTable& lextable);
	void WriteIdTable(LOG log, IT::IdTable& idtable);
	void WriteSyntaxRules(LOG log, MFST::Mfst& mfst);
	void WritePolish(LOG log, LT::LexTable& lextable);
	void Close(LOG log);
}