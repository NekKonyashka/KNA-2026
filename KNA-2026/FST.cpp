#include "FST.h"
#include <string.h>
#include <stdio.h>
namespace FST {
    RELATION::RELATION(char c, short ns) :
        symbol(c),
        nnode(ns)
    {
    }

    NODE::NODE() :
        n_relation(0),
        relations(nullptr)
    {
    }
    NODE::NODE(short n, RELATION rel, ...) :
        n_relation(n)
    {
        relations = new RELATION[n];
        relations[0] = rel;
        RELATION* ptr = &rel;
        for (int i = 1; i < n; ++i) {
            relations[i] = *(++ptr);
        }
    }

    FST::FST(std::string s, short ns, NODE n, ...) :
        string(s),
        nstates(ns),
        position(0)
    {
        nodes = new NODE[ns];
        nodes[0] = n;
        NODE* ptr = &n;
        for (int i = 1; i < ns; i++) {
            nodes[i] = *(++ptr);
        }

        rstates = new short[nstates];
    }

    bool step(FST& fst, short*& rstates)
    {
        bool rc = false;

        for (short i = 0; i < fst.nstates; i++)
            fst.rstates[i] = -1;

        for (short i = 0; i < fst.nstates; i++)
        {
            if (rstates[i] != -1)
                for (short j = 0; j < fst.nodes[i].n_relation; j++)
                    if (fst.nodes[i].relations[j].symbol == fst.string[fst.position])
                    {
                        fst.rstates[fst.nodes[i].relations[j].nnode] = fst.position + 1;
                        rc = true;
                    }
        }

        std::swap(rstates, fst.rstates);

        return rc;
    }

    bool execute(FST& fst)
    {
        short* rstates = new short[fst.nstates];
        for (short i = 0; i < fst.nstates; i++)
            rstates[i] = -1;
        rstates[0] = 0;

        short lstring = fst.string.length();
        bool rc = true;

        for (short i = 0; i < lstring && rc; i++)
        {
            rc = step(fst, rstates);
            fst.position++;
        }

        bool accepted = rc ? (rstates[fst.nstates - 1] == lstring) : false;

        fst.string.clear();

        return accepted;
    }
}