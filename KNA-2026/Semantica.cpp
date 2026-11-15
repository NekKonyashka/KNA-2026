#include "Semantica.h"
#include "Error.h"
#include <set>
#include <vector>
#include <string>
#include <iostream>

namespace SEM {
	namespace {

		struct Operand {
			IT::IDDATATYPE type; 
			bool isConst; 
			int val; 
		};

		void Fail(int id, LT::Entry& e) { 
			ERROR_THROW_IN(id, e.sn, -1); 
		}

		// строка берётся из первой лексемы идентификатора (если лексер заполняет idxfirstLE)
		void FailIT(int id, LT::LexTable& lt, const IT::Entry& e) {
			if (e.idxfirstLE >= 0 && e.idxfirstLE < lt.size) Fail(id, lt.table[e.idxfirstLE]);
			ERROR_THROW(id);
		}

		void Warn(int id, LT::Entry& e) {
			std::cout << "Предупреждение " << id << ": " << Error::geterror(id).messages
				<< " (строка " << e.sn << ")" << std::endl;
		}

		bool IsBuiltin(std::string& n) { 
			return n == "power" || n == "extent"; 
		}

		char PrevLex(LT::LexTable& lt, int i) {
			while (--i >= 0) {
				if (lt.table[i].lexema != LEX_FILL) 
					return lt.table[i].lexema;
			}
			return '\0';
		}

		// Вычисление типа выражения в ПОЛИЗе: from .. ближайшая ';'
		// valueNeeded=false только для вызова-оператора (procedure();)
		Operand Eval(LT::LexTable& lt, IT::IdTable& it, int from, int& end, bool full, bool valueNeeded) {
			std::vector<Operand> st;
			int i = from;
			for (; i < lt.size && lt.table[i].lexema != LEX_SEMICOLON; i++) {
				LT::Entry& e = lt.table[i];
				switch (e.lexema) {
				case LEX_LITERAL: {
					IT::Entry& ie = it.table[e.idxTi];
					bool num = ie.iddatatype == IT::INT;
					if (full && num && ie.value.vint.overflow) Fail(307, e);
					st.push_back({ ie.iddatatype, num, num ? ie.value.vint.val : 0 });
					break;
				}
				case LEX_ID: {
					bool callee = i + 1 < lt.size && lt.table[i + 1].lexema == LEX_AT;
					if (!callee) st.push_back({ it.table[e.idxTi].iddatatype, false, 0 });
					break;
				}
				case LEX_AT: {
					LT::Entry& callee = lt.table[i - 1];
					IT::Entry& fe = it.table[callee.idxTi];
					if (fe.idtype != IT::F) Fail(325, callee);
					auto& f = fe.value.vfunc;
					if (f.paramCount < 0) Fail(310, callee);
					int n = e.params;
					if (n != f.paramCount || n > (int)st.size()) Fail(308, callee);
					int base = (int)st.size() - n;
					for (int k = 0; k < n; k++)
						if (st[base + k].type != f.types[k]) Fail(309, callee);
					st.resize(base);
					bool outer = i + 1 < lt.size && lt.table[i + 1].lexema == LEX_SEMICOLON;
					if (fe.iddatatype == IT::VOID && !(outer && !valueNeeded)) Fail(311, callee);
					st.push_back({ fe.iddatatype, false, 0 });
					break;
				}
				case LEX_PLUS: {                       // 'v': + - * / #
					if (e.params == 1) {               // унарная (помечена в ПОЛИЗе)
						if (st.empty()) Fail(602, e);
						if (e.op != '-') Fail(323, e);
						if (full && st.back().type != IT::INT) Fail(306, e);
						if (st.back().isConst) st.back().val = -st.back().val;
						break;
					}
					if (st.size() < 2) Fail(602, e);
					Operand r = st.back(); st.pop_back();
					Operand l = st.back(); st.pop_back();
					bool divLike = (e.op == '/' || e.op == '#');
					if (full) {
						if (l.type == IT::STR || r.type == IT::STR) Fail(306, e);
						if (l.type != r.type) Fail(305, e);
						if (divLike && r.isConst && r.val == 0) Fail(315, e);
					}
					Operand res = { l.type, false, 0 };
					if (l.isConst && r.isConst && !(divLike && r.val == 0)) {
						long long a = l.val, b = r.val, c = 0;
						switch (e.op) {
						case '+': c = a + b; break;
						case '-': c = a - b; break;
						case '*': c = a * b; break;
						case '/': c = a / b; break;
						case '#': c = a % b; break;
						}
						res.isConst = true; res.val = (int)c;
					}
					st.push_back(res);
					break;
				}
				default: break;
				}
			}
			end = i;
			return st.empty() ? Operand{ IT::UNDEF, false, 0 } : st.back();
		}

		void Walk(LT::LexTable& lt, IT::IdTable& it, bool full) {
			int cur = -1;           // функция, в теле которой находимся
			bool inMain = false;
			for (int i = 0; i < lt.size; i++) {
				LT::Entry& e = lt.table[i];
				int end = i;
				switch (e.lexema) {
				case LEX_FUNCTION:
					if (lt.table[i + 1].lexema == LEX_MAIN) { inMain = true; cur = -1; }
					else { inMain = false; cur = (int)lt.table[i + 1].idxTi; }
					break;

				case LEX_EQUALS:        // i : t = <выражение> ;
				case LEX_ASSIGN: {      // i ~ <выражение> ;
					LT::Entry& tgt = lt.table[e.lexema == LEX_EQUALS ? i - 3 : i - 1];
					Operand r = Eval(lt, it, i + 1, end, full, true);
					if (full && tgt.idxTi != LT_TI_NULLIDX && it.table[tgt.idxTi].iddatatype != r.type)
						Fail(304, tgt);
					i = end;
					break;
				}
				case LEX_PROJECT:
					Eval(lt, it, i + 1, end, full, true);
					i = end;
					break;

				case LEX_RETURN: {
					if (lt.table[i + 1].lexema == LEX_SEMICOLON) break;     // ret;
					Operand r = Eval(lt, it, i + 1, end, full, true);
					if (full) {
						if (inMain) { if (r.type != IT::INT) Fail(322, e); }
						else if (cur >= 0 && it.table[cur].iddatatype != r.type) Fail(312, e);
					}
					i = end;
					break;
				}
				case LEX_REPEAT: {      // a : <число> ... (проверяется только первый операнд)
					int j = i + 2;
					if (j + 1 >= lt.size) break;
					bool neg = false;
					if (lt.table[j].lexema == LEX_PLUS && lt.table[j].op == '-') { neg = true; j++; }
					LT::Entry& c = lt.table[j];
					if (full && (c.lexema == LEX_LITERAL || c.lexema == LEX_ID)) {
						IT::Entry& ie = it.table[c.idxTi];
						if (ie.iddatatype != IT::INT) Fail(316, c);
						if (neg && c.lexema == LEX_LITERAL && ie.value.vint.val != 0) Fail(317, c);
					}
					break;
				}
				case LEX_ID:
				case LEX_LITERAL: {     // вызов-оператор: procedure(...);  (в ПОЛИЗе: args id @ ;)
					char prev = PrevLex(lt, i);
					char next = i + 1 < lt.size ? lt.table[i + 1].lexema : '\0';
					bool stmtStart = (prev == LEX_SEMICOLON || prev == LEX_END || prev == LEX_NUMBER || prev == LEX_VOID)
						&& next != LEX_COLON && next != LEX_ASSIGN;
					if (stmtStart) {
						Operand r = Eval(lt, it, i, end, full, false);
						if (r.type != IT::VOID && r.type != IT::UNDEF) Warn(802, e);
						i = end;
					}
					break;
				}
				default: break;
				}
			}
		}
	}

	void CollectSignatures(LT::LexTable& lt, IT::IdTable& it) {
		for (int k = 0; k < it.size; k++)
			if (it.table[k].idtype == IT::F && !IsBuiltin(it.table[k].id))
				it.table[k].value.vfunc.paramCount = -1;
		int mains = 0;
		for (int i = 0; i + 1 < lt.size; i++) {
			if (lt.table[i].lexema != LEX_FUNCTION) continue;
			LT::Entry& nx = lt.table[i + 1];
			if (nx.lexema == LEX_MAIN) { mains++; continue; }
			if (nx.lexema != LEX_ID || nx.idxTi == LT_TI_NULLIDX) continue;
			IT::Entry& fe = it.table[nx.idxTi];
			if (IsBuiltin(fe.id)) Fail(303, nx);
			auto& f = fe.value.vfunc;
			f.paramCount = 0;
			for (int j = i + 3; j < lt.size && lt.table[j].lexema != LEX_RIGHTHESIS; j++) {
				if (lt.table[j].lexema != LEX_ID || lt.table[j].idxTi == LT_TI_NULLIDX) continue;
				if (f.paramCount >= IT_MAXPARAMS) Fail(326, lt.table[j]);
				f.types[f.paramCount++] = it.table[lt.table[j].idxTi].iddatatype;
			}
		}
		if (mains == 0) ERROR_THROW(320);
		if (mains > 1) ERROR_THROW(321);
	}

	// Дубли в самой таблице (если лексер заводит отдельную запись на каждое объявление)
	void CheckRepeated(LT::LexTable& lt, IT::IdTable& it) {
		for (int i = 0; i < it.size; i++) {
			IT::Entry& a = it.table[i];
			if (a.idtype == IT::L) continue;
			if (a.idtype != IT::F && IsBuiltin(a.id)) FailIT(303, lt, a);
			for (int j = i + 1; j < it.size; j++) {
				IT::Entry& b = it.table[j];
				if (b.idtype == IT::L || a.id != b.id) continue;
				bool fa = a.idtype == IT::F, fb = b.idtype == IT::F;
				if (fa && fb) FailIT(301, lt, b);
				if (fa != fb) FailIT(302, lt, b);
				if (a.scope == b.scope) FailIT(301, lt, b);
			}
		}
	}

	// Использование до объявления + повторное объявление (если запись в ТИ переиспользуется)
	void CheckUsage(LT::LexTable& lt, IT::IdTable& it) {
		std::set<unsigned int> declared;
		for (int i = 0; i < lt.size; i++) {
			LT::Entry& e = lt.table[i];
			if (e.lexema != LEX_ID) continue;
			char prev = i > 0 ? lt.table[i - 1].lexema : '\0';
			char next = i + 1 < lt.size ? lt.table[i + 1].lexema : '\0';
			bool decl = prev == LEX_FUNCTION || next == LEX_COLON || prev == LEX_AS;   // f i | i : t | | i (счётчик)
			if (decl) {
				if (e.idxTi != LT_TI_NULLIDX) {
					IT::Entry& de = it.table[e.idxTi];
					if (de.idtype == IT::F && IsBuiltin(de.id) && !(i > 0 && lt.table[i - 1].lexema == LEX_FUNCTION))
						Fail(303, e);                                 
					if (!declared.insert(e.idxTi).second)
						Fail(de.idtype == IT::F ? 302 : 301, e);
				}
				continue;
			}
			if (e.idxTi == LT_TI_NULLIDX) Fail(300, e);
			IT::Entry& ie = it.table[e.idxTi];
			if (ie.idtype == IT::F && IsBuiltin(ie.id)) continue;
			if (!declared.count(e.idxTi)) Fail(ie.idtype == IT::F ? 310 : 300, e);
		}
	}

	// Цель присваивания / capture должна быть переменной (не блок, не литерал, не счётчик цикла)
	void CheckLeftOperands(LT::LexTable& lt, IT::IdTable& it) {
		std::set<unsigned int> counters;
		for (int i = 1; i < lt.size; i++)
			if (lt.table[i].lexema == LEX_ID && lt.table[i - 1].lexema == LEX_AS && lt.table[i].idxTi != LT_TI_NULLIDX)
				counters.insert(lt.table[i].idxTi);

		for (int i = 0; i < lt.size; i++) {
			LT::Entry& e = lt.table[i];
			if (e.lexema == LEX_ASSIGN && i > 0) {
				LT::Entry& t = lt.table[i - 1];
				if (t.lexema != LEX_ID || t.idxTi == LT_TI_NULLIDX ||
					(it.table[t.idxTi].idtype != IT::V && it.table[t.idxTi].idtype != IT::P)) Fail(324, t);
				if (counters.count(t.idxTi)) Fail(319, t);
			}
			else if (e.lexema == LEX_CAPTURE && i + 1 < lt.size) {
				LT::Entry& t = lt.table[i + 1];
				if (t.lexema != LEX_ID || t.idxTi == LT_TI_NULLIDX ||
					(it.table[t.idxTi].idtype != IT::V && it.table[t.idxTi].idtype != IT::P)) Fail(318, e);
				if (counters.count(t.idxTi)) Fail(319, t);
			}
		}
	}

	void CheckFunctionCalls(LT::LexTable& lt, IT::IdTable& it) {
		Walk(lt, it, false); 
	}
	void CheckTypes(LT::LexTable& lt, IT::IdTable& it) { 
		Walk(lt, it, true); 
	}
}