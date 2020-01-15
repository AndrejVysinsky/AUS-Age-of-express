#pragma once
#include "Importy.h"

class Miesto
{
public:
	Miesto(string region, unsigned int vzdialenost);
	~Miesto();

	string getRegion() const { return region_; }
	unsigned int getVzdialenost() const { return vzdialenost_; }

private:
	string region_;
	unsigned int vzdialenost_;
};

