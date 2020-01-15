#include "Dron.h"
#include <chrono>


Dron::Dron(TypDronu* typ, unsigned int datum, unsigned int serioveCislo, unsigned int cas)
	: typDronu_(typ), datumZaradeniaDoEvidencie_(datum), serioveCislo_(serioveCislo), obsadenostDo_(cas)
{
}

Dron::Dron(TypDronu * typ, unsigned int datum, unsigned int serioveCislo, unsigned int obsadenost, unsigned int bateria, unsigned int nalietaneHodiny, unsigned int prepraveneZasielky)
	: typDronu_(typ), datumZaradeniaDoEvidencie_(datum), serioveCislo_(serioveCislo), obsadenostDo_(obsadenost), bateria_(bateria), pocetNalienatychHodin_(nalietaneHodiny), pocetPrepravenychZasielok_(prepraveneZasielky)
{
}

Dron::~Dron()
{
}

void Dron::zavezObjednavku(const unsigned int vzdialenost)
{
	unsigned int obsadenost = getCasVyzdvihnutiaObjednavky(vzdialenost); //cas nabijania + cesta tam
	unsigned int cestaSpat = vzdialenost * 60 / typDronu_->getPriemernaRychlost();
	unsigned int minutaBateria = (cestaSpat * 2) * 100 / typDronu_->getMaxDobaLetu();

	if (minutaBateria > 100)
		minutaBateria = 100;

	bateria_ += pomocnaPreNabijanie_ * 10;

	if (bateria_ > 100)
		bateria_ = 100;

	bateria_ -= minutaBateria;

	obsadenostDo_ = obsadenost + cestaSpat;

	pocetNalienatychHodin_ += cestaSpat * 2;
	pocetPrepravenychZasielok_++;
}

unsigned int Dron::getCasNabitiaBaterie()
{
	/*
		zistuje za aky cas sa nabije na 100% ak tento cas + obsadenost je <= ako cas servera
		nastane pripad kedy je dron zbytocne necinny a sedi v sklade s plnou bateriou
	*/
	unsigned int bateria = bateria_;

	unsigned int casNaNabijanie = 0;
	while (bateria < 100) {
		casNaNabijanie += typDronu_->getCasNaNabitieDesatinyBaterky();
		bateria += 10;
	}

	return casNaNabijanie;
}

unsigned int Dron::getCasVyzdvihnutiaObjednavky(const unsigned int vzdialenost)
{
	unsigned int casNabijania = 0;
	pomocnaPreNabijanie_ = 0;
	while (getEfektivnyDolet(pomocnaPreNabijanie_ * 10) < 2 * vzdialenost) {
		casNabijania += typDronu_->getCasNaNabitieDesatinyBaterky();
		pomocnaPreNabijanie_++;
	}
	unsigned int casLetuKuZakaznikovi = vzdialenost * 60 / typDronu_->getPriemernaRychlost();
	return obsadenostDo_ + casNabijania + casLetuKuZakaznikovi;
}

unsigned int Dron::getEfektivnyDolet(unsigned int nabitie) const
{
	unsigned int ocakavanaBateria = bateria_ + nabitie > 100 ? 100 : bateria_ + nabitie;	
	return (typDronu_->getMaxDoletenaVzdialenost() * ocakavanaBateria) / 100;
}

unsigned int Dron::zistiKolkoPreleti(const unsigned int bateria) const
{
	unsigned int preletenaVzdialenost = 0;

	auto casOperovania = 18 * 60 - obsadenostDo_;
	unsigned int kolkoPoleti = bateria * typDronu_->getMaxDobaLetu() / 100;
	casOperovania -= kolkoPoleti;
	preletenaVzdialenost += typDronu_->getPriemernaRychlost() * kolkoPoleti / 60;
	unsigned int pocetCyklov = casOperovania / ((typDronu_->getMaxDobaLetu() / 10) + typDronu_->getCasNaNabitieDesatinyBaterky());
	unsigned int teoretickaDobaLetu = pocetCyklov * (typDronu_->getMaxDobaLetu() / 10);
	preletenaVzdialenost += typDronu_->getPriemernaRychlost() * teoretickaDobaLetu / 60;

	return preletenaVzdialenost;
}

void Dron::posunCasu(const unsigned int serverovyCas)
{
	if (obsadenostDo_ < serverovyCas) {
		unsigned int pocetNabiti = (serverovyCas - obsadenostDo_) / typDronu_->getCasNaNabitieDesatinyBaterky();
		unsigned int nabitie = pocetNabiti * 10;

		if (nabitie + bateria_ > 100)
			bateria_ = 100;
		else
			bateria_ += nabitie;

		obsadenostDo_ = serverovyCas;
	}
}

void Dron::resetAtributov()
{
	bateria_ = 100;
	obsadenostDo_ = 7 * 60;
}

void Dron::toString()
{
	unsigned int den = datumZaradeniaDoEvidencie_ / 10000;
	unsigned int cas = datumZaradeniaDoEvidencie_ % 10000;
	cas /= 60;
	string datum = to_string(den) + ".den " + to_string(cas) + ":00";

	unsigned int nalietaneHodiny = pocetNalienatychHodin_ / 60;
	unsigned int nalietaneMinuty = pocetNalienatychHodin_ % 60;

	string upravNalietanyCas = to_string(nalietaneHodiny) + ":";
	if (nalietaneMinuty < 10)
		upravNalietanyCas += "0" + to_string(nalietaneMinuty);
	else
		upravNalietanyCas += to_string(nalietaneMinuty);

	cout << left << setw(20) << setfill(' ') << datum;
	cout << left << setw(5) << setfill(' ') << typDronu_->getCisloTypu();
	cout << left << setw(20) << setfill(' ') << upravNalietanyCas;
	cout << left << setw(20) << setfill(' ') << pocetPrepravenychZasielok_ << endl;
}
