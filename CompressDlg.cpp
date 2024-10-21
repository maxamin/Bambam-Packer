// CompressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bambam.h"
#include "CompressDlg.h"
#include "OptionsDlg.h"

#include "perealign.h"
#include "peutils.h"

#include "..\aPLib-0.42\lib\mscoff\aplib.h"
#include "..\stub\gev.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VERSION 0.02

DWORD dwNewResourceSize = 0;
GLOBAL_EXTERNAL_VARS gev = { 0 };

/////////////////////////////////////////////////////////////////////////////
// CCompressDlg dialog


CCompressDlg::CCompressDlg(CWnd* pParent /*=NULL*/)
	: CTexturedDlg(CCompressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompressDlg)
	m_SourceFile = _T("");
	m_CompressionProgress = _T("");
	m_FileSize = _T("");
	//}}AFX_DATA_INIT
}


void CCompressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompressDlg)
	DDX_Control(pDX, IDC_COMP_SIZE, m_ctrlCompessedSize);
	DDX_Control(pDX, IDC_COMP_PROGRESS, m_ctrlCompressionProgress);
	DDX_Text(pDX, IDC_SOURCE_FILE, m_SourceFile);
	DDX_Text(pDX, IDC_COMP_PROG, m_CompressionProgress);
	DDX_Text(pDX, IDC_FILE_SIZE, m_FileSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompressDlg, CDialog)
	//{{AFX_MSG_MAP(CCompressDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_COMPRESS, OnCompress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompressDlg message handlers

void CCompressDlg::OnBrowse() 
{
	OPENFILENAME ofn;
	char szFile[_MAX_PATH] = {0};

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->GetSafeHwnd();
	ofn.lpstrFile = szFile;


	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFilter = "All\0*.*\0Executables\0*.EXE\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// open file dialog
	if(GetOpenFileName(&ofn))
	{
		m_CompressionProgress = "";
		m_ctrlCompressionProgress.SetPos(0);
		
		m_FileSize = "";
		m_ctrlCompessedSize.SetPos(0);

		m_SourceFile = ofn.lpstrFile;
	}

	UpdateData(FALSE);
}

void CCompressDlg::OnExit() 
{
	ExitProcess(0);
}

int CCompressDlg::Encrypt(char* szFile)
{
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID lpMapping;

	HANDLE hBackupFile;
	HANDLE hBackupMapping;
	LPVOID lpBackupMapping;

	DWORD dwOldOEP = 0;
	DWORD dwFullOnKey = 0;
	DWORD dwRetSec = 0;
	DWORD dwAlignment = 0;

	DWORD dwOrigSize = 0;
	DWORD dwNewSize = 0;

	char szInput[_MAX_PATH] = {0};
	char szBackup[_MAX_PATH] = {0};

	if(m_pOptionsDlg->m_Backup == TRUE)
	{
		strcpy(szInput, szFile);
		sprintf(szBackup, "%s.bak", szInput);
		CopyFile(szFile, szBackup, FALSE);
	}

	// create temp backup
	char szTempPath[_MAX_PATH] = {0};
	char szTempFile[_MAX_PATH] = {0};

	GetTempPath(_MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, "bam", 0, szTempFile);

	CopyFile(szFile, szTempFile, FALSE);

	// open target
	hFile = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	
	if(hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("Invalid file handle", 0, MB_OK | MB_ICONERROR);
		return 1;
	}
	
	DWORD dwSizeToMap = GetFileSize(hFile, 0);
	dwOrigSize = dwSizeToMap;
	dwSizeToMap = dwSizeToMap * 2;

	hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSizeToMap, 0);
	
	if(hMapping == NULL)
	{
		MessageBox("Invalid handle for file mapping", 0, MB_OK | MB_ICONERROR);
		return 1;
	}

	lpMapping = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, dwSizeToMap);


	if(lpMapping == NULL)
	{
		MessageBox("Invalid pointer for mapping", 0, MB_OK | MB_ICONERROR);
		return 1;
	}

	// open backup
	hBackupFile = CreateFile(szTempFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	
	if(hBackupFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("Invalid backup file handle", 0, MB_OK | MB_ICONERROR);
		return 1;
	}
	
	hBackupMapping = CreateFileMapping(hBackupFile, NULL, PAGE_READWRITE, 0, 0, 0);
	
	if(hBackupMapping == NULL)
	{
		MessageBox("Invalid handle for backup file mapping", 0, MB_OK | MB_ICONERROR);
		return 1;
	}
	
	lpBackupMapping = MapViewOfFile(hBackupMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	if(lpBackupMapping == NULL)
	{
		MessageBox("Invalid pointer for backup mapping", 0, MB_OK | MB_ICONERROR);
		return 1;
	}
	
	if(m_pOptionsDlg->m_Realign == TRUE)
	{
		dwNewSize = RealignPE(lpMapping, dwOrigSize);
		
		// realign backup also, so sections match
		RealignPE(lpBackupMapping, dwOrigSize);
	}

	if(m_pOptionsDlg->m_StripReloc == TRUE)
	{
		dwNewSize = StripReloc(lpMapping);
	}

	// open stub from resource
	DWORD dwWritten;
	LPVOID lpBuf;
	LPVOID lpStubMapping;
	HRSRC hRes = 0;
	HGLOBAL hResLoad = 0;
	hRes = FindResource(0, MAKEINTRESOURCE(IDR_STUB), "BINARY"); 
	hResLoad = LoadResource(0, hRes);
	lpStubMapping = LockResource(hResLoad);
	
	// Compress target sections
	try
	{
		CompressSections(lpMapping, lpBackupMapping);
	}
	catch(...)
	{
		MessageBox("Fatal error during compression", 0, MB_OK | MB_ICONERROR);

		// unmap backup
		UnmapViewOfFile(lpBackupMapping);
		CloseHandle(hBackupMapping);	
		CloseHandle(hBackupFile);
		DeleteFile(szTempFile);

		// unmap target
		UnmapViewOfFile(lpMapping);
		CloseHandle(hMapping);	
		CloseHandle(hFile);

		if(m_pOptionsDlg->m_Backup == TRUE)
		{
			DeleteFile(szFile);
			MoveFile(szBackup, szFile);
		}

		return -1;
	}
	
	DWORD dwKatSup;
	DWORD dwSecStart;
	WORD wNumSections;
	WORD wSizeO;
	LONG lJmp;

	PIMAGE_DOS_HEADER pStubDosHdr;
	PIMAGE_NT_HEADERS pStubNtHdr;
	PIMAGE_SECTION_HEADER pStubSecHdr;

	DWORD dwStubEntryPoint = 0;
	DWORD dwPointerToStubRawData = 0;
	DWORD dwStubSecStart = 0;

	pStubDosHdr = (PIMAGE_DOS_HEADER) lpStubMapping;
	
	lJmp = pStubDosHdr->e_lfanew; // oh yeh and we already checked sig shit so fuck off.

	dwKatSup = (DWORD) lpStubMapping;
	
	dwKatSup += lJmp;

	pStubNtHdr = (PIMAGE_NT_HEADERS) dwKatSup;

	dwStubEntryPoint = pStubNtHdr->OptionalHeader.AddressOfEntryPoint - pStubNtHdr->OptionalHeader.BaseOfCode;

	wNumSections = pStubNtHdr->FileHeader.NumberOfSections;
	wSizeO = pStubNtHdr->FileHeader.SizeOfOptionalHeader;
	
	dwStubSecStart = (DWORD) pStubNtHdr;
	dwStubSecStart += 24;
	dwStubSecStart += wSizeO;
	
	pStubSecHdr = (PIMAGE_SECTION_HEADER)dwStubSecStart;
	
	DWORD dwStubSize = SizeofResource(0, hRes);
	
	lpBuf = malloc(dwStubSize);
	memset(lpBuf, 0, dwStubSize);
	// copy stub data
	memcpy(lpBuf, lpStubMapping, dwStubSize);
	
	PIMAGE_DOS_HEADER pDosHdr;
	PIMAGE_NT_HEADERS pNtHdr;
	PIMAGE_SECTION_HEADER pSecHdr;
	IMAGE_SECTION_HEADER SecHdr;
	int i = 0;
	
	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	
	lJmp = pDosHdr->e_lfanew; // oh yeh and we already checked sig shit so fuck off.
	
	dwKatSup = (DWORD) lpMapping;
	
	dwKatSup += lJmp;
	
	pNtHdr = (PIMAGE_NT_HEADERS) dwKatSup;
	
	dwOldOEP = pNtHdr->OptionalHeader.AddressOfEntryPoint;
	
	wNumSections = pNtHdr->FileHeader.NumberOfSections;
	wSizeO = pNtHdr->FileHeader.SizeOfOptionalHeader;

	dwSecStart = (DWORD) pNtHdr;
	dwSecStart += 24;
	dwSecStart += wSizeO;
	
	dwSecStart += (wNumSections * 0x28);
	dwSecStart -= 0x28;
	
	pSecHdr = (PIMAGE_SECTION_HEADER) dwSecStart;
	
	DWORD dwRvaStart;
	DWORD dwPointerStar;
	DWORD dwSizeOfRawz;
	DWORD dwPointerNew;
	char szSecName[9] = {0};
	DWORD dwVirtSizeNew;

	strcpy(szSecName, m_pOptionsDlg->m_SectionName);

	dwVirtSizeNew = CalcStubSize(pStubDosHdr);
	
	dwRvaStart = pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize;
		
	dwPointerStar = pSecHdr->PointerToRawData;
	dwSizeOfRawz = pSecHdr->SizeOfRawData;
	
	dwPointerNew = dwPointerStar + dwSizeOfRawz; // a place to inject
	
	DWORD dwStubRvaStart = dwRvaStart;
	
	lstrcpyn( (LPSTR) &SecHdr.Name, (LPSTR) &szSecName, sizeof(szSecName));
	
	pSecHdr = (PIMAGE_SECTION_HEADER) &SecHdr;
	
	SecHdr.Misc.VirtualSize = ValidAlignment(dwVirtSizeNew, 0x1000);

	SecHdr.VirtualAddress = dwRvaStart;
	SecHdr.PointerToRawData = dwPointerNew;
	SecHdr.SizeOfRawData = ValidAlignment(dwVirtSizeNew, 0x200);
	SecHdr.Characteristics = 0xC0000040;
	
	dwSecStart += 0x28;
	
	memcpy( (void *) dwSecStart, &SecHdr, 0x28);
	
	pNtHdr->OptionalHeader.AddressOfEntryPoint = dwRvaStart + dwStubEntryPoint;
	pNtHdr->OptionalHeader.SizeOfImage = SecHdr.VirtualAddress + ValidAlignment(SecHdr.Misc.VirtualSize, pNtHdr->OptionalHeader.SectionAlignment);;
	pNtHdr->FileHeader.NumberOfSections += 1;
	
	// update some values in lpBuf
	// stub section info
	DWORD dwNumberOfStubSections = 0;
	DWORD dwSectionCount = 0;
	dwNumberOfStubSections = pStubNtHdr->FileHeader.NumberOfSections;

	BOOL fGevFound = FALSE;
	BOOL fCodeFound = FALSE;
	BOOL fImportsFound = FALSE;

	pStubSecHdr = (PIMAGE_SECTION_HEADER)dwStubSecStart;

	// fix code addresses
	for(dwSectionCount = 0; dwSectionCount < dwNumberOfStubSections; dwSectionCount++)
	{
		if(strcmp((char*)pStubSecHdr->Name, ".text") != 0)
		{
			pStubSecHdr++;
		}
		else
		{
			fCodeFound = TRUE;
			break;
		}
	}	

	dwPointerToStubRawData = pStubSecHdr->PointerToRawData;

	if(fCodeFound)
	{
		PIMAGE_SECTION_HEADER pSectionTemp;

		pSectionTemp = pStubSecHdr;

		PerformStubRelocations(lpBuf, dwStubRvaStart + pNtHdr->OptionalHeader.ImageBase);
	}

	// find stub imports
	pStubSecHdr = (PIMAGE_SECTION_HEADER)dwStubSecStart;

	// fix thunks in stub
	for(dwSectionCount = 0; dwSectionCount < dwNumberOfStubSections; dwSectionCount++)
	{
		if(strcmp((char*)pStubSecHdr->Name, ".rdata") != 0)
		//if(strcmp((char*)pStubSecHdr->Name, ".a") != 0)
		{
			pStubSecHdr++;
		}
		else
		{
			fImportsFound = TRUE;
			break;
		}
	}	

	if(fImportsFound)
	{
		// temp
		IMAGE_IMPORT_DESCRIPTOR* pIID = 0;
		IMAGE_THUNK_DATA* pThunk = 0;

		// translate thunk values
		TranslateAddresses((BYTE*)((DWORD)lpBuf + pStubSecHdr->PointerToRawData),
							pStubSecHdr->SizeOfRawData,
							pStubSecHdr->VirtualAddress,
							dwStubRvaStart + (pStubSecHdr->PointerToRawData - dwPointerToStubRawData),	//dwPointerToStubRawData
							pStubSecHdr->VirtualAddress);
	}

	// point PE header at stub IAT
	DWORD dwOriginalIAT = pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	DWORD dwTemp = (pStubNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress & 0x000000ff);
	pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = dwStubRvaStart + (pStubSecHdr->PointerToRawData - dwPointerToStubRawData) + dwTemp;	//dwPointerToStubRawData
	pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = pStubNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

	// wipe IAT
	pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress = 0;
	pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size = 0;


	// end find stub iat

	// find global external vars
	pStubSecHdr = (PIMAGE_SECTION_HEADER)dwStubSecStart;

	GlobalExternalVars* pgev;

	for(dwSectionCount = 0; dwSectionCount < dwNumberOfStubSections; dwSectionCount++)
	{
		if(strcmp((char*)pStubSecHdr->Name, ".a") != 0)
		{
			pStubSecHdr++;
		}
		else
		{
			fGevFound = TRUE;
			break;
		}
	}

	if(fGevFound)
	{
		pgev = (GlobalExternalVars*)((DWORD)lpBuf + pStubSecHdr->PointerToRawData);
		
		pgev->dwOEP = dwOldOEP;
		pgev->dwRVAStubEntry = dwRvaStart + dwStubEntryPoint;
		pgev->dwIATAddress = dwOriginalIAT;
		pgev->dwRVACompressedResourceSize = dwNewResourceSize;

		// Copy icon count
		pgev->dwFirstGroupIconCount = gev.dwFirstGroupIconCount;

		// TLS directory stuff
		if(pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress != 0)
		{
			memcpy(&(pgev->tlsOriginal), &(gev.tlsOriginal), sizeof(IMAGE_TLS_DIRECTORY));

			// complicated TLS stuff to fix problems with Delphi 7 apps
			DWORD dwStubOffsetIndex = (DWORD)&pgev->dwAddressOfIndex - (DWORD)lpBuf;
			dwStubOffsetIndex -= dwPointerToStubRawData;
			dwStubOffsetIndex += dwPointerNew;
			dwStubOffsetIndex = OffsetToRVA(dwStubOffsetIndex, lpMapping);

			pgev->tlsOriginal.AddressOfIndex = (DWORD*)(dwStubOffsetIndex + pNtHdr->OptionalHeader.ImageBase);
			//pgev->tlsOriginal.AddressOfIndex = (dwStubOffsetIndex + pNtHdr->OptionalHeader.ImageBase);

			DWORD dwTlsOffset = (DWORD)(&(gev.tlsOriginal)) - (DWORD)(&(gev));
			pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = dwStubRvaStart + (pStubSecHdr->PointerToRawData - dwPointerToStubRawData) + dwTlsOffset;
			pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size = sizeof(IMAGE_TLS_DIRECTORY);
		}

		// encrypt gev section
		BYTE* lpbUncompressedBuffer = 0;
		DWORD dwUncompressedSize = 0;
		BYTE* lpbCompressedBuffer = 0;
		DWORD dwCompressedSize = 0;

		lpbUncompressedBuffer = (BYTE*)pgev;
		dwUncompressedSize = (DWORD)&pgev->tlsOriginal - (DWORD)pgev;

		lpbCompressedBuffer = (BYTE*)Compress(lpbUncompressedBuffer, dwUncompressedSize, &dwCompressedSize, 0);

		// clear raw GEV and replace with compressed
		memset(pgev, 0, dwUncompressedSize);
		memcpy(pgev, lpbCompressedBuffer, dwCompressedSize);

		free(lpbCompressedBuffer);
		lpbCompressedBuffer = 0;
	}
	
	// end find gev

	// Write new section to disk
	SetFilePointer(hFile, dwPointerNew, NULL, FILE_BEGIN);
	WriteFile(hFile, ((char*)lpBuf + dwPointerToStubRawData), CalcStubSize(pStubDosHdr), &dwWritten, NULL);

	// truncation
	BYTE* lpbTruncateBuffer = 0;
	DWORD dwTruncateSize = ValidAlignment(CalcStubSize(pStubDosHdr), 0x200) - CalcStubSize(pStubDosHdr);

	lpbTruncateBuffer = (BYTE*)malloc(dwTruncateSize);
	memset(lpbTruncateBuffer, 0, dwTruncateSize);
	WriteFile(hFile, lpbTruncateBuffer, dwTruncateSize, &dwWritten, NULL);

	free(lpbTruncateBuffer);

	// unmap backup
	UnmapViewOfFile(lpBackupMapping);
	CloseHandle(hBackupMapping);	
	CloseHandle(hBackupFile);
	DeleteFile(szTempFile);

	// unmap target
	UnmapViewOfFile(lpMapping);
	CloseHandle(hMapping);	
	SetEndOfFile(hFile);

	//dwOrigSize = dwNewSize;
	dwNewSize = GetFileSize(hFile, 0);

	//printf(" Compressing shrunk from %dk to %dk\n", dwOrigSize/1024, dwNewSize/1024);

	CloseHandle(hFile);

	// Set compsize progress bar
	double fPercent = 0.0;
	fPercent = ((double)(dwNewSize)/(double)(dwOrigSize))*100;
	m_ctrlCompessedSize.SetPos((int)fPercent);	
	m_FileSize.Format("%.02f%%", fPercent);

	UpdateData(FALSE);

	return 0;
}

int CCompressDlg::TranslateAddresses(BYTE* pBuffer, DWORD dwLength, DWORD dwSourceVA, DWORD dwDestinationVA, DWORD dwRVAPage)
{
	// == 0x3F Warning: you should avoid to use this byte in patterns
	#define _XX_  '?' 

	unsigned char PAT_Address[] = { _XX_, _XX_, _XX_, _XX_ };

	DWORD dwAddress = 0;
	BYTE* pOrigBuffer = pBuffer;
	DWORD dwDelta = dwSourceVA - dwDestinationVA;
	DWORD dwMask = 0xffffff00;
	DWORD dwDontCare = 0x0000003f;
	DWORD* pAddress;
	DWORD* pPattern;

	// Setup search pattern
	pPattern = (DWORD*)&PAT_Address;

	// masking need to be figured out more
	*pPattern = ((dwSourceVA & 0xffff0000) | dwRVAPage ) & dwMask;
	*pPattern = *pPattern | (~*pPattern & dwDontCare);

	// Search and replace all addresses in range
	dwAddress = PatternSearch(PAT_Address, sizeof(PAT_Address), pBuffer, dwLength);
				
	while(((int)dwAddress >= 0) && (pBuffer < (pOrigBuffer + dwLength)))
	{
		pAddress = (DWORD*)((DWORD)pBuffer + dwAddress);		
		*pAddress -= dwDelta;

		pBuffer = pBuffer + dwAddress + sizeof(PAT_Address);
		dwAddress = PatternSearch(PAT_Address, sizeof(PAT_Address), pBuffer, dwLength);
	}

	return 0;
}

int CCompressDlg::PatternSearch(unsigned char *SearchString, int StringLen, unsigned char *SearchBuff, int BuffSize)
{
    int i,j;

    if (BuffSize < StringLen) return -64;

    for(i=0;i<(BuffSize-StringLen);i++)
    {
        for(j=0;j<StringLen;j++)
        {
            if (
                 (SearchString[j] != _XX_) &&
                 (SearchString[j] != SearchBuff[i+j])
               )
            break;
        }
        if (j==StringLen) return i;
    }

    return -100;
}

int CCompressDlg::CalcStubSize(PIMAGE_DOS_HEADER pDosHdr)
{
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	LONG lJmp = 0;
	DWORD dwSecStart = 0;
	DWORD dwResult = 0;
	DWORD dwKatSup = 0;
	WORD wNumSections = 0;
	WORD wSizeO = 0;

	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	wNumSections = pNtHdr->FileHeader.NumberOfSections;
	wSizeO = pNtHdr->FileHeader.SizeOfOptionalHeader;
	
	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += wSizeO;
	
	pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

	dwResult = pSecHdr->SizeOfRawData;
	pSecHdr++;
	dwResult += pSecHdr->SizeOfRawData;
	pSecHdr++;
	dwResult += pSecHdr->SizeOfRawData;

	return dwResult;
}

void CCompressDlg::PerformStubRelocations(LPVOID lpMapping, DWORD dwStubVA)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_BASE_RELOCATION pIBRCurrent = 0;
	PIMAGE_BASE_RELOCATION pIBREnd = 0;
	DWORD dwKatSup = 0;
	LONG lJmp = 0;
	DWORD* pAddress = 0;
	DWORD dwDelta = 0;

	pDosHdr = (PIMAGE_DOS_HEADER) lpMapping;
	lJmp = pDosHdr->e_lfanew;
	dwKatSup = (DWORD)pDosHdr;
	dwKatSup += lJmp;

	pNtHdr = (PIMAGE_NT_HEADERS)dwKatSup;

	if(pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress != 0)
	{
		pIBRCurrent = (PIMAGE_BASE_RELOCATION)RVAToMappedOffset((DWORD)pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, lpMapping);
		pIBREnd = (PIMAGE_BASE_RELOCATION)&((BYTE*)pIBRCurrent)[pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size];

		while((pIBRCurrent<pIBREnd) && (pIBRCurrent->VirtualAddress != 0))
		{
			DWORD dwRVAPage = pIBRCurrent->VirtualAddress;
			DWORD dwRelocation = 0;
			DWORD dwSectionOffset = 0;
			DWORD dwVirtualAddress = 0; 
			DWORD dwRelocationCount = (pIBRCurrent->SizeOfBlock - IMAGE_SIZEOF_BASE_RELOCATION) / sizeof(WORD);
			WORD* awRelType = (WORD*)((BYTE*)pIBRCurrent + IMAGE_SIZEOF_BASE_RELOCATION);

			for(DWORD dw=0; dw<dwRelocationCount; ++dw)
			{
				WORD wType = awRelType[dw] >> 12;
				WORD wValue = awRelType[dw] & 0x0fff;

				if(wType == IMAGE_REL_BASED_HIGHLOW)
				{
					dwRelocation = dwRVAPage + wValue;
					pAddress = (DWORD*)RVAToMappedOffset(dwRelocation, lpMapping);

					DWORD dwTempAddress = (*pAddress) & 0x0000ffff;
					dwSectionOffset = RVAToSectionRawDataSize(dwTempAddress, lpMapping);
					dwVirtualAddress = RVAToSectionVirtualAddress(dwTempAddress, lpMapping);
					dwDelta = (dwStubVA + dwSectionOffset) - (pNtHdr->OptionalHeader.ImageBase + dwVirtualAddress);
					
					*pAddress = *pAddress + dwDelta;
				}

				pIBRCurrent = (PIMAGE_BASE_RELOCATION)&((BYTE*)pIBRCurrent)[pIBRCurrent->SizeOfBlock];
			}
		}
	}
}

void CCompressDlg::CompressSections(LPVOID lpMapping, LPVOID lpBackup)
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

	PIMAGE_DOS_HEADER pBackupDosHdr = 0;
	PIMAGE_NT_HEADERS pBackupNtHdr = 0;
	PIMAGE_SECTION_HEADER pBackupSecHdr = 0;
	DWORD dwBackupSecStart = 0;
	DWORD dwBackupKatSup = 0;
	LONG lBackupJmp = 0;
	WORD wBackupNumSections = 0;
	WORD wBackupSizeO = 0;

	BYTE* lpbBuffer = 0;
	DWORD dwBufferSize = 0;
	DWORD dwBufferPos = 0;
	int i = 0;

	DWORD dwTempPointerToRawData = 0;
	bool bResource;
	BYTE* lpbNewResourceSection = 0;

	bool bSkipedZeroSizeSection;
	bool bBigger = false;

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

	pBackupDosHdr = (PIMAGE_DOS_HEADER) lpBackup;
	lBackupJmp = pBackupDosHdr->e_lfanew;
	dwBackupKatSup = (DWORD)pBackupDosHdr;
	dwBackupKatSup += lBackupJmp;

	pBackupNtHdr = (PIMAGE_NT_HEADERS)dwBackupKatSup;

	wBackupNumSections = pBackupNtHdr->FileHeader.NumberOfSections;
	wBackupSizeO = pBackupNtHdr->FileHeader.SizeOfOptionalHeader;

	dwBackupSecStart = (DWORD)pBackupNtHdr;
	dwBackupSecStart += 24;
	dwBackupSecStart += wBackupSizeO;

	// TLS directory stuff
	
	if(pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress != 0)
	{
		PIMAGE_TLS_DIRECTORY tlsOriginal = 0;
		tlsOriginal = (PIMAGE_TLS_DIRECTORY)RVAToMappedOffset((DWORD)pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress, lpMapping);

		// Copy original and blank original
		memcpy(&(gev.tlsOriginal), tlsOriginal, sizeof(IMAGE_TLS_DIRECTORY));
		memset(tlsOriginal, 0, sizeof(IMAGE_TLS_DIRECTORY));
	}

	DWORD dwTempSize = 0;
	
	m_ProgressFileSize = 0;
	m_ProgressOffset = 0;

	for(i = 0; i < wNumSections-1; i++)
	{
		pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

		m_ProgressFileSize += pSecHdr->Misc.VirtualSize;

		// move to next section header
		dwSecStart += sizeof(IMAGE_SECTION_HEADER);
	}

	dwSecStart = (DWORD)pNtHdr;
	dwSecStart += 24;
	dwSecStart += wSizeO;

	for(i = 0; i < wNumSections; i++)
	{
		// Reset resource flag
		bResource = false;

		pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;
		pBackupSecHdr = (PIMAGE_SECTION_HEADER)dwBackupSecStart;

		if(strcmp((char*)pBackupSecHdr->Name, ".rsrc") == 0)
		{
			// we are at resource section
			bResource = true;
		}

		// skip sections with raw size = 0
		if(pBackupSecHdr->SizeOfRawData == 0)
		{		
			// move to next section header
			dwSecStart += sizeof(IMAGE_SECTION_HEADER);
			pBackupSecHdr += sizeof(IMAGE_SECTION_HEADER);
			bSkipedZeroSizeSection = true;
			continue;
		}

		BYTE* lpbUncompressedBuffer = 0;
		DWORD dwUncompressedSize = 0;
		BYTE* lpbCompressedBuffer = 0;
		DWORD dwCompressedSize = 0;

		lpbUncompressedBuffer = (BYTE*)((DWORD)lpBackup + pBackupSecHdr->PointerToRawData);
		dwUncompressedSize = pBackupSecHdr->SizeOfRawData;

		if(bResource)
		{
			lpbNewResourceSection = (BYTE*)BuildNewResourceSection(lpBackup, &dwNewResourceSize);

			// This is the resource section, so skip the directory and only compress the data afterwards
			lpbUncompressedBuffer = (BYTE*)((DWORD)lpBackup + pBackupSecHdr->PointerToRawData);
			dwUncompressedSize = pBackupSecHdr->SizeOfRawData;
		}

		lpbCompressedBuffer = (BYTE*)Compress(lpbUncompressedBuffer, dwUncompressedSize, &dwCompressedSize, &CallbackFunc);

		double fPercent = ((double)dwCompressedSize/(double)dwUncompressedSize) * 100;

		//printf("\r %s %.1f%% (comp: %d, uncomp: %d)\n", pSecHdr->Name, fPercent, dwCompressedSize, pSecHdr->SizeOfRawData);
		
		// copy compressed buffer into original mapped file
		DWORD dwNewAlignedSectionSize = ValidAlignment(dwCompressedSize, 0x200);
		BYTE* lpbNewSectionBuffer = (BYTE*)malloc(dwNewAlignedSectionSize);
		memset(lpbNewSectionBuffer, 0, dwNewAlignedSectionSize);
		memcpy(lpbNewSectionBuffer, lpbCompressedBuffer, dwCompressedSize);


		if(i==0)
		{
			memcpy((LPVOID)((DWORD)lpMapping + pBackupSecHdr->PointerToRawData), lpbNewSectionBuffer, dwNewAlignedSectionSize);
			pSecHdr->SizeOfRawData = dwNewAlignedSectionSize;
		}
		else
		{
			if(bResource && lpbNewResourceSection)
			{
				memcpy((LPVOID)((DWORD)lpMapping + dwTempPointerToRawData), lpbNewResourceSection, dwNewResourceSize);
				pSecHdr->PointerToRawData = dwTempPointerToRawData;
				
				dwTempPointerToRawData += dwNewResourceSize;
				memcpy((LPVOID)((DWORD)lpMapping + dwTempPointerToRawData), lpbNewSectionBuffer, dwNewAlignedSectionSize);

				pSecHdr->SizeOfRawData = dwNewResourceSize + dwNewAlignedSectionSize;

				free(lpbNewResourceSection);
			}
			else
			{
				memcpy((LPVOID)((DWORD)lpMapping + dwTempPointerToRawData), lpbNewSectionBuffer, dwNewAlignedSectionSize);
				//pSecHdr->PointerToRawData = dwTempPointerToRawData;
				pSecHdr->PointerToRawData = dwTempPointerToRawData;

				// correct pointer to raw data if we skipped a zero size section
				if(bSkipedZeroSizeSection == true)
				{
					PIMAGE_SECTION_HEADER pLastSecHdr = pSecHdr;
					pLastSecHdr--;

					pLastSecHdr->PointerToRawData = dwTempPointerToRawData;

					bSkipedZeroSizeSection = false;
				}

				pSecHdr->SizeOfRawData = dwNewAlignedSectionSize;
			}
		}

		dwTempPointerToRawData = pSecHdr->PointerToRawData + pSecHdr->SizeOfRawData;

		free(lpbNewSectionBuffer);
		free(lpbCompressedBuffer);

		// Set VSize = RSize
		pSecHdr->Misc.VirtualSize = ValidAlignment(pSecHdr->Misc.VirtualSize, 0x1000);

		m_ProgressOffset += pSecHdr->Misc.VirtualSize;

		// move to next section header
		dwSecStart += sizeof(IMAGE_SECTION_HEADER);
		dwBackupSecStart += sizeof(IMAGE_SECTION_HEADER);
	}

	// Something strange in release mode ?!?!?!?
	// need to track this down really, but for now, just make sure
	// we reset the first sections PointerToRawData
	{
		dwSecStart = (DWORD)pNtHdr;
		dwSecStart += 24;
		dwSecStart += wSizeO;

		dwBackupSecStart = (DWORD)pBackupNtHdr;
		dwBackupSecStart += 24;
		dwBackupSecStart += wBackupSizeO;

		pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;
		pBackupSecHdr = (PIMAGE_SECTION_HEADER)dwBackupSecStart;

		pSecHdr->PointerToRawData = pBackupSecHdr->PointerToRawData;
	}

	//m_ProgressCtrl.SetPos(100);
}

LPVOID CCompressDlg::Compress(LPVOID lpInputBuffer, DWORD dwInputSize, DWORD* dwOutputSize, PFNAPLIBCALLBACK pfnCallback)
{
	// For aPlib
	BYTE* pWorkmem = 0;
	BYTE* pPacked = 0;

	pWorkmem = (BYTE*)malloc(aP_workmem_size(dwInputSize));
	pPacked = (BYTE*)malloc(aP_max_packed_size(dwInputSize));

	*dwOutputSize = aP_pack(lpInputBuffer, pPacked, dwInputSize, pWorkmem, pfnCallback, this);
	
	return pPacked;
}

LPVOID CCompressDlg::BuildNewResourceSection(LPVOID lpMapping, DWORD* dwOutputSize)
{
	PIMAGE_DOS_HEADER pDosHdr = 0;
	PIMAGE_NT_HEADERS pNtHdr = 0;
	PIMAGE_SECTION_HEADER pSecHdr = 0;
	DWORD dwSecStart = 0;
	DWORD dwKatSup = 0;
	LONG lJmp = 0;
	WORD wNumSections = 0;
	WORD wSizeO = 0;
	int i = 0;
	int j = 0;
	int k = 0;

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

	bool bResource = false;
	PIMAGE_RESOURCE_DIRECTORY pRootDirectory = 0;
	PIMAGE_RESOURCE_DIRECTORY pSubDirectory = 0;
	PIMAGE_RESOURCE_DIRECTORY pSubSubDirectory = 0;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry = 0;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pSubEntry = 0;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pSubSubEntry = 0;
	PIMAGE_RESOURCE_DATA_ENTRY pDataEntry = 0;

	PIMAGE_RESOURCE_DIR_STRING_U pName = 0;

	#pragma pack( push )
	#pragma pack( 2 )
	typedef struct
	{
	   BYTE   bWidth;               // Width, in pixels, of the image
	   BYTE   bHeight;              // Height, in pixels, of the image
	   BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
	   BYTE   bReserved;            // Reserved
	   WORD   wPlanes;              // Color Planes
	   WORD   wBitCount;            // Bits per pixel
	   DWORD   dwBytesInRes;         // how many bytes in this resource?
	   WORD   nID;                  // the ID
	} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;

	typedef struct 
	{
	   WORD            idReserved;   // Reserved (must be 0)
	   WORD            idType;       // Resource type (1 for icons)
	   WORD            idCount;      // How many images?
	   GRPICONDIRENTRY   idEntries[1]; // The entries for each image
	} GRPICONDIR, *LPGRPICONDIR;
	#pragma pack( pop )

	LPGRPICONDIR lpGroupIconDir = 0;

	for(i = 0; i < wNumSections; i++)
	{
		pSecHdr = (PIMAGE_SECTION_HEADER)dwSecStart;

		if(strcmp((char*)pSecHdr->Name, ".rsrc") == 0)
		{
			bResource = true;
			break;
		}

		// move to next section header
		dwSecStart += sizeof(IMAGE_SECTION_HEADER);
	}

	DWORD dwDirectorySize = 0;
	DWORD dwIconSize = 0;
	DWORD dwGroupIconSize = 0;
	DWORD dwVersionSize = 0;
	DWORD dwManifestSize = 0;

	DWORD dwIconCount = 0;
	DWORD dwGroupIconCount = 0;

	if(bResource)
	{
		pRootDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)lpMapping + pSecHdr->PointerToRawData);

		int nDirectoryCount = pRootDirectory->NumberOfIdEntries + pRootDirectory->NumberOfNamedEntries;

		pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pRootDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

		// First pass - figure out number of icons in first icon group
		for(i = 0; i < nDirectoryCount; i++)
		{
			if(pEntry->DataIsDirectory)
			{
				pSubDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pEntry->OffsetToDirectory + (DWORD)pRootDirectory);

				int nSubDirectoryCount = pSubDirectory->NumberOfIdEntries + pSubDirectory->NumberOfNamedEntries;

				pSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

				for(j = 0; j < nSubDirectoryCount; j++)
				{
					if(pSubEntry->DataIsDirectory)
					{

						pSubSubDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pSubEntry->OffsetToDirectory + (DWORD)pRootDirectory);

						int nSubSubDirectoryCount = pSubSubDirectory->NumberOfIdEntries + pSubSubDirectory->NumberOfNamedEntries;

						pSubSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubSubDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

						for(k = 0; k < nSubSubDirectoryCount; k++)
						{
							if(!pSubSubEntry->DataIsDirectory)
							{
								if(!(pSubSubEntry->OffsetToData & 0x8000000))
								{
									pDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRootDirectory + pSubSubEntry->OffsetToData);

									switch(pEntry->Name)
									{
									case RT_GROUP_ICON:
										lpGroupIconDir = (LPGRPICONDIR)(RVAToOffset(pDataEntry->OffsetToData, lpMapping) + (DWORD)lpMapping);
										gev.dwFirstGroupIconCount = lpGroupIconDir->idCount;
										break;
									}
								}
							}

							if(lpGroupIconDir)
								break;

							pSubSubEntry++;
						}
					}

					if(lpGroupIconDir)
						break;

					pSubEntry++;
				}
			}

			if(lpGroupIconDir)
				break;

			pEntry++;
		}

		pRootDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)lpMapping + pSecHdr->PointerToRawData);

		nDirectoryCount = pRootDirectory->NumberOfIdEntries + pRootDirectory->NumberOfNamedEntries;

		pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pRootDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

		// Second pass
		for(i = 0; i < nDirectoryCount; i++)
		{
			if(pEntry->DataIsDirectory)
			{
				pSubDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pEntry->OffsetToDirectory + (DWORD)pRootDirectory);

				int nSubDirectoryCount = pSubDirectory->NumberOfIdEntries + pSubDirectory->NumberOfNamedEntries;

				pSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

				for(j = 0; j < nSubDirectoryCount; j++)
				{
					if(pSubEntry->DataIsDirectory)
					{

						pSubSubDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pSubEntry->OffsetToDirectory + (DWORD)pRootDirectory);

						int nSubSubDirectoryCount = pSubSubDirectory->NumberOfIdEntries + pSubSubDirectory->NumberOfNamedEntries;

						pSubSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubSubDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

						for(k = 0; k < nSubSubDirectoryCount; k++)
						{
							if(!pSubSubEntry->DataIsDirectory)
							{
								if(!(pSubSubEntry->OffsetToData & 0x8000000))
								{
									pDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRootDirectory + pSubSubEntry->OffsetToData);

									switch(pEntry->Name)
									{
									case RT_ICON:
										// Copy however many icons are in the first icon group
										if(dwIconCount < lpGroupIconDir->idCount)
										{
											dwIconSize += pDataEntry->Size;
											dwIconCount++;
										}
										break;
									case RT_GROUP_ICON:
										// Only copy the first icon group
										if(dwGroupIconCount < 1)
										{
											dwGroupIconSize += pDataEntry->Size;
											dwGroupIconCount++;
										}
										break;
									case RT_VERSION:
										dwVersionSize += pDataEntry->Size;
										break;
									case 24:	// RT_MANIFEST
										dwManifestSize += pDataEntry->Size;
										break;
									}
								}
							}

							dwDirectorySize = pSubSubEntry->OffsetToData + sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);

							pSubSubEntry++;
						}
					}

					pSubEntry++;
				}
			}
			pEntry++;
		}

		// allocate size of original, will be more than enough for uncompressed resource
		dwDirectorySize = ValidAlignment(dwDirectorySize, 0x200);

		// Reset some counters
		dwIconCount = 0;
		dwGroupIconCount = 0;

		if(dwDirectorySize > 0)
		{
			DWORD dwNewResourceSectionSize = dwDirectorySize + dwIconSize + dwGroupIconSize + dwVersionSize + dwManifestSize;

			dwNewResourceSectionSize = ValidAlignment(dwNewResourceSectionSize, 0x200);

			DWORD dwNewResourceSectionPosition = 0;
			BYTE* lpbNewResourceSection = (BYTE*)malloc(dwNewResourceSectionSize);
			memset(lpbNewResourceSection, 0, dwNewResourceSectionSize);
			memcpy(lpbNewResourceSection, pRootDirectory, dwDirectorySize);
			dwNewResourceSectionPosition += dwDirectorySize;

			pRootDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)lpbNewResourceSection);

			nDirectoryCount = pRootDirectory->NumberOfIdEntries + pRootDirectory->NumberOfNamedEntries;

			pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pRootDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

			// Third pass
			for(i = 0; i < nDirectoryCount; i++)
			{
				if(pEntry->DataIsDirectory)
				{
					pSubDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pEntry->OffsetToDirectory + (DWORD)pRootDirectory);

					int nSubDirectoryCount = pSubDirectory->NumberOfIdEntries + pSubDirectory->NumberOfNamedEntries;

					pSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

					for(j = 0; j < nSubDirectoryCount; j++)
					{
						if(pSubEntry->DataIsDirectory)
						{
							pSubSubDirectory = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pSubEntry->OffsetToDirectory + (DWORD)pRootDirectory);

							int nSubSubDirectoryCount = pSubSubDirectory->NumberOfIdEntries + pSubSubDirectory->NumberOfNamedEntries;

							pSubSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubSubDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY));

							for(k = 0; k < nSubSubDirectoryCount; k++)
							{
								if(pSubSubEntry->DataIsDirectory)
								{
									// recurse again!
									//printf("Shouldn't normally get here, resource directory is already 3 levels deep!\n");
								}
								else
								{
									if(!(pSubSubEntry->OffsetToData & 0x8000000))
									{
										pDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRootDirectory + pSubSubEntry->OffsetToData);
										DWORD dwFileOffset = 0;

										switch(pEntry->Name)
										{
										case RT_ICON:
											if(dwIconCount < lpGroupIconDir->idCount)
											{
												dwFileOffset = RVAToOffset(pDataEntry->OffsetToData, lpMapping);
												memcpy((LPVOID)((DWORD)lpbNewResourceSection + dwNewResourceSectionPosition), (LPVOID)((DWORD)lpMapping + dwFileOffset), pDataEntry->Size);
												
												// blank original to enable higher compression
												memset((LPVOID)((DWORD)lpMapping + dwFileOffset), 0, pDataEntry->Size);

												pDataEntry->OffsetToData = OffsetToRVA(pSecHdr->PointerToRawData + dwNewResourceSectionPosition, lpMapping);

												dwNewResourceSectionPosition += pDataEntry->Size;

												dwIconCount++;
											}
											break;
										case RT_GROUP_ICON:
											if(dwGroupIconCount < lpGroupIconDir->idCount)
											{
												dwFileOffset = RVAToOffset(pDataEntry->OffsetToData, lpMapping);
												memcpy((LPVOID)((DWORD)lpbNewResourceSection + dwNewResourceSectionPosition), (LPVOID)((DWORD)lpMapping + dwFileOffset), pDataEntry->Size);
												
												// blank original to enable higher compression
												memset((LPVOID)((DWORD)lpMapping + dwFileOffset), 0, pDataEntry->Size);

												pDataEntry->OffsetToData = OffsetToRVA(pSecHdr->PointerToRawData + dwNewResourceSectionPosition, lpMapping);

												dwNewResourceSectionPosition += pDataEntry->Size;

												dwGroupIconCount++;
											}
											break;
										case RT_VERSION:
										case 24:	// RT_MANIFEST
											dwFileOffset = RVAToOffset(pDataEntry->OffsetToData, lpMapping);
											memcpy((LPVOID)((DWORD)lpbNewResourceSection + dwNewResourceSectionPosition), (LPVOID)((DWORD)lpMapping + dwFileOffset), pDataEntry->Size);
											
											// blank original to enable higher compression
											memset((LPVOID)((DWORD)lpMapping + dwFileOffset), 0, pDataEntry->Size);

											pDataEntry->OffsetToData = OffsetToRVA(pSecHdr->PointerToRawData + dwNewResourceSectionPosition, lpMapping);

											dwNewResourceSectionPosition += pDataEntry->Size;
											break;
										}
									}
								}

								pSubSubEntry++;
							}
						}

						pSubEntry++;
					}
				}

				pEntry++;
			}

			*dwOutputSize = dwNewResourceSectionSize;
			return lpbNewResourceSection;
		}
	}

	*dwOutputSize = 0;
	return 0;
}

int CCompressDlg::CallbackFunc(size_t length, size_t input, size_t output, void* cbparam)
{
	CCompressDlg* pDlg = (CCompressDlg*)cbparam;

	double fPercent = 0.0;
	fPercent = ((double)(input + pDlg->m_ProgressOffset)/(double)(pDlg->m_ProgressFileSize))*100;
	
	if(fPercent > 100)
		fPercent = 100.0;

	pDlg->m_ctrlCompressionProgress.SetPos((int)fPercent);
	pDlg->m_CompressionProgress.Format("%.02f%%", fPercent);

	pDlg->UpdateData(FALSE);

	return 1;
}

void CCompressDlg::OnCompress() 
{
	UpdateData(TRUE);

	m_ctrlCompressionProgress.SetPos(0);
	m_ctrlCompessedSize.SetPos(0);

	Encrypt((char*)m_SourceFile.GetBuffer(0));	
}

BOOL CCompressDlg::OnInitDialog() 
{
	CTexturedDlg::OnInitDialog();

	m_ctrlCompressionProgress.SetStep(1);
	m_ctrlCompressionProgress.SetRange(0, 100);
	m_ctrlCompressionProgress.SetPos(0);

	m_CompressionProgress = "";

	m_ctrlCompessedSize.SetStep(1);
	m_ctrlCompessedSize.SetRange(0, 100);
	m_ctrlCompessedSize.SetPos(0);

	m_FileSize = "";
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
