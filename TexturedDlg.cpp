// TexturedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TexturedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompressDlg dialog


CTexturedDlg::CTexturedDlg(int IDD, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CTexturedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTexturedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTexturedDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTexturedDlg, CDialog)
	//{{AFX_MSG_MAP(CTexturedDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTexturedDlg message handlers
BOOL CTexturedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HINSTANCE hDll = LoadLibrary("UxTheme.dll");
	
	if(hDll==NULL)
	{
		// The DLL won't be available on anything except Windows XP
		return TRUE;
	}

	ULONG (PASCAL *lpfnEnableTheme)(HWND, DWORD);
	(FARPROC&)lpfnEnableTheme = GetProcAddress(hDll,"EnableThemeDialogTexture");
	
	if(lpfnEnableTheme)
	{
		// ETDT_ENABLETAB = (ETDT_ENABLE | ETDT_USETABTEXTURE) = (2|4) = 6
		lpfnEnableTheme(this->GetSafeHwnd(), 6);
	}
	
	FreeLibrary(hDll);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
