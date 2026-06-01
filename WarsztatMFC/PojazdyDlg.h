#pragma once
#include "Database.h"
#include "afxcmn.h"

class CPojazdyDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CPojazdyDlg)
public:
    CPojazdyDlg(CWnd* pParent = nullptr);
    enum { IDD = IDD_POJAZDY_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    DECLARE_MESSAGE_MAP()
private:
    CListCtrl m_list;
    void RefreshList();
public:
    afx_msg void OnLvnItemchangedListPojazdy(NMHDR* pNMHDR, LRESULT* pResult);
};