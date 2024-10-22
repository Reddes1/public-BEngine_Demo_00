#include <windows.h>            //WinMain
#include <crtdbg.h>             //Debugging
#define _CRTDBG_MAP_ALLOC

#include "Game.h"
#include "Utils/Utils_D3D_Debug.h"
#include "Utils/Utils_General.h"

//Lets go make a game lad!
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

    //Start game app
    try
    {
        Game game(hInstance);
        if (!game.InitialiseGame())
            return 0;
        return game.RunApp();
    }
    //Failed to start game at all, run exception
    catch (DxException& e)
    {
        LPCSTR str = "HR Failed";
        std::string utf8String = WStringToString(e.ToString());
        LPCSTR lpStr = utf8String.c_str();
        MessageBox(nullptr, lpStr, str, MB_OK);
        return 0;
    }

    return 0;
}
