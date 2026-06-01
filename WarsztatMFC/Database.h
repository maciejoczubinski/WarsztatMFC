#pragma once
#include <afxdb.h>
#include <vector>

struct Marka {
    int     idMarki;
    CString nazwaMarki;
};

struct Model {
    int     idModelu;
    int     idMarki;
    CString nazwaModelu;
};

struct WersjaSilnikowa {
    int     idWersji;
    int     idModelu;
    CString opisWersji;
};

struct Klient {
    int     idKlienta;
    CString nazwa;
    CString nrTelefonu;
    CString miejscowosc;
    CString ulica;
};

struct Pojazd {
    int     idPojazdu;
    int     idWersji;
    int     idKlienta;
    CString nrRejestracyjny;
    int     rokProdukcji;
    CString marka;
    CString model;
    CString wersjaSilnikowa;
    CString nazwaKlienta;
};

struct PozycjaZlecenia {
    int     idPozycji;
    int     idZlecenia;
    CString nazwaPozycji;
    double  koszt;
};

struct Zlecenie {
    int     idZlecenia;
    CString numerZlecenia;
    int     idPojazdu;
    int     idKlienta;
    CString dataUtworzenia;
    double  kosztCalkowity;
    int status = 0;
    CString nrRejestracyjny;
    CString marka;
    CString model;
    CString nazwaKlienta;
};

struct KatalogUsluga {
    int     idUsluga;
    CString nazwaUslug;
    double  cenaDomyslna;
};


class CDatabase2
{
public:
    CDatabase2();
    ~CDatabase2();

    bool Connect(const CString& connectionString);
    void Disconnect();
    bool IsConnected() const { return m_db.IsOpen(); }

    std::vector<Marka>           GetMarki();
    std::vector<Model>           GetModele(int idMarki);
    std::vector<WersjaSilnikowa> GetWersje(int idModelu);

    std::vector<Klient> GetKlienci();
    bool AddKlient(Klient& klient);
    bool UpdateKlient(const Klient& klient);

    std::vector<Pojazd> GetPojazdy();
    bool AddPojazd(Pojazd& pojazd);

    std::vector<Zlecenie>        GetZlecenia();
    bool AddZlecenie(Zlecenie& zlecenie, const std::vector<PozycjaZlecenia>& pozycje);
    bool UpdateZlecenie(const Zlecenie& zlecenie, const std::vector<PozycjaZlecenia>& pozycje);
    bool DeleteZlecenie(int idZlecenia);
    bool UpdateStatusZlecenia(int idZlecenia, int nowyStatus);
    CString GenerujNumerZlecenia();

    std::vector<PozycjaZlecenia> GetPozycjeZlecenia(int idZlecenia);

    std::vector<KatalogUsluga> GetKatalogUslug();

private:
    CDatabase m_db;
    CString   ExecScalar(const CString& sql);
    bool      ExecNonQuery(const CString& sql);
};

extern CDatabase2* g_pDB;