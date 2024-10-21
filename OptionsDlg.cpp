// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bambam.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CTexturedDlg(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_Backup = FALSE;
	m_SectionName = _T("");
	m_Realign = FALSE;
	m_StripReloc = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Check(pDX, IDC_BACKUP, m_Backup);
	DDX_Text(pDX, IDC_SECTION_NAME, m_SectionName);
	DDV_MaxChars(pDX, m_SectionName, 8);
	DDX_Check(pDX, IDC_REALIGN, m_Realign);
	DDX_Check(pDX, IDC_REMOVE_RELOC, m_StripReloc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_RANDOM, OnRandomSectionName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COptionsDlg::OnInitDialog() 
{
	CTexturedDlg::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_SectionName = ".bedrock";
	m_Backup = TRUE;
	m_Realign = TRUE;
	m_StripReloc = TRUE;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnRandomSectionName() 
{
	int randomNumber = 0;
	int length = 0;
	CString randomString;
	
	srand(GetTickCount());

	do
	{	
		length = rand();
	}
	while((length < 1) || (length > 8));

	for(int i=0; i<length; i++)
	{
		do
		{
			randomNumber = rand();
		}
		while((randomNumber < 20) || (randomNumber > 127));

		randomString += randomNumber;
	}

	m_SectionName = randomString;

	UpdateData(FALSE);
}
