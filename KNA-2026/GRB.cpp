#include "GRB.h"
#include <stdio.h>
#define GRB_ERROR_SERIES 600

namespace GRB
{
#define NS(n) Rule::Chain::N(n)
#define TS(n) Rule::Chain::T(n)

    Rule::Chain::Chain(short psize, GRBALPHABET s, ...) : size(psize)
    {
        nt = new GRBALPHABET[size];
        int* p = (int*)&s;
        for (int i = 0; i < size; i++) {
            nt[i] = (GRBALPHABET) * (p++);
        }
    }

    char* Rule::Chain::getCChain(char* b) {
        for (int i = 0; i < size; i++)
            b[i] = Chain::alphabet_to_char(nt[i]);
        b[size] = '\0';
        return b;
    }

    Rule::Rule(GRBALPHABET pnn, int iderror, short psize, Chain c, ...) : nn(pnn), iderror(iderror), size(psize)
    {
        chains = new Chain[size];
        auto t = &c;
        for (int i = 0; i < size; i++) {
            chains[i] = *(t++);
        }
    }

    char* Rule::getCRule(char* b, short nchain)
    {
        char bchain[200];
        chains[nchain].getCChain(bchain);
        sprintf_s(b, 205, "%c->%s", Chain::alphabet_to_char(nn), bchain);
        return b;
    }

    short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
    {
        for (; j < size; j++) {
            if (chains[j].nt[0] == t) {
                pchain = chains[j];
                return j;
            }
        }
        return -1;
    }


    Greibach::Greibach(GRBALPHABET start, GRBALPHABET bottom, short psize, Rule r, ...) : startN(start), stbottomT(bottom), size(psize)
    {
        rules = new Rule[size];
        auto t = &r;
        for (int i = 0; i < size; i++) {
            rules[i] = *(t++);
        }
    }

    short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
    {
        short rc = -1;
        for (int i = 0; i < size; i++) {
            if (rules[i].nn == pnn) {
                prule = rules[i];
                rc = i;
                break;
            }
        }
        return rc;
    }

    Rule Greibach::getRule(short n) {
        Rule rc;
        if (n >= 0 && n < size) rc = rules[n];
        return rc;
    }

    Greibach greibach(NS('S'), TS('$'),     // стартовый символ, дно стека
        8,                                  // количество правил: S, N, R, V, E, W, F, M

        // S - программа: определения блоков
        Rule(NS('S'), GRB_ERROR_SERIES + 0, // Неверная структура программы
            14,
            // main
            Rule::Chain(8, TS('f'), TS('m'), TS('('), TS(')'), TS('>'), TS('t'), NS('R'), TS('e')),
            Rule::Chain(9, TS('f'), TS('m'), TS('('), TS(')'), TS('>'), TS('t'), NS('R'), TS('e'), NS('S')),
            // функция без параметров
            Rule::Chain(8, TS('f'), TS('i'), TS('('), TS(')'), TS('>'), TS('t'), NS('R'), TS('e')),
            Rule::Chain(9, TS('f'), TS('i'), TS('('), TS(')'), TS('>'), TS('t'), NS('R'), TS('e'), NS('S')),
            // функция с параметрами
            Rule::Chain(9, TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('>'), TS('t'), NS('R'), TS('e')),
            Rule::Chain(10, TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('>'), TS('t'), NS('R'), TS('e'), NS('S')),
            // процедура без параметров: пустое тело
            Rule::Chain(7, TS('f'), TS('i'), TS('('), TS(')'), TS('>'), TS('d'), TS('e')),
            Rule::Chain(8, TS('f'), TS('i'), TS('('), TS(')'), TS('>'), TS('d'), TS('e'), NS('S')),
            // процедура без параметров: с телом
            Rule::Chain(8, TS('f'), TS('i'), TS('('), TS(')'), TS('>'), TS('d'), NS('V'), TS('e')),
            Rule::Chain(9, TS('f'), TS('i'), TS('('), TS(')'), TS('>'), TS('d'), NS('V'), TS('e'), NS('S')),
            // процедура с параметрами: пустое тело
            Rule::Chain(8, TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('>'), TS('d'), TS('e')),
            Rule::Chain(9, TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('>'), TS('d'), TS('e'), NS('S')),
            // процедура с параметрами: с телом
            Rule::Chain(9, TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('>'), TS('d'), NS('V'), TS('e')),
            Rule::Chain(10, TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('>'), TS('d'), NS('V'), TS('e'), NS('S'))
        ),

        // N - инструкции внутри цикла (без ret)
        Rule(NS('N'), GRB_ERROR_SERIES + 1, // Ошибочный оператор
            22,
            // N -> i:t; | i:t=E; | i~E; | c i; | p E; | a:E N e | a:E e | a:E|i N e | a:E|i e | i(W); | i();
            //      и те же цепочки с продолжением N
            Rule::Chain(4, TS('i'), TS(':'), TS('t'), TS(';')),
            Rule::Chain(6, TS('i'), TS(':'), TS('t'), TS('='), NS('E'), TS(';')),
            Rule::Chain(4, TS('i'), TS('~'), NS('E'), TS(';')),
            Rule::Chain(3, TS('c'), TS('i'), TS(';')),
            Rule::Chain(3, TS('p'), NS('E'), TS(';')),
            Rule::Chain(5, TS('a'), TS(':'), NS('E'), NS('N'), TS('e')),
            Rule::Chain(4, TS('a'), TS(':'), NS('E'), TS('e')),
            Rule::Chain(7, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), NS('N'), TS('e')),
            Rule::Chain(6, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), TS('e')),
            Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), TS(';')),
            Rule::Chain(4, TS('i'), TS('('), TS(')'), TS(';')),

            Rule::Chain(5, TS('i'), TS(':'), TS('t'), TS(';'), NS('N')),
            Rule::Chain(7, TS('i'), TS(':'), TS('t'), TS('='), NS('E'), TS(';'), NS('N')),
            Rule::Chain(5, TS('i'), TS('~'), NS('E'), TS(';'), NS('N')),
            Rule::Chain(4, TS('c'), TS('i'), TS(';'), NS('N')),
            Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('N')),
            Rule::Chain(6, TS('a'), TS(':'), NS('E'), NS('N'), TS('e'), NS('N')),
            Rule::Chain(5, TS('a'), TS(':'), NS('E'), TS('e'), NS('N')),
            Rule::Chain(8, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), NS('N'), TS('e'), NS('N')),
            Rule::Chain(7, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), TS('e'), NS('N')),
            Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS(';'), NS('N')),
            Rule::Chain(5, TS('i'), TS('('), TS(')'), TS(';'), NS('N'))
        ),

        // R - тело функции: инструкции и обязательный ret E;
        Rule(NS('R'), GRB_ERROR_SERIES + 6, // Функция должна завершаться ret выражение;
            12,
            // R -> r E; | (инструкция) R
            Rule::Chain(3, TS('r'), NS('E'), TS(';')),

            Rule::Chain(5, TS('i'), TS(':'), TS('t'), TS(';'), NS('R')),
            Rule::Chain(7, TS('i'), TS(':'), TS('t'), TS('='), NS('E'), TS(';'), NS('R')),
            Rule::Chain(5, TS('i'), TS('~'), NS('E'), TS(';'), NS('R')),
            Rule::Chain(4, TS('c'), TS('i'), TS(';'), NS('R')),
            Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('R')),
            Rule::Chain(6, TS('a'), TS(':'), NS('E'), NS('N'), TS('e'), NS('R')),
            Rule::Chain(5, TS('a'), TS(':'), NS('E'), TS('e'), NS('R')),
            Rule::Chain(8, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), NS('N'), TS('e'), NS('R')),
            Rule::Chain(7, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), TS('e'), NS('R')),
            Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS(';'), NS('R')),
            Rule::Chain(5, TS('i'), TS('('), TS(')'), TS(';'), NS('R'))
        ),

        // V - тело процедуры: инструкции и необязательный ret;
        Rule(NS('V'), GRB_ERROR_SERIES + 7, // Ошибка в теле процедуры
            23,
            // V -> r; | (инструкция) | (инструкция) V
            Rule::Chain(2, TS('r'), TS(';')),

            Rule::Chain(4, TS('i'), TS(':'), TS('t'), TS(';')),
            Rule::Chain(6, TS('i'), TS(':'), TS('t'), TS('='), NS('E'), TS(';')),
            Rule::Chain(4, TS('i'), TS('~'), NS('E'), TS(';')),
            Rule::Chain(3, TS('c'), TS('i'), TS(';')),
            Rule::Chain(3, TS('p'), NS('E'), TS(';')),
            Rule::Chain(5, TS('a'), TS(':'), NS('E'), NS('N'), TS('e')),
            Rule::Chain(4, TS('a'), TS(':'), NS('E'), TS('e')),
            Rule::Chain(7, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), NS('N'), TS('e')),
            Rule::Chain(6, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), TS('e')),
            Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), TS(';')),
            Rule::Chain(4, TS('i'), TS('('), TS(')'), TS(';')),

            Rule::Chain(5, TS('i'), TS(':'), TS('t'), TS(';'), NS('V')),
            Rule::Chain(7, TS('i'), TS(':'), TS('t'), TS('='), NS('E'), TS(';'), NS('V')),
            Rule::Chain(5, TS('i'), TS('~'), NS('E'), TS(';'), NS('V')),
            Rule::Chain(4, TS('c'), TS('i'), TS(';'), NS('V')),
            Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('V')),
            Rule::Chain(6, TS('a'), TS(':'), NS('E'), NS('N'), TS('e'), NS('V')),
            Rule::Chain(5, TS('a'), TS(':'), NS('E'), TS('e'), NS('V')),
            Rule::Chain(8, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), NS('N'), TS('e'), NS('V')),
            Rule::Chain(7, TS('a'), TS(':'), NS('E'), TS('|'), TS('i'), TS('e'), NS('V')),
            Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS(';'), NS('V')),
            Rule::Chain(5, TS('i'), TS('('), TS(')'), TS(';'), NS('V'))
        ),

        // E - выражение
        Rule(NS('E'), GRB_ERROR_SERIES + 2, // Ошибка в выражении
            13,
            // E -> i | l | (E) | i(W) | i() | t(E) | vE | iM | lM | (E)M | i(W)M | i()M | t(E)M
            Rule::Chain(1, TS('i')),
            Rule::Chain(1, TS('l')),
            Rule::Chain(3, TS('('), NS('E'), TS(')')),
            Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),
            Rule::Chain(3, TS('i'), TS('('), TS(')')),
            Rule::Chain(4, TS('t'), TS('('), NS('E'), TS(')')),
            Rule::Chain(2, TS('v'), NS('E')),
            Rule::Chain(2, TS('i'), NS('M')),
            Rule::Chain(2, TS('l'), NS('M')),
            Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),
            Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), NS('M')),
            Rule::Chain(4, TS('i'), TS('('), TS(')'), NS('M')),
            Rule::Chain(5, TS('t'), TS('('), NS('E'), TS(')'), NS('M'))
        ),

        // W - аргументы вызова: выражение или выражение, W
        Rule(NS('W'), GRB_ERROR_SERIES + 4, // Ошибка в списке аргументов
            26,
            // W -> (любой вариант E) | (любой вариант E) , W
            Rule::Chain(1, TS('i')),
            Rule::Chain(1, TS('l')),
            Rule::Chain(3, TS('('), NS('E'), TS(')')),
            Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),
            Rule::Chain(3, TS('i'), TS('('), TS(')')),
            Rule::Chain(4, TS('t'), TS('('), NS('E'), TS(')')),
            Rule::Chain(2, TS('v'), NS('E')),
            Rule::Chain(2, TS('i'), NS('M')),
            Rule::Chain(2, TS('l'), NS('M')),
            Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),
            Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), NS('M')),
            Rule::Chain(4, TS('i'), TS('('), TS(')'), NS('M')),
            Rule::Chain(5, TS('t'), TS('('), NS('E'), TS(')'), NS('M')),

            Rule::Chain(3, TS('i'), TS(','), NS('W')),
            Rule::Chain(3, TS('l'), TS(','), NS('W')),
            Rule::Chain(5, TS('('), NS('E'), TS(')'), TS(','), NS('W')),
            Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS(','), NS('W')),
            Rule::Chain(5, TS('i'), TS('('), TS(')'), TS(','), NS('W')),
            Rule::Chain(6, TS('t'), TS('('), NS('E'), TS(')'), TS(','), NS('W')),
            Rule::Chain(4, TS('v'), NS('E'), TS(','), NS('W')),
            Rule::Chain(4, TS('i'), NS('M'), TS(','), NS('W')),
            Rule::Chain(4, TS('l'), NS('M'), TS(','), NS('W')),
            Rule::Chain(6, TS('('), NS('E'), TS(')'), NS('M'), TS(','), NS('W')),
            Rule::Chain(7, TS('i'), TS('('), NS('W'), TS(')'), NS('M'), TS(','), NS('W')),
            Rule::Chain(6, TS('i'), TS('('), TS(')'), NS('M'), TS(','), NS('W')),
            Rule::Chain(7, TS('t'), TS('('), NS('E'), TS(')'), NS('M'), TS(','), NS('W'))
        ),

        // F - параметры в заголовке
        Rule(NS('F'), GRB_ERROR_SERIES + 3, // Ошибка в списке параметров
            2, // F -> i:t | i:t,F
            Rule::Chain(3, TS('i'), TS(':'), TS('t')),
            Rule::Chain(5, TS('i'), TS(':'), TS('t'), TS(','), NS('F'))
        ),

        // M - продолжение выражения
        Rule(NS('M'), GRB_ERROR_SERIES + 5, // Ошибка в операции
            1, // M -> vE
            Rule::Chain(2, TS('v'), NS('E'))
        )
    );

    Greibach getGreibach() {
        return greibach;
    }
};