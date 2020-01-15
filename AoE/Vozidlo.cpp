#include "Vozidlo.h"
#include "Prekladisko.h"


Vozidlo::Vozidlo(unsigned int datum, unsigned int nosnost, unsigned int naklady, string spz, unsigned int dlzkaTrasy, unsigned int pocetSkladov)
	: datumZaradeniaDoEvidencie_(datum), nosnost_(nosnost * 1000), nakladyNaRegion_(naklady), spz_(std::move(spz)), dlzkaTrasy_(dlzkaTrasy), trasaVozidla_(structures::Array<Prekladisko*>(pocetSkladov))
{
}

Vozidlo::Vozidlo(unsigned int datum, unsigned int nosnost, unsigned int naklady, unsigned int celkoveNaklady, double vahaB, double vahaBzajtra, unsigned int pozicia, string spz, unsigned int dlzkaTrasy, unsigned int pocetSkladov)
	:datumZaradeniaDoEvidencie_(datum), nosnost_(nosnost * 1000), nakladyNaRegion_(naklady), celkoveNaklady_(celkoveNaklady), vahaBalikov_(vahaB), vahaBalikovNaDalsiDen_(vahaBzajtra),
	pozicia_(pozicia), spz_(spz), dlzkaTrasy_(dlzkaTrasy), trasaVozidla_(structures::Array<Prekladisko*>(pocetSkladov))
{
}

Vozidlo::~Vozidlo()
{
}

void Vozidlo::pridajPrekladisko(Prekladisko * prekladisko, const unsigned int i)
{
	trasaVozidla_[i] = prekladisko;
}



void Vozidlo::pridajBalik(const double vaha, const bool dalsiDen)
{
	if (dalsiDen) {
		vahaBalikovNaDalsiDen_ += vaha;
	}
	else {
		vahaBalikov_ += vaha;
	}
}

unsigned int Vozidlo::rozvezBaliky()
{
	/*
		vozidlo ma index kde presne na trase sa nachaza
		1.faza zvazanie do skladu
			- od indexu doprava skontrolovat ci su sklady prazdne
			- ak ano dlzkaTrasy je index -- ak aj netreba nic na dalsi den rozviest vozidlo sa posle vzdy do centraly
		2.faza rozvazanie zo skladu
			- znova sa kontroluje trasa od konca ako daleko treba zasielky rozviest
			- ak sa cosi najde nastavi sa index, cize vozidlo ostane v poslednom prekladisku kde zaviezlo objednavky
	*/
	
	unsigned int pocetRegionovDoCentra = pozicia_;
	unsigned int pocetRegionovZCentra = 0;

	for (size_t i = trasaVozidla_.size() - 1; i > pozicia_; i--) {
		if (trasaVozidla_[i]->vyzdvihniObjednavky()) {
			pocetRegionovDoCentra += i * 2;
			pozicia_ = 0;
			break;
		}
	}
	
	for (size_t i = trasaVozidla_.size() - 1; i > 0; i--) {
		if (trasaVozidla_[i]->maObjednavkyNaDalsiDen()) {
			pocetRegionovZCentra = i;
			pozicia_ = i;
			break;
		}
	}

	celkoveNaklady_ += (pocetRegionovDoCentra + pocetRegionovZCentra) * nakladyNaRegion_;
	vahaBalikov_ = 0;
	vahaBalikovNaDalsiDen_ = 0;

	//vrati vzdialenost vzhladom na prejdenu cast trasy
	return (pocetRegionovDoCentra + pocetRegionovZCentra) * dlzkaTrasy_ / trasaVozidla_.size();
}

string Vozidlo::toStringTrasaVozidla()
{
	string trasa = to_string(trasaVozidla_.size());
	for (size_t i = 0; i < trasaVozidla_.size(); i++)
		trasa += " " + trasaVozidla_[i]->getEcvRegionu();

	return trasa;
}

void Vozidlo::toString()
{
	unsigned int den = datumZaradeniaDoEvidencie_ / 10000;
	unsigned int cas = datumZaradeniaDoEvidencie_ % 10000;
	cas /= 60;
	string datum = to_string(den) + ".den " + to_string(cas) + ":00";
	
	cout << left << setw(20) << setfill(' ') << datum;
	cout << left << setw(10) << setfill(' ') << spz_;
	cout << left << setw(5) << setfill(' ') << (nosnost_ / 1000);
	cout << left << setw(5) << setfill(' ') << celkoveNaklady_ << endl;
}
