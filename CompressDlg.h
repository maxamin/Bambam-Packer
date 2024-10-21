#if !defined(AFX_COMPRESSDLG_H__B276B856_6324_4305_848B_21469952FF1F__INCLUDED_)
#define AFX_COMPRESSDLG_H__B276B856_6324_4305_848B_21469952FF1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TexturedDlg.h"
#include "OptionsDlg.h"

typedef int (*PFNAPLIBCALLBACK)(size_t, size_t, size_t, void *);

// CompressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompressDlg dialog

class CCompressDlg : public CTexturedDlg
{
// Construction
public:
	COptionsDlg* m_pOptionsDlg;
	CCompressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompressDlg)
	enum { IDD = IDD_COMPRESS_DIALOG };
	CProgressCtrl	m_ctrlCompessedSize;
	CProgressCtrl	m_ctrlCompressionProgress;
	CString	m_SourceFile;
	CString	m_CompressionProgress;
	CString	m_FileSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompressDlg)
	afx_msg void OnBrowse();
	afx_msg void OnExit();
	afx_msg void OnCompress();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int Encrypt(char* szFile);
	int TranslateAddresses(BYTE* pBuffer, DWORD dwLength, DWORD dwSourceVA, DWORD dwDestinationVA, DWORD dwRVAPage);
	int PatternSearch(unsigned char *SearchString, int StringLen, unsigned char *SearchBuff, int BuffSize);
	int CalcStubSize(PIMAGE_DOS_HEADER pDosHdr);
	void PerformStubRelocations(LPVOID lpMapping, DWORD dwStubVA);
	//void CompressSections(LPVOID lpMapping);
	void CompressSections(LPVOID lpMapping, LPVOID lpBackup);
	LPVOID Compress(LPVOID lpInputBuffer, DWORD dwInputSize, DWORD* dwOutputSize, PFNAPLIBCALLBACK pfnCallback);
	LPVOID BuildNewResourceSection(LPVOID lpMapping, DWORD* dwOutputSize);
	
	static int CallbackFunc(size_t length, size_t input, size_t output, void* cbparam);

	int m_ProgressFileSize;
	int m_ProgressOffset;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPRESSDLG_H__B276B856_6324_4305_848B_21469952FF1F__INCLUDED_)
