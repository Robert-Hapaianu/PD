#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryKey(HKEY hKey)
{
    TCHAR achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD cbName = 0;               // size of name string 
    FILETIME ftLastWriteTime;       // last write time 

    DWORD i = 0, retCode = 0;

    // Get the number of subkeys
    DWORD cSubKeys = 0;
    RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    printf("\nNumber of services: %d\n\n", cSubKeys);

    // Loop through each service (subkey)
    for (i = 0; i < cSubKeys; i++)
    {
        cbName = MAX_KEY_LENGTH;
        retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);

        if (retCode == ERROR_SUCCESS)
        {
            _tprintf(TEXT("Service Name: %s\n"), achKey);

            // Open the service subkey
            HKEY hServiceKey;
            if (RegOpenKeyEx(hKey, achKey, 0, KEY_READ, &hServiceKey) == ERROR_SUCCESS)
            {
                // Read the ImagePath value
                TCHAR imagePath[MAX_PATH];
                DWORD imagePathSize = sizeof(imagePath);
                DWORD type;

                if (RegQueryValueEx(hServiceKey, TEXT("ImagePath"), NULL, &type, (LPBYTE)imagePath, &imagePathSize) == ERROR_SUCCESS)
                {
                    _tprintf(TEXT("  ImagePath: %s\n\n"), imagePath);
                }
                else
                {
                    _tprintf(TEXT("  ImagePath: [Not Found]\n\n"));
                }

                RegCloseKey(hServiceKey);
            }
        }
    }
}

int __cdecl _tmain()
{
    HKEY hTestKey = 0;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("SYSTEM\\CurrentControlSet\\Services"),
        0,
        KEY_READ,
        &hTestKey) == ERROR_SUCCESS)
    {
        QueryKey(hTestKey);
        RegCloseKey(hTestKey);
    }

    return 0;
}
