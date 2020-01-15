#pragma once
#include "Importy.h"
#include "Dron.h"
#include "Objednavka.h"

class Vozidlo;

class Prekladisko
{
public:
	Prekladisko(string ecvRegionu);
	Prekladisko(string ecvRegionu, unsigned int pocetDorucenych, unsigned int pocetOdoslanych, bool vyzdvihni);
	~Prekladisko();

	void pridajDrona(unsigned int den, unsigned int cas, unsigned int cislo, TypDronu* typ);
	void pridajVozidlo(Vozidlo* vozidlo) { vozidloPrekladiska_ = vozidlo; }

	void rozvezObjednavky(unsigned int serverovyCas, unsigned int& pocetDorucenychCelkovo);
	void novyDen();
	void prijmiObjednavku(Objednavka* obj, Dron* dron, Vozidlo* vozidlo);
	void prijmiObjednavkuNaDalsiDen(Objednavka* obj, Vozidlo* vozidlo);

	void pridajZamietnutuZasielku(Objednavka* obj) { zamietnuteZasielky_.add(obj); }

	bool skontrolujDostupnostDronov(unsigned int vzdialenost, double vaha);
	Vozidlo* getVhodneVozidlo(double vahaNoveho, bool dalsiDen);
	Dron* getVhodnyDron(double vaha, unsigned int vzdialenost, unsigned int &casVyzdvihnutia);

	bool zistiCiDronyStihaju(double vahaNoveho, unsigned int vzdialenostNoveho, bool dalsiDen);
	unsigned int prepocitajVzdialenost(unsigned int obsadenost, unsigned int bateria, unsigned int typ);

	void vypisZoznamDronov();

	//double getVahaVsetkychBalikov() const { return vahaVsetkychBalikov_; }
	bool maVozidlo() const { return vozidloPrekladiska_ != nullptr; }
	bool maObjednavky() const { return !objednavky_.isEmpty(); }
	bool maObjednavkyNaDalsiDen() const { return !objednavkyNaDalsiDen_.isEmpty(); }
	bool vyzdvihniObjednavky() const { return vyzdvihniObjednavky_; }


	unsigned int getPocetDorucenych() const { return pocetDorucenychZasielok_; }
	unsigned int getPocetOdoslanych() const { return pocetOdoslanychZasielok_; }
	unsigned int getPocetZrusenych() const { return zamietnuteZasielky_.size(); }
	string getEcvRegionu() const { return ecvRegionu_; }

private:
	string ecvRegionu_;
	structures::ArrayList<Dron*> dronyTypu1_{};
	structures::ArrayList<Dron*> dronyTypu2_{};
	Vozidlo* vozidloPrekladiska_ = nullptr;

	structures::LinkedList<Objednavka*> objednavky_{};
	structures::LinkedList<Objednavka*> objednavkyNaDalsiDen_{};
	structures::LinkedList<Objednavka*> zamietnuteZasielky_{};	

	unsigned int pocetDorucenychZasielok_ = 0;
	unsigned int pocetOdoslanychZasielok_ = 0;

	bool vyzdvihniObjednavky_;

	friend class CentralnySklad;
};

