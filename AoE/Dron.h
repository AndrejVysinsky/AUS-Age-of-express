#pragma once
#include "Importy.h"
#include "TypDronu.h"

class Dron
{
public:
	Dron(TypDronu* typ, unsigned int datum, unsigned int serioveCislo, unsigned int cas);
	Dron(TypDronu* typ, unsigned int datum, unsigned int serioveCislo, unsigned int obsadenost, unsigned int bateria, unsigned int nalietaneHodiny, unsigned int prepraveneZasielky);
	~Dron();

	void zavezObjednavku(unsigned int vzdialenost);
	
	unsigned int getCasNabitiaBaterie();
	unsigned int getCasVyzdvihnutiaObjednavky(unsigned int vzdialenost);
	unsigned int getEfektivnyDolet(unsigned int nabitie = 0) const;

	unsigned int zistiKolkoPreleti(unsigned int bateria) const;

	void posunCasu(unsigned int serverovyCas);
	void resetAtributov();

	void toString();

	unsigned int getBateria() const { return bateria_; }
	unsigned int getObsadenostDo() const { return obsadenostDo_; }
	TypDronu* getTyp() const { return typDronu_; }

	unsigned int getDatumZaradenia() const { return datumZaradeniaDoEvidencie_; }
	unsigned int getSerioveCislo() const { return serioveCislo_; }
	unsigned int getNalietaneHodiny() const { return pocetNalienatychHodin_; }
	unsigned int getPrepraveneZasielky() const { return pocetPrepravenychZasielok_; }

private:
	TypDronu* typDronu_;
	unsigned int datumZaradeniaDoEvidencie_;
	unsigned int serioveCislo_;	
	unsigned int obsadenostDo_;
	unsigned int bateria_ = 100;
	unsigned int pomocnaPreNabijanie_;

	unsigned int pocetNalienatychHodin_ = 0;
	unsigned int pocetPrepravenychZasielok_ = 0;
};

