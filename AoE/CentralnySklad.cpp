#include "CentralnySklad.h"


CentralnySklad::CentralnySklad()
{
}

CentralnySklad::~CentralnySklad()
{
	clear();
}

void CentralnySklad::clear()
{
	for (size_t i = 0; i < prekladiska_.size(); i++)
		delete prekladiska_[i];

	while (!vozidla_.isEmpty())
		delete vozidla_.removeAt(vozidla_.size() - 1);
}

void CentralnySklad::pridajVozidlo()
{
	unsigned int nosnost, naklady, dlzkaTrasy, pocetSkladov;
	string spz, region;

	cout << "Zadajte spz:\n";
	cin >> spz;	
	cout << "Zadajte nosnost:\n";
	cin >> nosnost;
	cout << "Zadajte naklady na region:\n";
	cin >> naklady;
	cout << "Zadajte dlzka trasy:\n";
	cin >> dlzkaTrasy;
	cout << "Zadajte pocet regionov a vypiste ich:\n";
	cin >> pocetSkladov;

	Vozidlo* nove = new Vozidlo(den_ * 10000 + serverovyCas_, nosnost, naklady, spz, dlzkaTrasy, pocetSkladov); 
	unsigned int index = 0;
	for (unsigned int i = 0; i < pocetSkladov; i++) {
		while (true) {
			cin >> region;
			for (size_t j = 0; j < prekladiska_.size(); j++) {
				if (region == prekladiska_[j]->ecvRegionu_) {
					nove->pridajPrekladisko(prekladiska_[j], index++);
					prekladiska_[j]->pridajVozidlo(nove);
					goto NacitajDalsiSklad;
				}
			}
			cout << "Zadane prekladisko neexistuje!\n";
		}
		NacitajDalsiSklad:;
	}
	vozidla_.add(nove);
}

void CentralnySklad::pridajDrona()
{
	unsigned int cislo, typ;
	string ecv;

	cout << "Zadajte seriove cislo drona:\n";
	cin >> cislo;
	cout << "Zadajte typ drona (cislom 1/2):\n";
	cin >> typ;

	TypDronu* typDronu = typ == 1 ? &t1_ : &t2_;

	cout << "Zadajte prekladisko do ktoreho sa ma pridat novy dron:\n";
	while (true) {
		cin >> ecv;
		for (size_t i = 0; i < prekladiska_.size(); i++) {
			if (ecv == prekladiska_[i]->ecvRegionu_) {
				prekladiska_[i]->pridajDrona(den_, serverovyCas_, cislo, typDronu);
				goto Skonci;
			}
		}
		cout << "Zadane prekladisko neexistuje!\n";
	}
	Skonci:;	
}

Objednavka* CentralnySklad::vytvorObjednavku(const string& odkialEcv, unsigned int vzdOdkial, const string& kamEcv, unsigned int vzdKam, double vaha)
{
	/*
		zisti ci nazvy prekladisiek boli zadane spravne, teda ci existuju
	*/

	Prekladisko* odkial = nullptr;
	Prekladisko* kam = nullptr;
	for (size_t i = 0; i < prekladiska_.size(); i++) {
		if (prekladiska_[i]->ecvRegionu_ == odkialEcv) {
			odkial = prekladiska_[i];
		}
		if (prekladiska_[i]->ecvRegionu_ == kamEcv) {
			kam = prekladiska_[i];
		}
	}

	if (odkial == nullptr || kam == nullptr) {
		cout << "Zadane prekladisko neexistuje.\n";
		return nullptr;
	}

	//do predu sa rata ze zasielka bude zamietnuta a ak prejde vsetkymi checkmi tak sa prida medzi prijate z dovodu vacsej prehladnosti v kode
	Objednavka * obj = new Objednavka(odkialEcv, vzdOdkial, kamEcv, vzdKam, vaha);
	odkial->pridajZamietnutuZasielku(obj);

	/*
		vychadzajme z prepokladu ze prekladisko ma vzdy aspon jeden dron
		ak je vaha viac ako 5, nesplna to ani typ1 ani typ2
		ak je vaha viac ako 2, nesplna to typ1, preto sa treba opytat ci existuje typ2 v danom prekladisku
		kontroluje aj pre zaciatok aj destinaciu (prekladisko)
		kontroluje sa ci drony su schopne preletiet zadanu vzdialenost * 2, tj. pre balik a spat
	*/
	if (vaha > 5) {
		obj->setDovodZamietnutia("Balik nesplna vahovy limit 5kg.\n");
		return obj;
	}

	if (!odkial->skontrolujDostupnostDronov(vzdOdkial, vaha)) {
		obj->setDovodZamietnutia("V zaciatocnom prekladisku sa nenachadzaju vhodne drony.\n");
		return obj;
	}

	if (!kam->skontrolujDostupnostDronov(vzdKam, vaha)) {
		obj->setDovodZamietnutia("V koncovom prekladisku sa nenachadzaju vhodne drony.\n");
		return obj;
	}

	/*
		kazdy sklad bud nema alebo ma prave jedno vozidlo, a trasa vsetkych vozidiel je nemenna (fixna)
		z prekladiska "odkial" zisti vozidlo ktore nim prechadza
		nasledne sa spyta ci dokaze ten balik prepravit
	*/
	if (!odkial->maVozidlo()) {
		obj->setDovodZamietnutia("Zaciatocny sklad nema priradene vozidlo.\n");
		return obj;
	}
	if (!kam->maVozidlo()) {
		obj->setDovodZamietnutia("Koncovy sklad nema priradene vozidlo\n");
		return obj;
	}

	Vozidlo* vhodneVozidlo = odkial->getVhodneVozidlo(vaha, false);
	Vozidlo* vhodneVozidloNaDalsiDen = kam->getVhodneVozidlo(vaha, true);

	if (vhodneVozidlo == nullptr || vhodneVozidloNaDalsiDen == nullptr) {
		obj->setDovodZamietnutia("Prijatie objednavky by prekrocilo nosnost dostupnych vozidiel.\n");
		return obj;
	}

	unsigned int najlepsiCas;
	//najlepsiCas sa posiela cez referenciu, cize mala by sa dat upravit hodnota v tele funkcie getVhodnyDron
	Dron* vhodnyDron = odkial->getVhodnyDron(vaha, vzdOdkial, najlepsiCas);

	//nullptr jedine vtedy ak sa nenasiel dron ktory by vyzdvihol balik do 20:00
	if (vhodnyDron == nullptr) {
		obj->setDovodZamietnutia("Objednavku bolo mozne vydzvihnut az po 20:00\n");
		return obj;
	}
		
	/*
		zistiCiDronyStihaju:
			-viac menej simulacia ktora prepocita (takmer presne) kolko dron dokaze preletiet do 18:00
			-dalej sa zisti kolko potrebuju baliky aby sa preletelo
			-a zisti sa ci drony zvladnu preletiet viac alebo rovnako ako si vyzaduju objednavky
			-kontroluje sa aj pre zaciatocny a koncovy region

			-ak je cas vacsi alebo rovny 18:00 zistovat ci drony stihaju rozviest baliky do 18:00 je zbytocne, kedze ich uz rozviezli
			-metoda ma jeden z argumentov boolean, kvoli rozliseniu ci sa jedna o momentalny stav prekladiska a dronov alebo
			 o nasledujuci den v koncovom prekladisku
	*/

	//ak nema objednavky tak netreba kontrolovat lebo nema co rozvazat do 18:00
	if (serverovyCas_ < 18 * 60 && odkial->maObjednavky()) {
		if (!odkial->zistiCiDronyStihaju(vaha, vzdOdkial, false)) {
			obj->setDovodZamietnutia("Drony v zaciatocnom prekladisku by nestihli dorucit objednavky do 18:00\n");
			return obj;
		}
	}

	if (!kam->zistiCiDronyStihaju(vaha, vzdKam, true)) {
		obj->setDovodZamietnutia("Drony v koncovom prekladisku by nestihli dorucit objednavky do 18:00\n");
		return obj;
	}

	if (najlepsiCas - serverovyCas_ > 60) {
		string rozhodnutie;
		cout << "Cas vyzdvihnutia zasielky je viac ako 1 hodina\n";
		cout << "Prajete si objednavku zrusit ?\n";
		cout << "ano/nie\n";
		cin >> rozhodnutie;
		if (rozhodnutie == "ano") {
			obj->setDovodZamietnutia("Objednavka bola zamietnuta zakaznikom.\n");
			return obj;
		}
	}

	/*
		vytvorenie objednavky
	*/
	odkial->prijmiObjednavku(obj, vhodnyDron, vhodneVozidlo);
	kam->prijmiObjednavkuNaDalsiDen(obj, vhodneVozidloNaDalsiDen);

	cout << "Objednavka bola uspesne vytvorena.\n";

	return obj;
}

void CentralnySklad::posunCasuOHodinu()
{
	/*
		POSUN CASU O JEDNU HODINU
		den zacina na 7:00 kedy je mozne vyslat drony po objednavky (bud doniest, alebo rozviest)
		ak uzivatel nevytvori ziadnu objednavku a cas sa posunie o jednu hodinu
			-vyslu sa drony po objednavky vo fronte nasledovne:
				-posielaju sa baliky kym obsadenost kazdeho drona nie je vacsia ako aktualnyCas + 1 (alebo kym existuju baliky vo fronte)
				-ak je vyssie uvedena podmienka splnena dron nerozvaza dalsie zasielky az kym sa neposunie cas o dalsiu hodinu
				-tymto sa zabezpeci ze ak niekto vytvori objednavku medzi posunmi, moze mu byt (takmer) okamzite vyslany dron
			-taktiez treba kontrolovat ci s novou objednavkou sa stihnu naplanovane baliky rozposlat do 18:00 (riesi sa pri vytvoreni objednavky)
	*/
	if (serverovyCas_ == 21 * 60) {
		serverovyCas_ = 7 * 60;
		den_++;
		posunCasuCezNoc();
		return;
	}
	serverovyCas_ += 1 * 60;

	for (size_t i = 0; i < prekladiska_.size(); i++) {
		prekladiska_[i]->rozvezObjednavky(serverovyCas_, pocetDorucenychZasielok_);
	}

}

void CentralnySklad::posunCasuCezNoc()
{	
	//vozidla
	for (size_t i = 0; i < vozidla_.size(); i++) {
		vzdialenostVozidiel_ += vozidla_[i]->rozvezBaliky();
	}

	string volba;
	cout << "Zapisat statistiku? ano/nie\n";
	cin >> volba;
	if (volba == "ano")
		zapisanieStatistiky();

	//nastavi baterku na 100 a obsadenost na 7:00
	for (size_t i = 0; i < prekladiska_.size(); i++) {
		prekladiska_[i]->novyDen();
	}
}

void CentralnySklad::zapisanieStatistiky()
{
	ofstream vystup("resources/statistika/" + to_string(den_ - 1) + ".txt");

	for (size_t i = 0; i < prekladiska_.size(); i++) {
		Prekladisko* p = prekladiska_[i];
		vystup << p->getEcvRegionu() << " " << p->getPocetDorucenych() << " " << p->getPocetOdoslanych() << endl;
		vystup << p->zamietnuteZasielky_.size() << endl;
		for (size_t j = 0; j < p->zamietnuteZasielky_.size(); j++) {
			Objednavka* o = p->zamietnuteZasielky_[j];
			vystup << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << " " << o->getKamECV() << " " << o->getKamVzdialenost() << " " << o->getHmotnost() << " " << o->getDovodZamietnutia();
		}
	}
}

void CentralnySklad::statNajviacZasielok(unsigned int odKedy, unsigned int doKedy, bool dorucene)
{
	structures::Array<int> pocetZasielok = structures::Array<int>(prekladiska_.size());

	unsigned int doruceneZ, odoslaneZ;
	unsigned int zamietnute;
	string tempS;
	for (unsigned int i = odKedy; i <= doKedy; i++) {
		ifstream vstup("resources/statistika/" + to_string(i) + ".txt");
		if (!vstup.is_open()) {
			cout << "Nepodarilo sa nacitat subor " << "resources/statistika/" << to_string(i) << ".txt" << endl;
			continue;
		}
		
		for (size_t j = 0; j < pocetZasielok.size(); j++) {
			vstup >> tempS >> doruceneZ >> odoslaneZ;
			if (dorucene)
				pocetZasielok[j] += doruceneZ;
			else
				pocetZasielok[j] += odoslaneZ;

			vstup >> zamietnute;
			getline(vstup, tempS);
			for (unsigned int k = 0; k < zamietnute; k++)
				getline(vstup, tempS);
		}
		vstup.close();		
	}

	if (dorucene)
		cout << "Najviac dorucenych zasielok v danom obdovi bolo v:\n";
	else
		cout << "Najviac odoslanych zasielok v danom obdovi bolo v:\n";

	size_t index = 0;
	int max = 0;
	for (size_t i = 0; i < pocetZasielok.size(); i++) {
		if (pocetZasielok[i] > max) {
			index = i;
			max = pocetZasielok[i];
		}
	}

	cout << prekladiska_[index]->getEcvRegionu() << endl;
	cout << max << endl;
}

void CentralnySklad::statZamietnute(unsigned int odKedy, unsigned int doKedy)
{
	string ecv;
	cout << "Zadajte ecv regionu\n";
	cin >> ecv;
	bool existuje = false;
	for (size_t i = 0; i < prekladiska_.size(); i++) {
		if (ecv == prekladiska_[i]->getEcvRegionu()) {
			existuje = true;
			break;
		}
	}
	if (!existuje) {
		cout << "Zadany region neexistuje\n";
		return;
	}

	structures::ArrayList<Objednavka*> temp{};

	string retazec1, retazec2, dovod;
	unsigned int cislo1, cislo2, pocetZamietnutych;
	double realne1;
	for (unsigned int i = odKedy; i <= doKedy; i++) {
		ifstream vstup("resources/statistika/" + to_string(i) + ".txt");
		if (!vstup.is_open()) {
			cout << "Nepodarilo sa nacitat subor " << "resources/statistika/" << to_string(i) << ".txt" << endl;
			continue;
		}

		for (size_t j = 0; j < prekladiska_.size(); j++) {
			vstup >> retazec1 >> cislo1 >> cislo2;

			vstup >> pocetZamietnutych;
			for (unsigned int k = 0; k < pocetZamietnutych; k++) {
				vstup >> retazec1 >> cislo1 >> retazec2 >> cislo2 >> realne1;
				getline(vstup, dovod);
				dovod.erase(0, 1);
				if (retazec1 == ecv) {
					Objednavka* o = new Objednavka(retazec1, cislo1, retazec2, cislo2, realne1);
					o->setDovodZamietnutia(dovod + "\n");
					temp.add(o);
				}
			}
		}
		vstup.close();
	}

	for (size_t i = 0; i < temp.size(); i++)
		temp[i]->toString();

	while (!temp.isEmpty())
		delete temp.removeAt(temp.size() - 1);
}

void CentralnySklad::statPocetZrusenych(unsigned int odKedy, unsigned int doKedy)
{
	structures::Array<int> pocetZrusenych = structures::Array<int>(prekladiska_.size());

	unsigned int cislo;
	string retazec;

	for (unsigned int i = odKedy; i <= doKedy; i++) {
		ifstream vstup("resources/statistika/" + to_string(i) + ".txt");
		if (!vstup.is_open()) {
			cout << "Nepodarilo sa nacitat subor " << "resources/statistika/" << to_string(i) << ".txt" << endl;
			continue;
		}

		for (size_t j = 0; j < prekladiska_.size(); j++) {
			vstup >> retazec >> cislo >> cislo;			
			vstup >> cislo;
			pocetZrusenych[j] += cislo;

			getline(vstup, retazec);
			for (unsigned int k = 0; k < cislo; k++)
				getline(vstup, retazec);
		}
		vstup.close();
	}

	for (size_t i = 0; i < prekladiska_.size(); i++)
		cout << prekladiska_[i]->getEcvRegionu() << " " << pocetZrusenych[i] << endl;
}

void CentralnySklad::statHodinyDronov()
{
	for (size_t i = 0; i < prekladiska_.size(); i++) {
		Prekladisko* p = prekladiska_[i];
		cout << p->getEcvRegionu() << endl;
		unsigned int hodinyTypu1 = 0, hodinyTypu2 = 0;

		for (size_t j = 0; j < p->dronyTypu1_.size(); j++)
			hodinyTypu1 += p->dronyTypu1_[j]->getNalietaneHodiny();

		for (size_t j = 0; j < p->dronyTypu2_.size(); j++)
			hodinyTypu2 += p->dronyTypu2_[j]->getNalietaneHodiny();

		cout << "Typ1 : " << hodinyTypu1 / 60 << "hod " << hodinyTypu1 % 60 << "min" << endl;
		cout << "Typ2 : " << hodinyTypu2 / 60 << "hod " << hodinyTypu2 % 60 << "min" << endl;
	}
}

void CentralnySklad::vypisEvidenciuVozidiel()
{
	cout << left << setw(20) << setfill(' ') << "Datum zaradenia" << setw(10) << setfill(' ') << "Spz" << setw(5) << setfill(' ') << "T" << "Naklady" << endl;
	for (size_t i = 0; i < vozidla_.size(); i++)
		vozidla_[i]->toString();
}

void CentralnySklad::vypisEvidenciuDronov(const string& ecv)
{
	for (size_t i = 0; i < prekladiska_.size(); i++) {
		if (ecv == prekladiska_[i]->ecvRegionu_) {
			prekladiska_[i]->vypisZoznamDronov();
			return;
		}
	}
	cout << "Zadane prekladisko neexistuje\n";
}

void CentralnySklad::nacitanie(char* subor)
{
	string nazov;

	if (subor == nullptr) {
		cout << "Zadajte nazov suboru na nacitanie:\n";
		cin >> nazov;
		nazov = "resources/" + nazov + ".txt";
	}
	else {
		nazov = subor;
	}

	ifstream vstup(nazov);
	if (!vstup.is_open()) {
		cout << "Nepodarilo sa nacitat vstupny subor\n";
		return;
	}

	clear();

	unsigned int pocetPrekladisiek;
	vstup >> den_ >> serverovyCas_ >> pocetDorucenychZasielok_ >> vzdialenostVozidiel_ >> pocetPrekladisiek;
	prekladiska_ = structures::Array<Prekladisko*>(pocetPrekladisiek);

	for (size_t i = 0; i < prekladiska_.size(); i++) {
		string ecv;
		unsigned int pocetDorucenych, pocetOdoslanych;
		bool vyzdvihniObjednavky;
		vstup >> ecv >> pocetDorucenych >> pocetOdoslanych >> vyzdvihniObjednavky;
		prekladiska_[i] = new Prekladisko(ecv, pocetDorucenych, pocetOdoslanych, vyzdvihniObjednavky);

		unsigned int pocetDronovTypu1, pocetDronovTypu2;
		unsigned int datum, cislo, obsadenost, bateria, nalietaneHodiny, prepraveneZasielky;
		vstup >> pocetDronovTypu1;
		for (unsigned int j = 0; j < pocetDronovTypu1; j++) {
			vstup >> datum >> cislo >> obsadenost >> bateria >> nalietaneHodiny >> prepraveneZasielky;
			prekladiska_[i]->dronyTypu1_.add(new Dron(&t1_, datum, cislo, obsadenost, bateria, nalietaneHodiny, prepraveneZasielky));
		}
		vstup >> pocetDronovTypu2;
		for (unsigned int j = 0; j < pocetDronovTypu2; j++) {
			vstup >> datum >> cislo >> obsadenost >> bateria >> nalietaneHodiny >> prepraveneZasielky;
			prekladiska_[i]->dronyTypu2_.add(new Dron(&t2_, datum, cislo, obsadenost, bateria, nalietaneHodiny, prepraveneZasielky));
		}

		unsigned int pocetObj, pocetObjDalsiDen, pocetZamietnychObj;
		string odkialEcv, kamEcv, dovodZamietnutia;
		unsigned int odkialVzd, kamVzd;
		double vaha;
		vstup >> pocetObj;
		for (unsigned int j = 0; j < pocetObj; j++) {
			vstup >> odkialEcv >> odkialVzd >> kamEcv >> kamVzd >> vaha;
			prekladiska_[i]->objednavky_.add(new Objednavka(odkialEcv, odkialVzd, kamEcv, kamVzd, vaha));
		}
		vstup >> pocetObjDalsiDen;
		for (unsigned int j = 0; j < pocetObjDalsiDen; j++) {
			vstup >> odkialEcv >> odkialVzd >> kamEcv >> kamVzd >> vaha;
			prekladiska_[i]->objednavkyNaDalsiDen_.add(new Objednavka(odkialEcv, odkialVzd, kamEcv, kamVzd, vaha));
		}
		vstup >> pocetZamietnychObj;
		for (unsigned int j = 0; j < pocetZamietnychObj; j++) {
			vstup >> odkialEcv >> odkialVzd >> kamEcv >> kamVzd >> vaha;
			getline(vstup, dovodZamietnutia);
			dovodZamietnutia.erase(0, 1);
			Objednavka* nova = new Objednavka(odkialEcv, odkialVzd, kamEcv, kamVzd, vaha);
			nova->setDovodZamietnutia(dovodZamietnutia + "\n");
			prekladiska_[i]->zamietnuteZasielky_.add(nova);
		}
	}
	unsigned int pocetVozidiel;
	unsigned int datum, nosnost, naklady, celkoveNaklady, pozicia, dlzkaTrasy, pocetSkladov;
	double vahaBalikov, vahaBalikovDalsiDen;
	string spz, ecv;

	vstup >> pocetVozidiel;
	for (unsigned int i = 0; i < pocetVozidiel; i++) {
		vstup >> datum >> nosnost >> naklady >> celkoveNaklady >> vahaBalikov >> vahaBalikovDalsiDen >> pozicia >> spz >> dlzkaTrasy >> pocetSkladov;
		Vozidlo* nove = new Vozidlo(datum, nosnost, naklady, celkoveNaklady, vahaBalikov, vahaBalikovDalsiDen, pozicia, spz, dlzkaTrasy, pocetSkladov);
		for (unsigned int j = 0; j < pocetSkladov; j++) {
			vstup >> ecv;
			for (size_t k = 0; k < prekladiska_.size(); k++) {
				if (prekladiska_[k]->ecvRegionu_ == ecv) {
					nove->pridajPrekladisko(prekladiska_[k], j);
					prekladiska_[k]->vozidloPrekladiska_ = nove;
					break;
				}
			}
		}
		vozidla_.add(nove);
	}
	vstup.close();
}

void CentralnySklad::ulozenie()
{
	string subor;
	cout << "Zadajte nazov suboru na ulozenie:\n";
	cin >> subor;
	ofstream vystup("resources/" + subor + ".txt");

	vystup << den_ << endl;
	vystup << serverovyCas_ << endl;
	vystup << pocetDorucenychZasielok_ << endl;
	vystup << vzdialenostVozidiel_ << endl;
	vystup << prekladiska_.size() << endl;

	for (size_t i = 0; i < prekladiska_.size(); i++) {
		Prekladisko* p = prekladiska_[i];
		vystup << p->ecvRegionu_ << " " << p->pocetDorucenychZasielok_ << " " << p->pocetOdoslanychZasielok_ << " " << p->vyzdvihniObjednavky_ << endl;
		vystup << p->dronyTypu1_.size() << endl;
		for (size_t j = 0; j < p->dronyTypu1_.size(); j++) {
			Dron* d = p->dronyTypu1_[j];
			vystup << d->getDatumZaradenia() << " " << d->getSerioveCislo() << " " << d->getObsadenostDo() << " " << d->getBateria() << " " << d->getNalietaneHodiny() << " " << d->getPrepraveneZasielky() << endl;
		}
		vystup << p->dronyTypu2_.size() << endl;
		for (size_t j = 0; j < p->dronyTypu2_.size(); j++) {
			Dron* d = p->dronyTypu2_[j];
			vystup << d->getDatumZaradenia() << " " << d->getSerioveCislo() << " " << d->getObsadenostDo() << " " << d->getBateria() << " " << d->getNalietaneHodiny() << " " << d->getPrepraveneZasielky() << endl;
		}
		vystup << p->objednavky_.size() << endl;
		for (size_t j = 0; j < p->objednavky_.size(); j++) {
			Objednavka* o = p->objednavky_[j];
			vystup << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << " " << o->getKamECV() << " " << o->getKamVzdialenost() << " " << o->getHmotnost() << endl;
		}
		vystup << p->objednavkyNaDalsiDen_.size() << endl;
		for (size_t j = 0; j < p->objednavkyNaDalsiDen_.size(); j++) {
			Objednavka* o = p->objednavkyNaDalsiDen_[j];
			vystup << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << " " << o->getKamECV() << " " << o->getKamVzdialenost() << " " << o->getHmotnost() << endl;
		}
		vystup << p->zamietnuteZasielky_.size() << endl;
		for (size_t j = 0; j < p->zamietnuteZasielky_.size(); j++) {
			Objednavka* o = p->zamietnuteZasielky_[j];
			vystup << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << " " << o->getKamECV() << " " << o->getKamVzdialenost() << " " << o->getHmotnost() << " " << o->getDovodZamietnutia();
		}
	}
	vystup << vozidla_.size() << endl;
	for (size_t i = 0; i < vozidla_.size(); i++) {
		Vozidlo* v = vozidla_[i];
		vystup << v->getDatumZaradenia() << " " << v->getNosnost() / 1000 << " " << v->getNakladyNaRegion() << " " << v->getCelkoveNaklady() << " " << v->getVahaBalikov() << " ";
		vystup << v->getVahaBalikovDalsiDen() << " " << v->getPozicia() << " " << v->getSpz() << " " << v->getDlzkaTrasy() << " " << v->toStringTrasaVozidla() << endl;
	}
	vystup.close();
}

void CentralnySklad::debug()
{

	for (size_t i = 0; i < prekladiska_.size(); i++) {
		Prekladisko* p = prekladiska_[i];

		if (!(p->getEcvRegionu() == "MT" || p->getEcvRegionu() == "LM"))
			continue;

		cout << p->getEcvRegionu() << " " << p->getPocetDorucenych() << " " << p->getPocetOdoslanych() << endl;

		cout << "\nDrony typu 1\n";
		for (size_t j = 0; j < p->dronyTypu1_.size(); j++) {
			Dron* d = p->dronyTypu1_[j];
			string obsadenost = to_string(d->getObsadenostDo() / 60) + ":";
			if (d->getObsadenostDo() % 60 < 10)
				obsadenost += "0";
			obsadenost += to_string(d->getObsadenostDo() % 60);
			cout << d->getBateria() << "% " << obsadenost << " " << d->getNalietaneHodiny() << "min " << d->getPrepraveneZasielky() << "ks" << endl;
		}
		cout << "\nDrony typu 2\n";
		for (size_t j = 0; j < p->dronyTypu2_.size(); j++) {
			Dron* d = p->dronyTypu2_[j];
			string obsadenost = to_string(d->getObsadenostDo() / 60) + ":";
			if (d->getObsadenostDo() % 60 < 10)
				obsadenost += "0";
			obsadenost += to_string(d->getObsadenostDo() % 60);
			cout << d->getBateria() << "% " << obsadenost << " " << d->getNalietaneHodiny() << "min " << d->getPrepraveneZasielky() << "ks" << endl;
		}
		cout << "\nObjednavky\n";
		for (size_t j = 0; j < p->objednavky_.size(); j++) {
			Objednavka* o = p->objednavky_[j];
			cout << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << "km " << o->getKamECV() << " " << o->getKamVzdialenost() << "km " << o->getHmotnost() << "kg" << endl;
		}		
		cout << "\nObjednavky na dalsi den\n";
		for (size_t j = 0; j < p->objednavkyNaDalsiDen_.size(); j++) {
			Objednavka* o = p->objednavkyNaDalsiDen_[j];
			cout << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << "km " << o->getKamECV() << " " << o->getKamVzdialenost() << "km " << o->getHmotnost() << "kg" << endl;
		}
		cout << "\nZamietnute objednavky\n";
		for (size_t j = 0; j < p->zamietnuteZasielky_.size(); j++) {
			Objednavka* o = p->zamietnuteZasielky_[j];
			cout << o->getOdkialECV() << " " << o->getOdkialVzdialenost() << "km " << o->getKamECV() << " " << o->getKamVzdialenost() << "km " << o->getHmotnost() << "kg " << o->getDovodZamietnutia();
		}
		cout << endl;
	}

	cout << "\nVozidla\n";
	for (size_t i = 0; i < vozidla_.size(); i++) {
		Vozidlo* v = vozidla_[i];
		cout << v->getPozicia() << " " << v->getCelkoveNaklady() << "eur " << v->getVahaBalikov() << "kg " << v->getVahaBalikovDalsiDen() << "kg" << endl;
	}
}
