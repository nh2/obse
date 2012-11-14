#include "obse.h"
#include <Windows.h>

BOOL WINAPI DllMain(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        )
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			OBSE_Initialize();
			break;

		case DLL_PROCESS_DETACH:
			OBSE_DeInitialize();
			break;
	};

	return TRUE;
}
