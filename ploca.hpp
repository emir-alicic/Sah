#ifndef PLOCA_HPP_INCLUDED
#define PLOCA_HPP_INCLUDED

#include<vector>
#include<string>
#include<ctime>

enum konstante{BIJELI=0, CRNI, KRALJ=1, KRALJICA,
     TOP, LOVAC, SKAKAC, PIJUN};

/*  
const char PIJUN    = 6;
const char TOP      = 3;
const char SKAKAC   = 5;
const char LOVAC    = 4;
const char KRALJICA = 2;
const char KRALJ    = 1;
const bool BIJELI = 0;
const bool CRNI = 1;
*/

const std::string notacija("KDTLS ");

class ploca
{
private:
    char _polja[8][8]; // Svaki element oblika <4b><1b><3b> - <0><boja><figura>
    char _moguca_polja[8]; /* Trenutna moguća polja za pomak neke figure
                              1b za svako polje */
    int _polazno_polje[2]; /* Varijabla u koju će se snimati polazno polje pri
                              pokušaju pomaka figure {red, kolona} */
    int _an_passant_polje[2]; /* Red i kolona polja koje označava
                                 mogućnost an-passanta (default: -1, -1) */
    bool _an_passant_boja; /* TRUE - Crni je dao priliku bijelom za an-passant */
    bool _transformacija;  /* Ako je informacija o transformaciji pijuna na
                              čekanju - TRUE */
    int _mjesto_transformacije[2];

    int _vrijeme[2];  /* Niz u kome su pohranjene vrijednosti preostalog vremena bijelog i crnog */

    time_t _vrijeme_mjerenja; /* Vrijeme u kome je zadnji put vršeno mjerenje */

    void (ploca::*_fje_provjera_pomaka[6])(const int&, const int&);  /* Pointeri na fje provjera pomaka */

    bool _potez; /* CRNI ili BIJELI */

    bool _rokade[4]; /* Mogućnosti rokade -> velika rokada, mala rokada... */

    char _kraljevi[4];     /* Polja na kojima se trenutno nalaze crni(_kraljevi[1]) i bijeli kralj
                             Od interesa tokom cijele obrade mogucnosti pomaka figura
                             {bijeli_kralj_red, bijeli_kralj_kolona, crni_kralj_red, crni_kralj_kolona} */

    std::string sah_mat_pat(const bool &);
    /* Provjera da li je kralj boje u argumentu
       u šahu, šah-matu ili patu */

    int provjera_na_sah(bool,  int = -1, int = -1);
    /* Provjera da li je kralj sa bojom u argumentu u šahu
     Vraćeni broj označava smjer u kome je kralj u šahu:

                                                1 5 2
                                                8 K 7
                                                3 6 4

                                Kombinacije brojeva (dvije strane) (npr. 13, 45, ...)
                                 9 - Šah od strane protivničkog kralja
                                10 - Šah od strane skakača

                                */

    void provjera_po_dijagonalama(const int&, const int&, int* , void (ploca::*)(const int&, const int&), int);
        /* Četvrti argument je pointer na f-ju koja prima jedan argument
           Metod će izvršiti funkciju nad svakim slobodnim
           poljem na dijagonali, a polja prekida (zbog druge
           figure ili kraja ploce) pohraniti u niz dat sa
           pointerom u trećem argumentu na način (red1, kolona1, red2, kolona2...)
           Poslednji argument služi za ograničenje smjerova za testiranje
           0 znaci sve smjerove, a kombinacije samo odredjene, npr. 21
           (Uvijek po veličini upisani smjerovi od najvećeg ka najmanjem!)*/

    void provjera_po_pravcima(const int&, const int&, int* , void (ploca::*)(const int&, const int&), int);
        /* Argumenti kao i kod prethodne funkcije, samo što se posmatraju
           horizontalni i vertikalni pravac */

    int provjera_po_L(const int&, const int&,  int* , void (ploca::*)(const int&, const int&));
        /* Na svakom slobodnom polju izvršit će se unesena funkcija,
           dok će svako polje na koje skakač može ići biti snimljeno
           u niz koji je dat sa pointerom */

    void provjera_za_kretanje_pijuna(const int&, const int&, void (ploca::*)(const int&, const int&), int);
        /* Na svakom slobodnom polju u odgovarajućem vertikalnom pravcu za pijuna, izvršiće se
           unesena funkcija, kao i na poljima na kojima je protivnička figura */

    void pomak_pijuna(const int&, const int&);
    void pomak_topa(const int&, const int&);
    void pomak_skakaca(const int&, const int&);
    void pomak_lovca(const int&, const int&);
    void pomak_kraljice(const int&, const int&);
    void pomak_kralja(const int&, const int&);

    void upisi_u_moguca_polja(const int&, const int&);
        /* Metod koji će navedeno polje upisati u moguća polja */

    void provjeri_zavrsna_polja(int*, const bool&, const std::vector<int>& pozicije = std::vector<int>());
        /* Metod koji prolazi kroz uneseni niz završnih polja
           i ona na kojima su figure suprotne boje od unesene
           upisuje u niz mogućih polja */

    void upisi_putanju_saha(const int&, const bool&);
       /* Metod koji upisuje putanju šaha u moguća polja
          Metod je od interesa za provjeru mogućnosti
          presjeka putanje šaha */

    bool ima_materijal_za_mat(const bool&);
       /* Vraća true ako unesena boja ima materijal za mat */

public:
    ploca();
    void novi_start(const int&); /* Postavka početnog stanja za igru */
    /* Metod namjenjen za primjenu od strane GUI dijela programa */
    std::string potez(const int&, const int&);   /* Metod koji provjerava trenutno kliknuto polje
                               te računa moguća mjesta za figuru na tom polju
                               ili izvršava potez na uneseno polje, ako je ono pohranjeno u
                               "moguca_polja[8]". O svemu tome vraća odgovarajuću informaciju */

    /* Vraća vrijednost upisanu u niz na unesenom indeksu */
    char poz(const int&, const int&);
    bool pripada_mogucim_poljima(const int&, const int&); /* Provjera pripadnosti unesenog
                                                             polja mogućim poljima */
    char izvrsi_transformaciju(const char&); /* Funkcija za slanje podatka o figuri
                                                koju treba promjeniti */

    int preostalo_vrijeme(const bool&, const int& = 1); /* Vraća preostalo vrijeme za potez */

    bool napravi_potez(const std::string&); /* Pravi potez iz notacije */
};

#include "ploca.cpp"
#endif // PLOCA_HPP_INCLUDED
