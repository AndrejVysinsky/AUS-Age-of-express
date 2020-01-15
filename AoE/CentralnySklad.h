#pragma once
#include "Importy.h"
#include "Vozidlo.h"
#include "Prekladisko.h"

class CentralnySklad
{
public:
	CentralnySklad();
	~CentralnySklad();
	void clear();

	void pridajVozidlo();
	void pridajDrona();
	Objednavka* vytvorObjednavku(const string& odkialEcv, unsigned int vzdOdkial, const string& kamEcv, unsigned int vzdKam, double vaha);
	
	void posunCasuOHodinu();
	void posunCasuCezNoc();
	void zapisanieStatistiky();

	void statNajviacZasielok(unsigned int odKedy, unsigned int doKedy, bool dorucene);
	void statZamietnute(unsigned int odKedy, unsigned int doKedy);
	void statPocetZrusenych(unsigned int odKedy, unsigned int doKedy);
	void statHodinyDronov();

	void vypisEvidenciuVozidiel();
	void vypisEvidenciuDronov(const string& ecv);

	void nacitanie(char * subor = nullptr);
	void ulozenie();

	void debug();

	unsigned int getDen() const { return den_; }
	unsigned int getCas() const { return serverovyCas_; }
	unsigned int getVzdialenostVozidiel() const { return vzdialenostVozidiel_; }
	unsigned int getPocetDorucenychZasielok() const { return pocetDorucenychZasielok_; }

private:
	unsigned int den_ = 1;
	unsigned int serverovyCas_ = 7 * 60;
	unsigned int pocetDorucenychZasielok_ = 0;
	unsigned int vzdialenostVozidiel_ = 0;
	structures::Array<Prekladisko*> prekladiska_ = NULL;
	structures::ArrayList<Vozidlo*> vozidla_;
	TypDronu t1_ = TypDronu(1, 2, 80, 40, 3);
	TypDronu t2_ = TypDronu(2, 5, 40, 60, 5);
};

