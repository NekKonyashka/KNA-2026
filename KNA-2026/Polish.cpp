#include "Polish.h"
#include <stack>
#include <vector>

namespace POLISH {
    static int GetPrioritet(char op) {
        switch (op) {
        case '/':
        case '*': 
        case '#': 
            return 3;
        case '+':
        case '-':
            return 2;
        default:
            return 0;
        }
    }
    static int Prio(LT::Entry& e) {
        return e.params == 1 ? 4 : GetPrioritet(e.op); 
    }

    static bool IsCallStmtStart(LT::LexTable& lt, int i) {
        int p = i - 1;
        while (p >= 0 && lt.table[p].lexema == LEX_FILL) {
            p--;
        }
        if (p < 0) return false;

        char c = lt.table[p].lexema;
        return c == LEX_SEMICOLON || c == LEX_END || c == LEX_NUMBER || c == LEX_VOID;
    }

    bool CheckExpressions(LT::LexTable& lt, IT::IdTable& it) {
        for (int i = 0; i < lt.size; i++) {
            char l = lt.table[i].lexema;
            int start = -1;
            if (l == LEX_EQUALS || l == LEX_ASSIGN || l == LEX_PROJECT || l == LEX_RETURN) start = i + 1;
            else if (l == LEX_ID && i + 1 < lt.size && lt.table[i + 1].lexema == LEX_LEFTHESIS && IsCallStmtStart(lt, i))
                start = i;
            if (start >= 0 && !PolishNotation(start, lt, it)) return false;
        }
        return true;
    }

    bool PolishNotation(int lex_pos, LT::LexTable& lt, IT::IdTable& it) {
        std::stack<LT::Entry> st;
        std::stack<int> argc;
        std::vector<LT::Entry> out;

        int i = lex_pos;
        while (lt.table[i].lexema != LEX_SEMICOLON) {
            LT::Entry e = lt.table[i];
            switch (e.lexema) {
            case LEX_ID:
            case LEX_LITERAL: {
                if (lt.table[i + 1].lexema == LEX_LEFTHESIS) {
                    st.push(e);
                    argc.push(lt.table[i + 2].lexema == LEX_RIGHTHESIS ? 0 : 1);
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
                    return false;
                }
                argc.top()++;
                break;
            }
            case LEX_PLUS: {
                LT::Entry v = e;
                char pl = (i > lex_pos) ? lt.table[i - 1].lexema : '\0';
                bool unary = (i == lex_pos) || pl == LEX_LEFTHESIS || pl == LEX_COMMA || pl == LEX_PLUS;
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
                    return false;
                }
                st.pop();
                if (!st.empty() && st.top().lexema == LEX_ID) {
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
                return false;
            }
            }
            i++;
        }
        while (!st.empty()) {
            if (st.top().lexema == LEX_LEFTHESIS) {
                return false;
            }
            out.push_back(st.top());
            st.pop();
        }
        out.push_back(lt.table[i]);
        int k = lex_pos;
        for (auto& e : out) {
            lt.table[k++] = e;
        }
        while (k <= i) {
            lt.table[k].idxTi = LT_TI_NULLIDX;
            lt.table[k++].lexema = LEX_FILL;
        }
        return true;
    }
}