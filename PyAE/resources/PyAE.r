// PyAE.r
// PyAE - Python for After Effects
// PiPL Resource Definition (Rezk format)
//
// This file is processed by Adobe PiPL compiler (PiPLTool)
// Windows: PiPLTool.exe PyAE.r PyAE.rr
// Mac: PiPLTool PyAE.r PyAE.rsrc

#include "AEConfig.h"

#ifndef AE_OS_WIN
	#include "AE_General.r"
#endif

resource 'PiPL' (16000) {
	{	/* array properties: 5 elements */
		/* [1] */
		Kind {
			AEGP
		},
		/* [2] */
		Name {
			"PyAE"
		},
		/* [3] */
		Category {
			"General Plugin"
		},
		/* [4] */
		Version {
			65536
		},
		/* [5] */
#ifdef AE_OS_WIN
    #if defined(AE_PROC_INTELx64)
		CodeWin64X86 {"EntryPointFunc"},
    #elif defined(AE_PROC_ARM64)
		CodeWinARM64 {"EntryPointFunc"},
    #endif
#elif defined(AE_OS_MAC)
		CodeMacIntel64 {"EntryPointFunc"},
		CodeMacARM64 {"EntryPointFunc"},
#endif
	}
};
