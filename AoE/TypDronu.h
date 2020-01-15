#pragma once
#include "Importy.h"

class TypDronu
{
public:
	TypDronu(unsigned int typDronu, unsigned int nosnost, unsigned int priemernaRychlost, unsigned int maxDobaLetu, unsigned int nabitieDesatinyBaterky);
	~TypDronu();
	
	unsigned int getCisloTypu() const { return typDronu_; }
	unsigned int getNosnost() const { return nosnost_; }
	unsigned int getPriemernaRychlost() const { return priemernaRychlost_; }
	unsigned int getMaxDobaLetu() const { return maxDobaLetu_; }
	unsigned int getMaxDoletenaVzdialenost() const { return priemernaRychlost_ * maxDobaLetu_ / 60; };
	unsigned int getCasNaNabitieDesatinyBaterky() const { return casNaNabitieDesatinyBaterie_; }

private:
	unsigned int typDronu_;
	unsigned int nosnost_;
	unsigned int priemernaRychlost_;
	unsigned int maxDobaLetu_;
	unsigned int casNaNabitieDesatinyBaterie_;
};

