#if !defined(AFX_LOGDLG_H__DC2815E1_0148_4C4D_8408_40BED1A8590C__INCLUDED_)
#define AFX_LOGDLG_H__DC2815E1_0148_4C4D_8408_40BED1A8590C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TexturedDlg.h"

// LogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogDlg dialog

class CLogDlg : public CTexturedDlg
{
// Construction
public:
	CLogDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogDlg)
	enum { IDD = IDD_LOG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGDLG_H__DC2815E1_0148_4C4D_8408_40BED1A8590C__INCLUDED_)
