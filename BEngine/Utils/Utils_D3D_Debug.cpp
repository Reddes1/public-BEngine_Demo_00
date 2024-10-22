#pragma once
#include "Utils_D3D_Debug.h"
#include "Utils_General.h"

#include <fstream>          //File Loading
#include <comdef.h>         //_com_error, ::Wide..
#include <codecvt>          //For wstring conversion

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
    :
    ErrorCode(hr),
    FunctionName(functionName),
    Filename(filename),
    LineNumber(lineNumber)
{
}

std::wstring DxException::ToString()const
{
    // Get the string description of the error code.
    _com_error err(ErrorCode);
    std::string msg = err.ErrorMessage();
    std::wstring newMsg = StringtoWString(msg);
    return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + newMsg;
}



