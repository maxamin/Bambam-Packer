// bambam.h : main header file for the BAMBAM application
//

#if !defined(AFX_BAMBAM_H__BB7A89B2_C7D9_4E94_8495_A6E8F04D541A__INCLUDED_)
#define AFX_BAMBAM_H__BB7A89B2_C7D9_4E94_8495_A6E8F04D541A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBambamApp:
// See bambam.cpp for the implementation of this class
//

class CBambamApp : public CWinApp
{
public:
	CBambamApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBambamApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBambamApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAMBAM_H__BB7A89B2_C7D9_4E94_8495_A6E8F04D541A__INCLUDED_)
