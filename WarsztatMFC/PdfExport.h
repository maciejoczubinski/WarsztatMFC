#pragma once
#include "Database.h"
#include <hpdf.h>

class CPdfExport
{
public:
    static bool GenerujFakture(const Zlecenie& zlecenie,
        const std::vector<PozycjaZlecenia>& pozycje,
        const Klient& klient,
        const Pojazd& pojazd);
private:
    static void RysujLinie(HPDF_Page page, float x1, float y1, float x2, float y2);
    static void PiszTekst(HPDF_Page page, HPDF_Font font, float size, float x, float y, const char* text);
};