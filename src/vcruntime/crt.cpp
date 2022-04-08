#include <stdio.h>

EXTERN_C_START

_Success_(return != EOF)
_Check_return_opt_
_ACRTIMP int __cdecl fputs(
    _In_z_  char const* _Buffer,
    _Inout_ FILE*       _Stream
    )
{
    UNREFERENCED_PARAMETER(_Buffer);
    UNREFERENCED_PARAMETER(_Stream);

    // TODO

    return EOF;
}

_Success_(return != EOF)
_Check_return_opt_
_ACRTIMP int __cdecl fputc(
    _In_    int   _Character,
    _Inout_ FILE* _Stream
    )
{
    UNREFERENCED_PARAMETER(_Character);
    UNREFERENCED_PARAMETER(_Stream);

    // TODO

    return EOF;
}
EXTERN_C_END



#include <windows.h>

EXTERN_C_START

BOOL __cdecl __crtQueueUserWorkItem(__in LPTHREAD_START_ROUTINE function, __in_opt PVOID context, __in ULONG flags)
{
    return QueueUserWorkItem(function, context, flags);
}

EXTERN_C_END