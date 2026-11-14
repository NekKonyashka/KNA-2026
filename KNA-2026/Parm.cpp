#include "Parm.h"
#include "Error.h"
#include <tchar.h>

namespace Parm {
	PARM getparm(int argc, _TCHAR* argv[]) {
		PARM parm;
		parm.in[0] = '\0';
		parm.out[0] = '\0';
		parm.log[0] = '\0';
		wchar_t* in_ptr;
		for (short i = 1; i < argc; ++i) {
			if (wcslen(argv[i]) > PARM_MAX_SIZE) {
				ERROR_THROW(104);
			}
			if (wcsncmp(argv[i], PARM_IN, wcslen(PARM_IN)) == 0) {
				wcscpy_s(parm.in, PARM_MAX_SIZE, argv[i] + wcslen(PARM_IN));
			}
			else if (wcsncmp(argv[i], PARM_OUT, wcslen(PARM_OUT)) == 0) {
				wcscpy_s(parm.out, PARM_MAX_SIZE, argv[i] + wcslen(PARM_OUT));
			}
			else if (wcsncmp(argv[i], PARM_LOG, wcslen(PARM_LOG)) == 0) {
				wcscpy_s(parm.log, PARM_MAX_SIZE, argv[i] + wcslen(PARM_LOG));
			}
		}
		if (wcslen(parm.in) == 0) {
			ERROR_THROW(100);
		}
		if (wcslen(parm.out) == 0) {
			wcscpy_s(parm.out, PARM_MAX_SIZE, parm.in);
			wcscat_s(parm.out, PARM_MAX_SIZE, PARM_OUT_DEFAULT_EXT);
		}
		if (wcslen(parm.log) == 0) {
			wcscpy_s(parm.log, PARM_MAX_SIZE, parm.in);
			wcscat_s(parm.log, PARM_MAX_SIZE, PARM_LOG_DEFAULT_EXT);
		}

		return parm;
	}
}

