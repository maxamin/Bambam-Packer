#include "peutils.h"

DWORD RVAToOffset(DWORD dwRVA, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if((pSecHdr[dw].VirtualAddress <= dwRVA) && (pSecHdr[dw].VirtualAddress + pSecHdr[dw].SizeOfRawData > dwRVA))
		{
			return (pSecHdr[dw].PointerToRawData + (dwRVA - pSecHdr[dw].VirtualAddress));
		}
	}
	
	return 0;
}

DWORD RVAToMappedOffset(DWORD dwRVA, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if((pSecHdr[dw].VirtualAddress <= dwRVA) && (pSecHdr[dw].VirtualAddress + pSecHdr[dw].SizeOfRawData > dwRVA))
		{
			
			return (pSecHdr[dw].PointerToRawData + (dwRVA - pSecHdr[dw].VirtualAddress) + (DWORD)lpMapping);
		}
	}

	return 0;
}

DWORD RVAToSectionRawDataSize(DWORD dwRVA, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;
	DWORD dwSectionOffset = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if((pSecHdr[dw].VirtualAddress <= dwRVA) && (pSecHdr[dw+1].VirtualAddress > dwRVA))
		{
			return dwSectionOffset;
		}

		dwSectionOffset += pSecHdr[dw].SizeOfRawData; 
	}

	return 0;
}

DWORD RVAToSectionVirtualAddress(DWORD dwRVA, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if((pSecHdr[dw].VirtualAddress <= dwRVA) && (pSecHdr[dw+1].VirtualAddress > dwRVA))
		{
			return pSecHdr[dw].VirtualAddress;
		}
	}

	return 0;
}

DWORD OffsetToRVA(DWORD dwOffset, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if((pSecHdr[dw].PointerToRawData <= dwOffset) && (pSecHdr[dw].PointerToRawData + pSecHdr[dw].SizeOfRawData > dwOffset))
		{
			return (pSecHdr[dw].VirtualAddress + (dwOffset - pSecHdr[dw].PointerToRawData));
		}
	}
	
	return 0;
}

DWORD MappedOffsetToRVA(DWORD dwOffset, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	dwOffset -= (DWORD)lpMapping;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if((pSecHdr[dw].PointerToRawData <= dwOffset) && (pSecHdr[dw].PointerToRawData + pSecHdr[dw].SizeOfRawData > dwOffset))
		{
			return (pSecHdr[dw].VirtualAddress + (dwOffset - pSecHdr[dw].PointerToRawData));
		}
	}
	
	return 0;
}

char* RVAToSectionName(DWORD dwRVA, LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwKatSup = 0;
	DWORD dwSecStart = 0;
	LONG lJmp = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += pNtHdr->FileHeader.SizeOfOptionalHeader;

	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	// Find the section that contains the RVA
	for (DWORD dw=0; dw<pNtHdr->FileHeader.NumberOfSections; ++dw)
	{
		if(pSecHdr[dw].VirtualAddress == dwRVA)
		{
			return (char*)pSecHdr[dw].Name;
		}
	}

	return 0;
}