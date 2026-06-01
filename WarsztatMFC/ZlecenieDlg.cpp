#include "pch.h"
#include "WarsztatMFC.h"
#include "ZlecenieDlg.h"

IMPLEMENT_DYNAMIC(CZlecenieDlg, CDialogEx)

CZlecenieDlg::CZlecenieDlg(CWnd* pParent, int idZlecenia)
    : CDialogEx(IDD_ZLECENIE_DIALOG, pParent)
    , m_idZlecenia(idZlecenia)
{
}

void CZlecenieDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CMB_MARKA, m_cmbMarka);
    DDX_Control(pDX, IDC_CMB_MODEL, m_cmbModel);
    DDX_Control(pDX, IDC_CMB_WERSJA, m_cmbWersja);
    DDX_Control(pDX, IDC_EDIT_NRREJ, m_editNrRej);
    DDX_Control(pDX, IDC_EDIT_ROK, m_editRokProd);
    DDX_Control(pDX, IDC_EDIT_NAZWA, m_editNazwa);
    DDX_Control(pDX, IDC_EDIT_TELEFON, m_editTelefon);
    DDX_Control(pDX, IDC_EDIT_MIASTO, m_editMiejscowosc);
    DDX_Control(pDX, IDC_EDIT_ULICA, m_editUlica);
    DDX_Control(pDX, IDC_LIST_POZYCJE, m_listPozycje);
    DDX_Control(pDX, IDC_CMB_KATALOG, m_cmbKatalog);
    DDX_Control(pDX, IDC_EDIT_NAZWAPOZ, m_editNazwaPoz);
    DDX_Control(pDX, IDC_EDIT_KOSZTPOZ, m_editKosztPoz);
    DDX_Control(pDX, IDC_STATIC_SUMA, m_lblSuma);
    DDX_Control(pDX, IDC_STATIC_NUMER, m_lblNumer);
}

BEGIN_MESSAGE_MAP(CZlecenieDlg, CDialogEx)
    ON_CBN_SELCHANGE(IDC_CMB_MARKA, &CZlecenieDlg::OnCbnSelchangeCmbMarka)
    ON_CBN_SELCHANGE(IDC_CMB_MODEL, &CZlecenieDlg::OnCbnSelchangeCmbModel)
    ON_CBN_SELCHANGE(IDC_CMB_KATALOG, &CZlecenieDlg::OnCbnSelchangeCmbKatalog)
    ON_BN_CLICKED(IDC_BTN_DODAJ_POZ, &CZlecenieDlg::OnBnClickedBtnDodajPoz)
    ON_BN_CLICKED(IDC_BTN_USUN_POZ, &CZlecenieDlg::OnBnClickedBtnUsunPoz)
    ON_BN_CLICKED(IDC_BTN_ZAPISZ, &CZlecenieDlg::OnBnClickedBtnZapisz)
    ON_BN_CLICKED(IDC_BTN_ANULUJ, &CZlecenieDlg::OnBnClickedBtnAnuluj)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_POZYCJE, &CZlecenieDlg::OnLvnItemchangedListPozycje)
END_MESSAGE_MAP()

BOOL CZlecenieDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_listPozycje.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listPozycje.InsertColumn(0, _T("Nazwa us³ugi/towaru"), LVCFMT_LEFT, 380);
    m_listPozycje.InsertColumn(1, _T("Koszt"), LVCFMT_RIGHT, 90);

    LoadMarki();
    LoadKatalog();

    if (m_idZlecenia > 0) {
        SetWindowText(_T("Edycja zlecenia"));
        auto zlecenia = g_pDB->GetZlecenia();
        for (const auto& z : zlecenia) {
            if (z.idZlecenia == m_idZlecenia) { m_zlecenie = z; break; }
        }
        m_lblNumer.SetWindowText(m_zlecenie.numerZlecenia);

        auto pojazdy = g_pDB->GetPojazdy();
        for (const auto& p : pojazdy) {
            if (p.idPojazdu == m_zlecenie.idPojazdu) {
                m_editNrRej.SetWindowText(p.nrRejestracyjny);
                CString rok; rok.Format(_T("%d"), p.rokProdukcji);
                m_editRokProd.SetWindowText(rok);
                int markaIdx = m_cmbMarka.FindStringExact(-1, p.marka);
                if (markaIdx != CB_ERR) {
                    m_cmbMarka.SetCurSel(markaIdx);
                    OnCbnSelchangeCmbMarka();
                    int modelIdx = m_cmbModel.FindStringExact(-1, p.model);
                    if (modelIdx != CB_ERR) {
                        m_cmbModel.SetCurSel(modelIdx);
                        OnCbnSelchangeCmbModel();
                        int wersjaIdx = m_cmbWersja.FindStringExact(-1, p.wersjaSilnikowa);
                        if (wersjaIdx != CB_ERR)
                            m_cmbWersja.SetCurSel(wersjaIdx);
                    }
                }
                break;
            }
        }

        auto klienci = g_pDB->GetKlienci();
        for (const auto& k : klienci) {
            if (k.idKlienta == m_zlecenie.idKlienta) {
                m_editNazwa.SetWindowText(k.nazwa);
                m_editTelefon.SetWindowText(k.nrTelefonu);
                m_editMiejscowosc.SetWindowText(k.miejscowosc);
                m_editUlica.SetWindowText(k.ulica);
                break;
            }
        }

        m_pozycje = g_pDB->GetPozycjeZlecenia(m_idZlecenia);
        FillPozycjeList();
    }
    else {
        SetWindowText(_T("Nowe zlecenie"));
        m_lblNumer.SetWindowText(g_pDB->GenerujNumerZlecenia());
        UpdateSuma();
    }

    return TRUE;
}

void CZlecenieDlg::LoadMarki()
{
    m_cmbMarka.ResetContent();
    m_marki = g_pDB->GetMarki();
    for (const auto& m : m_marki)
        m_cmbMarka.AddString(m.nazwaMarki);
}

void CZlecenieDlg::LoadModele(int idMarki)
{
    m_cmbModel.ResetContent();
    m_cmbWersja.ResetContent();
    m_modele = g_pDB->GetModele(idMarki);
    for (const auto& m : m_modele)
        m_cmbModel.AddString(m.nazwaModelu);
}

void CZlecenieDlg::LoadWersje(int idModelu)
{
    m_cmbWersja.ResetContent();
    m_wersje = g_pDB->GetWersje(idModelu);
    for (const auto& w : m_wersje)
        m_cmbWersja.AddString(w.opisWersji);
}

void CZlecenieDlg::LoadKatalog()
{
    m_cmbKatalog.ResetContent();
    m_cmbKatalog.AddString(_T("-- wybierz z katalogu --"));
    m_katalog = g_pDB->GetKatalogUslug();
    for (const auto& u : m_katalog)
        m_cmbKatalog.AddString(u.nazwaUslug);
    m_cmbKatalog.SetCurSel(0);
}

void CZlecenieDlg::FillPozycjeList()
{
    m_listPozycje.DeleteAllItems();
    int i = 0;
    for (const auto& p : m_pozycje) {
        m_listPozycje.InsertItem(i, p.nazwaPozycji);
        CString koszt; koszt.Format(_T("%.2f zl"), p.koszt);
        m_listPozycje.SetItemText(i, 1, koszt);
        ++i;
    }
    UpdateSuma();
}

void CZlecenieDlg::UpdateSuma()
{
    double suma = 0.0;
    for (const auto& p : m_pozycje) suma += p.koszt;
    CString txt; txt.Format(_T("Laczny koszt: %.2f zl"), suma);
    m_lblSuma.SetWindowText(txt);
}

void CZlecenieDlg::OnCbnSelchangeCmbMarka()
{
    int sel = m_cmbMarka.GetCurSel();
    if (sel < 0 || sel >= (int)m_marki.size()) return;
    LoadModele(m_marki[sel].idMarki);
}

void CZlecenieDlg::OnCbnSelchangeCmbModel()
{
    int sel = m_cmbModel.GetCurSel();
    if (sel < 0 || sel >= (int)m_modele.size()) return;
    LoadWersje(m_modele[sel].idModelu);
}

void CZlecenieDlg::OnCbnSelchangeCmbKatalog()
{
    int sel = m_cmbKatalog.GetCurSel();
    if (sel <= 0 || sel > (int)m_katalog.size()) return;
    const auto& u = m_katalog[sel - 1];
    m_editNazwaPoz.SetWindowText(u.nazwaUslug);
    CString koszt; koszt.Format(_T("%.2f"), u.cenaDomyslna);
    m_editKosztPoz.SetWindowText(koszt);
}

void CZlecenieDlg::OnBnClickedBtnDodajPoz()
{
    CString nazwa, kosztStr;
    m_editNazwaPoz.GetWindowText(nazwa);
    m_editKosztPoz.GetWindowText(kosztStr);
    nazwa.Trim(); kosztStr.Trim();

    if (nazwa.IsEmpty()) {
        AfxMessageBox(_T("Podaj nazwê pozycji."), MB_ICONWARNING);
        return;
    }
    PozycjaZlecenia poz{};
    poz.idZlecenia = m_idZlecenia;
    poz.nazwaPozycji = nazwa;
    poz.koszt = _ttof(kosztStr);
    m_pozycje.push_back(poz);
    FillPozycjeList();

    m_editNazwaPoz.SetWindowText(_T(""));
    m_editKosztPoz.SetWindowText(_T(""));
    m_cmbKatalog.SetCurSel(0);
}

void CZlecenieDlg::OnBnClickedBtnUsunPoz()
{
    int sel = m_listPozycje.GetNextItem(-1, LVNI_SELECTED);
    if (sel < 0) {
        AfxMessageBox(_T("Wybierz pozycjê do usuniêcia."), MB_ICONINFORMATION);
        return;
    }
    m_pozycje.erase(m_pozycje.begin() + sel);
    FillPozycjeList();
}

bool CZlecenieDlg::Validate()
{
    CString nrRej, nazwa, telefon;
    m_editNrRej.GetWindowText(nrRej);     nrRej.Trim();
    m_editNazwa.GetWindowText(nazwa);     nazwa.Trim();
    m_editTelefon.GetWindowText(telefon); telefon.Trim();

    if (nrRej.IsEmpty()) {
        AfxMessageBox(_T("Pole 'Nr rejestracyjny' jest wymagane."), MB_ICONWARNING);
        m_editNrRej.SetFocus(); return false;
    }
    if (m_cmbMarka.GetCurSel() < 0) {
        AfxMessageBox(_T("Wybierz markê pojazdu."), MB_ICONWARNING);
        m_cmbMarka.SetFocus(); return false;
    }
    if (nazwa.IsEmpty()) {
        AfxMessageBox(_T("Pole 'Nazwa klienta' jest wymagane."), MB_ICONWARNING);
        m_editNazwa.SetFocus(); return false;
    }
    if (telefon.IsEmpty()) {
        AfxMessageBox(_T("Pole 'Nr telefonu' jest wymagane."), MB_ICONWARNING);
        m_editTelefon.SetFocus(); return false;
    }
    return true;
}

bool CZlecenieDlg::ResolveKlientPojazd(int& outIdKlienta, int& outIdPojazdu)
{
    CString nazwaK, telK, miastoK, ulicaK;
    m_editNazwa.GetWindowText(nazwaK);       nazwaK.Trim();
    m_editTelefon.GetWindowText(telK);       telK.Trim();
    m_editMiejscowosc.GetWindowText(miastoK); miastoK.Trim();
    m_editUlica.GetWindowText(ulicaK);       ulicaK.Trim();

    auto klienci = g_pDB->GetKlienci();
    outIdKlienta = -1;
    for (const auto& k : klienci) {
        if (k.nazwa == nazwaK && k.nrTelefonu == telK) {
            outIdKlienta = k.idKlienta;
            Klient kUpd = k;
            kUpd.miejscowosc = miastoK;
            kUpd.ulica = ulicaK;
            g_pDB->UpdateKlient(kUpd);
            break;
        }
    }
    if (outIdKlienta < 0) {
        Klient nowyK{};
        nowyK.nazwa = nazwaK;
        nowyK.nrTelefonu = telK;
        nowyK.miejscowosc = miastoK;
        nowyK.ulica = ulicaK;
        if (!g_pDB->AddKlient(nowyK)) return false;
        outIdKlienta = nowyK.idKlienta;
    }

    CString nrRej, rokStr;
    m_editNrRej.GetWindowText(nrRej);    nrRej.Trim();
    m_editRokProd.GetWindowText(rokStr); rokStr.Trim();
    int rok = rokStr.IsEmpty() ? 0 : _ttoi(rokStr);

    int selWersja = m_cmbWersja.GetCurSel();
    int idWersji = (selWersja >= 0 && selWersja < (int)m_wersje.size())
        ? m_wersje[selWersja].idWersji : -1;

    auto pojazdy = g_pDB->GetPojazdy();
    outIdPojazdu = -1;
    for (const auto& p : pojazdy) {
        if (p.nrRejestracyjny.CompareNoCase(nrRej) == 0) {
            if (p.idKlienta != outIdKlienta) {
                CString msg;
                msg.Format(
                    _T("Pojazd o nr rej. %s jest ju¿ przypisany do klienta:\n%s\n\n")
                    _T("Czy chcesz przypisaæ ten pojazd do nowego zlecenia mimo to?"),
                    nrRej.GetString(), p.nazwaKlienta.GetString());
                if (AfxMessageBox(msg, MB_YESNO | MB_ICONWARNING) != IDYES)
                    return false;
            }
            outIdPojazdu = p.idPojazdu;
            break;
        }
    }
    if (outIdPojazdu < 0) {
        if (idWersji < 0) {
            AfxMessageBox(_T("Wybierz wersjê silnikow¹ pojazdu."), MB_ICONWARNING);
            return false;
        }
        Pojazd nowyP{};
        nowyP.idWersji = idWersji;
        nowyP.idKlienta = outIdKlienta;
        nowyP.nrRejestracyjny = nrRej;
        nowyP.rokProdukcji = rok;
        if (!g_pDB->AddPojazd(nowyP)) return false;
        outIdPojazdu = nowyP.idPojazdu;
    }
    return true;
}

void CZlecenieDlg::OnBnClickedBtnZapisz()
{
    if (!Validate()) return;

    int idKlienta = -1, idPojazdu = -1;
    if (!ResolveKlientPojazd(idKlienta, idPojazdu)) return;

    double suma = 0.0;
    for (const auto& p : m_pozycje) suma += p.koszt;

    if (m_idZlecenia < 0) {
        Zlecenie z{};
        m_lblNumer.GetWindowText(z.numerZlecenia);
        z.idPojazdu = idPojazdu;
        z.idKlienta = idKlienta;
        z.kosztCalkowity = suma;
        if (g_pDB->AddZlecenie(z, m_pozycje))
            EndDialog(IDOK);
    }
    else {
        m_zlecenie.idPojazdu = idPojazdu;
        m_zlecenie.idKlienta = idKlienta;
        m_zlecenie.kosztCalkowity = suma;
        if (g_pDB->UpdateZlecenie(m_zlecenie, m_pozycje))
            EndDialog(IDOK);
    }
}

void CZlecenieDlg::OnBnClickedBtnAnuluj()
{
    EndDialog(IDCANCEL);
}

void CZlecenieDlg::OnOK()
{
   
}

void CZlecenieDlg::OnLvnItemchangedListPozycje(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;
}
