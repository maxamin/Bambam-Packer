// bambamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bambam.h"
#include "bambamDlg.h"
#include "minifmod.h"
#include "fmod_callbacks.h"
#include "TexturedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CTexturedDlg
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	FMUSIC_MODULE *m_mod;
};

CAboutDlg::CAboutDlg() : CTexturedDlg(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBambamDlg dialog

CBambamDlg::CBambamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBambamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBambamDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBambamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBambamDlg)
	DDX_Control(pDX, IDC_TAB, m_ctrlTab);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBambamDlg, CDialog)
	//{{AFX_MSG_MAP(CBambamDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBambamDlg message handlers

BOOL CBambamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_ctrlTab.InsertItem(0, "Compress");	
	m_CompressDlg.Create(IDD_COMPRESS_DIALOG, &m_ctrlTab);
	m_DlgArray.Add(&m_CompressDlg);

	m_ctrlTab.InsertItem(1, "Options");
	m_OptionsDlg.Create(IDD_OPTIONS_DIALOG, &m_ctrlTab);
	m_DlgArray.Add(&m_OptionsDlg);

	m_CompressDlg.m_pOptionsDlg = &m_OptionsDlg;

	//m_ctrlTab.InsertItem(2, "Debug Log");
	//m_LogDlg.Create(IDD_LOG_DIALOG, &m_ctrlTab);
	//m_DlgArray.Add(&m_LogDlg);
	
	CRect rectTabClient;
	CRect rectTab;

	m_ctrlTab.GetClientRect(&rectTabClient);
	m_ctrlTab.GetItemRect(0, &rectTab);

	// Adjust rect for dialog placement
	rectTabClient.top += rectTab.Height() + 4;
	rectTabClient.left += 4;

	for(int i=m_DlgArray.GetSize()-1; i>=0; i--)
	{
		CDialog* pDlg;
		pDlg = (CDialog*)m_DlgArray.GetAt(i);

		pDlg->SetWindowPos(&CWnd::wndTopMost, rectTabClient.left, rectTabClient.top, rectTabClient.Width()-4, rectTabClient.Height()-4, SWP_NOZORDER);
		
		if(i>0)
			pDlg->ShowWindow(SW_HIDE);
		else
			pDlg->ShowWindow(SW_SHOW);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBambamDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBambamDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBambamDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBambamDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CDialog* pDlg;

	m_ctrlTab.LockWindowUpdate();

	// Hide all tab dialogs
	for(int i=0; i<m_DlgArray.GetSize(); i++)
	{
		pDlg = (CDialog*)m_DlgArray.GetAt(i);
		pDlg->ShowWindow(SW_HIDE);
	}

	// Show the selected tab dialog
	pDlg = (CDialog*)m_DlgArray.GetAt(m_ctrlTab.GetCurSel());
	pDlg->ShowWindow(SW_SHOW);

	m_ctrlTab.UnlockWindowUpdate();
	
	*pResult = 0;
}

BOOL CAboutDlg::OnInitDialog() 
{
	CTexturedDlg::OnInitDialog();

	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
	
	m_mod = FMUSIC_LoadSong((char*)IDR_XM_MOD, NULL);

	if(m_mod)
	{
		FMUSIC_PlaySong(m_mod);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnOK() 
{
	if(m_mod)
	{
		FMUSIC_FreeSong(m_mod);
		m_mod = 0;
	}

	CDialog::OnOK();
}

void CBambamDlg::OnClose() 
{
	CDialog::OnOK();
}

void CBambamDlg::OnCancel() 
{
}

void CBambamDlg::OnOK() 
{
}