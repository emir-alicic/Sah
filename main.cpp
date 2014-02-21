/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  Test funkcija za klasu ploca
 *
 *        Version:  1.0
 *        Created:  21/02/14 15:10:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Emir Aličić, alicic.emir@hotmail.com, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include<iostream>
#include"ploca.hpp"

using namespace std;

void iscrtaj_plocu(const ploca&);

int main()
{
    ploca sah;
    /* Počni igru */
  //  sah.novi_start(20);

    /* Pokušaj napraviti nekoliko poteza i 
     * ispiši povratnu informaciju */

        cout<<sah.potez(1, 4)<<endl;
        cout<<sah.potez(3, 4)<<endl;
	iscrtaj_plocu(sah);

       return 0;
}

void iscrtaj_plocu(const ploca& src)
{

    char oznake[]{'0', 'K', 'D', 'T', 'L', 'S', 'P'};
    cout<<endl;
    for(int j=0; j<8;j++)
    {
	for(int i=0; i<8;i++)
	    cout<<oznake[src.poz(7-j, i)%8]<<" ";	
	cout<<endl;
    }

    cout<<endl<<endl;

    return;
}
