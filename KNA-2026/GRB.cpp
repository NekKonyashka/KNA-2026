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
        11,                                 // количество правил: S, N, R, V, A, L, C, E, W, F, M

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

        // Оставшиеся части инструкций вынесены в отдельные нетерминалы A, L, C: при ошибке внутри инструкции
        // диагностика указывает на них (601 - ошибочный оператор), а не на N, R или V, где инструкция лишь стоит.
        // Первым символом каждой цепочки остаётся терминал, как требует нормальная форма Грейбах.

        // N - инструкции внутри цикла (без ret)
        Rule(NS('N'), GRB_ERROR_SERIES + 1, // Ошибочный оператор
            8,
            // N -> i A | c C | p E ; | a L | i A N | c C N | p E ; N | a L N
            Rule::Chain(2, TS('i'), NS('A')),
            Rule::Chain(2, TS('c'), NS('C')),
            Rule::Chain(3, TS('p'), NS('E'), TS(';')),
            Rule::Chain(2, TS('a'), NS('L')),

            Rule::Chain(3, TS('i'), NS('A'), NS('N')),
            Rule::Chain(3, TS('c'), NS('C'), NS('N')),
            Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('N')),
            Rule::Chain(3, TS('a'), NS('L'), NS('N'))
        ),

        // R - тело функции: инструкции и обязательный ret E;
        Rule(NS('R'), GRB_ERROR_SERIES + 6, // Функция должна завершаться ret
            5,
            // R -> r E ; | i A R | c C R | p E ; R | a L R
            Rule::Chain(3, TS('r'), NS('E'), TS(';')),

            Rule::Chain(3, TS('i'), NS('A'), NS('R')),
            Rule::Chain(3, TS('c'), NS('C'), NS('R')),
            Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('R')),
            Rule::Chain(3, TS('a'), NS('L'), NS('R'))
        ),

        // V - тело процедуры: инструкции и необязательный ret;
        Rule(NS('V'), GRB_ERROR_SERIES + 7, // Ошибка в теле процедуры
            9,
            // V -> r ; | i A | c C | p E ; | a L | i A V | c C V | p E ; V | a L V
            Rule::Chain(2, TS('r'), TS(';')),

            Rule::Chain(2, TS('i'), NS('A')),
            Rule::Chain(2, TS('c'), NS('C')),
            Rule::Chain(3, TS('p'), NS('E'), TS(';')),
            Rule::Chain(2, TS('a'), NS('L')),

            Rule::Chain(3, TS('i'), NS('A'), NS('V')),
            Rule::Chain(3, TS('c'), NS('C'), NS('V')),
            Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('V')),
            Rule::Chain(3, TS('a'), NS('L'), NS('V'))
        ),

        // A - инструкция после идентификатора: объявление, присваивание, вызов блока
        Rule(NS('A'), GRB_ERROR_SERIES + 1, // Ошибочный оператор
            5,
            // A -> :t; | :t=E; | ~E; | (W); | ();
            Rule::Chain(3, TS(':'), TS('t'), TS(';')),
            Rule::Chain(5, TS(':'), TS('t'), TS('='), NS('E'), TS(';')),
            Rule::Chain(3, TS('~'), NS('E'), TS(';')),
            Rule::Chain(4, TS('('), NS('W'), TS(')'), TS(';')),
            Rule::Chain(3, TS('('), TS(')'), TS(';'))
        ),

        // L - цикл после ключевого слова a: число повторений, необязательный счётчик, тело
        Rule(NS('L'), GRB_ERROR_SERIES + 1, // Ошибочный оператор
            4,
            // L -> :E N e | :E e | :E|i N e | :E|i e
            Rule::Chain(4, TS(':'), NS('E'), NS('N'), TS('e')),
            Rule::Chain(3, TS(':'), NS('E'), TS('e')),
            Rule::Chain(6, TS(':'), NS('E'), TS('|'), TS('i'), NS('N'), TS('e')),
            Rule::Chain(5, TS(':'), NS('E'), TS('|'), TS('i'), TS('e'))
        ),

        // C - capture после ключевого слова c
        Rule(NS('C'), GRB_ERROR_SERIES + 1, // Ошибочный оператор
            1, // C -> i ;
            Rule::Chain(2, TS('i'), TS(';'))
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