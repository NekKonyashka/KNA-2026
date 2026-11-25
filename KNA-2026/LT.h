#pragma once
#define LEXEMA_FIXSIZE 1
#define LT_MAXSIZE 4096
#define LT_TI_NULLIDX 0xffffffff
#define LEX_VOID 'd'
#define LEX_NUMBER 't'
#define LEX_SMNUMBER 't'
#define LEX_TEXT 't'
#define LEX_ID 'i'
#define LEX_LITERAL 'l'
#define LEX_FUNCTION 'f'
#define LEX_RETURN 'r'
#define LEX_PROJECT 'p'
#define LEX_CAPTURE 'c'
#define LEX_SEMICOLON ';'
#define LEX_COMMA ','
#define LEX_LEFTHESIS '('
#define LEX_RIGHTHESIS ')'
#define LEX_PLUS 'v'
#define LEX_MINUS 'v'
#define LEX_STAR 'v'
#define LEX_MAIN 'm'
#define LEX_DIRSLASH 'v'
#define LEX_EQUALS '='
#define LEX_SHARP 'v'
#define LEX_COLON ':'
#define LEX_ARROW '>'
#define LEX_END 'e'
#define LEX_REPEAT 'a'
#define LEX_AS '|'
#define LEX_ASSIGN '~'
#define LEX_FILL '0'
#define LEX_AT '@'

namespace LT
{
    struct Entry
    {
        char lexema;
        int sn;               
        int col;
        unsigned int idxTi;
        char op;
        int params;           
        
        Entry() { lexema = '\0'; sn = -1; idxTi = LT_TI_NULLIDX; op = LT_TI_NULLIDX; params = LT_TI_NULLIDX; col = -1; };
        Entry(char l, int sn, unsigned int idxti = LT_TI_NULLIDX, char op = '\0', int params = -1, int col = -1);
    };

    struct LexTable
    {
        int maxsize;
        int size;
        Entry* table;
    };

    LexTable Create(int size);
    void Add(LexTable& lextable, Entry entry);
    Entry GetEntry(LexTable& lextable, int n);
    void Delete(LexTable& lextable);
};