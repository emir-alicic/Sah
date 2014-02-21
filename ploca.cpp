
bool ploca::pripada_mogucim_poljima(const int& red, const int& kolona) const
{
    return (bool)(_moguca_polja[red] & (1<<kolona));
}

void ploca::novi_start(const int& vrijeme)
{
    for(int i=0; i<8; i++)
        _moguca_polja[i]=0;

    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            _polja[i][j]=0;

    _potez=BIJELI;
    _kraljevi[0]=0;
    _kraljevi[1]=4;
    _kraljevi[2]=7;
    _kraljevi[3]=4;
    _an_passant_polje[0]=-1;
    _an_passant_polje[1]=-1;
    _transformacija=false;

    _mjesto_transformacije[0]=-1;
    _mjesto_transformacije[1]=-1;

    for(int i=0; i<4; i++)
        _rokade[i]=true;

    for(int j=0; j<2; j++) // Izbor boje za unos j=0 <=> BIJELI
    {


    // Upis pijuna
        for(int i=0; i<8; i++)
            _polja[1+j*5][i]=j*CRNI*8+PIJUN; // 0*CRNI==BIJELI

    // Upis topova
        for(int i=0; i<2; i++)
            _polja[7*j][i*7]=j*CRNI*8+TOP;

    // Upis skakaca
        for(int i=0; i<2; i++)
            _polja[7*j][1+i*5]=j*CRNI*8+SKAKAC;

    // Upis lovaca
        for(int i=0; i<2; i++)
            _polja[7*j][2+i*3]=j*CRNI*8+LOVAC;

    // Upis kraljica
            _polja[7*j][3]=j*CRNI*8+KRALJICA;

    // Upis kraljeva
            _polja[7*j][4]=j*CRNI*8+KRALJ;

    }

    // Punjenje niza pointera na f-je

    _fje_provjera_pomaka[PIJUN-1]=&ploca::pomak_pijuna;
    _fje_provjera_pomaka[TOP-1]=&ploca::pomak_topa;
    _fje_provjera_pomaka[SKAKAC-1]=&ploca::pomak_skakaca;
    _fje_provjera_pomaka[LOVAC-1]=&ploca::pomak_lovca;
    _fje_provjera_pomaka[KRALJICA-1]=&ploca::pomak_kraljice;
    _fje_provjera_pomaka[KRALJ-1]=&ploca::pomak_kralja;

    //Početno vrijeme partije
    _vrijeme[0]=vrijeme*60;
    _vrijeme[1]=vrijeme*60;
    _vrijeme_mjerenja=time(0);

    return;
}

ploca::ploca()
{
        novi_start(15);
}

std::string ploca::potez(const int& red, const int& kolona)
{
    if(preostalo_vrijeme(_potez, 0)<1)
        {
            /* Vrijeme za ovog igrača je isteklo */
            if(ima_materijal_za_mat(!_potez))
            {
                if(!_potez==BIJELI)
                    return "1:0";
                else
                    return "0:1";
            }
            else
                return "1/2 1/2"; /* Kraj igre - remi */
         }

    if(_transformacija)
        {
        if(_potez==BIJELI)
            return std::string("ww"); /* Čeka se informacija od bijelog */
        else
            return std::string("wb"); /* Čeka se informacija od crnog  */
        }

    if(pripada_mogucim_poljima(red, kolona))
    {
        bool izvrsen_an_passant=false;
        std::string result("");
        /* Pomjera se kralj? */
        char figura=_polja[_polazno_polje[0]][_polazno_polje[1]];

     if(figura%8==KRALJ)
        {
        _kraljevi[(figura>>3)*2]=red;
        _kraljevi[(figura>>3)*2+1]=kolona;

        /* Premješta se kralj, a pri tom ne rokira? Gubi se mogućnost rokade */
            if(_polazno_polje[1]==4)
                {
                if(kolona-_polazno_polje[1]==2)
                {
                    /* Mala rokada */
                    _polja[red][kolona-1]=_polja[red][7]; // Prenos topa
                    _polja[red][7]=0;
                    result="0-0";
                }

                if(_polazno_polje[1]-kolona==2)
                {
                    /* Velika rokada */
                    _polja[red][kolona+1]=_polja[red][0]; // Prenos topa
                    _polja[red][0]=0;
                    result="0-0-0";
                }

                    _rokade[_potez*2]=false;
                    _rokade[_potez*2+1]=false;
                }
        }

        /* Je li bila mogućnost za an-passant? */
            if(_an_passant_polje[0]!=-1)
            {
                _an_passant_polje[0]=-1;
                _an_passant_polje[1]=-1;
            }

        /* Igrač je iskoristio an-passant? */
        if(figura%8==PIJUN)
        {
            /* Da li je pijun napao neku figuru? */
            if(_polazno_polje[1]!=kolona)
            {
                /* Je li an-passant? */
                if(_polja[red][kolona]==0)
                {
                    /* Je li bijeli pijun? */
                    if(_polja[_polazno_polje[0]][_polazno_polje[1]]>>3==BIJELI)
                    {
                        _polja[red-1][kolona]=0;
                    }
                    else
                    {
                        _polja[red+1][kolona]=0;
                    }

                    izvrsen_an_passant=true;
                    _an_passant_polje[0]=-1;
                    _an_passant_polje[1]=-1;
                }
            }

        /* An-passant?  */ 
	if(figura%8==PIJUN)
        {
            if(_polazno_polje[0]>red && _polazno_polje[0]-red==2)
            {
                _an_passant_polje[0]=red+1;
                _an_passant_boja=CRNI;
            }

            if(_polazno_polje[0]<red && red-_polazno_polje[0]==2)
            {
            _an_passant_polje[0]=red-1;
            _an_passant_boja=BIJELI;
            }

        _an_passant_polje[1]=kolona;
        }

            /* Pijun je na kraju, potrebna transformacija? */
            if(red==7 || red==0)
            {
                _mjesto_transformacije[0]=red;
                _mjesto_transformacije[1]=kolona;
                _transformacija=true;
                if(_potez==BIJELI)
                    result+="ww";
                    else
                    result+="wb";
            }
        }

        if(figura%8==TOP)
            {
                if(_polazno_polje[0]==_potez*7)
                {
        /* Premješta se "lijevi" top? Gubi se mogućnost velike rokade */
            if(_polazno_polje[1]==0)
                _rokade[_potez*2]=false;

        /* Premješta se "desni" top? Gubi se mogućnost male rokade */
            if(_polazno_polje[1]==7)
                _rokade[_potez*2+1]=false;
                }
            }

        _vrijeme_mjerenja=time(0);

        /* Izvršiti premještaj figure */
        _polja[red][kolona]=_polja[_polazno_polje[0]][_polazno_polje[1]];
        _polja[_polazno_polje[0]][_polazno_polje[1]]=0;

        /* Provjera na remi */
        if(!(ima_materijal_za_mat(BIJELI) ||
           ima_materijal_za_mat(CRNI)))
        {
            /* Remi */
            return "1/2 1/2";
        }

        if(!_transformacija)
            _potez=!_potez;

        if(result=="")
        result=result+
               std::string(1, notacija[figura%8-1])+
               std::string(1, _polazno_polje[1]+'a')+
               std::string(1, _polazno_polje[0]+'1')+
               std::string("-")+
               std::string(1, kolona+'a')+
               std::string(1, red+'1');

        if(izvrsen_an_passant)
            result+="a";

       if(!_transformacija)
            result=result + sah_mat_pat(_potez);

        for(int i=0; i<8; i++)
            _moguca_polja[i]=0;

        return result;
    }

    /* Promjena odluke za kretanje figure */
    for(int i=0; i<8; i++)
        if(_moguca_polja[i]!=0)
    {
        for(int j=0; j<8; j++)
        _moguca_polja[j]=0;
        return "f";
    }

    /* Tretiranje toka programa kada je 
       kliknuto polje za poèetak poteza */

    for(int i=0; i<8; i++)
        _moguca_polja[i]=0;

    char figura=_polja[red][kolona];

    if(figura==0 || figura>>3 != _potez)
        /* Polje je prazno ili je na njemu figura suprotne boje
           tako da nema raspoloživih polja za pomak */
        return "f";

    /* Na kliknutom polju nalazi se figura odgovarajuæe boje i
       potrebno je poèeti proraèunavanje moguæih polja pomaka */

    if(provjera_na_sah(_potez)>10 && figura%8!=KRALJ)
        return "f"; /* Šah sa dvije strane */

    /* Poziv odgovarajuće funkcije u zavisnosti od figure koja je
       na tom polju */

    _polazno_polje[0]=red;
    _polazno_polje[1]=kolona;

    ((*this).*_fje_provjera_pomaka[_polja[red][kolona]%8-1])(red, kolona);

        return "f"; /* Moguća polja su inicijalizirana */
}

void ploca::pomak_pijuna(const int& red, const int& kolona)
{
    /* Smjer kretanja pijuna */
    const int skp = (_polja[red][kolona]>>3)?-1:1;

    int smjer_saha=provjera_na_sah(_potez);

    /* Uklonimo figuru i provjerimo da li je kralj
       u šahu */

        char temp=_polja[red][kolona];
         _polja[red][kolona]=0;
         int smjer=provjera_na_sah(_potez);
        _polja[red][kolona]=temp;

    /* Ako je smjer šaha > 10 napad je sa dvije strane,
       figura se ne smije nikako kretati */
       if(smjer>10)
            return;

       if(smjer_saha!=smjer && smjer)
       {
        /* Šah je bez figure, a nije ako je figura tu
        ova figura ima ograničeno kretanje u zavisnosti
        od smjera šaha */

        /* Ako je šah po horizontalnom pravcu, bez obzira
           na boju, pijun se ne smije kretati */

           if(smjer==7 || smjer==8)
                  return;

        /* Ako je šah po onom dijagonalnom pravcu koji ne
           odgovara mogućem pravcu napada tog pijuna,
           njegov pomak sigurno nije moguć */

           if(smjer==skp+2 || smjer==skp+3)
                 return;

            if(smjer==5 || smjer==6)
                provjera_za_kretanje_pijuna(red, kolona, &ploca::upisi_u_moguca_polja, (-skp+11)/2);
            else
                provjera_za_kretanje_pijuna(red, kolona, &ploca::upisi_u_moguca_polja, smjer);
            return;
       }
       else /* Slobodno kretanje pijuna */
            provjera_za_kretanje_pijuna(red, kolona, &ploca::upisi_u_moguca_polja, 0);

        if(smjer_saha)
        {

            /* Kralj je u šahu, može li ova figura pokriti šah */
            char polja_pomaka_pijuna[8];

            for(int i=0; i<8; i++){
                polja_pomaka_pijuna[i]=_moguca_polja[i];
                _moguca_polja[i]=0;
            }

            upisi_putanju_saha(smjer_saha, _potez);

        for(int i=0; i<8; i++) /* Ako neko od polja pomaka pijuna spada u putanju šaha
                                  tada će ono biti i upisano */
            _moguca_polja[i]=_moguca_polja[i]&polja_pomaka_pijuna[i];
        }
    return;
}

void ploca::pomak_kralja(const int& red, const int& kolona)
{
   /* Provjera pristupa svih slobodnih polja oko kralja
      obavlja se u situaciji da se prvo ukloni kralj */

   _polja[red][kolona]=0;

   for(int i=-1; i<2; i++)
    for(int j=-1; j<2; j++)
   {
       if(i==0 && j==0)
            continue;

       if(red+i<8 && red+i>-1 && kolona+j<8 && kolona+j>-1)
       {
            char figura = _polja[red+i][kolona+j];
            /* Prazno polje ili protivnička figura */
            if(figura==0 || figura>>3 != _potez)
            {
                int smjer_saha=provjera_na_sah(_potez, red+i, kolona+j);

                if(smjer_saha==0)
                    upisi_u_moguca_polja(red+i, kolona+j);
            }
       }
   }
   _polja[red][kolona]=8*_potez+KRALJ;

   /* Provjera za rokadu */
   if(red==_potez*7 && kolona==4)
   {
       /* Kralj je na polaznom polju
          za rokadu */

       /* Mala rokada: */
       if(_rokade[_potez*2+1])
       {
           /* Dva polja desno su slobodna? */
           if(_polja[red][5]==0 && _polja[red][6]==0)
           {
               /* Niti jedno od tri navedena polja 
		  nije tučeno polje? */
               int i;
               for(i=0; i<3; i++)
                if(provjera_na_sah(_potez, red, 4+i))
                    break;

               if(i==3)
                    upisi_u_moguca_polja(red, kolona+2);
           }
       }

       /* Velika rokada */
       if(_rokade[_potez*2])
       {
           /* Tri polja lijevo su slobodna? */
           int i;
           for(i=1; i<4; i++)
            if(_polja[red][4-i]!=0)
                break;

           if(i==4)
           {/* Putanja za rokadu je slobodna
               provjera je li neko polje tučeno polje */
               for(i=0; i<3; i++)
                if(provjera_na_sah(_potez, red, kolona-i))
                    break;

               if(i==3)
                upisi_u_moguca_polja(red, kolona-2);
           }
       }
   }
   return;
}

void ploca::pomak_kraljice(const int& red, const int& kolona)
{
    pomak_lovca(red, kolona);
    pomak_topa(red, kolona);
    return;
}

void ploca::pomak_lovca(const int& red, const int& kolona)
{
    int smjer_saha=provjera_na_sah(_potez);
    int zavrsna[18];

    /* Uklonimo figuru i provjerimo da li je kralj
       u šahu */

        char temp=_polja[red][kolona];
         _polja[red][kolona]=0;
         int smjer=provjera_na_sah(_potez);
        _polja[red][kolona]=temp;

    /* Ako je smjer šaha > 10 napad je sa dvije strane,
       figura se ne smije nikako micati */
       if(smjer>10)
            return;

    if(smjer_saha!=smjer && smjer)
    {
        /* Šah je bez figure, a nije ako je figura tu
        ova figura ima ograničeno kretanje u zavisnosti
        od smjera šaha */

        /* Ako je šah iz vertikalnog i horizontalnog smjera,
        lovac se ne smije pomijerati */

            if(smjer>4 && smjer<9)
                return;

            /* U suprotnom, šah je iz dijagonalnog smjera i lovac
               se smije kretati samo u toj dijagonali */

            if(smjer==1 || smjer==4){
                provjera_po_dijagonalama(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja, 41);
                std::vector<int> temp = {1, 4};
                provjeri_zavrsna_polja(zavrsna, _potez, temp);
            }
            else
            {
                provjera_po_dijagonalama(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja, 32);
                std::vector<int> temp = {2, 3};
                provjeri_zavrsna_polja(zavrsna, _potez, temp);
            }
            return;
    }
    else
    {
        /* Slobodno kretanje figure */
        provjera_po_dijagonalama(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja, 0);
        std::vector<int> temp = {1, 2, 3, 4};
        provjeri_zavrsna_polja(zavrsna, _potez, temp);
    }


        if(smjer_saha)
        {
        // Kralj je u šahu, provjeriti da li ova figura ima mogućnost da pokrije šah

        char polja_pomaka_lovca[8];

        for(int i=0; i<8; i++){
            polja_pomaka_lovca[i]=_moguca_polja[i];
            _moguca_polja[i]=0;
        }

        // Upiši polja koja se nalaze između kralja i napadačke figure
        upisi_putanju_saha(smjer_saha, _potez);

        for(int i=0; i<8; i++) /* Ako neko od polja pomaka lovca spada u putanju šaha
                                  tada će ono biti i upisano */
            _moguca_polja[i]=_moguca_polja[i]&polja_pomaka_lovca[i];
        }
    return;
}

void ploca::pomak_topa(const int& red, const int& kolona)
{
    int smjer_saha=provjera_na_sah(_potez);
    int zavrsna[18];

    /* Uklonimo figuru i provjerimo da li je kralj
       u šahu */

        char temp=_polja[red][kolona];
         _polja[red][kolona]=0;
         int smjer=provjera_na_sah(_potez);
        _polja[red][kolona]=temp;

       /* Ako je smjer Šaha > 10 napad je sa dvije strane,
       figura se ne smije nikako micati */
       if(smjer>10)
            return;

         if(smjer_saha!=smjer && smjer )
         {
             /* Šah je bez figure, a nije ako je figura tu
                ova figura ima ograničeno kretanje u zavisnosti
                od smjera šaha */

            /* Ako je šah iz dijagonalnog smjera,
               top se ne smije pomijerati */

            if(smjer<5)
                return;

            /* U suprotnom, šah je iz vertikalnog ili horizontalnog smjera
               i top se smije kretati samo u tom pravcu */

            if(smjer==5 || smjer==6){
                provjera_po_pravcima(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja, 65);
                std::vector<int> temp = {5, 6};
                provjeri_zavrsna_polja(zavrsna, _potez, temp);
            }
            else
            {
                provjera_po_pravcima(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja, 87);
                std::vector<int> temp = {7, 8};
                provjeri_zavrsna_polja(zavrsna, _potez, temp);
            }

            return;
         }else
         {
             /* Figura ne otkriva šah svojim pomakom
                slobodno kretanje */
            provjera_po_pravcima(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja, 0);
            std::vector<int> temp = {5, 6, 7, 8};
            provjeri_zavrsna_polja(zavrsna, _potez, temp);
         }

          /* Da li je protivnik dao kralju šah */

          if(smjer_saha)
          {
        /* Kralj je u sahu, provjeriti da li ova figura 
	   ima mogućnost da pokrije šah
	   Moguća polja topa su upisana ranije, sada se 
	   prepisuju u naredni niz */

        char polja_pomaka_topa[8];

        for(int i=0; i<8; i++){
            polja_pomaka_topa[i]=_moguca_polja[i];
            _moguca_polja[i]=0;
        }

        /* Upiši polja koja se nalaze između kralja i napadačke
           figure uključujući napadačku figuru */
        upisi_putanju_saha(smjer_saha, _potez);

        for(int i=0; i<8; i++) /* Ako neko od polja pomaka topa spada u putanju šaha
                                  tada će ono biti i upisano */
            _moguca_polja[i]=_moguca_polja[i] & polja_pomaka_topa[i];
          }
    return;
}

void ploca::pomak_skakaca(const int& red, const int& kolona)
{

    int smjer_saha=provjera_na_sah(_potez);
    int zavrsna[18];

    /* Uklonimo figuru i provjerimo da li je kralj
       sada u šahu */

        char temp=_polja[red][kolona];
         _polja[red][kolona]=0;
         int smjer=provjera_na_sah(_potez);
        _polja[red][kolona]=temp;

         if(smjer_saha!=smjer && smjer)
            /* Skakač se ne smije micati jer otkriva šah */
            return;
        else
        {
            /* Skakač ne otkriva šah svojim kretanjem */ 
            int broj_polja=provjera_po_L(red, kolona, zavrsna, &ploca::upisi_u_moguca_polja);
            std::vector<int> temp;
            for(int i=0; i<broj_polja/2; i++)
                temp.push_back(i);
            provjeri_zavrsna_polja(zavrsna, _potez, temp);
        }

        if(smjer_saha)
        {
        /* Kralj je u šahu, provjeriti da li ova 
	   figura ima mogućnost da pokrije šah */ 

        char polja_pomaka_skakaca[8];

        for(int i=0; i<8; i++){
            polja_pomaka_skakaca[i]=_moguca_polja[i];
            _moguca_polja[i]=0;
        }

        /* Upiši polja koja se nalaze između kralja i napadačke
           figure uključujući napadačku figuru */
        upisi_putanju_saha(smjer_saha, _potez);

        for(int i=0; i<8; i++) /* Ako neko od polja pomaka skakača spada u putanju šaha
                                  tada će ono biti i upisano */
            _moguca_polja[i]=_moguca_polja[i]&polja_pomaka_skakaca[i];
        }
    return;
}

int ploca::provjera_na_sah(bool boja, int red, int kolona)
{ /* boja: true - crna , false - bijela */

    int granicne[18]={0};
    int strana_napada=0;

    if(red==-1 && kolona==-1)
    {
        red=_kraljevi[boja*2];
        kolona=_kraljevi[boja*2+1];
    }

    /* Provjera za napad skakača */
    int broj_polja=provjera_po_L(red, kolona, granicne, nullptr);

    /* Provjera svih potencijalnih polja
       sa kojih napada protivnički skakač */
    for(int i=0; i<broj_polja/2; i++)
    {
        char figura=_polja[granicne[i*2]][granicne[i*2+1]];

        if(figura%8==SKAKAC && figura>>3 != boja)
            return 10;
    }

    /* Provjera dijagonale */
    provjera_po_dijagonalama(red, kolona,granicne, nullptr, 0);

   /* Imamo granične figure na lokacijama granicne, potrebno provjeriti
      jesu li suprotne boje i pri tome jos i lovac ili kraljica, odnosno pijun
      sa smjerova 0 i 1 za bijelog kralja, smjerovi 2 i 3 za crnog */

    for(int i=1; i<5; i++)
    {
        if(red==granicne[i*2] && kolona==granicne[i*2+1])
            continue;

        char figura=_polja[granicne[i*2]][granicne[i*2+1]];

        if(figura>>3 != boja)
        {
            /* Protivnicka figura je na tom polju
               Lovac ili kraljica? */
            if(figura%8 == LOVAC || figura%8 == KRALJICA)
                {
                    if(strana_napada) return i*10+strana_napada;
                        else strana_napada=i;
                }
            /* Da li pijun napada protivničko polje */
            if(figura%8 == PIJUN && ((boja==BIJELI && granicne[i*2]-1==red)
                // PIJUN ----------       NAPADA CRNOG-------
                                      || (boja==CRNI && granicne[i*2]+1==red)))
                                       // NAPADA BIJELOG ---------------
            {
                if(strana_napada) return i*10+strana_napada;
                else strana_napada=i;
            }

        }

    }

    provjera_po_pravcima(red, kolona,granicne, nullptr, 0);

    for(int i=5; i<9; i++)
    {
        if(red==granicne[i*2] && kolona==granicne[i*2+1])
            continue;

        char figura=_polja[granicne[i*2]][granicne[i*2+1]];

        if(figura>>3 != boja)
        {
            /* Protivnicka figura je na tom polju
               Top ili kraljica? */
            if(figura%8 == TOP || figura%8 == KRALJICA)
                {
                    if(strana_napada) return i*10+strana_napada;
                        else strana_napada=i;
                }
        }

    }

    for(int r=-1; r<2; r++){
        for(int k=-1; k<2; k++)
    {
        if(r==0 && k==0)
            continue;

        if((red+r)>7 || (red+r)<0 || (kolona+k)>7 || (kolona+k)<0)
            break;

        /* Provjera da li je na polju +i, +j
           protivnički kralj */

        if(_polja[red+r][kolona+k]== (8*(!boja)+KRALJ))
            return 9;
    }
    }

    return strana_napada;
}

void ploca::provjera_po_dijagonalama(const int& red, const int& kolona, int* zavrsna_polja, void (ploca::*ptr)(const int&, const int&), int smjer)
{
int i,j;

    if(smjer==0 || smjer%10==1)
    {/* Provjera pravca 1 */
    for(i=red+1, j=kolona-1; i<8 && j>-1 && _polja[i][j]==0; i++, j--)
        if(ptr) ((*this).*ptr)(i, j);

        if(zavrsna_polja){
            if(i<8 && j>-1)
            {
            zavrsna_polja[2]=i;
            zavrsna_polja[3]=j;
            }
        else{
            zavrsna_polja[2]=i-1;
            zavrsna_polja[3]=j+1;
            }
        }

        if(smjer>10)
            smjer/=10; /* Uklanjanje poslednje cifre smjera */
    }


    if(smjer==0 || smjer%10==2)
    {
    /* Provjera pravca 2 */
    for(i=red+1, j=kolona+1; i<8 && j<8 && _polja[i][j]==0; i++, j++)
        if(ptr) ((*this).*ptr)(i, j);

    if(zavrsna_polja){
        if(i<8 && j<8)
            {
            zavrsna_polja[4]=i;
            zavrsna_polja[5]=j;
            }
        else{
            zavrsna_polja[4]=i-1;
            zavrsna_polja[5]=j-1;
            }
        }

        if(smjer>10)
        smjer/=10;
    }

    if(smjer==0 || smjer%10==3)
    {
    /* Provjera pravca 3 */
    for(i=red-1, j=kolona-1; i>-1 && j>-1 && _polja[i][j]==0; i--, j--)
        if(ptr) ((*this).*ptr)(i, j);

    if(zavrsna_polja){
        if(i>-1 && j>-1)
            {
            zavrsna_polja[6]=i;
            zavrsna_polja[7]=j;
            }
        else{
            zavrsna_polja[6]=i+1;
            zavrsna_polja[7]=j+1;
            }
        }

        if(smjer>10)
        smjer/=10;
    }

    if(smjer==0 || smjer%10==4)
    {/* Provjera pravca */
    for(i=red-1, j=kolona+1; i>-1 && j<8 && _polja[i][j]==0; i--, j++)
        if(ptr) ((*this).*ptr)(i,j);

    if(zavrsna_polja){
        if(i>-1 && j<8)
            {
            zavrsna_polja[8]=i;
            zavrsna_polja[9]=j;
            }
        else{
            zavrsna_polja[8]=i+1;
            zavrsna_polja[9]=j-1;
            }
        }
    }
}

void ploca::provjera_po_pravcima(const int& red, const int& kolona, int* zavrsna_polja, void (ploca::*ptr)(const int&, const int&), int smjer)
{
    int i;

    if(smjer==0 || smjer%10==5)
    {
    /* Provjera pravca 5 */
    for(i=red+1; i<8 && _polja[i][kolona]==0; i++)
        if(ptr) ((*this).*ptr)(i, kolona);

    if(zavrsna_polja){

        if(i<8)
            zavrsna_polja[10]=i;
        else
            zavrsna_polja[10]=i-1;

        zavrsna_polja[11]=kolona;
        }

    if(smjer>10)
    smjer/=10;
    }

    if(smjer==0 || smjer%10==6)
    {
    /* Provjera pravca 6 */
     for(i=red-1; i>-1 && _polja[i][kolona]==0; i--)
        if(ptr) ((*this).*ptr)(i, kolona);

    if(zavrsna_polja)
    {
        if(i>-1)
            zavrsna_polja[12]=i;
        else
            zavrsna_polja[12]=i+1;

        zavrsna_polja[13]=kolona;
        }

    if(smjer>10)
    smjer/=10;
    }

    if(smjer==0 || smjer%10==7)
    {
   /* Provjera pravca 7 */
    for(i=kolona+1; i<8 && _polja[red][i]==0; i++)
        if(ptr) ((*this).*ptr)(red, i);

    if(zavrsna_polja){
        if(i<8)
            zavrsna_polja[15]=i;
        else
            zavrsna_polja[15]=i-1;

        zavrsna_polja[14]=red;
        }

    if(smjer>10)
    smjer/=10;
    }

    if(smjer==0 || smjer%10==8)
    {
    /* Provjera pravca 8 */
     for(i=kolona-1; i>-1 && _polja[red][i]==0; i--)
        if(ptr) ((*this).*ptr)(red, i);

    if(zavrsna_polja){
        if(i>-1)
            zavrsna_polja[17]=i;
        else
            zavrsna_polja[17]=i+1;

        zavrsna_polja[16]=red;
        }
    }
}

int ploca::provjera_po_L(const int& red, const int& kolona, int* niz_polja, void (ploca::*ptr)(const int&, const int&))
{
    /* Potrebno provjeriti relativna mjesta u parovima (red, kolona)
       (-2, -1), (-2, 1), (-1, -2), (-1, 2)
       ( 1, -2), ( 1, 2), (2 , -1), (2 , 1)
       */
    int pos=0; /* Pozicija za pisanje u dati niz */
    char temp;

    /* Prvi element gornjeg para */
    for(int i=-2; i<=2; i++)
    {
        if (i==0) continue;
        /* Predznak drugog elementa para */
        int r, k;
        for(int j=-1; j<2; j+=2)
        {
            r=red+i;

            if(i>0)
                k=kolona+j*(i%2+1);
            else
                k=kolona+j*(i%2-1);

            if(r>7 || k>7 || r<0 || k<0)
                continue;

            temp=_polja[r][k];
            if(temp==0 && ptr)
                ((*this).*ptr)(r, k);

           if(niz_polja)
           {
               niz_polja[pos]=r;
               niz_polja[pos+1]=k;
           }

           pos+=2;
        }
    }

return pos;
}

void ploca::provjera_za_kretanje_pijuna(const int& red, const int& kolona, void (ploca::*ptr)(const int&, const int&) , int smjer)
{
    /* Smjer kretanja pijuna */
    const int skp = (_polja[red][kolona]>>3)?-1:1;

    /* Pravci 1 ili 3 */
    if((smjer==0 || (smjer%10)==(-skp+2)) && kolona>0)
    {
        if((_polja[red+skp][kolona-1]!=0 &&
           (_polja[red+skp][kolona-1]>>3)!=(_polja[red][kolona]>>3))
             /* an passant */
         || (red+skp==_an_passant_polje[0] && kolona-1==_an_passant_polje[1]
             && _an_passant_boja != (skp-1)))
                ((*this).*ptr)(red+skp,kolona-1);

    if(smjer>10)
    smjer/=10;
    }

    /* Pravci 2 ili 4 */
    if((smjer==0 || (smjer%10)==(-skp+3)) && kolona<7)
    {
        if((_polja[red+skp][kolona+1]!=0 &&
           (_polja[red+skp][kolona+1]>>3)!=(_polja[red][kolona]>>3))
           || (red+skp==_an_passant_polje[0] && kolona+1==_an_passant_polje[1]))
                ((*this).*ptr)(red+skp,kolona+1);

        if(smjer>10)
            smjer/=10;
    }

    /* Pravci 5 ili 6 */
    if(smjer==0 || (smjer%10)==(-skp+11)/2)
    {
        if(_polja[red+skp][kolona]==0)
        {
            ((*this).*ptr)(red+skp,kolona);
            /* Polazni red? */
        if(red==(-5*skp+7)/2 && _polja[(7-skp)/2][kolona]==0)
            ((*this).*ptr)(red+2*skp,kolona);
        }
    }
    return;
}

void ploca::upisi_u_moguca_polja(const int& red, const int& kolona)
{
    _moguca_polja[red]+=(1<<kolona);
    return;
}

void ploca::provjeri_zavrsna_polja(int* zavrsna, const bool& boja, const std::vector<int>& pozicije)
{
        for(unsigned int i=0; i<pozicije.size(); i++)
        {
            if(pozicije[i]>8) continue;
            if(_polja[zavrsna[2*pozicije[i]]][zavrsna[2*pozicije[i]+1]]!=0 &&
               (_polja[zavrsna[2*pozicije[i]]][zavrsna[2*pozicije[i]+1]]>>3)!=boja)
                    upisi_u_moguca_polja(zavrsna[2*pozicije[i]], zavrsna[2*pozicije[i]+1]);
        }
return;
}

void ploca::upisi_putanju_saha(const int& smjer_saha, const bool& boja)
{
    int zavrsna[18];

    if(smjer_saha==10)
        {
           /* Pronađi na kojem polju je protivnički skakač */
            provjera_po_L(_kraljevi[boja*2], _kraljevi[boja*2+1], zavrsna, nullptr);
            for(int i=0; i<8; i++)
                if(_polja[zavrsna[2*i]][zavrsna[2*i+1]]==8*(!boja)+SKAKAC)
                {
                    upisi_u_moguca_polja(zavrsna[2*i],zavrsna[2*i+1]);
                    break;
                }
        }
        /* Šah nije od strane skakača, tako da upisujemo sva polja
           na kojima se može spriječiti šah */
        else if(smjer_saha<5){
            provjera_po_dijagonalama(_kraljevi[boja*2], _kraljevi[boja*2+1], zavrsna, &ploca::upisi_u_moguca_polja, smjer_saha);
            std::vector<int> temp={smjer_saha};
            provjeri_zavrsna_polja(zavrsna, boja, temp);
        }
        else{
                provjera_po_pravcima(_kraljevi[boja*2], _kraljevi[boja*2+1], zavrsna, &ploca::upisi_u_moguca_polja, smjer_saha);
                std::vector<int> temp={smjer_saha};
                provjeri_zavrsna_polja(zavrsna, boja, temp);
            }
            return;
}

char ploca::poz(const int& red, const int& kolona) const
{
    return _polja[red][kolona];
}

char ploca::izvrsi_transformaciju(const char& figura)
{
            _polja[_mjesto_transformacije[0]][_mjesto_transformacije[1]]
            =(-8*_mjesto_transformacije[0]+56)/7+figura;
            _transformacija=false;
            _potez=!_potez;
            return notacija[figura-1];
}

std::string ploca::sah_mat_pat(const bool& boja)
{
    std::string res("");

    int smjer=provjera_na_sah(boja);

        for(int i=0; i<8; i++)
            _moguca_polja[i]=0;

            /* Provjera da li se neka figura
               kraljeve boje može kretati */

            for(int i=0; i<8; i++)
            {
                for(int j=0; j<8; j++)
                {
                /* Kretanje kroz sva polja */
                    if(_polja[i][j]!=0 && _polja[i][j]>>3==boja)
                    {
                        /* Pronađeno polje na kome se nalazi
                        figura ovog kralja
                        Može li se ona kretati? */
                        ((*this).*_fje_provjera_pomaka[_polja[i][j]%8-1])(i, j);

                        int k;
                        for(k=0; k<8; k++)
                            if(_moguca_polja[k]!=0)
                                break;

                        if(k<8)
                            goto izlaz; /* Postoji figura koja se može
                                        kretati, nije ni šah-mat ni pat */
                    }
                }
            }
            /* Ne postoji figura koja se može kretati
               -> šah-mat ili pat */

            if(smjer)
                res="++";
            else
                res="1/2 1/2";

            return res;

            izlaz:

            if(smjer)
                res="+";

            return res;
}

int ploca::preostalo_vrijeme(const bool& boja, const int& init)
{
    if(_vrijeme[0]<1 || _vrijeme[1]<1)
    {
        if(_vrijeme[0]<1)
            _vrijeme[0]=0;
        else
            _vrijeme[1]=0;

        return _vrijeme[boja];
    }

    if(boja!=_potez)
        return _vrijeme[boja];

    time_t trenutno_vrijeme=time(0);
    if(init==1)
    {
        _vrijeme[_potez]-=difftime(trenutno_vrijeme, _vrijeme_mjerenja);
        _vrijeme_mjerenja=time(0);
        return _vrijeme[_potez];
    }
    else if(init==0)
        return _vrijeme[_potez]-difftime(trenutno_vrijeme, _vrijeme_mjerenja);
    else
        return _vrijeme[boja];
}

bool ploca::ima_materijal_za_mat(const bool& boja)
{
    int broj_figura=0;
    bool ima_lovac=false;

    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
    {
        char figura=_polja[i][j];
        if(figura!=0 && figura%8!=KRALJ)
        {
            if((figura>>3)==boja)
            {   /* Figura je odgovarajuće boje */
                if(figura%8==SKAKAC || figura%8==LOVAC)
                {
                    if(figura%8==LOVAC)
                        ima_lovac=true;

                    broj_figura++;
                    if(broj_figura>=2 && ima_lovac)
                        return true;
                }
                else
                    return true;
            }
        }
    }
    return false;
}

bool ploca::napravi_potez(const std::string& ptz)
{
    potez(ptz[1]-'1', ptz[0]-'a');
    if(potez(ptz[4]-'1', ptz[3]-'a') != "f")
        return true;

    return false;
}
