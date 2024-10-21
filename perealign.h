#ifndef _PEREALIGN_H_
#define _PEREALIGN_H_

DWORD ValidAlignment(DWORD dwSize, DWORD dwAlignment);
BOOL IsRoundedTo(DWORD dwTarNum, DWORD dwRoundNum);
DWORD RealignPE(LPVOID lpMapping, DWORD dwFileSize);
DWORD StripReloc(LPVOID lpMapping);

#endif _PEREALIGN_H_