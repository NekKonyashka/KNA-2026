#include "Log.h"
#include "IT.h"
#include "LT.h"
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include "MFST.h"
#include "Polish.h"

namespace Log {
	LOG getlog(wchar_t logfile[]) {
		LOG log;
		log.stream = new std::ofstream(logfile);
		if (log.stream->fail()) {
			delete log.stream;
			ERROR_THROW(112);
		}
		wcscpy_s(log.logfile, PARM_MAX_SIZE, logfile);
		return log;
	}
	void WriteLine(LOG log, char* c, ...) {
		char* result = new char[IN_MAX_LEN_TEXT];
		result[0] = '\0';
		char** temp = &c;
		int size = 0;
		while (*temp != "") {
			size += strlen(*temp);
			strcat_s(result, IN_MAX_LEN_TEXT, *temp);
			temp++;
		}
		log.stream->write(result, size);
		delete[] result;
	}
	void WriteLine(LOG log, wchar_t* c, ...) {
		char* result = new char[IN_MAX_LEN_TEXT];
		result[0] = '\0';
		wchar_t** temp = &c;
		int size = 0;
		while (*temp != L"") {
			size_t bufferSize;
			wcstombs_s(&bufferSize, nullptr, 0, *temp, _TRUNCATE);
			char* buf = new char[bufferSize];
			size_t converted;
			wcstombs_s(&converted, buf, bufferSize, *temp, _TRUNCATE);
			size += strlen(buf);
			strcat_s(result, IN_MAX_LEN_TEXT, buf);
			temp++;
		}
		log.stream->write(result, size);
		delete[] result;
	}
	void WriteLog(LOG log) {
		time_t now = time(nullptr);
		tm localTime;
		localtime_s(&localTime, &now);
		char buffer[128];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
		WriteLine(log, (char*)"---- Протокол ----- ", buffer, " ----- ", "\n", "");
	}
	void WriteParm(LOG log, Parm::PARM parm) {
		WriteLine(log, (wchar_t*)L"---- Параметры -----\n",
			(wchar_t*)L"-in: ", parm.in, "\n",
			(wchar_t*)L"-out: ", parm.out, "\n",
			(wchar_t*)L"-log: ", parm.log, "\n", L"");
	}
	void WriteIn(LOG log, In::IN in) {
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "\nКоличество символов: %d\nПроигнорировано: %d\nКоличество строк: %d", in.size, in.ignor, in.lines);
		WriteLine(log, (char*)"---- Исходные данные -----\n", buf, "");
	}
	void WriteError(LOG log, Error::ERROR error) {
		if (!log.stream->is_open()) {
			ERROR_THROW(112);
		}
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "\nИнформация об ошибке:\nID - %d,\nсообщение - %s,\nпозиция - %d,\nномер строки - %d",
			error.id, error.messages, error.inext.col, error.inext.line);
		*log.stream << buf;
	}

	static std::string TypeName(IT::IDDATATYPE t) {
		switch (t) {
		case IT::NUM: return "NUMBER";
		case IT::SMNUM: return "SMALLNUMBER";
		case IT::TXT: return "TEXT";
		case IT::VOID: return "VOID";
		default: return "UNDEF";
		}
	}

	static std::string KindName(IT::IDTYPE t) {
		switch (t) {
		case IT::V: return "VAR";
		case IT::F: return "FUNC";
		case IT::P: return "PARM";
		case IT::L: return "LIT";
		default: return "?";
		}
	}


	// Дополнительный файл журнала: <журнал><ext> (спецификация, п. 2.3, таблица 2.3)
	LOG getextra(LOG log, const wchar_t* ext) {
		LOG extra;
		std::wstring name = std::wstring(log.logfile) + ext;
		extra.stream = new std::ofstream(name.c_str());
		if (extra.stream->fail()) {
			delete extra.stream;
			ERROR_THROW(112);
		}
		wcsncpy_s(extra.logfile, PARM_MAX_SIZE, name.c_str(), _TRUNCATE);
		return extra;
	}

	// Таблица лексем: номер строки и лексемы этой строки подряд; после лексемы с записью в ТИ - (idxTi).
	// Формат по листингу 3.15 спецификации. Служебные '0' (место, освободившееся после ПОЛИЗ) не выводятся.
	static void PrintLexemes(std::ofstream& out, LT::LexTable& lextable, bool polish) {
		out << std::left << std::setw(8) << "СТРОКА" << "ЛЕКСЕМЫ";
		int currentLine = -1;
		for (int i = 0; i < lextable.size; i++) {
			LT::Entry ent = lextable.table[i];
			if (ent.lexema == LEX_FILL) {
				continue;
			}
			if (ent.sn != currentLine) {
				currentLine = ent.sn;
				out << "\n" << std::left << std::setw(8) << ent.sn;
			}
			out << ent.lexema;
			if (ent.idxTi != LT_TI_NULLIDX) {
				out << "(" << ent.idxTi << ")";
			}
			if (polish && ent.lexema == LEX_PLUS) {
				out << "(" << ent.op << ")";          // символ операции: + - * / #
			}
			if (polish && ent.lexema == LEX_AT) {
				out << ent.params;                     // число аргументов вызова
			}
			if (polish && ent.lexema == LEX_NUMBER) {
				out << "(" << TypeName((IT::IDDATATYPE)ent.op) << ")";   // целевой тип преобразования
			}
		}
		out << "\n";
	}

	// Таблица лексем -> файл <журнал>.lex (п. 3.5.2 спецификации)
	void WriteLexTable(LOG log, LT::LexTable& lextable)
	{
		LOG f = getextra(log, L".lex");
		PrintLexemes(*f.stream, lextable,false);
		Close(f);
	}

	void WriteWarning(LOG log, Error::ERROR warning) {
		if (!log.stream->is_open()) {
			ERROR_THROW(112);
		}
		*log.stream << "\nПредупреждение: ID - " << warning.id << ": " << warning.messages
			<< ",номер строки - " << warning.inext.line << ",номер столбца - " << warning.inext.col;
	}

	// Таблица идентификаторов -> файл <журнал>.id (п. 3.5.3, таблица 3.11 спецификации):
	// одна строка на запись, значение - последним столбцом, чтобы длинные литералы не сдвигали остальные
	void WriteIdTable(LOG log, IT::IdTable& idtable)
	{
		LOG f = getextra(log, L".id");
		std::ofstream& out = *f.stream;
		out << std::left << std::setfill(' ')
			<< std::setw(5) << "№" << std::setw(34) << "ИМЯ" << std::setw(8) << "ТИП" << std::setw(6) << "ВИД"
			<< std::setw(11) << "ИНДЕКС_ТЛ" << std::setw(20) << "ОБЛАСТЬ_ВИДИМОСТИ" << "ЗНАЧЕНИЕ" << "\n";
		for (int i = 0; i < idtable.size; i++) {
			IT::Entry ent = idtable.table[i];
			std::string value;
			if (ent.idtype == IT::F) {
				int n = ent.value.vfunc.paramCount;
				if (n < 0) {
					value = "-";
				}
				else {
					value = "параметров: " + std::to_string(n);
					for (int j = 0; j < n && j < TI_MAXPARAMS; j++) {
						value += (j == 0 ? " (" : ", ");
						value += TypeName(ent.value.vfunc.types[j]);
					}
					if (n > 0) {
						value += ")";
					}
				}
			}
			else if (ent.iddatatype == IT::NUM || ent.iddatatype == IT::SMNUM) {
				value = std::to_string(ent.value.vint.val);
				if (ent.idtype == IT::L && ent.value.vint.overflow) {
					value += " (переполнение)";
				}
			}
			else if (ent.iddatatype == IT::TXT) {
				if (ent.idtype == IT::L) {
					value = ent.value.vstr.len == 0 ? "(пусто)" : std::string(ent.value.vstr.str, ent.value.vstr.len);
				}
				else {
					value = "NULL";
				}
			}
			else {
				value = "-";
			}
			out << std::setw(5) << i << std::setw(34) << ent.id << std::setw(8) << TypeName(ent.iddatatype)
				<< std::setw(6) << KindName(ent.idtype) << std::setw(11) << ent.idxfirstLE
				<< std::setw(20) << (ent.scope.empty() ? "global" : ent.scope) << value << "\n";
		}
		Close(f);
	}

	// Правила грамматики, применённые при разборе -> файл <журнал>.rules (п. 2.3 спецификации):
	// порядковый номер и правило. Вызывать после успешного mfst.start().
	void WriteSyntaxRules(LOG log, MFST::Mfst& mfst)
	{
		LOG f = getextra(log, L".rules");
		char buf[205];
		for (unsigned short k = 0; k < mfst.storestate.size(); k++) {
			MFST::MfstState state = mfst.storestate.c[k];
			GRB::Rule rule = mfst.grebach.getRule(state.nrule);
			*f.stream << std::left << std::setw(6) << k + 1 << rule.getCRule(buf, state.nrulechain) << "\n";
		}
		Close(f);
	}

	// ПОЛИЗ -> файл <журнал>.pol: та же таблица лексем после преобразования выражений;
	// после v - символ операции в скобках, после @ - число аргументов, после t - целевой тип
	void WritePolish(LOG log, LT::LexTable& lextable)
	{
		LOG f = getextra(log, L".pol");
		PrintLexemes(*f.stream, lextable, true);
		Close(f);
	}

	void Close(LOG log) {
		log.stream->close();
		delete log.stream;
	}
}