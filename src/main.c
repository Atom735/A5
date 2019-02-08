#include <windows.h>

/* информация о системе */
SYSTEM_INFO g_SystemInfo    = { };

/* Точка входа в приложение */
INT APIENTRY wWinMain (
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine,
    INT nShowCmd )
{
    GetSystemInfo ( &g_SystemInfo );
    return 0;
}
