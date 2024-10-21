#if !defined(AFX_TEXTUREDDLG_H__B276B856_6324_4305_848B_21469952FF1F__INCLUDED_)
#define AFX_TEXTUREDDLG_H__B276B856_6324_4305_848B_21469952FF1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexturedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTexturedDlg dialog

class CTexturedDlg : public CDialog
{
// Construction
public:
	CTexturedDlg(int IDD, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTexturedDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexturedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTexturedDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREDDLG_H__B276B856_6324_4305_848B_21469952FF1F__INCLUDED_)
