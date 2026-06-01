#pragma once
#include "Database.h"
#include "afxcmn.h"

class CWarsztatMFCDlg : public CDialogEx
{
public:
    CWarsztatMFCDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WARSZTATMFC_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    DECLARE_MESSAGE_MAP()

private:
    HICON       m_hIcon;
    CListCtrl   m_listZlecenia;
    std::vector<Zlecenie> m_zlecenia;

    void SetupListCtrl();
    void RefreshList();
    int  GetSelectedZlecenieId();
    CEdit m_editSzukaj;

public:
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedBtnDodaj();
    afx_msg void OnBnClickedBtnEdytuj();
    afx_msg void OnBnClickedBtnUsun();
    afx_msg void OnBnClickedBtnPojazdy();
    afx_msg void OnBnClickedBtnKlienci();
    afx_msg void OnBnClickedBtnStatus();
    afx_msg void OnNMCustomdrawListZlecenia(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMDblclkListZlecenia(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedBtnSzukaj();
    afx_msg void OnBnClickedBtnWyczysc();
    afx_msg void OnBnClickedBtnPdf();
};