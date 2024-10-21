#if !defined(AFX_OPTIONSDLG_H__5E27B84B_27F6_494F_8861_5F4EB1F9BF4B__INCLUDED_)
#define AFX_OPTIONSDLG_H__5E27B84B_27F6_494F_8861_5F4EB1F9BF4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TexturedDlg.h"

// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CTexturedDlg
{
// Construction
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS_DIALOG };
	BOOL	m_Backup;
	CString	m_SectionName;
	BOOL	m_Realign;
	BOOL	m_StripReloc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRandomSectionName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__5E27B84B_27F6_494F_8861_5F4EB1F9BF4B__INCLUDED_)
