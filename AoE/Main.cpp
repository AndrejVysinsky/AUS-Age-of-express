#include "CentralnySklad.h"
#include "heap_monitor.h"

CentralnySklad cSklad = CentralnySklad();

void menu();
void vytvorObjednavku();
void objednavkyZoSuboru();
void statistika();

int main(int argc, char** argv)
{
	initHeapMonitor();

	cSklad.nacitanie(argv[1]);

	menu();
		
	return 0;
}

void menu()
{
	string retazec;
	int cislo;

	while (true) {
		system("cls");

		cout << "\nAktualny cas: " << cSklad.getDen() << ". den " << cSklad.getCas() / 60 << ":00" << endl << endl;
		cout << "1. Pridanie vozidla do vozoveho parku\n";
		cout << "2. Pridanie noveho dronu do prekladiska\n";
		cout << "3. Vytvorenie novej objednavky\n";
		cout << "4. Posunutie casu o hodinu\n";
		cout << "5. Vypisanie evidencie vozidiel\n";
		cout << "6. Vypisanie zoznamu dronov prekladiska\n";
		cout << "7. Nacitanie evidencie zo suboru\n";
		cout << "8. Uloz evidenciu do suboru\n";
		cout << "9. Vypisanie sumarnych statistik\n";
		cout << "10. Debug\n";
		cout << "0. Ukoncenie programu\n";
		cout << "\n";

		cin >> cislo;

		system("cls");
		switch (cislo) {
		case 1:
			cSklad.pridajVozidlo();
			cout << "\nVozidlo bolo uspesne pridane.\n";
			break;
		case 2:
			cSklad.pridajDrona();
			cout << "\nDron bol uspesne pridany.\n";
			break;
		case 3:
			vytvorObjednavku();
			break;
		case 4:
			cSklad.posunCasuOHodinu();
			break;
		case 5:
			cSklad.vypisEvidenciuVozidiel();
			break;
		case 6:
			cout << "Zadajte nazov prekladiska\n";
			cin >> retazec;
			cSklad.vypisEvidenciuDronov(retazec);
			break;
		case 7:
			cSklad.nacitanie();
			cout << "\nEvidencia bola uspesne nacitana.\n";
			break;
		case 8:
			cSklad.ulozenie();
			cout << "\nEvidencia bola uspesne ulozena.\n";
			break;
		case 9:
			statistika();
			break;
		case 10:
			cSklad.debug();
			break;
		default:;
		}

		if (cislo == 0)
			break;

		cout << "Pre navrat do menu stlacte enter\n";
		cin.ignore(2);

	}
}

void vytvorObjednavku()
{
	cout << "Prajete si nacitat objednavky zo suboru ?\n";
	cout << "ano/nie\n";
	string odpoved;
	cin >> odpoved;
	if (odpoved == "ano") {
		objednavkyZoSuboru();
		return;
	}
	system("cls");

	string odkial, kam;
	unsigned int vzdOdkial, vzdKam;
	double vaha;

	cout << "Vlastnosti dronov:\n";
	cout << "Typ1: dolet - 53 km, nosnost - 2kg\n";
	cout << "Typ2: dolet - 40 km, nosnost - 5kg\n\n";

	cout << "Kde sa ma vyzdvihnut balik:\n";
	cin >> odkial;
	cout << "Vzdialenost od vyzdvihnutia:\n";
	cin >> vzdOdkial;
	cout << "Kde sa ma poslat balik:\n";
	cin >> kam;
	cout << "Vzdialenost od destinacie:\n";
	cin >> vzdKam;
	cout << "Vaha balika:\n";
	cin >> vaha;

	Objednavka* nova = cSklad.vytvorObjednavku(odkial, vzdOdkial, kam, vzdKam, vaha);
	if (nova != nullptr) {
		if (!nova->getDovodZamietnutia().empty())
			cout << nova->getDovodZamietnutia();
	}
}

void objednavkyZoSuboru()
{
	string odkial, kam;
	unsigned int vzdOdkial, vzdKam;
	double vaha;
	string subor;

	cout << "Zadajte nazov suboru:\n";
	cin >> subor;
	ifstream vstup("resources/" + subor + ".txt");
	if (!vstup.is_open()) {
		cout << "Nepodarilo sa nacitat subor s objednavkami.\n";
		return;
	}
	
	unsigned int pocet;
	vstup >> pocet;
	for (unsigned int i = 0; i < pocet; i++) {
		vstup >> odkial >> vzdOdkial >> kam >> vzdKam >> vaha;
		Objednavka* nova = cSklad.vytvorObjednavku(odkial, vzdOdkial, kam, vzdKam, vaha);
		if (nova != nullptr) {
			if (!nova->getDovodZamietnutia().empty())
				cout << nova->getDovodZamietnutia();
		}
	}
	vstup.close();
}

void statistika()
{
	unsigned int odKedy, doKedy;

	cout << "\nAktualny cas: " << cSklad.getDen() << ". den " << cSklad.getCas() / 60 << ":00" << endl << endl;
	cout << "1. Vypisanie regionu v ktorom bolo dorucenych najviac zasielok\n"; //dane casove obdobie
	cout << "2. Vypisanie regionu v ktorom bolo odoslanych najviac zasielok\n"; //dane casove obdobie
	cout << "3. Vypisanie zamietnutych zasielok v danom regione\n"; //dane casove obdobie
	cout << "4. Vypisanie poctu zamietnutych zasielok v jednotlivych regionoch\n"; //dane casove obdobie
	cout << "5. Vypisanie celkoveho poctu dorucenych zasielok\n";
	cout << "6. Vypisanie celkoveho poctu najazdenych kilometrov vsetkych vozidiel\n";
	cout << "7. Vypisanie celkoveho poctu nalietanych hodin dronov v regionoch\n\n";

	unsigned int cislo;
	cin >> cislo;

	if (cislo < 1 || cislo > 7)
		return;

	system("cls");
	if (cislo <= 4) {
		if (cSklad.getDen() == 1) {
			//musi uplynut aspon jeden den
			cout << "Statistika este nebola vytvorena!\n";
			return;
		}
		cout << "Zadajte casove obdobie v rozmezdi od 1 do " << cSklad.getDen() << endl;
		cout << "Od:\n";
		cin >> odKedy;
		cout << "Do:\n";
		cin >> doKedy;
		doKedy--;

		if (odKedy > doKedy) {
			cout << "Prve cislo musi byt mensie ako druhe\n";
			return;
		}

		system("cls");
		switch (cislo) {
			case 1:
				cSklad.statNajviacZasielok(odKedy, doKedy, true);
				break;
			case 2:
				cSklad.statNajviacZasielok(odKedy, doKedy, false);
				break;
			case 3:
				cSklad.statZamietnute(odKedy, doKedy);
				break;
			case 4:
				cSklad.statPocetZrusenych(odKedy, doKedy);
				break;
			default:;
		}		
	}
	else {
		system("cls");
		switch (cislo) {
		case 5:
			cout << "Pocet dorucenych zasielok: " << cSklad.getPocetDorucenychZasielok() << endl;
			break;
		case 6:
			cout << "Prejdena vzdialenost vsetkych vozidiel: " << cSklad.getVzdialenostVozidiel() << endl;
			break;
		case 7:
			cSklad.statHodinyDronov();
			break;
		default:;
		}
	}
}

