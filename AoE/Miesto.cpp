#include "Miesto.h"
#include <utility>


Miesto::Miesto(string region, unsigned int vzdialenost) : region_(std::move(region)), vzdialenost_(vzdialenost)
{
}


Miesto::~Miesto()
{
}
