#pragma once
#include "Importy.h"

class Prekladisko;

using namespace std;

class Vozidlo
{
public:
	Vozidlo(unsigned int datum, unsigned int nosnost, unsigned int naklady, string spz, unsigned int dlzkaTrasy, unsigned int pocetSkladov);
	Vozidlo(unsigned int datum, unsigned int nosnost, unsigned int naklady, unsigned int celkoveNaklady, double vahaB, double vahaBzajtra, unsigned int pozicia, string spz, unsigned int dlzkaTrasy, unsigned int pocetSkladov);
	~Vozidlo();

	void pridajPrekladisko(Prekladisko* prekladisko, unsigned int i);

	void pridajBalik(double vaha, bool dalsiDen);
	unsigned int rozvezBaliky();

	double getZostavajucaNosnostVozidla() const { return nosnost_ - vahaBalikov_; }
	double getZostavajucaNosnostVozidlaNaDalsiDen() const { return nosnost_ - vahaBalikovNaDalsiDen_; }

	unsigned int getDatumZaradenia() const { return datumZaradeniaDoEvidencie_; }
	unsigned int getNosnost() const { return nosnost_; }
	unsigned int getNakladyNaRegion() const { return nakladyNaRegion_; }
	unsigned int getCelkoveNaklady() const { return celkoveNaklady_; }
	double getVahaBalikov() const { return vahaBalikov_; }
	double getVahaBalikovDalsiDen() const { return vahaBalikovNaDalsiDen_; }
	//bool getSiVcentrale() const { return siVCentrale_; }
	unsigned int getPozicia() const { return pozicia_; }
	string getSpz() const { return spz_; }
	unsigned int getDlzkaTrasy() const { return dlzkaTrasy_; }
	unsigned int getPocetRegionov() const { return trasaVozidla_.size(); }
	string toStringTrasaVozidla();

	void toString();

private:
	unsigned int datumZaradeniaDoEvidencie_;
	unsigned int nosnost_; // * 1000 na kg
	unsigned int nakladyNaRegion_;
	unsigned int celkoveNaklady_ = 0;
	double vahaBalikov_ = 0;
	double vahaBalikovNaDalsiDen_ = 0;
	unsigned int pozicia_ = 0;
	string spz_;
	unsigned int dlzkaTrasy_;
	structures::Array<Prekladisko*> trasaVozidla_;
};

