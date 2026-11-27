#pragma once
#include <string>
#include <vector>
#define ID_MAXSIZE		32
#define TI_MAXSIZE		4096
#define TI_NUMBER_DEFAULT	0x00000000
#define TI_TEXT_DEFAULT	0x00
#define TI_NULLIDX		0xffffffff
#define TI_TEXT_MAXSIZE  255
#define TI_MAXPARAMS	8	
#define TI_NUMBER_MAXSIZE 2147483647
#define TI_SMNUMBER_MAXSIZE 32767

namespace IT
{
	enum IDDATATYPE { UNDEF = 0, NUM = 1, TXT = 2, SMNUM = 3, VOID = 4 };
	enum IDTYPE { V = 1, F = 2, P = 3, L = 4 };

	struct Entry
	{
		int idxfirstLE;
		std::string id;
		IDDATATYPE iddatatype;
		IDTYPE	idtype;
		std::string scope;
		union
		{
			struct {
				int val = TI_NUMBER_DEFAULT;
				bool overflow = false;
			}vint = {};
			struct
			{
				unsigned char len;
				char str[TI_TEXT_MAXSIZE + 1];
			} vstr;
			struct
			{
				int paramCount;
				IDDATATYPE types[TI_MAXPARAMS];
			} vfunc;
		} value;
	};

	struct IdTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	IdTable Create(int size);

	void Add(IdTable& idtable, Entry entry);

	Entry& GetEntry(IdTable& idtable, int n);

	int IsId(IdTable& idtable, std::string id, std::string scope);
	int FindVisible(IdTable& it, std::vector<std::string>& scopes, std::string name);

	void AddLibrary(IdTable& idtable);

	void Delete(IdTable& idtable);

};
