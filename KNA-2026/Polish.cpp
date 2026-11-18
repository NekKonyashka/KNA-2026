#include "Polish.h"
#include <stack>
#include <vector>

namespace POLISH {
    static int GetPrioritet(char op) {
        switch (op) {
        case '/':
        case '*':
        case '#': {
            return 3;
        }
        case '+':
        case '-': {
            return 2;
        }
        default: {
            return 0;
        }
        }
    }

   
    static int Prio(const LT::Entry& e) {
        return e.params == 1 ? 4 : GetPrioritet(e.op);
    }

    static int FindSemicolon(LT::LexTable& lt, int from) {
        for (int i = from; i < lt.size; i++) {
            if (lt.table[i].lexema == LEX_SEMICOLON) {
                return i;
            }
        }
        return lt.size;
    }


    static int SkipParens(LT::LexTable& lt, int i) {
        int depth = 0;
        for (; i < lt.size; i++) {
            if (lt.table[i].lexema == LEX_LEFTHESIS) {
                depth++;
            }
            else if (lt.table[i].lexema == LEX_RIGHTHESIS && --depth == 0) {
                return i + 1;
            }
        }
        return -1;
    }

    static int ExprEnd(LT::LexTable& lt, int i) {
        for (;;) {
            while (i < lt.size && lt.table[i].lexema == LEX_PLUS) {         
                i++;
            }
            if (i >= lt.size) {
                return -1;
            }
            char c = lt.table[i].lexema;
            if (c == LEX_ID && i + 1 < lt.size && lt.table[i + 1].lexema == LEX_LEFTHESIS) {
                i = SkipParens(lt, i + 1);                               
            }
            else if (c == LEX_LEFTHESIS) {
                i = SkipParens(lt, i);                                 
            }
            else if (c == LEX_ID || c == LEX_LITERAL) {
                i++;
            }
            else {
                return -1;
            }
            if (i < 0) {
                return -1;
            }
            if (i < lt.size && lt.table[i].lexema == LEX_PLUS) {         
                i++;
                continue;
            }
            return i;
        }
    }


    static int Convert(LT::LexTable& lt, int from, int to, bool terminated) {
        std::stack<LT::Entry> st;
        std::stack<int> argc;
        std::vector<LT::Entry> out;

        for (int i = from; i < to; i++) {
            LT::Entry e = lt.table[i];
            switch (e.lexema) {
            case LEX_ID:
            case LEX_LITERAL: {
                if (e.lexema == LEX_ID && i + 1 < to && lt.table[i + 1].lexema == LEX_LEFTHESIS) {
                    st.push(e);                                           
                    argc.push(i + 2 < to && lt.table[i + 2].lexema == LEX_RIGHTHESIS ? 0 : 1);
                }
                else {
                    out.push_back(e);
                }
                break;
            }
            case LEX_LEFTHESIS: {
                st.push(e);
                break;
            }
            case LEX_COMMA: {
                while (!st.empty() && st.top().lexema != LEX_LEFTHESIS) {
                    out.push_back(st.top());
                    st.pop();
                }
                if (st.empty() || argc.empty()) {
                    return -1;
                }
                argc.top()++;
                break;
            }
            case LEX_PLUS: {
                LT::Entry v = e;
                char prev = i > from ? lt.table[i - 1].lexema : '\0';
                bool unary = i == from || prev == LEX_LEFTHESIS || prev == LEX_COMMA || prev == LEX_PLUS;
                if (unary) {
                    v.params = 1;                                           
                    st.push(v);
                    break;
                }
                v.params = -1;
                int cur = GetPrioritet(v.op);
                while (!st.empty() && st.top().lexema == LEX_PLUS && Prio(st.top()) >= cur) {
                    out.push_back(st.top());
                    st.pop();
                }
                st.push(v);
                break;
            }
            case LEX_RIGHTHESIS: {
                while (!st.empty() && st.top().lexema != LEX_LEFTHESIS) {
                    out.push_back(st.top());
                    st.pop();
                }
                if (st.empty()) {
                    return -1;
                }
                st.pop();
                if (!st.empty() && st.top().lexema == LEX_ID) {         
                    if (argc.empty()) {
                        return -1;
                    }
                    out.push_back(st.top());
                    st.pop();
                    LT::Entry at = e;
                    at.lexema = LEX_AT;
                    at.params = argc.top();
                    argc.pop();
                    out.push_back(at);
                }
                break;
            }
            default: {
                return -1;
            }
            }
        }
        while (!st.empty()) {
            if (st.top().lexema == LEX_LEFTHESIS) {
                return -1;
            }
            out.push_back(st.top());
            st.pop();
        }

        LT::Entry semicolon = terminated ? lt.table[to] : LT::Entry();
        int k = from;
        for (auto& e : out) {
            lt.table[k++] = e;
        }
        int len = (int)out.size();
        if (terminated) {
            lt.table[k++] = semicolon;
        }
        while (k < to || (terminated && k <= to)) {
            lt.table[k].idxTi = LT_TI_NULLIDX;
            lt.table[k++].lexema = LEX_FILL;
        }
        return len;
    }

    bool PolishNotation(int lex_pos, LT::LexTable& lt, IT::IdTable& it) {
        int semi = FindSemicolon(lt, lex_pos);
        return semi < lt.size && Convert(lt, lex_pos, semi, true) >= 0;
    }

    bool CheckExpressions(LT::LexTable& lt, IT::IdTable& it) {
        for (int i = 0; i < lt.size; i++) {
            switch (lt.table[i].lexema) {
            case LEX_EQUALS:        // i : t = E ;
            case LEX_ASSIGN:        // i ~ E ;
            case LEX_PROJECT:       // p E ;
            case LEX_RETURN: {      // r E ;
                int semi = FindSemicolon(lt, i + 1);
                if (semi >= lt.size || Convert(lt, i + 1, semi, true) < 0) {
                    return false;
                }
                i = semi;
                break;
            }
            case LEX_REPEAT: {     
                int from = i + 2;
                int to = ExprEnd(lt, from);
                if (to < 0) {
                    return false;
                }
                int len = Convert(lt, from, to, false);
                if (len < 0) {
                    return false;
                }
                lt.table[i].params = len;
                i = to - 1;
                break;
            }
            case LEX_ID: {          
                bool call = i + 1 < lt.size && lt.table[i + 1].lexema == LEX_LEFTHESIS;
                bool header = i > 0 && lt.table[i - 1].lexema == LEX_FUNCTION;
                if (call && !header) {
                    int semi = FindSemicolon(lt, i);
                    int len = semi < lt.size ? Convert(lt, i, semi, true) : -1;
                    if (len < 0) {
                        return false;
                    }
                    lt.table[i + len].params = i;     
                    i = semi;
                }
                break;
            }
            default: {
                break;
            }
            }
        }
        return true;
    }
}
