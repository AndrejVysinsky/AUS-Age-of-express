#include "TypDronu.h"

TypDronu::TypDronu(unsigned int typDronu, unsigned int nosnost, unsigned int priemernaRychlost, unsigned int maxDobaLetu, unsigned int nabitieDesatinyBaterky) :
	typDronu_(typDronu), nosnost_(nosnost), priemernaRychlost_(priemernaRychlost), maxDobaLetu_(maxDobaLetu), casNaNabitieDesatinyBaterie_(nabitieDesatinyBaterky)
{
}

TypDronu::~TypDronu()
{
}
