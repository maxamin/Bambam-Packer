; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCompressDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "bambam.h"
LastPage=0

ClassCount=7
Class1=CBambamApp
Class2=CAboutDlg
Class3=CBambamDlg
Class4=CCompressDlg
Class5=COptionsDlg
Class6=CTexturedDlg

ResourceCount=6
Resource1=IDD_COMPRESS
Resource2=IDD_COMPRESS_DIALOG
Resource3=IDD_ABOUTBOX (English (U.S.))
Resource4=IDD_OPTIONS
Resource5=IDD_MAIN_DIALOG (English (U.S.))
Class7=LogDlg
Resource6=IDD_OPTIONS_DIALOG

[CLS:CBambamApp]
Type=0
BaseClass=CWinApp
HeaderFile=bambam.h
ImplementationFile=bambam.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=bambamDlg.cpp
ImplementationFile=bambamDlg.cpp
LastObject=CAboutDlg
Filter=D
VirtualFilter=dWC

[CLS:CBambamDlg]
Type=0
BaseClass=CDialog
HeaderFile=bambamDlg.h
ImplementationFile=bambamDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CBambamDlg

[CLS:CCompressDlg]
Type=0
BaseClass=CDialog
HeaderFile=CompressDlg.h
ImplementationFile=CompressDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_EXIT

[CLS:COptionsDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsDlg.h
ImplementationFile=OptionsDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_REMOVE_RELOC

[CLS:CTexturedDlg]
Type=0
BaseClass=CDialog
HeaderFile=TexturedDlg.h
ImplementationFile=TexturedDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CTexturedDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_MAIN_DIALOG]
Type=1
Class=CBambamDlg

[DLG:IDD_COMPRESS]
Type=1
Class=CCompressDlg
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352

[DLG:IDD_OPTIONS]
Type=1
Class=COptionsDlg
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352

[DLG:IDD_MAIN_DIALOG (English (U.S.))]
Type=1
Class=CBambamDlg
ControlCount=1
Control1=IDC_TAB,SysTabControl32,1342177280

[DLG:IDD_COMPRESS_DIALOG]
Type=1
Class=CCompressDlg
ControlCount=12
Control1=IDC_SOURCE_FILE,edit,1350631424
Control2=IDC_BROWSE,button,1342242816
Control3=IDC_COMPRESS,button,1342242816
Control4=IDC_EXIT,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_COMP_PROGRESS,msctls_progress32,1342177281
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_COMP_SIZE,msctls_progress32,1342177281
Control10=IDC_FILE_SIZE,static,1342308352
Control11=IDC_COMP_PROG,static,1342308352
Control12=IDC_STATIC,button,1342177287

[DLG:IDD_OPTIONS_DIALOG]
Type=1
Class=COptionsDlg
ControlCount=12
Control1=IDC_BACKUP,button,1342242819
Control2=IDC_REMOVE_RELOC,button,1342242819
Control3=IDC_REALIGN,button,1342242819
Control4=IDC_SECTION_NAME,edit,1350631552
Control5=IDC_RANDOM,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,button,1342177287
Control9=IDC_CHECK1,button,1073807363
Control10=IDC_CHECK2,button,1073807363
Control11=IDC_CHECK3,button,1073807363
Control12=IDC_CHECK4,button,1073807363

[CLS:LogDlg]
Type=0
HeaderFile=LogDlg.h
ImplementationFile=LogDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CLEAR

