#include "Objednavka.h"
#include <utility>


Objednavka::Objednavka(string odkial, const unsigned int vzdialenostOdkial, string kam, const unsigned int vzdialenostKam, const double hmotnost)
	:odkial_(odkial, vzdialenostOdkial), kam_(kam, vzdialenostKam), hmotnost_(hmotnost)
{
}

Objednavka::~Objednavka()
{
}

void Objednavka::toString()
{
	cout << left << setw(5) << setfill(' ') << odkial_.getRegion();
	cout << left << setw(5) << setfill(' ') << odkial_.getVzdialenost();
	cout << left << setw(5) << setfill(' ') << kam_.getRegion();
	cout << left << setw(5) << setfill(' ') << kam_.getVzdialenost();
	cout << left << setw(5) << setfill(' ') << hmotnost_ << endl;
	cout << dovodZamietnutia_;
}
