#include"stdafx.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL, "rus");
    vector<pair<string, int>> words;
    Parm::PARM parm;
    Log::LOG log = { L"",NULL };
    Out::OUT out = { L"",NULL };

    try {
        parm = Parm::getparm(argc, argv);
        In::IN in = In::getin(parm.in, words);
        log = Log::getlog(parm.log);
        LT::LexTable lextab = LT::Create(LT_MAXSIZE - 1);
        IT::IdTable idtab = IT::Create(TI_MAXSIZE - 1);
        //IT::IdTable idtab = IT::Create(0);
        IT::AddLibrary(idtab);
        LEX::lexer lex = LEX::LexAnaliz(lextab, idtab, words);
        Log::WriteLexTable(log, lextab);
        Log::WriteIdTable(log, idtab);

        out = Out::getout(parm.out);
        Out::WriteToFile(out, in);
        MFST::Mfst mfst = MFST::Mfst(lex, GRB::getGreibach());
        if (!mfst.start()) {
            return -1;
        }
        mfst.savededucation();
        mfst.printrules();

        SEM::CollectSignatures(lextab, idtab);
        SEM::CheckRepeated(lextab, idtab);
        SEM::CheckUsage(lextab, idtab);          

        if (POLISH::CheckExpressions(lextab, idtab)) {
            cout << "Польская запись для всех выражений построена\n";
            Log::WriteLexTable(log, lextab);
        }

        SEM::CheckLeftOperands(lextab, idtab);
        SEM::CheckFunctionCalls(lextab, idtab);
        SEM::CheckTypes(lextab, idtab);
    }
    catch (Error::ERROR e) {
        cout << "Ошибка " << e.id << ": " << e.messages << endl << endl;
        Log::WriteError(log, e);
        out = Out::getout(parm.out);
        Out::WriteToError(out, e);
    }

    system("pause");
    return 0;
}