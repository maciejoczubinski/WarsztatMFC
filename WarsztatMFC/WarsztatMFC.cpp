#include "pch.h"
#include "framework.h"
#include "WarsztatMFC.h"
#include "WarsztatMFCDlg.h"
#include "Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWarsztatMFCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CWarsztatMFCApp::CWarsztatMFCApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CWarsztatMFCApp theApp;

BOOL CWarsztatMFCApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Aplikacje lokalne wygenerowane przez kreatora aplikacji"));

	CString connStr =
		_T("Driver={ODBC Driver 17 for SQL Server};")
		_T("Server=localhost\\SQLEXPRESS01;")
		_T("Database=IntegraSerwis;")
		_T("Trusted_Connection=yes;");

	g_pDB = new CDatabase2();
	if (!g_pDB->Connect(connStr)) {
		AfxMessageBox(
			_T("Nie można połączyć się z bazą danych!\n")
			_T("Sprawdź czy SQL Server jest uruchomiony."),
			MB_ICONERROR);
		delete g_pDB;
		g_pDB = nullptr;
		return FALSE;
	}

	CWarsztatMFCDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Ostrzeżenie: tworzenie okna dialogowego nie powiodło się, więc praca aplikacji została nieoczekiwanie przerwana.\n");
		TRACE(traceAppMsg, 0, "Ostrzeżenie: jeśli używasz kontrolek MFC w oknie dialogowym, nie możesz użyć instrukcji #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}

