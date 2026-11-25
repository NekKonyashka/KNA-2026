#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <cctype>

#if defined(_MSVC_LANG)
#define TEST_CPP_VER _MSVC_LANG
#else
#define TEST_CPP_VER __cplusplus
#endif
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


namespace TEST {
	namespace {
		bool SHOW_RULES = true;	

		void PrintDiagTest(const char* kind, const Error::ERROR& e) {
			std::cout << "  " << kind << " " << e.id << ": " << e.messages;
			if (e.inext.line >= 0) std::cout << ", строка " << e.inext.line;
			if (e.inext.col >= 0) std::cout << ", позиция " << e.inext.col;
			std::cout << std::endl;
		}

		bool ExpectedFromName(const std::string& name, char& kind, int& code) {
			if (name.compare(0, 2, "ok") == 0) { kind = 'o'; code = 0; return true; }
			if (name.size() < 2 || (name[0] != 'e' && name[0] != 'w') || !std::isdigit((unsigned char)name[1])) return false;
			kind = name[0];
			code = std::atoi(name.c_str() + 1);
			return true;
		}

		struct Result { std::string name; std::vector<int> errors, warnings; bool syntaxFailed = false; int lexError = 0; };

		Result RunOne(const fs::path& file, int number) {
			Result res;
			res.name = file.filename().string();
			std::cout << "\n====================== ТЕСТ " << number << ": " << res.name << " ======================\n";

			std::vector<In::WORD> words;
			LT::LexTable lextab = { 0, 0, nullptr };
			IT::IdTable idtab = { 0, 0, nullptr };
			try {
				std::wstring path = file.wstring();
				In::getin(&path[0], words);
				lextab = LT::Create(LT_MAXSIZE - 1);
				idtab = IT::Create(TI_MAXSIZE - 1);
				IT::AddLibrary(idtab);
				LEX::lexer lex = LEX::LexAnaliz(lextab, idtab, words);

				MFST::Mfst mfst = MFST::Mfst(lex, GRB::getGreibach());
				if (!mfst.start()) {
					res.syntaxFailed = true;
					std::cout << "  Синтаксический анализ: найдены ошибки, семантика пропущена\n";
				}
				else {
					if (SHOW_RULES) {
						mfst.savededucation();
						mfst.printrules();
					}
					SEM::Diagnostics diag;
					SEM::CollectSignatures(lextab, idtab, diag);
					SEM::CheckRepeated(lextab, idtab, diag);
					SEM::CheckUsage(lextab, idtab, diag);
					if (POLISH::CheckExpressions(lextab, idtab)) {
						SEM::CheckLeftOperands(lextab, idtab, diag);
						SEM::CheckFunctionCalls(lextab, idtab, diag);
						SEM::CheckTypes(lextab, idtab, diag);
						SEM::CheckWarnings(lextab, idtab, diag);
					}
					else {
						diag.errors.push_back(Error::geterror(602));
					}
					SEM::Finish(diag);

					for (auto& w : diag.warnings) { PrintDiagTest("Предупреждение", w); res.warnings.push_back(w.id); }
					for (auto& e : diag.errors) { PrintDiagTest("Ошибка", e); res.errors.push_back(e.id); }
					if (diag.errors.empty() && diag.warnings.empty()) std::cout << "  Замечаний нет\n";
				}
			}
			catch (Error::ERROR& e) {                       // ошибки In / лексера: тест прерывается, остальные идут дальше
				PrintDiagTest("Ошибка", e);
				res.lexError = e.id;
				res.errors.push_back(e.id);
			}
			if (lextab.table) LT::Delete(lextab);
			if (idtab.table) IT::Delete(idtab);
			return res;
		}

		bool Has(const std::vector<int>& v, int code) {
			return std::find(v.begin(), v.end(), code) != v.end();
		}
	}

	void DoTests(std::string path) {
		fs::path dir(path);
		std::error_code ec;
		if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
			std::cout << "Каталог с тестами не найден: " << fs::absolute(dir).string() << std::endl;
			return;
		}
		std::vector<fs::path> files;
		for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
			if (fs::is_regular_file(it->status())) files.push_back(it->path());
		}
		std::sort(files.begin(), files.end());

		std::vector<Result> results;
		int number = 1;
		for (auto& f : files) {
			results.push_back(RunOne(f, number++));
		}

		std::cout << "\n\n====================== ИТОГИ ======================\n";
		int passed = 0, checked = 0;
		for (size_t i = 0; i < results.size(); i++) {
			Result& r = results[i];
			std::cout << (i + 1) << ". " << r.name << ": ";
			if (r.syntaxFailed) std::cout << "синтаксическая ошибка";
			else {
				std::cout << "ошибки [";
				for (size_t k = 0; k < r.errors.size(); k++) std::cout << (k ? "," : "") << r.errors[k];
				std::cout << "] предупреждения [";
				for (size_t k = 0; k < r.warnings.size(); k++) std::cout << (k ? "," : "") << r.warnings[k];
				std::cout << "]";
			}
			char kind; int code;
			if (ExpectedFromName(r.name, kind, code)) {
				bool ok = kind == 'o' ? (r.errors.empty() && r.warnings.empty() && !r.syntaxFailed)
					: kind == 'e' ? Has(r.errors, code) : Has(r.warnings, code);
				checked++;
				if (ok) passed++;
				std::cout << (ok ? "   СОВПАЛО" : "   НЕ СОВПАЛО (ожидалось по имени файла)");
			}
			std::cout << std::endl;
		}
		if (checked) std::cout << "\nПроверено по имени файла: " << checked << ", совпало: " << passed << std::endl;
	}
}
