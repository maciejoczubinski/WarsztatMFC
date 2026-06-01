#include "pch.h"
#include "framework.h"
#include "WarsztatMFC.h"
#include "WarsztatMFCDlg.h"
#include "ZlecenieDlg.h"
#include "PojazdyDlg.h"
#include "KlienciDlg.h"
#include "PdfExport.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CWarsztatMFCDlg::CWarsztatMFCDlg(CWnd* pParent)
    : CDialogEx(IDD_WARSZTATMFC_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWarsztatMFCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ZLECENIA, m_listZlecenia);
    DDX_Control(pDX, IDC_EDIT_SZUKAJ, m_editSzukaj);
}

BEGIN_MESSAGE_MAP(CWarsztatMFCDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_DODAJ, &CWarsztatMFCDlg::OnBnClickedBtnDodaj)
    ON_BN_CLICKED(IDC_BTN_EDYTUJ, &CWarsztatMFCDlg::OnBnClickedBtnEdytuj)
    ON_BN_CLICKED(IDC_BTN_USUN, &CWarsztatMFCDlg::OnBnClickedBtnUsun)
    ON_BN_CLICKED(IDC_BTN_POJAZDY, &CWarsztatMFCDlg::OnBnClickedBtnPojazdy)
    ON_BN_CLICKED(IDC_BTN_KLIENCI, &CWarsztatMFCDlg::OnBnClickedBtnKlienci)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_ZLECENIA, &CWarsztatMFCDlg::OnNMDblclkListZlecenia)
    ON_BN_CLICKED(IDC_BTN_STATUS, &CWarsztatMFCDlg::OnBnClickedBtnStatus)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_ZLECENIA, &CWarsztatMFCDlg::OnNMCustomdrawListZlecenia)
    ON_BN_CLICKED(IDC_BTN_SZUKAJ, &CWarsztatMFCDlg::OnBnClickedBtnSzukaj)
    ON_BN_CLICKED(IDC_BTN_WYCZYSC, &CWarsztatMFCDlg::OnBnClickedBtnWyczysc)
    ON_BN_CLICKED(IDC_BTN_PDF, &CWarsztatMFCDlg::OnBnClickedBtnPdf)
END_MESSAGE_MAP()

BOOL CWarsztatMFCDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    SetupListCtrl();
    RefreshList();

    return TRUE;
}

void CWarsztatMFCDlg::SetupListCtrl()
{
    m_listZlecenia.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listZlecenia.InsertColumn(0, _T("Nr zlecenia"), LVCFMT_LEFT, 100);
    m_listZlecenia.InsertColumn(1, _T("Marka/Model"), LVCFMT_LEFT, 160);
    m_listZlecenia.InsertColumn(2, _T("Nr rej."), LVCFMT_LEFT, 100);
    m_listZlecenia.InsertColumn(3, _T("Klient"), LVCFMT_LEFT, 180);
    m_listZlecenia.InsertColumn(4, _T("Koszt"), LVCFMT_RIGHT, 90);
    m_listZlecenia.InsertColumn(5, _T("Data"), LVCFMT_LEFT, 130);
    m_listZlecenia.InsertColumn(6, _T("Status"), LVCFMT_LEFT, 90);
}

void CWarsztatMFCDlg::RefreshList()
{
    m_listZlecenia.DeleteAllItems();
    if (!g_pDB || !g_pDB->IsConnected()) {
        AfxMessageBox(_T("Brak połączenia z bazą danych."), MB_ICONWARNING);
        return;
    }
    m_zlecenia = g_pDB->GetZlecenia();
    int i = 0;
    for (const auto& z : m_zlecenia) {
        m_listZlecenia.InsertItem(i, z.numerZlecenia);
        CString markaModel;
        markaModel.Format(_T("%s %s"), z.marka.GetString(), z.model.GetString());
        m_listZlecenia.SetItemText(i, 1, markaModel);
        m_listZlecenia.SetItemText(i, 2, z.nrRejestracyjny);
        m_listZlecenia.SetItemText(i, 3, z.nazwaKlienta);
        CString koszt;
        koszt.Format(_T("%.2f zl"), z.kosztCalkowity);
        m_listZlecenia.SetItemText(i, 4, koszt);
        m_listZlecenia.SetItemText(i, 5, z.dataUtworzenia);
        CString status;
        switch (z.status) {
        case 0: status = _T("Nowe");     break;
        case 1: status = _T("W trakcie"); break;
        case 2: status = _T("Gotowe");   break;
        default: status = _T("Nowe");    break;
        }
        m_listZlecenia.SetItemText(i, 6, status);
        m_listZlecenia.SetItemData(i, (DWORD_PTR)z.idZlecenia);
        ++i;
    }
}

int CWarsztatMFCDlg::GetSelectedZlecenieId()
{
    int sel = m_listZlecenia.GetNextItem(-1, LVNI_SELECTED);
    if (sel < 0) return -1;
    return (int)m_listZlecenia.GetItemData(sel);
}

void CWarsztatMFCDlg::OnBnClickedBtnDodaj()
{
    CZlecenieDlg dlg(this);
    if (dlg.DoModal() == IDOK)
        RefreshList();
}

void CWarsztatMFCDlg::OnBnClickedBtnEdytuj()
{
    int id = GetSelectedZlecenieId();
    if (id < 0) {
        AfxMessageBox(_T("Wybierz zlecenie z listy."), MB_ICONINFORMATION);
        return;
    }
    CZlecenieDlg dlg(this, id);
    if (dlg.DoModal() == IDOK)
        RefreshList();
}

void CWarsztatMFCDlg::OnBnClickedBtnUsun()
{
    int id = GetSelectedZlecenieId();
    if (id < 0) {
        AfxMessageBox(_T("Wybierz zlecenie z listy."), MB_ICONINFORMATION);
        return;
    }
    if (AfxMessageBox(_T("Czy na pewno chcesz usunąć wybrane zlecenie?"),
        MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        g_pDB->DeleteZlecenie(id);
        RefreshList();
    }
}

void CWarsztatMFCDlg::OnBnClickedBtnPojazdy()
{
    CPojazdyDlg dlg(this);
    dlg.DoModal();
}

void CWarsztatMFCDlg::OnBnClickedBtnKlienci()
{
    CKlienciDlg dlg(this);
    dlg.DoModal();
}

void CWarsztatMFCDlg::OnNMDblclkListZlecenia(NMHDR* pNMHDR, LRESULT* pResult)
{
    OnBnClickedBtnEdytuj();
    *pResult = 0;
}

void CWarsztatMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CWarsztatMFCDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CWarsztatMFCDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CWarsztatMFCDlg::OnBnClickedBtnStatus()
{
    int id = GetSelectedZlecenieId();
    if (id < 0) {
        AfxMessageBox(_T("Wybierz zlecenie z listy."), MB_ICONINFORMATION);
        return;
    }

    int aktualnyStatus = 0;
    int sel = m_listZlecenia.GetNextItem(-1, LVNI_SELECTED);
    for (const auto& z : m_zlecenia) {
        if (z.idZlecenia == id) {
            aktualnyStatus = z.status;
            break;
        }
    }

    int nowyStatus = (aktualnyStatus + 1) % 3;

    g_pDB->UpdateStatusZlecenia(id, nowyStatus);
    RefreshList();
}

void CWarsztatMFCDlg::OnNMCustomdrawListZlecenia(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT) {
        *pResult = CDRF_NOTIFYITEMDRAW;
        return;
    }

    if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
        int idx = (int)pLVCD->nmcd.dwItemSpec;
        if (idx >= 0 && idx < (int)m_zlecenia.size()) {
            switch (m_zlecenia[idx].status) {
            case 0: 
                pLVCD->clrTextBk = RGB(255, 180, 180);
                break;
            case 1: 
                pLVCD->clrTextBk = RGB(255, 220, 150);
                break;
            case 2: 
                pLVCD->clrTextBk = RGB(180, 255, 180);
                break;
            }
        }
        *pResult = CDRF_DODEFAULT;
        return;
    }
}

void CWarsztatMFCDlg::OnBnClickedBtnSzukaj()
{
    CString szukaj;
    m_editSzukaj.GetWindowText(szukaj);
    szukaj.Trim();

    if (szukaj.IsEmpty()) {
        AfxMessageBox(_T("Wpisz tekst do wyszukania."), MB_ICONINFORMATION);
        return;
    }

    CString szukajLower = szukaj;
    szukajLower.MakeLower();

    m_listZlecenia.DeleteAllItems();
    int i = 0;
    bool znaleziono = false;

    for (const auto& z : m_zlecenia) {
        CString klient = z.nazwaKlienta;
        CString nrRej = z.nrRejestracyjny;
        klient.MakeLower();
        nrRej.MakeLower();

        if (klient.Find(szukajLower) >= 0 || nrRej.Find(szukajLower) >= 0) {
            m_listZlecenia.InsertItem(i, z.numerZlecenia);
            CString markaModel;
            markaModel.Format(_T("%s %s"), z.marka.GetString(), z.model.GetString());
            m_listZlecenia.SetItemText(i, 1, markaModel);
            m_listZlecenia.SetItemText(i, 2, z.nrRejestracyjny);
            m_listZlecenia.SetItemText(i, 3, z.nazwaKlienta);
            CString koszt;
            koszt.Format(_T("%.2f zl"), z.kosztCalkowity);
            m_listZlecenia.SetItemText(i, 4, koszt);
            m_listZlecenia.SetItemText(i, 5, z.dataUtworzenia);
            CString status;
            switch (z.status) {
            case 0: status = _T("Nowe");      break;
            case 1: status = _T("W trakcie"); break;
            case 2: status = _T("Gotowe");    break;
            }
            m_listZlecenia.SetItemText(i, 6, status);
            m_listZlecenia.SetItemData(i, (DWORD_PTR)z.idZlecenia);
            ++i;
            znaleziono = true;
        }
    }

    if (!znaleziono) {
        AfxMessageBox(_T("Nie znaleziono zlecenia dla podanego klienta lub numeru rejestracyjnego."), MB_ICONINFORMATION);
        RefreshList();
    }
}

void CWarsztatMFCDlg::OnBnClickedBtnWyczysc()
{
    m_editSzukaj.SetWindowText(_T(""));
    RefreshList();
}

void CWarsztatMFCDlg::OnBnClickedBtnPdf()
{
    int id = GetSelectedZlecenieId();
    if (id < 0) {
        AfxMessageBox(_T("Wybierz zlecenie z listy."), MB_ICONINFORMATION);
        return;
    }

    Zlecenie zlecenie;
    for (const auto& z : m_zlecenia) {
        if (z.idZlecenia == id) { zlecenie = z; break; }
    }

    auto pozycje = g_pDB->GetPozycjeZlecenia(id);

    Klient klient;
    auto klienci = g_pDB->GetKlienci();
    for (const auto& k : klienci) {
        if (k.idKlienta == zlecenie.idKlienta) { klient = k; break; }
    }

    Pojazd pojazd;
    auto pojazdy = g_pDB->GetPojazdy();
    for (const auto& p : pojazdy) {
        if (p.idPojazdu == zlecenie.idPojazdu) { pojazd = p; break; }
    }

    CPdfExport::GenerujFakture(zlecenie, pozycje, klient, pojazd);
}



