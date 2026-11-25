#include "Semantica.h"
#include <set>
#include <string>
#include <algorithm>
#include <climits>
#include <tuple>

namespace SEM {
	namespace {

		struct Operand { 
			IT::IDDATATYPE type; 
			bool isConst; 
			int val; 
		};

		void Fail(int id, const LT::Entry& e) {
			throw Error::geterrorin(id, e.sn, e.col);
		}

		void FailIT(int id, LT::LexTable& lt, const IT::Entry& e) {
			if (e.idxfirstLE >= 0 && e.idxfirstLE < lt.size) {
				Fail(id, lt.table[e.idxfirstLE]);
			}
			throw Error::geterror(id);
		}

		void Warn(Diagnostics& d, int id, const LT::Entry& e) {
			d.warnings.push_back(Error::geterrorin(id, e.sn, e.col));
		}

		bool IsBuiltin(const std::string& n) {
			return n == "power" || n == "extent";
		}

		char PrevLex(LT::LexTable& lt, int i) {
			while (--i >= 0) {
				if (lt.table[i].lexema != LEX_FILL) return lt.table[i].lexema;
			}
			return '\0';
		}

		char NextLex(LT::LexTable& lt, int i) {
			while (++i < lt.size) {
				if (lt.table[i].lexema != LEX_FILL) return lt.table[i].lexema;
			}
			return '\0';
		}

		int Semi(LT::LexTable& lt, int from) {
			for (int i = from; i < lt.size; i++) {
				if (lt.table[i].lexema == LEX_SEMICOLON) return i;
			}
			return lt.size;
		}

		// Вычисление типа выражения, записанного в ПОЛИЗе в [from, to).
		// full == false: только проверка вызовов; true: ещё операции и константы.
		// valueNeeded == false только у вызова-оператора (proc(...);).
		// Неизвестный тип (UNDEF) совместим с любым - так после уже найденной ошибки нет каскада.
		Operand Eval(LT::LexTable& lt, IT::IdTable& it, int from, int to, bool full, bool valueNeeded) {
			std::vector<Operand> st;
			for (int i = from; i < to; i++) {
				const LT::Entry& e = lt.table[i];
				switch (e.lexema) {
				case LEX_LITERAL: {
					IT::Entry& ie = it.table[e.idxTi];
					bool num = ie.iddatatype == IT::INT;
					if (full && num && ie.value.vint.overflow) Fail(307, e);
					st.push_back({ ie.iddatatype, num, num ? ie.value.vint.val : 0 });
					break;
				}
				case LEX_ID: {
					bool callee = i + 1 < to && lt.table[i + 1].lexema == LEX_AT;
					if (e.idxTi == LT_TI_NULLIDX) {
						if (!callee) st.push_back({ IT::UNDEF, false, 0 });
						break;
					}
					if (!callee) st.push_back({ it.table[e.idxTi].iddatatype, false, 0 });
					break;
				}
				case LEX_AT: {
					const LT::Entry& callee = lt.table[i - 1];
					if (callee.lexema != LEX_ID || callee.idxTi == LT_TI_NULLIDX) Fail(602, e);
					IT::Entry& fe = it.table[callee.idxTi];
					int n = e.params;
					if (n < 0 || n >(int)st.size()) Fail(308, callee);
					if (fe.iddatatype == IT::UNDEF) {          // имя не объявлено: ошибка уже выдана CheckUsage
						st.resize(st.size() - n);
						st.push_back({ IT::UNDEF, false, 0 });
						break;
					}
					if (fe.idtype != IT::F) Fail(325, callee);
					auto& f = fe.value.vfunc;
					if (f.paramCount < 0) {                    // сигнатура не собрана
						st.resize(st.size() - n);
						st.push_back({ IT::UNDEF, false, 0 });
						break;
					}
					if (n != f.paramCount) Fail(308, callee);
					int base = (int)st.size() - n;
					for (int k = 0; k < n; k++) {
						IT::IDDATATYPE t = st[base + k].type;
						if (t != IT::UNDEF && t != f.types[k]) Fail(309, callee);
					}
					st.resize(base);
					bool outer = (i + 1 >= to);                // последняя операция выражения
					if (fe.iddatatype == IT::VOID && !(outer && !valueNeeded)) Fail(311, callee);
					st.push_back({ fe.iddatatype, false, 0 });
					break;
				}
				case LEX_PLUS: {                               // 'v': + - * / #
					if (e.params == 1) {                       // унарная (помечена в ПОЛИЗе)
						if (st.empty()) Fail(602, e);
						if (e.op != '-') Fail(323, e);
						Operand& o = st.back();
						if (full && o.type == IT::STR) Fail(306, e);
						if (o.isConst) o.val = -o.val;
						break;
					}
					if (st.size() < 2) Fail(602, e);
					Operand r = st.back(); st.pop_back();
					Operand l = st.back(); st.pop_back();
					bool divLike = (e.op == '/' || e.op == '#');
					if (full) {
						if (l.type == IT::STR || r.type == IT::STR) Fail(306, e);
						if (l.type != IT::UNDEF && r.type != IT::UNDEF && l.type != r.type) Fail(305, e);
						if (divLike && r.isConst && r.val == 0) Fail(315, e);
					}
					Operand res = { l.type != IT::UNDEF ? l.type : r.type, false, 0 };
					if (l.isConst && r.isConst && !(divLike && r.val == 0)) {
						long long a = l.val, b = r.val, c = 0;
						switch (e.op) {
						case '+': c = a + b; break;
						case '-': c = a - b; break;
						case '*': c = a * b; break;
						case '/': c = a / b; break;
						case '#': c = a % b; break;
						}
						res.isConst = true;
						res.val = (int)c;
					}
					st.push_back(res);
					break;
				}
				default: break;
				}
			}
			return st.empty() ? Operand{ IT::UNDEF, false, 0 } : st.back();
		}

		// Обход всех инструкций с выражениями. Ошибка в инструкции прерывает только её.
		void Walk(LT::LexTable& lt, IT::IdTable& it, bool full, Diagnostics& d) {
			int cur = -1;                  // блок, в теле которого находимся (индекс в ТИ)
			bool inMain = false;
			for (int i = 0; i < lt.size; i++) {
				const LT::Entry& e = lt.table[i];
				int next = i;              // с какой лексемы продолжить после инструкции
				try {
					switch (e.lexema) {
					case LEX_FUNCTION: {
						if (i + 1 >= lt.size) break;
						cur = (int)lt.table[i + 1].idxTi;
						inMain = lt.table[i + 1].lexema == LEX_MAIN;
						break;
					}
					case LEX_EQUALS:               // i : t = E ;
					case LEX_ASSIGN: {             // i ~ E ;
						int semi = Semi(lt, i + 1);
						next = semi;
						int ti = e.lexema == LEX_EQUALS ? i - 3 : i - 1;
						Operand r = Eval(lt, it, i + 1, semi, full, true);
						if (full && ti >= 0 && lt.table[ti].idxTi != LT_TI_NULLIDX) {
							IT::IDDATATYPE want = it.table[lt.table[ti].idxTi].iddatatype;
							if (r.type != IT::UNDEF && want != IT::UNDEF && want != r.type) Fail(304, lt.table[ti]);
						}
						break;
					}
					case LEX_PROJECT: {
						int semi = Semi(lt, i + 1);
						next = semi;
						Eval(lt, it, i + 1, semi, full, true);
						break;
					}
					case LEX_RETURN: {
						int semi = Semi(lt, i + 1);
						next = semi;
						if (semi == i + 1) break;                  // ret;
						Operand r = Eval(lt, it, i + 1, semi, full, true);
						if (full && cur >= 0 && r.type != IT::UNDEF) {
							IT::IDDATATYPE want = it.table[cur].iddatatype;
							if (want != IT::UNDEF && want != r.type) Fail(inMain ? 322 : 312, e);
						}
						break;
					}
					case LEX_REPEAT: {             // a : <ПОЛИЗ числа повторений> ...
						int from = i + 2, len = e.params;
						if (len <= 0) break;
						next = from + len - 1;
						Operand r = Eval(lt, it, from, from + len, full, true);
						if (full) {
							if (r.type != IT::UNDEF && r.type != IT::INT) Fail(316, lt.table[from]);
							if (r.isConst && r.val < 0) Fail(317, lt.table[from]);
						}
						break;
					}
					case LEX_SEMICOLON: {          // ';' вызова-оператора помечена началом его ПОЛИЗа
						if (e.params < 0 || e.params >= i) break;
						Operand r = Eval(lt, it, e.params, i, full, false);
						if (r.type != IT::VOID && r.type != IT::UNDEF && i >= 2) Warn(d, 802, lt.table[i - 2]);
						break;
					}
					default: break;
					}
				}
				catch (Error::ERROR& er) {
					d.errors.push_back(er);
				}
				i = next;
			}
		}
	}

	void CollectSignatures(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		for (int k = 0; k < it.size; k++) {
			if (it.table[k].idtype == IT::F && !IsBuiltin(it.table[k].id)) {
				it.table[k].value.vfunc.paramCount = -1;
			}
		}
		int mains = 0;
		for (int i = 0; i + 1 < lt.size; i++) {
			if (lt.table[i].lexema != LEX_FUNCTION) continue;
			const LT::Entry& nx = lt.table[i + 1];
			try {
				if (nx.idxTi == LT_TI_NULLIDX) continue;
				IT::Entry& fe = it.table[nx.idxTi];
				if (nx.lexema == LEX_MAIN) {
					mains++;
					if (fe.iddatatype != IT::INT) Fail(322, nx);
					continue;
				}
				if (nx.lexema != LEX_ID) continue;
				if (IsBuiltin(fe.id)) Fail(303, nx);
				auto& f = fe.value.vfunc;
				f.paramCount = 0;
				for (int j = i + 3; j < lt.size && lt.table[j].lexema != LEX_RIGHTHESIS; j++) {
					if (lt.table[j].lexema != LEX_ID || lt.table[j].idxTi == LT_TI_NULLIDX) continue;
					if (f.paramCount >= TI_MAXPARAMS) Fail(326, lt.table[j]);
					f.types[f.paramCount++] = it.table[lt.table[j].idxTi].iddatatype;
				}
			}
			catch (Error::ERROR& e) {
				d.errors.push_back(e);
			}
		}
		if (mains == 0) d.errors.push_back(Error::geterror(320));
		else if (mains > 1) d.errors.push_back(Error::geterror(321));
	}

	// Дубли среди записей ТИ (если лексер завёл отдельные записи под одно имя)
	void CheckRepeated(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		for (int i = 0; i < it.size; i++) {
			IT::Entry& a = it.table[i];
			if (a.idtype == IT::L) continue;
			if (a.idtype == IT::F && IsBuiltin(a.id)) continue;
			if (a.idtype == IT::V && a.iddatatype == IT::UNDEF) continue;      // запись создана использованием без объявления - это 300/310
			try {
				for (int j = i + 1; j < it.size; j++) {
					IT::Entry& b = it.table[j];
					if (b.idtype == IT::L || a.id != b.id) continue;
					if (b.idtype == IT::V && b.iddatatype == IT::UNDEF) continue;
					bool fa = a.idtype == IT::F, fb = b.idtype == IT::F;
					if (fa && fb) FailIT(302, lt, b);
					if (fa != fb) FailIT(302, lt, b);
					if (a.scope == b.scope) FailIT(301, lt, b);
				}
			}
			catch (Error::ERROR& e) {
				d.errors.push_back(e);
			}
		}
	}

	// Использование до объявления и повторное объявление (по лексемам в исходном порядке)
	void CheckUsage(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		std::set<unsigned int> declared, reported;
		for (int i = 0; i < lt.size; i++) {
			const LT::Entry& e = lt.table[i];
			if (e.lexema != LEX_ID) continue;
			char prev = i > 0 ? lt.table[i - 1].lexema : '\0';
			char next = i + 1 < lt.size ? lt.table[i + 1].lexema : '\0';
			bool isFuncDecl = prev == LEX_FUNCTION;
			bool decl = isFuncDecl || next == LEX_COLON || prev == LEX_AS;      // f i | i : t | | i (счётчик)
			try {
				if (e.idxTi == LT_TI_NULLIDX) {
					if (!decl) Fail(300, e);
					continue;
				}
				IT::Entry& ie = it.table[e.idxTi];
				if (decl) {
					if (ie.idtype == IT::F && IsBuiltin(ie.id) && !isFuncDecl) Fail(303, e);
					if (!declared.insert(e.idxTi).second) Fail(ie.idtype == IT::F ? 302 : 301, e);
					continue;
				}
				if (ie.idtype == IT::F && IsBuiltin(ie.id)) continue;
				if (!declared.count(e.idxTi) && reported.insert(e.idxTi).second) {
					Fail(next == LEX_LEFTHESIS ? 310 : 300, e);
				}
			}
			catch (Error::ERROR& er) {
				d.errors.push_back(er);
			}
		}
	}

	// Слева от := и в capture должна стоять переменная или параметр; счётчик цикла только для чтения
	void CheckLeftOperands(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		std::set<unsigned int> counters;
		for (int i = 1; i < lt.size; i++) {
			if (lt.table[i].lexema == LEX_ID && lt.table[i - 1].lexema == LEX_AS && lt.table[i].idxTi != LT_TI_NULLIDX) {
				counters.insert(lt.table[i].idxTi);
			}
		}
		for (int i = 0; i < lt.size; i++) {
			const LT::Entry& e = lt.table[i];
			try {
				if (e.lexema == LEX_ASSIGN && i > 0) {
					const LT::Entry& t = lt.table[i - 1];
					if (t.lexema != LEX_ID || t.idxTi == LT_TI_NULLIDX ||
						(it.table[t.idxTi].idtype != IT::V && it.table[t.idxTi].idtype != IT::P)) Fail(324, t);
					if (counters.count(t.idxTi)) Fail(319, t);
				}
				else if (e.lexema == LEX_CAPTURE && i + 1 < lt.size) {
					const LT::Entry& t = lt.table[i + 1];
					if (t.lexema != LEX_ID || t.idxTi == LT_TI_NULLIDX ||
						(it.table[t.idxTi].idtype != IT::V && it.table[t.idxTi].idtype != IT::P)) Fail(318, e);
					if (counters.count(t.idxTi)) Fail(319, t);
				}
			}
			catch (Error::ERROR& er) {
				d.errors.push_back(er);
			}
		}
	}

	void CheckFunctionCalls(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		Walk(lt, it, false, d);
	}

	void CheckTypes(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		Walk(lt, it, true, d);
	}

	// 800 - объявлена и нигде не используется; 801 - значение присваивается, но нигде не читается.
	// Чтение - любое вхождение, кроме объявления, цели := и цели capture.
	void CheckWarnings(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		struct Use { int declTok = -1; bool read = false; bool write = false; };
		std::vector<Use> use(it.size);
		for (int i = 0; i < lt.size; i++) {
			const LT::Entry& e = lt.table[i];
			if (e.lexema != LEX_ID || e.idxTi == LT_TI_NULLIDX) continue;
			IT::Entry& ie = it.table[e.idxTi];
			if (ie.idtype != IT::V && ie.idtype != IT::P) continue;
			char prev = PrevLex(lt, i), next = NextLex(lt, i);
			Use& u = use[e.idxTi];
			if (next == LEX_COLON || prev == LEX_AS) {                       // объявление
				if (u.declTok < 0) u.declTok = i;
				if (i + 3 < lt.size && lt.table[i + 3].lexema == LEX_EQUALS) u.write = true;   // i : t = E
			}
			else if (next == LEX_ASSIGN || prev == LEX_CAPTURE) {
				u.write = true;
			}
			else {
				u.read = true;
			}
		}
		for (int k = 0; k < it.size; k++) {
			if (use[k].declTok < 0 || use[k].read) continue;
			Warn(d, use[k].write ? 801 : 800, lt.table[use[k].declTok]);
		}
	}

	void Finish(Diagnostics& d) {
		auto key = [](const Error::ERROR& e) {
			int line = e.inext.line < 0 ? INT_MAX : e.inext.line;
			int col = e.inext.col < 0 ? INT_MAX : e.inext.col;
			return std::make_tuple(line, col, e.id);
			};
		auto fix = [&](std::vector<Error::ERROR>& v) {
			std::stable_sort(v.begin(), v.end(), [&](const Error::ERROR& a, const Error::ERROR& b) { return key(a) < key(b); });
			v.erase(std::unique(v.begin(), v.end(), [&](const Error::ERROR& a, const Error::ERROR& b) { return key(a) == key(b); }), v.end());
			};
		fix(d.errors);
		fix(d.warnings);
	}
}
