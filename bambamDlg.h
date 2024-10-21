// bambamDlg.h : header file
//

#if !defined(AFX_BAMBAMDLG_H__2EB31319_3E04_4840_B84D_DAF23AB2536B__INCLUDED_)
#define AFX_BAMBAMDLG_H__2EB31319_3E04_4840_B84D_DAF23AB2536B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CompressDlg.h"
#include "OptionsDlg.h"
#include "LogDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CBambamDlg dialog

class CBambamDlg : public CDialog
{
// Construction
public:
	CBambamDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBambamDlg)
	enum { IDD = IDD_MAIN_DIALOG };
	CTabCtrl	m_ctrlTab;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBambamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBambamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPtrArray m_DlgArray;
	CCompressDlg m_CompressDlg;
	COptionsDlg m_OptionsDlg;
	CLogDlg m_LogDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAMBAMDLG_H__2EB31319_3E04_4840_B84D_DAF23AB2536B__INCLUDED_)
