#include "pch.h"
#include "WarsztatMFC.h"
#include "PojazdyDlg.h"

IMPLEMENT_DYNAMIC(CPojazdyDlg, CDialogEx)

CPojazdyDlg::CPojazdyDlg(CWnd* pParent)
    : CDialogEx(IDD_POJAZDY_DIALOG, pParent) {
}

void CPojazdyDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_POJAZDY, m_list);
}

BEGIN_MESSAGE_MAP(CPojazdyDlg, CDialogEx)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_POJAZDY, &CPojazdyDlg::OnLvnItemchangedListPojazdy)
END_MESSAGE_MAP()

BOOL CPojazdyDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list.InsertColumn(0, _T("Nr rej."), LVCFMT_LEFT, 80);
    m_list.InsertColumn(1, _T("Marka"), LVCFMT_LEFT, 80);
    m_list.InsertColumn(2, _T("Model"), LVCFMT_LEFT, 80);
    m_list.InsertColumn(3, _T("Wersja silnikowa"), LVCFMT_LEFT, 130);
    m_list.InsertColumn(4, _T("Rok prod."), LVCFMT_LEFT, 80);
    m_list.InsertColumn(5, _T("W³aœciciel"), LVCFMT_LEFT, 130);
    RefreshList();
    return TRUE;
}

void CPojazdyDlg::RefreshList()
{
    m_list.DeleteAllItems();
    auto pojazdy = g_pDB->GetPojazdy();
    int i = 0;
    for (const auto& p : pojazdy) {
        m_list.InsertItem(i, p.nrRejestracyjny);
        m_list.SetItemText(i, 1, p.marka);
        m_list.SetItemText(i, 2, p.model);
        m_list.SetItemText(i, 3, p.wersjaSilnikowa);
        CString rok; rok.Format(_T("%d"), p.rokProdukcji);
        m_list.SetItemText(i, 4, rok);
        m_list.SetItemText(i, 5, p.nazwaKlienta);
        ++i;
    }
}
void CPojazdyDlg::OnLvnItemchangedListPojazdy(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;
}
