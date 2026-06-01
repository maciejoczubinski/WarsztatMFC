#include "pch.h"
#include "Database.h"

CDatabase2* g_pDB = nullptr;  //globalna zmienna 

CDatabase2::CDatabase2() {}  //konstruktor 
CDatabase2::~CDatabase2() { Disconnect(); } //destruktor

bool CDatabase2::Connect(const CString& connectionString)
{
    try {
        m_db.OpenEx(connectionString, CDatabase::noOdbcDialog);  //otwieramy po³¹czenie z baz¹
        return true;
    }
    catch (CDBException* e) {  //³apiemy wyj¹tek
        CString msg;
        msg.Format(_T("B³¹d po³¹czenia z baz¹ danych:\n%s"), e->m_strError.GetString());
        AfxMessageBox(msg, MB_ICONERROR); //okienko b³êdu
        e->Delete(); //usuwamy wyj¹tki
        return false;
    }
}

void CDatabase2::Disconnect() //zamykamy po³¹czenie jeœli jest otwarte
{
    if (m_db.IsOpen())
        m_db.Close();
}

CString CDatabase2::ExecScalar(const CString& sql)  //ExecScalar -> tylko gdy potrzebujemy dok³adnie jedn¹ wartoœæ np. po INSERT ¿eby poznaæ ID lub przy COUNT
{
    CString result;
    try {
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, sql, CRecordset::readOnly);  // do przodu, tylko odczyt
        if (!rs.IsEOF())                                              //EOF - End Of File
            rs.GetFieldValue((short)0, result);                       // 0 - pierwsza kolumna
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }  
    return result;
}

bool CDatabase2::ExecNonQuery(const CString& sql)  //ExecNonQuery -> zapytania kótre nic nie zwracaj¹ - INSERT, UPDATE, DELETE
{
    try {
        m_db.ExecuteSQL(sql);
        return true;
    }
    catch (CDBException* e) {
        CString msg;
        msg.Format(_T("B³¹d SQL:\n%s"), e->m_strError.GetString()); 
        AfxMessageBox(msg, MB_ICONERROR);
        e->Delete();
        return false;
    }
}

std::vector<Marka> CDatabase2::GetMarki()
{
    std::vector<Marka> result;
    try {
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly,
            _T("SELECT IdMarki, NazwaMarki FROM Marki ORDER BY NazwaMarki"),
            CRecordset::readOnly);
        while (!rs.IsEOF()) {
            Marka m;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); m.idMarki = _ttoi(tmp);
            rs.GetFieldValue((short)1, m.nazwaMarki);
            result.push_back(m);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

std::vector<Model> CDatabase2::GetModele(int idMarki)
{
    std::vector<Model> result;
    try {
        CString sql;
        sql.Format(_T("SELECT IdModelu, IdMarki, NazwaModelu FROM Modele WHERE IdMarki=%d ORDER BY NazwaModelu"), idMarki);
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, sql, CRecordset::readOnly);
        while (!rs.IsEOF()) {
            Model m;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); m.idModelu = _ttoi(tmp);
            rs.GetFieldValue((short)1, tmp); m.idMarki = _ttoi(tmp);
            rs.GetFieldValue((short)2, m.nazwaModelu);
            result.push_back(m);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

std::vector<WersjaSilnikowa> CDatabase2::GetWersje(int idModelu)
{
    std::vector<WersjaSilnikowa> result;
    try {
        CString sql;
        sql.Format(_T("SELECT IdWersji, IdModelu, OpisWersji FROM WersjeSilnikowe WHERE IdModelu=%d ORDER BY OpisWersji"), idModelu);
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, sql, CRecordset::readOnly);
        while (!rs.IsEOF()) {
            WersjaSilnikowa w;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); w.idWersji = _ttoi(tmp);
            rs.GetFieldValue((short)1, tmp); w.idModelu = _ttoi(tmp);
            rs.GetFieldValue((short)2, w.opisWersji);
            result.push_back(w);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

std::vector<Klient> CDatabase2::GetKlienci()
{
    std::vector<Klient> result;
    try {
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, _T("SELECT IdKlienta, Nazwa, NrTelefonu, Miejscowosc, Ulica FROM Klienci ORDER BY Nazwa"),
            CRecordset::readOnly);
        while (!rs.IsEOF()) {
            Klient k;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); k.idKlienta = _ttoi(tmp);
            rs.GetFieldValue((short)1, k.nazwa);
            rs.GetFieldValue((short)2, k.nrTelefonu);
            rs.GetFieldValue((short)3, k.miejscowosc);
            rs.GetFieldValue((short)4, k.ulica);
            result.push_back(k);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

bool CDatabase2::AddKlient(Klient& klient)
{
    CString nazwa = klient.nazwa;        nazwa.Replace(_T("'"), _T("''"));
    CString tel = klient.nrTelefonu;  tel.Replace(_T("'"), _T("''"));
    CString miej = klient.miejscowosc; miej.Replace(_T("'"), _T("''"));
    CString ul = klient.ulica;       ul.Replace(_T("'"), _T("''"));
    CString sql;
    sql.Format(_T("INSERT INTO Klienci (Nazwa, NrTelefonu, Miejscowosc, Ulica) VALUES (N'%s', N'%s', N'%s', N'%s')"),
        nazwa.GetString(), tel.GetString(), miej.GetString(), ul.GetString());
    if (!ExecNonQuery(sql)) return false;
    CString idStr = ExecScalar(_T("SELECT IDENT_CURRENT('Klienci')"));
    klient.idKlienta = _ttoi(idStr);
    return klient.idKlienta > 0;
}

bool CDatabase2::UpdateKlient(const Klient& klient)
{
    CString nazwa = klient.nazwa;        nazwa.Replace(_T("'"), _T("''"));
    CString tel = klient.nrTelefonu;  tel.Replace(_T("'"), _T("''"));
    CString miej = klient.miejscowosc; miej.Replace(_T("'"), _T("''"));
    CString ul = klient.ulica;       ul.Replace(_T("'"), _T("''"));
    CString sql;
    sql.Format(_T("UPDATE Klienci SET Nazwa=N'%s', NrTelefonu=N'%s', Miejscowosc=N'%s', Ulica=N'%s' WHERE IdKlienta=%d"),
        nazwa.GetString(), tel.GetString(), miej.GetString(), ul.GetString(), klient.idKlienta);
    return ExecNonQuery(sql);
}

std::vector<Pojazd> CDatabase2::GetPojazdy()
{
    std::vector<Pojazd> result;
    try {
        CString sql =
            _T("SELECT p.IdPojazdu, p.IdWersji, p.IdKlienta, p.NrRejestracyjny, p.RokProdukcji, ")
            _T("ma.NazwaMarki, mo.NazwaModelu, ws.OpisWersji, k.Nazwa ")
            _T("FROM Pojazdy p ")
            _T("JOIN WersjeSilnikowe ws ON ws.IdWersji = p.IdWersji ")
            _T("JOIN Modele mo ON mo.IdModelu = ws.IdModelu ")
            _T("JOIN Marki ma ON ma.IdMarki = mo.IdMarki ")
            _T("JOIN Klienci k ON k.IdKlienta = p.IdKlienta ")
            _T("ORDER BY p.NrRejestracyjny");
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, sql, CRecordset::readOnly);
        while (!rs.IsEOF()) {
            Pojazd p;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); p.idPojazdu = _ttoi(tmp);
            rs.GetFieldValue((short)1, tmp); p.idWersji = _ttoi(tmp);
            rs.GetFieldValue((short)2, tmp); p.idKlienta = _ttoi(tmp);
            rs.GetFieldValue((short)3, p.nrRejestracyjny);
            rs.GetFieldValue((short)4, tmp); p.rokProdukcji = _ttoi(tmp);
            rs.GetFieldValue((short)5, p.marka);
            rs.GetFieldValue((short)6, p.model);
            rs.GetFieldValue((short)7, p.wersjaSilnikowa);
            rs.GetFieldValue((short)8, p.nazwaKlienta);
            result.push_back(p);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

bool CDatabase2::AddPojazd(Pojazd& pojazd)
{
    CString nrRej = pojazd.nrRejestracyjny; nrRej.Replace(_T("'"), _T("''"));
    CString sql;
    sql.Format(_T("INSERT INTO Pojazdy (IdWersji, IdKlienta, NrRejestracyjny, RokProdukcji) VALUES (%d, %d, N'%s', %d)"),
        pojazd.idWersji, pojazd.idKlienta, nrRej.GetString(), pojazd.rokProdukcji);
    if (!ExecNonQuery(sql)) return false;
    CString idStr = ExecScalar(_T("SELECT IDENT_CURRENT('Pojazdy')"));
    pojazd.idPojazdu = _ttoi(idStr);
    return pojazd.idPojazdu > 0;
}

std::vector<Zlecenie> CDatabase2::GetZlecenia()
{
    std::vector<Zlecenie> result;
    try {
        CString sql =
            _T("SELECT z.IdZlecenia, z.NumerZlecenia, z.IdPojazdu, z.IdKlienta, ")
            _T("CONVERT(NVARCHAR(16), z.DataUtworzenia, 120), z.KosztCalkowity, z.Status, ")
            _T("p.NrRejestracyjny, ma.NazwaMarki, mo.NazwaModelu, k.Nazwa ")
            _T("FROM Zlecenia z ")
            _T("JOIN Pojazdy p ON p.IdPojazdu = z.IdPojazdu ")
            _T("JOIN WersjeSilnikowe ws ON ws.IdWersji = p.IdWersji ")
            _T("JOIN Modele mo ON mo.IdModelu = ws.IdModelu ")
            _T("JOIN Marki ma ON ma.IdMarki = mo.IdMarki ")
            _T("JOIN Klienci k ON k.IdKlienta = z.IdKlienta ")
            _T("ORDER BY z.IdZlecenia DESC");
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, sql, CRecordset::readOnly);
        while (!rs.IsEOF()) {
            Zlecenie z;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); z.idZlecenia = _ttoi(tmp);
            rs.GetFieldValue((short)1, z.numerZlecenia);
            rs.GetFieldValue((short)2, tmp); z.idPojazdu = _ttoi(tmp);
            rs.GetFieldValue((short)3, tmp); z.idKlienta = _ttoi(tmp);
            rs.GetFieldValue((short)4, z.dataUtworzenia);
            rs.GetFieldValue((short)5, tmp); z.kosztCalkowity = _ttof(tmp);
            rs.GetFieldValue((short)6, tmp); z.status = _ttoi(tmp);
            rs.GetFieldValue((short)7, z.nrRejestracyjny);
            rs.GetFieldValue((short)8, z.marka);
            rs.GetFieldValue((short)9, z.model);
            rs.GetFieldValue((short)10, z.nazwaKlienta);
            result.push_back(z);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

CString CDatabase2::GenerujNumerZlecenia()
{
    int rok = CTime::GetCurrentTime().GetYear();
    CString sql;
    sql.Format(_T("SELECT ISNULL(MAX(CAST(LEFT(NumerZlecenia, CHARINDEX('/', NumerZlecenia)-1) AS INT)), 0) FROM Zlecenia WHERE NumerZlecenia LIKE N'%%/%d'"), rok);
    CString tmp = ExecScalar(sql);
    int nr = _ttoi(tmp) + 1;
    CString result;
    result.Format(_T("%d/%d"), nr, rok);
    return result;
}

bool CDatabase2::AddZlecenie(Zlecenie& zlecenie, const std::vector<PozycjaZlecenia>& pozycje)
{
    CString numer = zlecenie.numerZlecenia; numer.Replace(_T("'"), _T("''"));
    CString sql;
    sql.Format(_T("INSERT INTO Zlecenia (NumerZlecenia, IdPojazdu, IdKlienta, KosztCalkowity) VALUES (N'%s', %d, %d, %.2f)"),
        numer.GetString(), zlecenie.idPojazdu, zlecenie.idKlienta, zlecenie.kosztCalkowity);
    if (!ExecNonQuery(sql)) return false;
    CString idStr = ExecScalar(_T("SELECT IDENT_CURRENT('Zlecenia')"));
    zlecenie.idZlecenia = _ttoi(idStr);
    for (const auto& poz : pozycje) {
        CString nazwa = poz.nazwaPozycji; nazwa.Replace(_T("'"), _T("''"));
        sql.Format(_T("INSERT INTO PozycjeZlecenia (IdZlecenia, NazwaPozycji, Koszt) VALUES (%d, N'%s', %.2f)"),
            zlecenie.idZlecenia, nazwa.GetString(), poz.koszt);
        ExecNonQuery(sql);
    }
    return true;
}

bool CDatabase2::UpdateZlecenie(const Zlecenie& zlecenie, const std::vector<PozycjaZlecenia>& pozycje)
{
    CString sql;
    sql.Format(_T("UPDATE Zlecenia SET IdPojazdu=%d, IdKlienta=%d, KosztCalkowity=%.2f WHERE IdZlecenia=%d"),
        zlecenie.idPojazdu, zlecenie.idKlienta, zlecenie.kosztCalkowity, zlecenie.idZlecenia);
    if (!ExecNonQuery(sql)) return false;
    sql.Format(_T("DELETE FROM PozycjeZlecenia WHERE IdZlecenia=%d"), zlecenie.idZlecenia);
    ExecNonQuery(sql);
    for (const auto& poz : pozycje) {
        CString nazwa = poz.nazwaPozycji; nazwa.Replace(_T("'"), _T("''"));
        sql.Format(_T("INSERT INTO PozycjeZlecenia (IdZlecenia, NazwaPozycji, Koszt) VALUES (%d, N'%s', %.2f)"),
            zlecenie.idZlecenia, nazwa.GetString(), poz.koszt);
        ExecNonQuery(sql);
    }
    return true;
}

bool CDatabase2::DeleteZlecenie(int idZlecenia)
{
    CString sql;
    sql.Format(_T("DELETE FROM Zlecenia WHERE IdZlecenia=%d"), idZlecenia);
    return ExecNonQuery(sql);
}

std::vector<PozycjaZlecenia> CDatabase2::GetPozycjeZlecenia(int idZlecenia)
{
    std::vector<PozycjaZlecenia> result;
    try {
        CString sql;
        sql.Format(_T("SELECT IdPozycji, IdZlecenia, NazwaPozycji, Koszt FROM PozycjeZlecenia WHERE IdZlecenia=%d"), idZlecenia);
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly, sql, CRecordset::readOnly);
        while (!rs.IsEOF()) {
            PozycjaZlecenia p;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); p.idPozycji = _ttoi(tmp);
            rs.GetFieldValue((short)1, tmp); p.idZlecenia = _ttoi(tmp);
            rs.GetFieldValue((short)2, p.nazwaPozycji);
            rs.GetFieldValue((short)3, tmp); p.koszt = _ttof(tmp);
            result.push_back(p);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

std::vector<KatalogUsluga> CDatabase2::GetKatalogUslug()
{
    std::vector<KatalogUsluga> result;
    try {
        CRecordset rs(&m_db);
        rs.Open(CRecordset::forwardOnly,
            _T("SELECT IdUsluga, NazwaUslug, CenaDomyslna FROM KatalogUslug ORDER BY NazwaUslug"),
            CRecordset::readOnly);
        while (!rs.IsEOF()) {
            KatalogUsluga u;
            CString tmp;
            rs.GetFieldValue((short)0, tmp); u.idUsluga = _ttoi(tmp);
            rs.GetFieldValue((short)1, u.nazwaUslug);
            rs.GetFieldValue((short)2, tmp); u.cenaDomyslna = _ttof(tmp);
            result.push_back(u);
            rs.MoveNext();
        }
        rs.Close();
    }
    catch (CDBException* e) { e->Delete(); }
    return result;
}

bool CDatabase2::UpdateStatusZlecenia(int idZlecenia, int nowyStatus)
{
    CString sql;
    sql.Format(_T("UPDATE Zlecenia SET Status=%d WHERE IdZlecenia=%d"),
        nowyStatus, idZlecenia);
    return ExecNonQuery(sql);
}
