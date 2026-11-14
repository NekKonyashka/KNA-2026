#include "Log.h"
#include "IT.h"
#include "LT.h"
#include <iomanip>

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
		WriteLine(log, buf, "");
	}
	void Log::WriteLexTable(LOG log, LT::LexTable& lextable)
	{
		int currentLine = 1;
		*log.stream << "Таблица лексем:\n";
		*log.stream << std::setw(2) << std::setfill('0') << currentLine << " ";
		for (int i = 0; i < lextable.size; i++) {
			if (lextable.table[i].sn != currentLine) {
				currentLine = lextable.table[i].sn;     
				*log.stream << "\n";
				*log.stream << std::setw(2) << std::setfill('0') << currentLine << " ";
			}
			auto ent = LT::GetEntry(lextable, i);
			*log.stream << ent.lexema;
			if (ent.idxTi != LT_TI_NULLIDX) {
				*log.stream << "(" << ent.idxTi << ")";
			}
		}
	}

	void Log::WriteIdTable(LOG log, IT::IdTable& idtable)
	{
		int currentLine = 1;
		*log.stream << "\n\nТаблица идентификаторов:\n";
		*log.stream << "ИМЯ" << "ТИП" << "ИНДЕКС_ТЛ" << "ЗНАЧЕНИЕ" << "ОБЛ_ВИДИМОСТИ\n";
		for (int i = 0; i < idtable.size; i++) {
			IT::Entry ent = IT::GetEntry(idtable, i);
			*log.stream << ent.id << ", ";
			switch (ent.iddatatype) {
			case IT::INT: {
				*log.stream << "NUMBER";
				break;
			}
			case IT::STR: {
				*log.stream << "TEXT";
				break;
			}
			case IT::VOID: {
				*log.stream << "VOID";
				break;
			}
			case IT::UNDEF: {
				*log.stream << "UNDEF";
				break;
			}
			}

			*log.stream << ", ";

			switch (ent.idtype) {
			case IT::V: {
				*log.stream << "VAR";
				break;
			}
			case IT::F: {
				*log.stream << "FUNC";
				break;
			}
			case IT::P: {
				*log.stream << "PARM";
				break;
			}
			case IT::L: {
				*log.stream << "LIT";
				break;
			}
			}

			*log.stream << ", " << ent.idxfirstLE << ", ";



			switch (ent.iddatatype) {
			case IT::INT: {
				*log.stream << ent.value.vint.val;
				break;
			}
			case IT::STR: {
				if (ent.value.vstr.len == 0) {
					*log.stream << "NULL";
				}
				for (int j = 0; ent.value.vstr.str[j] != '\0'; j++)
				{
					*log.stream << ent.value.vstr.str[j];
				}
				break;
			}
			}
			*log.stream << ", " << (ent.scope == "" ? "global" : ent.scope);

			*log.stream << std::endl;
		}
	}

	void Close(LOG log) {
		log.stream->close();
		delete log.stream;
	}
}