#include "stdafx.h"
using namespace std;

//#define DO_TEST


int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL, "rus");
#ifdef DO_TEST
    TEST::DoTests("../../tests");
#else
    vector<In::WORD> words;
    Parm::PARM parm;
    Log::LOG log = { L"",NULL };
    Out::OUT out = { L"",NULL };

    try {
        parm = Parm::getparm(argc, argv);
        In::IN in = In::getin(parm.in, words);
        log = Log::getlog(parm.log);
        LT::LexTable lextab = LT::Create(LT_MAXSIZE - 1);

        IT::IdTable idtab = IT::Create(TI_MAXSIZE - 1);
        IT::AddLibrary(idtab);                     

        LEX::lexer lex = LEX::LexAnaliz(lextab, idtab, words);
        Log::WriteLexTable(log, lextab);

        out = Out::getout(parm.out);
        Out::WriteToFile(out, in);
        MFST::Mfst mfst = MFST::Mfst(lex, GRB::getGreibach());
        if (mfst.start()) {
            mfst.savededucation();
            mfst.printrules();

            SEM::Diagnostics diag;
            SEM::CollectSignatures(lextab, idtab, diag);
            SEM::CheckRepeated(lextab, idtab, diag);
            SEM::CheckUsage(lextab, idtab, diag);        

            if (POLISH::CheckExpressions(lextab, idtab)) {
                cout << "Польская запись для всех выражений построена\n";
                SEM::CheckLeftOperands(lextab, idtab, diag);
                SEM::CheckFunctionCalls(lextab, idtab, diag);
                SEM::CheckTypes(lextab, idtab, diag);
                SEM::CheckWarnings(lextab, idtab, diag);
            }
            else {
                diag.errors.push_back(Error::geterror(602));
            }
            SEM::Finish(diag);

            Log::WriteIdTable(log, idtab);              
            for (auto& w : diag.warnings) {
                Log::WriteWarning(log, w);
            }
            for (auto& e : diag.errors) {
                Log::WriteError(log, e);
            }
            if (diag.errors.empty()) {
                cout << "Семантический анализ завершён без ошибок\n";
            }
            else {
                cout << "Семантическая ошибка\n";
                Out::WriteToError(out, diag.errors[0]);
            }
        }
    }
    catch (Error::ERROR e) {
        cout << endl;
        Log::WriteError(log, e);
        out = Out::getout(parm.out);
        Out::WriteToError(out, e);
    }
#endif
    system("pause");
    return 0;
}
