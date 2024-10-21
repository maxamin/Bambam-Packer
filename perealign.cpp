#include <windows.h>
#include "perealign.h"
#include "peutils.h"

#define MAX_SEC_NUM         30

const   DWORD ScanStartDS = 0x40;
const   MinSectionTerm    = 5;

DWORD RealignPE(LPVOID lpMapping, DWORD dwFileSize)
{
	DWORD                dwMapBase;
	LPVOID               pMap;
	DWORD				 dwTmpNum,dwSectionBase;
	WORD                 wTmpNum;
	CHAR *	             pCH;
	WORD *			     pW;
	LPVOID               pSections[MAX_SEC_NUM];

	PIMAGE_DOS_HEADER       pDosh;
	PIMAGE_NT_HEADERS		pPeh;
	PIMAGE_SECTION_HEADER   pSectionh;
	int                     i,ii;

	ZeroMemory(&pSections, sizeof(pSections));
	
	// get the other parameters
	pMap = lpMapping;
	dwMapBase = (DWORD)pMap;
	
	if (dwFileSize == 0 || pMap == NULL)
	{
		return 1;
	}
	
	// access the PE Header and check whether it's a valid one
	pDosh = (PIMAGE_DOS_HEADER)(pMap);
	if (pDosh->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return 2;
	}
	pPeh = (PIMAGE_NT_HEADERS)((DWORD)pDosh+pDosh->e_lfanew);
	if (pPeh->Signature != IMAGE_NT_SIGNATURE)
	{
		return 2;
	}
	if (pPeh->FileHeader.NumberOfSections > MAX_SEC_NUM)
	{
		return 3;
	}
	
	__try
	{
		/* START */
		pPeh->OptionalHeader.FileAlignment = 0x200;
		
		/* Realign the PE Header */
		// get the size of all headers
		dwTmpNum = 0xf8 + pPeh->FileHeader.NumberOfSections * 0x28;

		// kill room between the "win32 pls" message and the PE signature
		// find the end of the message
		pW = (WORD*)(dwMapBase + ScanStartDS);
		while (*pW != 0 || (!IsRoundedTo((DWORD)pW,0x10)))
		{
			pW = (WORD*)((DWORD)pW+1);
		}
		wTmpNum = (WORD)((DWORD)pW-dwMapBase);
		if (wTmpNum < pDosh->e_lfanew)
		{
			CopyMemory((LPVOID)pW,(VOID*)pPeh,dwTmpNum); // copy the Header to the right place

			dwSectionBase = ValidAlignment(dwTmpNum + pDosh->e_lfanew, 0x200);
			// blank out end of aligned optional header
			DWORD dwT1 = (DWORD)pW + dwTmpNum;
			DWORD dwT2 = dwSectionBase-(dwT1 - dwMapBase);
			memset((LPVOID)dwT1, 0, dwT2);

			pDosh->e_lfanew = wTmpNum;
		}
				
		dwSectionBase = ValidAlignment(dwTmpNum + pDosh->e_lfanew, 0x200);
		pPeh = (PIMAGE_NT_HEADERS)(dwMapBase+pDosh->e_lfanew); // because the NT header moved
		// correct the SizeOfHeaders
		pPeh->OptionalHeader.SizeOfHeaders = dwSectionBase;
		
		/* Realign all sections */
		// make a copy of all sections
		// this is needed if the sections aren't sorted by their RawOffset (e.g. Petite)
		pSectionh = (PIMAGE_SECTION_HEADER)(dwMapBase+pDosh->e_lfanew+0xf8);
		for (i=0; i<pPeh->FileHeader.NumberOfSections; i++)
		{
			if (pSectionh->SizeOfRawData == 0 || pSectionh->PointerToRawData == 0)
			{
				++pSectionh;
				continue;
			}
			// get a valid size
			dwTmpNum = pSectionh->SizeOfRawData;
			if ((pSectionh->SizeOfRawData+pSectionh->PointerToRawData) > dwFileSize)
				dwTmpNum = dwFileSize-pSectionh->PointerToRawData;
			dwTmpNum -= 1;
			// copy the section into some memory
			pSections[i] = GlobalAlloc(0,dwTmpNum);
			if (pSections[i] == NULL) // fatal error !!!
			{
				for (ii=0; ii<i-1; ii++)
					if (pSections[ii])
						GlobalFree(pSections[ii]);
					return 4;
			}
			CopyMemory(pSections[i],(LPVOID)(pSectionh->PointerToRawData+dwMapBase),dwTmpNum);
			++pSectionh;
		}
		
		// start realigning the sections
		pSectionh = (PIMAGE_SECTION_HEADER)(dwMapBase+pDosh->e_lfanew+0xf8);
		for (i=0;i<pPeh->FileHeader.NumberOfSections;i++)
		{
			// some anti crash code :P
			if (pSectionh->SizeOfRawData == 0 || pSectionh->PointerToRawData == 0)
			{
				++pSectionh;
				if (pSectionh->PointerToRawData == 0)
					continue;
				pSectionh->PointerToRawData = dwSectionBase;
				continue;
			}
			// let pCH point to the end of the current section
			if ((pSectionh->PointerToRawData+pSectionh->SizeOfRawData) <= dwFileSize)
				pCH = (char*)(dwMapBase+pSectionh->PointerToRawData+pSectionh->SizeOfRawData-1);
			else
				pCH = (char*)(dwMapBase+dwFileSize-1);
			// look for the end of this section
			while (*pCH == 0)
				--pCH;
			// calculate the new RawSize
			dwTmpNum = ((DWORD)pCH-dwMapBase)+MinSectionTerm-pSectionh->PointerToRawData;

			// copy the section to the new place
			//if (i != pPeh->FileHeader.NumberOfSections-1)
				dwTmpNum = 	ValidAlignment(dwTmpNum, 0x200);
			
			if (dwTmpNum < pSectionh->SizeOfRawData)
				pSectionh->SizeOfRawData = dwTmpNum;
			else // the new size is too BIG
				dwTmpNum = pSectionh->SizeOfRawData;

			CopyMemory((LPVOID)(dwMapBase+dwSectionBase),pSections[i],
				dwTmpNum);
			// set the RawOffset
			pSectionh->PointerToRawData = dwSectionBase;
			// get the RawOffset for the next section
			dwSectionBase = dwTmpNum+dwSectionBase; // the last section doesn't need to be aligned
			// go to the next section
			++pSectionh;
		}
		
		// delete bound import directories because it is destroyed if present
		pPeh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;
		pPeh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
		
		// clean up
		for (i=0;i<pPeh->FileHeader.NumberOfSections;i++)
			if (pSections[i])
				GlobalFree(pSections[i]);
	}
	__except(1)
	{
		// clean up
		for (i=0; i<pPeh->FileHeader.NumberOfSections; i++)
			if (pSections)
				GlobalFree(pSections[i]);
			return 0;
	}
	return dwSectionBase; // return the new filesize
}

BOOL IsRoundedTo(DWORD dwTarNum,DWORD dwRoundNum)
{
	div_t  d;

	d = div(dwTarNum,dwRoundNum);
	return (d.rem == 0);
}

DWORD ValidAlignment(DWORD dwSize, DWORD dwAlignment)
{
	div_t DivRes;

	DivRes = div(dwSize, dwAlignment);
	if (DivRes.rem == 0)
		return dwSize;
	return ((DivRes.quot+1) * dwAlignment);
}

DWORD StripReloc(LPVOID lpMapping)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwSecStart = 0;
	DWORD dwKatSup = 0;
	DWORD dwBytesWritten = 0;
	LONG lJmp = 0;
	WORD wNumSections = 0;
	WORD wSizeO = 0;
	int i = 0;
	char* szSectionName = 0;
	DWORD dwTempSize = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	wNumSections = pNtHdr->FileHeader.NumberOfSections;
	wSizeO = pNtHdr->FileHeader.SizeOfOptionalHeader;

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += wSizeO;

	// Get reloc section name
	if(pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress != 0)
	{
		szSectionName = RVAToSectionName(pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, lpMapping);

		if(szSectionName)
		{
			for(i = 0; i < wNumSections; i++)
			{
				pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

				if((strcmp((char*)pSecHdr->Name, szSectionName) == 0) && (i == wNumSections-1))
				{
					pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = 0;
					pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0;

					pNtHdr->FileHeader.NumberOfSections--;
					pNtHdr->OptionalHeader.SizeOfImage = pSecHdr->VirtualAddress + ValidAlignment(pSecHdr->Misc.VirtualSize, pNtHdr->OptionalHeader.SectionAlignment);

					return dwTempSize;
				}

				dwTempSize += pSecHdr->SizeOfRawData;

				// move to next section header
				dwSecStart += sizeof(IMAGE_SECTION_HEADER);
			}
		}
	}

	return 0;
}