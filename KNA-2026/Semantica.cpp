#include "Semantica.h"
#include <string>
#include <climits>

namespace SEM {

	struct Item {                       // элемент стека при проверке выражения
		IT::IDDATATYPE type;            // тип значения
		bool isConst;                   // значение известно на этапе трансляции
		int val;                        // значение константы
		bool lit;                       // целочисленный литерал: совместим с number и smallnumber (п. 1.8)
		int pos;                        // индекс лексемы-литерала в таблице лексем (для сообщения об ошибке)
	};

	struct Use {                        // использование переменной (для предупреждений 800 и 801)
		int declTok = -1;               // лексема объявления
		bool read = false;              // значение где-то читается
		bool write = false;             // значение где-то присваивается
	};

	static void Fail(int id, const LT::Entry& e) {
		throw Error::geterrorin(id, e.sn, e.col);
	}

	static void FailIT(int id, LT::LexTable& lt, const IT::Entry& e) {
		if (e.idxfirstLE >= 0 && e.idxfirstLE < lt.size) {
			Fail(id, lt.table[e.idxfirstLE]);
		}
		throw Error::geterror(id);
	}

	static void Warn(Diagnostics& d, int id, const LT::Entry& e) {
		d.warnings.push_back(Error::geterrorin(id, e.sn, e.col));
	}

	static bool IsBuiltin(const std::string& n) {
		return n == "power" || n == "extent";
	}

	static char PrevLex(LT::LexTable& lt, int i) {
		while (--i >= 0) {
			if (lt.table[i].lexema != LEX_FILL) return lt.table[i].lexema;
		}
		return '\0';
	}

	static char NextLex(LT::LexTable& lt, int i) {
		while (++i < lt.size) {
			if (lt.table[i].lexema != LEX_FILL) return lt.table[i].lexema;
		}
		return '\0';
	}

	static int Semi(LT::LexTable& lt, int from) {
		for (int i = from; i < lt.size; i++) {
			if (lt.table[i].lexema == LEX_SEMICOLON) return i;
		}
		return lt.size;
	}

	static bool IsNumber(IT::IDDATATYPE t) {
		return t == IT::NUM || t == IT::SMNUM;
	}

	// Диапазон типа smallnumber: от -32768 до 32767 (п. 1.5 спецификации)
	static bool InSmall(int v) {
		return v >= -TI_SMNUMBER_MAXSIZE - 1 && v <= TI_SMNUMBER_MAXSIZE;
	}

	// Совместим ли операнд с типом want. Целочисленный литерал совместим с number, а с smallnumber -
	// если входит в его диапазон, иначе ошибка 307 (п. 1.6, 1.8 спецификации).
	// Неизвестный тип (UNDEF) совместим с любым - так после уже найденной ошибки нет каскада.
	static bool Fits(LT::LexTable& lt, const Item& o, IT::IDDATATYPE want) {
		if (o.type == IT::UNDEF || want == IT::UNDEF || o.type == want) {
			return true;
		}
		if (o.lit && want == IT::SMNUM) {
			if (!InSmall(o.val)) {
				Fail(307, lt.table[o.pos]);
			}
			return true;
		}
		return false;
	}

	// Лексема i - объявление идентификатора: f i | i : t | | i (счётчик цикла)
	static bool IsDecl(LT::LexTable& lt, int i) {
		char prev = i > 0 ? lt.table[i - 1].lexema : '\0';
		char next = i + 1 < lt.size ? lt.table[i + 1].lexema : '\0';
		return prev == LEX_FUNCTION || next == LEX_COLON || prev == LEX_AS;
	}

	// Было ли до лексемы pos объявление записи ТИ idx
	static bool DeclaredBefore(LT::LexTable& lt, unsigned int idx, int pos) {
		for (int j = 0; j < pos; j++) {
			if (lt.table[j].lexema == LEX_ID && lt.table[j].idxTi == idx && IsDecl(lt, j)) return true;
		}
		return false;
	}

	// Счётчик цикла: его запись ТИ создаётся на лексеме сразу после as ('|')
	static bool IsCounter(LT::LexTable& lt, IT::IdTable& it, unsigned int idx) {
		int first = it.table[idx].idxfirstLE;
		return first > 0 && first < lt.size && lt.table[first - 1].lexema == LEX_AS;
	}

	// Проверка выражения, записанного в ПОЛИЗе в [from, to). Значение выражения здесь не вычисляется (это делает
	// готовая программа): стек автомата хранит типы операндов, а для констант ещё и значения - они нужны для
	// проверок 307, 314, 315 и 317 (п. 1.16 спецификации). Возвращает тип и константность всего выражения.
	// valueNeeded == false только у вызова-оператора (proc(...);).
	static Item CheckExpr(LT::LexTable& lt, IT::IdTable& it, int from, int to, bool valueNeeded) {
		std::vector<Item> st;
		for (int i = from; i < to; i++) {
			const LT::Entry& e = lt.table[i];
			switch (e.lexema) {
			case LEX_LITERAL: {
				IT::Entry& ie = it.table[e.idxTi];
				bool num = ie.iddatatype == IT::NUM;
				if (num && ie.value.vint.overflow) Fail(307, e);
				st.push_back({ ie.iddatatype, num, num ? ie.value.vint.val : 0, num, i });
				break;
			}
			case LEX_ID: {
				bool callee = i + 1 < to && lt.table[i + 1].lexema == LEX_AT;
				if (callee) break;                             // имя вызываемого блока в стек не кладётся
				IT::IDDATATYPE t = e.idxTi == LT_TI_NULLIDX ? IT::UNDEF : it.table[e.idxTi].iddatatype;
				st.push_back({ t, false, 0, false, i });
				break;
			}
			case LEX_AT: {
				const LT::Entry& callee = lt.table[i - 1];
				int n = e.params;
				if (callee.lexema != LEX_ID || callee.idxTi == LT_TI_NULLIDX) {     // вызываемый блок неизвестен
					st.clear();
					st.push_back({ IT::UNDEF, false, 0, false, i });
					break;
				}
				IT::Entry& fe = it.table[callee.idxTi];
				if (n < 0 || n >(int)st.size()) Fail(308, callee);
				if (fe.iddatatype == IT::UNDEF) {          // имя не объявлено: ошибка уже выдана CheckUsage
					st.resize(st.size() - n);
					st.push_back({ IT::UNDEF, false, 0, false, i });
					break;
				}
				if (fe.idtype != IT::F) Fail(325, callee);
				auto& f = fe.value.vfunc;
				if (f.paramCount < 0) {                    // сигнатура не собрана
					st.resize(st.size() - n);
					st.push_back({ IT::UNDEF, false, 0, false, i });
					break;
				}
				if (n != f.paramCount) Fail(308, callee);
				int base = (int)st.size() - n;
				for (int k = 0; k < n; k++) {
					if (!Fits(lt, st[base + k], f.types[k])) Fail(309, callee);
				}
				st.resize(base);
				bool outer = (i + 1 >= to);                // последняя операция выражения
				if (fe.iddatatype == IT::VOID && !(outer && !valueNeeded)) Fail(311, callee);
				st.push_back({ fe.iddatatype, false, 0, false, i });
				break;
			}
			case LEX_NUMBER: {                             // тип(E): явное преобразование (п. 1.6, таблица 1.4)
				if (st.empty()) break;
				IT::IDDATATYPE to = (IT::IDDATATYPE)e.op;  // целевой тип записан лексером в op лексемы
				Item o = st.back();
				st.pop_back();
				if (o.type != IT::UNDEF && (o.type == IT::TXT) != (to == IT::TXT)) Fail(313, e);
				if (to == IT::SMNUM && o.isConst && !InSmall(o.val)) Fail(314, e);
				st.push_back({ to, o.isConst, o.val, false, i });
				break;
			}
			case LEX_PLUS: {                               // 'v': + - * / #
				if (e.params == 1) {                       // унарная (помечена в ПОЛИЗе)
					if (st.empty()) break;
					if (e.op != '-') Fail(323, e);
					Item& o = st.back();
					if (o.type == IT::TXT) Fail(306, e);
					if (o.isConst) {
						o.val = -o.val;                    // литерал остаётся литералом: -5 совместим с smallnumber
						if (o.type == IT::SMNUM) o.val = (short)o.val;
					}
					break;
				}
				if (st.size() < 2) {
					st.clear();
					st.push_back({ IT::UNDEF, false, 0, false, i });
					break;
				}
				Item r = st.back(); st.pop_back();
				Item l = st.back(); st.pop_back();
				bool divLike = (e.op == '/' || e.op == '#');
				if (l.type == IT::TXT || r.type == IT::TXT) Fail(306, e);
				IT::IDDATATYPE t = IT::UNDEF;
				if (l.type == IT::UNDEF) t = r.type;
				else if (r.type == IT::UNDEF) t = l.type;
				else if (Fits(lt, l, r.type)) t = r.type;      // литерал подстраивается под второй операнд
				else if (Fits(lt, r, l.type)) t = l.type;
				else Fail(305, e);
				if (divLike && r.isConst && r.val == 0) Fail(315, e);
				Item res = { t, false, 0, false, i };
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
					res.val = t == IT::SMNUM ? (short)c : (int)c;      // по модулю 2^N (п. 1.5)
				}
				st.push_back(res);
				break;
			}
			default: break;
			}
		}
		return st.empty() ? Item{ IT::UNDEF, false, 0, false, -1 } : st.back();
	}

	// Обход всех инструкций с выражениями. Ошибка в инструкции прерывает только её (п. 1.24 спецификации).
	static void Walk(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
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
					Item r = CheckExpr(lt, it, i + 1, semi, true);
					if (ti >= 0 && lt.table[ti].idxTi != LT_TI_NULLIDX) {
						IT::IDDATATYPE want = it.table[lt.table[ti].idxTi].iddatatype;
						if (!Fits(lt, r, want)) Fail(304, lt.table[ti]);
					}
					break;
				}
				case LEX_PROJECT: {
					int semi = Semi(lt, i + 1);
					next = semi;
					CheckExpr(lt, it, i + 1, semi, true);
					break;
				}
				case LEX_RETURN: {
					int semi = Semi(lt, i + 1);
					next = semi;
					if (semi == i + 1) break;                  // ret;
					Item r = CheckExpr(lt, it, i + 1, semi, true);
					if (cur >= 0 && !Fits(lt, r, it.table[cur].iddatatype)) Fail(inMain ? 322 : 312, e);
					break;
				}
				case LEX_REPEAT: {             // a : <ПОЛИЗ числа повторений> ...
					int from = i + 2, len = e.params;
					if (len <= 0) break;
					next = from + len - 1;
					Item r = CheckExpr(lt, it, from, from + len, true);
					if (r.type != IT::UNDEF && !IsNumber(r.type)) Fail(316, lt.table[from]);
					if (r.isConst && r.val < 0) Fail(317, lt.table[from]);
					break;
				}
				case LEX_SEMICOLON: {          // ';' вызова-оператора помечена началом его ПОЛИЗа
					if (e.params < 0 || e.params >= i) break;
					Item r = CheckExpr(lt, it, e.params, i, false);
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
					if (fe.iddatatype != IT::NUM) Fail(322, nx);
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
		for (int i = 0; i < lt.size; i++) {
			const LT::Entry& e = lt.table[i];
			if (e.lexema != LEX_ID) continue;
			char next = i + 1 < lt.size ? lt.table[i + 1].lexema : '\0';
			bool isFuncDecl = i > 0 && lt.table[i - 1].lexema == LEX_FUNCTION;
			bool decl = IsDecl(lt, i);
			try {
				if (e.idxTi == LT_TI_NULLIDX) {
					if (!decl) Fail(300, e);
					continue;
				}
				IT::Entry& ie = it.table[e.idxTi];
				if (decl) {
					if (ie.idtype == IT::F && IsBuiltin(ie.id) && !isFuncDecl) Fail(303, e);
					if (DeclaredBefore(lt, e.idxTi, i)) Fail(ie.idtype == IT::F ? 302 : 301, e);
					continue;
				}
				if (ie.idtype == IT::F && IsBuiltin(ie.id)) continue;
				// ошибка выдаётся один раз на запись ТИ - на первом её вхождении (idxfirstLE)
				if (ie.idxfirstLE == i && !DeclaredBefore(lt, e.idxTi, i)) {
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
		for (int i = 0; i < lt.size; i++) {
			const LT::Entry& e = lt.table[i];
			try {
				if (e.lexema == LEX_ASSIGN && i > 0) {
					const LT::Entry& t = lt.table[i - 1];
					if (t.lexema != LEX_ID || t.idxTi == LT_TI_NULLIDX ||
						(it.table[t.idxTi].idtype != IT::V && it.table[t.idxTi].idtype != IT::P)) Fail(324, t);
					if (IsCounter(lt, it, t.idxTi)) Fail(319, t);
				}
				else if (e.lexema == LEX_CAPTURE && i + 1 < lt.size) {
					const LT::Entry& t = lt.table[i + 1];
					if (t.lexema != LEX_ID || t.idxTi == LT_TI_NULLIDX ||
						(it.table[t.idxTi].idtype != IT::V && it.table[t.idxTi].idtype != IT::P)) Fail(318, e);
					if (IsCounter(lt, it, t.idxTi)) Fail(319, t);
				}
			}
			catch (Error::ERROR& er) {
				d.errors.push_back(er);
			}
		}
	}

	// Вызовы блоков, типы выражений, константы, ret, число повторений - один проход по всем выражениям
	void CheckTypes(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
		Walk(lt, it, d);
	}

	// 800 - объявлена и нигде не используется; 801 - значение присваивается, но нигде не читается.
	// Чтение - любое вхождение, кроме объявления, цели := и цели capture.
	void CheckWarnings(LT::LexTable& lt, IT::IdTable& it, Diagnostics& d) {
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

	// Порядок сообщений: по строке, затем по позиции, затем по коду; неизвестные строка и позиция (-1) - в конце
	static bool Before(const Error::ERROR& a, const Error::ERROR& b) {
		int la = a.inext.line < 0 ? TI_NUMBER_MAXSIZE : a.inext.line;
		int lb = b.inext.line < 0 ? TI_NUMBER_MAXSIZE : b.inext.line;
		if (la != lb) return la < lb;
		int ca = a.inext.col < 0 ? TI_NUMBER_MAXSIZE : a.inext.col;
		int cb = b.inext.col < 0 ? TI_NUMBER_MAXSIZE : b.inext.col;
		if (ca != cb) return ca < cb;
		return a.id < b.id;
	}

	// Сортировка вставками: порядок равных сообщений сохраняется
	static void Sort(std::vector<Error::ERROR>& v) {
		for (size_t i = 1; i < v.size(); i++) {
			Error::ERROR cur = v[i];
			size_t j = i;
			while (j > 0 && Before(cur, v[j - 1])) {
				v[j] = v[j - 1];
				j--;
			}
			v[j] = cur;
		}
	}

	void Finish(Diagnostics& d) {
		Sort(d.errors);
		Sort(d.warnings);
	}
}
