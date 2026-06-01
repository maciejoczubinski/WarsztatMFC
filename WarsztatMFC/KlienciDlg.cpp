#include "pch.h"
#include "WarsztatMFC.h"
#include "KlienciDlg.h"

IMPLEMENT_DYNAMIC(CKlienciDlg, CDialogEx)

CKlienciDlg::CKlienciDlg(CWnd* pParent)
    : CDialogEx(IDD_KLIENCI_DIALOG, pParent) {
}

void CKlienciDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_KLIENCI, m_list);
}

BEGIN_MESSAGE_MAP(CKlienciDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CKlienciDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list.InsertColumn(0, _T("Nazwa"), LVCFMT_LEFT, 180);
    m_list.InsertColumn(1, _T("Telefon"), LVCFMT_LEFT, 120);
    m_list.InsertColumn(2, _T("Miejscowoœæ"), LVCFMT_LEFT, 140);
    m_list.InsertColumn(3, _T("Ulica"), LVCFMT_LEFT, 125);
    RefreshList();
    return TRUE;
}

void CKlienciDlg::RefreshList()
{
    m_list.DeleteAllItems();
    auto klienci = g_pDB->GetKlienci();
    int i = 0;
    for (const auto& k : klienci) {
        m_list.InsertItem(i, k.nazwa);
        m_list.SetItemText(i, 1, k.nrTelefonu);
        m_list.SetItemText(i, 2, k.miejscowosc);
        m_list.SetItemText(i, 3, k.ulica);
        ++i;
    }
}