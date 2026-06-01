#include "pch.h"
#include "PdfExport.h"
#include <hpdf.h>
#include <string>

static std::string CStringToAnsi(const CString& str)
{
    CT2A ansi(str, CP_ACP);
    return std::string(ansi);
}

void CPdfExport::RysujLinie(HPDF_Page page, float x1, float y1, float x2, float y2)
{
    HPDF_Page_MoveTo(page, x1, y1);
    HPDF_Page_LineTo(page, x2, y2);
    HPDF_Page_Stroke(page);
}

void CPdfExport::PiszTekst(HPDF_Page page, HPDF_Font font, float size, float x, float y, const char* text)
{
    HPDF_Page_SetFontAndSize(page, font, size);
    HPDF_Page_BeginText(page);
    HPDF_Page_MoveTextPos(page, x, y);
    HPDF_Page_ShowText(page, text);
    HPDF_Page_EndText(page);
}

bool CPdfExport::GenerujFakture(const Zlecenie& zlecenie,
    const std::vector<PozycjaZlecenia>& pozycje,
    const Klient& klient,
    const Pojazd& pojazd)
{
    HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
    if (!pdf) {
        AfxMessageBox(_T("Nie mo¿na utworzyæ dokumentu PDF."), MB_ICONERROR);
        return false;
    }

    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, "UTF-8");

    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    float pageW = HPDF_Page_GetWidth(page);   
    float pageH = HPDF_Page_GetHeight(page);  

    HPDF_Font fontBold = HPDF_GetFont(pdf, "Helvetica-Bold", "ISO8859-2");
    HPDF_Font fontNormal = HPDF_GetFont(pdf, "Helvetica", "ISO8859-2");

    float margin = 50.0f;
    float y = pageH - margin;

    //nag³ówek
    PiszTekst(page, fontBold, 24, margin, y, "INTEGRA");
    y -= 20;
    PiszTekst(page, fontNormal, 10, margin, y, "Serwis naprawy pojazdow");
    y -= 30;

    //linia pod nag³ówkiem
    RysujLinie(page, margin, y, pageW - margin, y);
    y -= 20;

    //numer zlecenia i data
    std::string nrZlecenia = "Zlecenie nr: " + CStringToAnsi(zlecenie.numerZlecenia);
    std::string dataStr = "Data: " + CStringToAnsi(zlecenie.dataUtworzenia);
    PiszTekst(page, fontBold, 12, margin, y, nrZlecenia.c_str());
    PiszTekst(page, fontNormal, 10, pageW - 200, y, dataStr.c_str());
    y -= 30;

    //dane klienta
    PiszTekst(page, fontBold, 11, margin, y, "DANE KLIENTA:");
    y -= 18;
    std::string nazwaK = "Nazwa:        " + CStringToAnsi(klient.nazwa);
    std::string telK = "Telefon:      " + CStringToAnsi(klient.nrTelefonu);
    std::string miastoK = "Miejscowosc:  " + CStringToAnsi(klient.miejscowosc);
    std::string ulicaK = "Ulica:        " + CStringToAnsi(klient.ulica);
    PiszTekst(page, fontNormal, 10, margin, y, nazwaK.c_str());  y -= 15;
    PiszTekst(page, fontNormal, 10, margin, y, telK.c_str());    y -= 15;
    PiszTekst(page, fontNormal, 10, margin, y, miastoK.c_str()); y -= 15;
    PiszTekst(page, fontNormal, 10, margin, y, ulicaK.c_str());  y -= 25;

    //dane pojazdu
    PiszTekst(page, fontBold, 11, margin, y, "DANE POJAZDU:");
    y -= 18;
    std::string markaModel = "Marka/Model:  " + CStringToAnsi(pojazd.marka) + " " + CStringToAnsi(pojazd.model);
    std::string wersja = "Silnik:       " + CStringToAnsi(pojazd.wersjaSilnikowa);
    std::string nrRej = "Nr rej.:      " + CStringToAnsi(pojazd.nrRejestracyjny);
    std::string rok = "Rok prod.:    ";
    char rokBuf[10];
    sprintf_s(rokBuf, "%d", pojazd.rokProdukcji);
    rok += rokBuf;
    PiszTekst(page, fontNormal, 10, margin, y, markaModel.c_str()); y -= 15;
    PiszTekst(page, fontNormal, 10, margin, y, wersja.c_str());     y -= 15;
    PiszTekst(page, fontNormal, 10, margin, y, nrRej.c_str());      y -= 15;
    PiszTekst(page, fontNormal, 10, margin, y, rok.c_str());        y -= 25;

    //tabela z us³ugami
    RysujLinie(page, margin, y, pageW - margin, y);
    y -= 15;
    PiszTekst(page, fontBold, 10, margin, y, "Nazwa uslugi/towaru");
    PiszTekst(page, fontBold, 10, pageW - 130, y, "Koszt");
    y -= 5;
    RysujLinie(page, margin, y, pageW - margin, y);
    y -= 15;

    for (const auto& poz : pozycje) {
        std::string nazwa = CStringToAnsi(poz.nazwaPozycji);
        char kosztBuf[30];
        sprintf_s(kosztBuf, "%.2f zl", poz.koszt);
        PiszTekst(page, fontNormal, 10, margin, y, nazwa.c_str());
        PiszTekst(page, fontNormal, 10, pageW - 130, y, kosztBuf);
        y -= 15;
    }

    y -= 5;
    RysujLinie(page, margin, y, pageW - margin, y);
    y -= 20;

    //podsumowanie
    char sumaBuffer[50];
    sprintf_s(sumaBuffer, "Laczny koszt: %.2f zl", zlecenie.kosztCalkowity);
    PiszTekst(page, fontBold, 12, pageW - 200, y, sumaBuffer);
    y -= 60;

    //pieczatka
    RysujLinie(page, margin, y, margin + 150, y);
    y -= 15;
    PiszTekst(page, fontNormal, 9, margin, y, "Data i pieczatka");

    //zapis
    CString filter = _T("PDF Files (*.pdf)|*.pdf||");
    CString nazwaPliku = zlecenie.numerZlecenia;
    nazwaPliku.Replace(_T("/"), _T("-"));
    CFileDialog dlg(FALSE, _T("pdf"), nazwaPliku, OFN_OVERWRITEPROMPT, filter);
    if (dlg.DoModal() != IDOK) {
        HPDF_Free(pdf);
        return false;
    }

    CString sciezka = dlg.GetPathName();
    CT2A sciezkaAnsi(sciezka, CP_ACP);

    HPDF_STATUS status = HPDF_SaveToFile(pdf, sciezkaAnsi);
    HPDF_Free(pdf);

    if (status != HPDF_OK) {
        AfxMessageBox(_T("B³¹d podczas zapisywania pliku PDF."), MB_ICONERROR);
        return false;
    }

    AfxMessageBox(_T("Faktura PDF zosta³a zapisana pomyœlnie!"), MB_ICONINFORMATION);
    return true;
}