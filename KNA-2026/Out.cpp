#include "Out.h"

namespace Out {
	OUT getout(wchar_t outfile[]) {
		std::ofstream* fs = new std::ofstream(outfile);

		if (!fs->is_open()) {
			delete fs;
			ERROR_THROW(113);
		}

		OUT out;
		wcscpy_s(out.outfile, outfile);
		out.stream = fs;
		return out;
	}

	void WriteToFile(OUT out, In::IN in) {
		*out.stream << in.text;
	}

	void WriteToError(OUT out, Error::ERROR err) {
		*out.stream << "Ошибка " << err.id << ": " << err.messages << "\n";
		if (err.inext.line != -1) {
			*out.stream << "Строка " << err.inext.line << " позиция " << err.inext.col << "\n";
		}
	}

	void CloseFile(OUT out) {
		out.stream->close();
		delete out.stream;
	}
}