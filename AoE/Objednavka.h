#pragma once
#include "Importy.h"
#include "Miesto.h"

class Objednavka
{
public:
	Objednavka(string odkial, unsigned int vzdialenostOdkial, string kam, unsigned int vzdialenostKam, double hmotnost);
	~Objednavka();

	void toString();

	void setDovodZamietnutia(const string& dovod) { dovodZamietnutia_ = dovod; }
	string getDovodZamietnutia() const { return dovodZamietnutia_; }

	string getKamECV() const { return kam_.getRegion(); }
	string getOdkialECV() const { return odkial_.getRegion(); }
	unsigned int getKamVzdialenost() const { return kam_.getVzdialenost(); }
	unsigned int getOdkialVzdialenost() const { return odkial_.getVzdialenost(); }
	double getHmotnost() const { return hmotnost_; }
private:
	Miesto odkial_;
	Miesto kam_;
	string dovodZamietnutia_;
	double hmotnost_;	
};

