#pragma once
#include "Database.h"
#include "afxcmn.h"

class CZlecenieDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CZlecenieDlg)
public:
    CZlecenieDlg(CWnd* pParent = nullptr, int idZlecenia = -1);

    enum { IDD = IDD_ZLECENIE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;
    DECLARE_MESSAGE_MAP()

private:
    int      m_idZlecenia;
    Zlecenie m_zlecenie;
    std::vector<PozycjaZlecenia> m_pozycje;

    // Pojazd
    CComboBox m_cmbMarka;
    CComboBox m_cmbModel;
    CComboBox m_cmbWersja;
    CEdit     m_editNrRej;
    CEdit     m_editRokProd;

    // Klient
    CEdit m_editNazwa;
    CEdit m_editTelefon;
    CEdit m_editMiejscowosc;
    CEdit m_editUlica;

    // Pozycje
    CListCtrl m_listPozycje;
    CComboBox m_cmbKatalog;
    CEdit     m_editNazwaPoz;
    CEdit     m_editKosztPoz;
    CStatic   m_lblSuma;
    CStatic   m_lblNumer;

    std::vector<Marka>           m_marki;
    std::vector<Model>           m_modele;
    std::vector<WersjaSilnikowa> m_wersje;
    std::vector<KatalogUsluga>   m_katalog;

    void LoadMarki();
    void LoadModele(int idMarki);
    void LoadWersje(int idModelu);
    void LoadKatalog();
    void FillPozycjeList();
    void UpdateSuma();
    bool Validate();
    bool ResolveKlientPojazd(int& outIdKlienta, int& outIdPojazdu);

public:
    afx_msg void OnCbnSelchangeCmbMarka();
    afx_msg void OnCbnSelchangeCmbModel();
    afx_msg void OnCbnSelchangeCmbKatalog();
    afx_msg void OnBnClickedBtnDodajPoz();
    afx_msg void OnBnClickedBtnUsunPoz();
    afx_msg void OnBnClickedBtnZapisz();
    afx_msg void OnBnClickedBtnAnuluj();
    afx_msg void OnLvnItemchangedListPozycje(NMHDR* pNMHDR, LRESULT* pResult);
};
