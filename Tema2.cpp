//Problema care citeste toate dispozitivele conectate la porturi USB

#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

void ListUSBDevices() {
   // Initialize COM library
   HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
   if (FAILED(hres)) {
       std::cerr << "COM initialization failed" << std::endl;
       return;
   }

   // Set up security
   hres = CoInitializeSecurity(
       NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
       NULL, EOAC_NONE, NULL);
   if (FAILED(hres)) {
       std::cerr << "Failed to initialize security" << std::endl;
       CoUninitialize();
       return;
   }

   // Connect to WMI
   IWbemLocator* pLoc = NULL;
   IWbemServices* pSvc = NULL;

   hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
   if (FAILED(hres)) {
       std::cerr << "Failed to create IWbemLocator object" << std::endl;
       CoUninitialize();
       return;
   }

   // Connect to the WMI service
   hres = pLoc->ConnectServer(
       BSTR(L"ROOT\\CIMV2"), // WMI namespace
       NULL, NULL, 0, NULL, 0, 0, &pSvc);
   if (FAILED(hres)) {
       std::cerr << "Failed to connect to WMI" << std::endl;
       pLoc->Release();
       CoUninitialize();
       return;
   }

   // Set security levels
   hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
   if (FAILED(hres)) {
       std::cerr << "Failed to set security levels" << std::endl;
       pSvc->Release();
       pLoc->Release();
       CoUninitialize();
       return;
   }

   // Query USB devices through WMI
   IEnumWbemClassObject* pEnumerator = NULL;
   hres = pSvc->ExecQuery(
       BSTR(L"WQL"),
       BSTR(L"SELECT * FROM Win32_USBHub"),
       WBEM_FLAG_FORWARD_ONLY, NULL, &pEnumerator);
   if (FAILED(hres)) {
       std::cerr << "Query for USB hubs failed" << std::endl;
       pSvc->Release();
       pLoc->Release();
       CoUninitialize();
       return;
   }

   // Retrieve and display the results
   IWbemClassObject* pClassObject;
   ULONG uReturn = 0;

   while (pEnumerator) {
       hres = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn);
       if (0 == uReturn) {
           break;
       }

       VARIANT vtDeviceID;
       hres = pClassObject->Get(L"DeviceID", 0, &vtDeviceID, 0, 0);
       if (SUCCEEDED(hres)) {
           std::wcout << L"Device ID: " << vtDeviceID.bstrVal << std::endl;
           VariantClear(&vtDeviceID);
       }

       pClassObject->Release();
   }

   // Clean up
   pEnumerator->Release();
   pSvc->Release();
   pLoc->Release();
   CoUninitialize();
}

int main() {
   ListUSBDevices();
   return 0;
}