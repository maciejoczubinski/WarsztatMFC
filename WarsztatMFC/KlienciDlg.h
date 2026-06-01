#pragma once
#include "Database.h"
#include "afxcmn.h"

class CKlienciDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CKlienciDlg)
public:
    CKlienciDlg(CWnd* pParent = nullptr);
    enum { IDD = IDD_KLIENCI_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    DECLARE_MESSAGE_MAP()
private:
    CListCtrl m_list;
    void RefreshList();
};