#ifndef _PEUTILS_H_
#define _PEUTILS_H_

#include <windows.h>

DWORD RVAToOffset(DWORD dwRVA, LPVOID lpMapping);
DWORD RVAToMappedOffset(DWORD dwRVA, LPVOID lpMapping);
DWORD RVAToSectionRawDataSize(DWORD dwRVA, LPVOID lpMapping);
DWORD RVAToSectionVirtualAddress(DWORD dwRVA, LPVOID lpMapping);
DWORD OffsetToRVA(DWORD dwOffset, LPVOID lpMapping);
DWORD MappedOffsetToRVA(DWORD dwOffset, LPVOID lpMapping);
char* RVAToSectionName(DWORD dwRVA, LPVOID lpMapping);

#endif _PEUTILS_H_