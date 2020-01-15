#include "Prekladisko.h"
#include "Vozidlo.h"
#include <iostream>
#include <utility>


Prekladisko::Prekladisko(string ecvRegionu) : ecvRegionu_(std::move(ecvRegionu))
{
}

Prekladisko::Prekladisko(string ecvRegionu, unsigned int pocetDorucenych, unsigned int pocetOdoslanych, bool vyzdvihni)
	: ecvRegionu_(std::move(ecvRegionu)), pocetDorucenychZasielok_(pocetDorucenych), pocetOdoslanychZasielok_(pocetOdoslanych), vyzdvihniObjednavky_(vyzdvihni)
{
}


Prekladisko::~Prekladisko()
{
	while (!dronyTypu1_.isEmpty())
		delete dronyTypu1_.removeAt(dronyTypu1_.size() - 1);
	while (!dronyTypu2_.isEmpty())
		delete dronyTypu2_.removeAt(dronyTypu2_.size() - 1);

	vozidloPrekladiska_ = nullptr;

	//maz iba objednavky ktore uz nikam nejdu (cize destinacia je tento sklad/region)
	while (!objednavky_.isEmpty()) {
		if (objednavky_[0]->getKamECV() == ecvRegionu_)
			delete objednavky_.removeAt(0);
		else
			objednavky_.removeAt(0);
	}

	while (!objednavkyNaDalsiDen_.isEmpty())
		delete objednavkyNaDalsiDen_.removeAt(0);

	while (!zamietnuteZasielky_.isEmpty())
		delete zamietnuteZasielky_.removeAt(0);
}

void Prekladisko::pridajDrona(unsigned int den, unsigned int cas, unsigned int cislo, TypDronu* typ)
{
	unsigned int t = typ->getCisloTypu();

	if (t == 1)
		dronyTypu1_.add(new Dron(typ, den * 10000 + cas, cislo, cas));
	else
		dronyTypu2_.add(new Dron(typ, den * 10000 + cas, cislo, cas));
}

void Prekladisko::rozvezObjednavky(unsigned int serverovyCas, unsigned int& pocetDorucenychCelkovo)
{
	/*
		v zozname sa nachadzaju vzdy baliky ktore treba rozviest v danom regione
		rozvazaj objednavky kym obsadenost vsetkych dronov nie je >= ako serverovyCas
		alebo kym sa neminu naplanovane objednavky
	*/
	while (!objednavky_.isEmpty()) {
		Objednavka* obj = objednavky_[0];
		unsigned int pomocna = 0;

		unsigned int vzdialenost = ecvRegionu_ == obj->getOdkialECV() ? obj->getOdkialVzdialenost() : obj->getKamVzdialenost();

		Dron* vhodnyDron = getVhodnyDron(obj->getHmotnost(), vzdialenost, pomocna);

		if (vhodnyDron->getObsadenostDo() + vhodnyDron->getCasNabitiaBaterie() >= serverovyCas)
			break;
		/*
			objednavky si pamataju sklady odkial ide a aj kam ide
			ak objednavka este kdesi smeruje tak sa NESMIE volat delete
			iba ak z tohoto skladu smeruje priamo ku zakaznikovi a nikto uz o nej nemusi vediet
		*/
		if (ecvRegionu_ == obj->getOdkialECV())
			objednavky_.removeAt(0);
		else
			delete objednavky_.removeAt(0);
		
		vhodnyDron->zavezObjednavku(vzdialenost);
		// + nejaka statistika
		pocetDorucenychCelkovo++;
		pocetDorucenychZasielok_++;
	}

	for (size_t i = 0; i < dronyTypu1_.size(); i++) {
		dronyTypu1_[i]->posunCasu(serverovyCas);
	}
	for (size_t i = 0; i < dronyTypu2_.size(); i++) {
		dronyTypu2_[i]->posunCasu(serverovyCas);
	}
}

void Prekladisko::novyDen()
{
	for (size_t i = 0; i < dronyTypu1_.size(); i++)
		dronyTypu1_[i]->resetAtributov();

	for (size_t i = 0; i < dronyTypu2_.size(); i++)
		dronyTypu2_[i]->resetAtributov();
	
	while (!objednavkyNaDalsiDen_.isEmpty()) {
		objednavky_.add(objednavkyNaDalsiDen_[0]);
		objednavkyNaDalsiDen_.removeAt(0);
	}

	while (!zamietnuteZasielky_.isEmpty())
		delete zamietnuteZasielky_.removeAt(0);

	vyzdvihniObjednavky_ = false;
	pocetOdoslanychZasielok_ = 0;
	pocetDorucenychZasielok_ = 0;
}

void Prekladisko::prijmiObjednavku(Objednavka * obj, Dron* dron, Vozidlo* vozidlo)
{
	/*
		dron si aktualizuje obsadenost do a bateriu
		vozidlo si pripocita atribut vahaObjednavok
	*/
	zamietnuteZasielky_.removeAt(zamietnuteZasielky_.size() - 1);

	/*
		tym ze dronu sa povie nech ide po objednavku tak je zbytocne ju pridavat do zoznamu
		v zozname budu tym padom len zasielky ktore treba rozviest v danom regione 
		(cize zasielky ktore boli dovezene z centralneho skladu)
	*/
	//objednavky_.insert(obj, poradovnik_++);
	dron->zavezObjednavku(obj->getOdkialVzdialenost());
	vozidlo->pridajBalik(obj->getHmotnost(), false);

	if (!vyzdvihniObjednavky_)
		vyzdvihniObjednavky_ = true;

	pocetOdoslanychZasielok_++;
}



void Prekladisko::prijmiObjednavkuNaDalsiDen(Objednavka * obj, Vozidlo * vozidlo)
{
	/*
		objednavka sa prida do poradia na dalsi den
		vozidlo si aktualizuje atribut vahaObjednavokNaDalsiDen
	*/
	objednavkyNaDalsiDen_.add(obj);
	vozidlo->pridajBalik(obj->getHmotnost(), true);
}

bool Prekladisko::skontrolujDostupnostDronov(const unsigned int vzdialenost, const double vaha)
{
	TypDronu* typ1 = nullptr;
	TypDronu* typ2 = nullptr;
	if (!dronyTypu1_.isEmpty())
		typ1 = dronyTypu1_[0]->getTyp();
	if (!dronyTypu2_.isEmpty())
		typ2 = dronyTypu2_[0]->getTyp();

	if (vaha > 2) {
		if (!dronyTypu2_.isEmpty()) {
			if (typ2->getMaxDoletenaVzdialenost() < vzdialenost * 2) {
				//nedoleteli
				return false;
			}
		}
		else {
			//nie su typy2
			return false;
		}
	}

	if (vaha <= 2) {
		if (!dronyTypu1_.isEmpty()) {
			if (typ1->getMaxDoletenaVzdialenost() < vzdialenost * 2) {
				//nedoleteli a typu 2 sa ani nepytaj
				return false;
			}
		}
		else {
			//typy1 nie su
			if (!dronyTypu2_.isEmpty()) {
				if (typ2->getMaxDoletenaVzdialenost() < vzdialenost * 2) {
					//nedoleteli
					return false;
				}
			}
			else {
				//nie su ani typ1 ani typ2
				return false;
			}
		}
	}

	return true;
}

Vozidlo* Prekladisko::getVhodneVozidlo(const double vahaNoveho, const bool dalsiDen)
{
	Vozidlo* vhodneVozidlo = nullptr;
	if (dalsiDen) {
		if (vozidloPrekladiska_->getZostavajucaNosnostVozidlaNaDalsiDen() >= vahaNoveho)
			vhodneVozidlo = vozidloPrekladiska_;
	}
	else {
		if (vozidloPrekladiska_->getZostavajucaNosnostVozidla() >= vahaNoveho)
			vhodneVozidlo = vozidloPrekladiska_;
	}
	return vhodneVozidlo;
}

Dron * Prekladisko::getVhodnyDron(const double vaha, const unsigned int vzdialenost, unsigned int &casVyzdvihnutia)
{
	/*
		ak je vaha > 2, najdi vhodny dron spomedzi dronov typu 2
			- vo vnutri triedy Dron sa prepocita najblizsi mozny cas kedy je dron schopny doletiet po balik
		ak je vaha <= 2, uprednostnuju sa najprv drony typu 1, vzhladom na ich lepsie parametre
		a nasledne ak sa nenasiel vhodny dron typu 1, hlada sa spomedzi dronov typu 2
		uprednostuje sa dron s najviac nabitou bateriou
		vracia nullptr len vtedy ak sa nenajde dron ktory by stihol vyzdvihnut balik do 20:00
	*/
	Dron* vhodnyDron = nullptr;
	// 20:00 je limit pre vyzdvihnutie zasielky
	unsigned int najlepsiCas = 20 * 60 + 1;
	if (vaha <= 2) {
		for (size_t i = 0; i < dronyTypu1_.size(); i++) {
			const auto cas = dronyTypu1_[i]->getCasVyzdvihnutiaObjednavky(vzdialenost);
			if (najlepsiCas > cas) {
				najlepsiCas = cas;
				vhodnyDron = dronyTypu1_[i];
			}
			if (najlepsiCas == cas && vhodnyDron->getBateria() < dronyTypu1_[i]->getBateria())
				vhodnyDron = dronyTypu1_[i];
		}
	}
	if (vhodnyDron == nullptr) {
		for (size_t i = 0; i < dronyTypu2_.size(); i++) {
			const auto cas = dronyTypu2_[i]->getCasVyzdvihnutiaObjednavky(vzdialenost);
			if (najlepsiCas > cas) {
				najlepsiCas = cas;
				vhodnyDron = dronyTypu2_[i];
			}
			if (najlepsiCas == cas && vhodnyDron->getBateria() < dronyTypu2_[i]->getBateria())
				vhodnyDron = dronyTypu2_[i];
		}
	}
	casVyzdvihnutia = najlepsiCas;
	return vhodnyDron;
}

bool Prekladisko::zistiCiDronyStihaju(double vahaNoveho, unsigned int vzdialenostNoveho, bool dalsiDen)
{
	structures::Array<unsigned int> vzdialenostTypu1(dronyTypu1_.size());
	structures::Array<unsigned int> vzdialenostTypu2(dronyTypu2_.size());
	/*
		polia vzdialenosti obsahuju vzdialenost ktoru je schopny preletiet dany dron
		od svojej obsadenosti az po limit 18:00
		(do vypoctu je zahrnuty aj cas nabijania)
	*/

	for (size_t i = 0; i < vzdialenostTypu1.size(); i++) {
		if (dalsiDen)
			vzdialenostTypu1[i] = prepocitajVzdialenost(7 * 60, 100, 1);
		else
			vzdialenostTypu1[i] = prepocitajVzdialenost(dronyTypu1_[i]->getObsadenostDo(), dronyTypu1_[i]->getBateria(), 1);
	}

	for (size_t i = 0; i < vzdialenostTypu2.size(); i++) {
		if (dalsiDen)
			vzdialenostTypu2[i] = prepocitajVzdialenost(7 * 60, 100, 2);
		else
			vzdialenostTypu2[i] = prepocitajVzdialenost(dronyTypu2_[i]->getObsadenostDo(), dronyTypu2_[i]->getBateria(), 2);
	}

	Objednavka* novaAkoze = new Objednavka(ecvRegionu_, vzdialenostNoveho, ecvRegionu_, vzdialenostNoveho, vahaNoveho);
	objednavky_.add(novaAkoze);
	
	bool nestihaju = false;

	for (Objednavka* obj : objednavky_) {

		unsigned int vzd = ecvRegionu_ == obj->getOdkialECV() ? obj->getOdkialVzdialenost() : obj->getKamVzdialenost();
		vzd *= 2;

		if (obj->getHmotnost() > 2) {
			for (size_t j = 0; j < vzdialenostTypu2.size(); j++) {
				if (vzdialenostTypu2[j] > vzd) {
					vzdialenostTypu2[j] -= vzd;
					goto zoberDalsiBalik;
				}
			}
		}
		else {
			for (size_t j = 0; j < vzdialenostTypu1.size(); j++) {
				if (vzdialenostTypu1[j] > vzd) {
					vzdialenostTypu1[j] -= vzd;
					goto zoberDalsiBalik;
				}
			}
			for (size_t j = 0; j < vzdialenostTypu2.size(); j++) {
				if (vzdialenostTypu2[j] > vzd) {
					vzdialenostTypu2[j] -= vzd;
					goto zoberDalsiBalik;
				}
			}
		}
		//ak sa dostane na break to znamena ze sa nenasiel dron ktory by teoreticky dokazal odniest balik a nevykonal sa prikaz goto
		nestihaju = true;
		break;

		zoberDalsiBalik:;
	}
	objednavky_.removeAt(objednavky_.size() - 1);
	delete novaAkoze;

	return !nestihaju;
}

unsigned int Prekladisko::prepocitajVzdialenost(unsigned int obsadenost, unsigned int bateria, unsigned int typ) {
	if (obsadenost >= 18 * 60)
		return 0;

	TypDronu* t = (typ == 1) ? dronyTypu1_[0]->getTyp() : dronyTypu2_[0]->getTyp();

	unsigned int cas = 18 * 60 - obsadenost;
	unsigned int casLetuSbateriou = t->getMaxDobaLetu() * bateria / 100; //kolko minut bude vo vzduchu s jeho bateriou
	unsigned int doletSbateriou = t->getPriemernaRychlost() * casLetuSbateriou / 60; //kolko preleti s bateriou

	//nestihne minut celu bateriu, vypocitaj iba kolko preleti za dostupny cas
	if (casLetuSbateriou > cas) {
		return t->getPriemernaRychlost() * cas / 60;
	}
	cas -= casLetuSbateriou;

	unsigned int dlzkaCyklu = t->getCasNaNabitieDesatinyBaterky() + (t->getMaxDobaLetu() / 10); // 7 min pre typ1 / 11 min pre typ2
	unsigned int pocetCyklov = cas / dlzkaCyklu;
	unsigned int casLetu = pocetCyklov * (t->getMaxDobaLetu() / 10);
	unsigned int dlzkaLetu = t->getPriemernaRychlost() * casLetu / 60;

	return dlzkaLetu + doletSbateriou;
}

void Prekladisko::vypisZoznamDronov()
{
	cout << left << setw(20) << setfill(' ') << "Datum Zaradenia";
	cout << left << setw(5) << setfill(' ') << "Typ";
	cout << left << setw(20) << setfill(' ') << "Nalietane Hodiny";
	cout << left << setw(20) << setfill(' ') << "Pocet zasielok" << endl;

	size_t index1 = 0;
	size_t index2 = 0;

	Dron* typ1 = nullptr;
	Dron* typ2 = nullptr;
	if (!dronyTypu1_.isEmpty() && !dronyTypu2_.isEmpty()) {
		while (index1 <= dronyTypu1_.size() - 1 && index2 <= dronyTypu2_.size() - 1) {

			typ1 = dronyTypu1_[index1];
			typ2 = dronyTypu2_[index2];

			if (typ1->getDatumZaradenia() <= typ2->getDatumZaradenia()) {
				typ1->toString();
				index1++;
			}
			else {
				typ2->toString();
				index2++;
			}
		}
	}

	if (!dronyTypu1_.isEmpty()) {
		while (index1 <= dronyTypu1_.size() - 1) {
			typ1 = dronyTypu1_[index1];
			typ1->toString();
			index1++;
		}
	}
	if (!dronyTypu2_.isEmpty()) {
		while (index2 <= dronyTypu2_.size() - 1) {
			typ2 = dronyTypu2_[index2];
			typ2->toString();
			index2++;
		}
	}
}