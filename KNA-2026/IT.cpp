#include "IT.h"
#include "Error.h"
#include <iostream>

namespace IT {
	IdTable Create(int size) {
		if (size <= 0) {
			ERROR_THROW(116);
		}
		IdTable it;
		it.maxsize = size;
		it.size = 0;
		it.table = new Entry[size];
		return it;
	}

	void Add(IdTable& idtable, Entry entry) {
		if (idtable.size >= idtable.maxsize){
			ERROR_THROW(117);
		}
		idtable.table[idtable.size] = entry;
		idtable.size++;
	}

	Entry& GetEntry(IdTable& idtable, int n) {
		if (n < 0 || n >= idtable.size) {
			ERROR_THROW(118);
		}
		return idtable.table[n];
	}

	int IsId(IdTable& idtable, std::string id,std::string scope) {
		for (int i = 0; i < idtable.size; i++) {
			if (idtable.table[i].id == id && idtable.table[i].scope == scope) {
				return i;
			}
		}
		return TI_NULLIDX;
	}

	int FindVisible(IdTable& idtable, std::vector<std::string>& scopes, std::string name) {
		for (int s = scopes.size() - 1; s >= 0; --s) {
			int idx = IsId(idtable, name,scopes[s]);
			if (idx != TI_NULLIDX) {
				return idx;
			}
		}
		return TI_NULLIDX;
	}

	void AddLibrary(IdTable& idtable) {
		Entry e;
		e.idxfirstLE = -1;                
		e.iddatatype = INT;
		e.idtype = F;
		e.scope = "";
		for (int k = 0; k < IT_MAXPARAMS; k++) e.value.vfunc.types[k] = UNDEF;

		e.id = "power";                    
		e.value.vfunc.paramCount = 2;
		e.value.vfunc.types[0] = INT;
		e.value.vfunc.types[1] = INT;
		Add(idtable, e);

		e.id = "extent";                   
		e.value.vfunc.paramCount = 1;
		e.value.vfunc.types[0] = STR;
		Add(idtable, e);
	}


	void Delete(IdTable& idtable) {
		delete[] idtable.table;
		idtable.maxsize = 0;
		idtable.size = 0;
	}
}
