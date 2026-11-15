#include "LT.h"
#include "Error.h"

namespace LT {

	Entry::Entry(char l, int sn, unsigned int idxti,char op,int paramsCount){
		lexema = l;
		this->sn = sn;
		idxTi = idxti;
		this->op = op;
		params = paramsCount;
	}

	LexTable Create(int size) {
		if (size >= LT_MAXSIZE) {
			ERROR_THROW(113);
		}
		LexTable lt;
		lt.maxsize = size;
		lt.size = 0;
		lt.table = new Entry[size];
		return lt;
	}

	void Add(LexTable& lextable, Entry entry) {
		if (lextable.size >= lextable.maxsize) {
			ERROR_THROW(114);
		}
		lextable.table[lextable.size] = entry;
		lextable.size++;
	}

	Entry& GetEntry(LexTable& lextable, int n) {
		if (n < lextable.size && n >= 0) {
			return lextable.table[n];
		}
		else {
			ERROR_THROW(115);
		}
	}
	void Delete(LexTable& lextable) {
		delete[] lextable.table;
		lextable.size = 0;
		lextable.maxsize = 0;
	}
}