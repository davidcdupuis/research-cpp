#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#include "sfmt/SFMT.h"
#include "head.h"

class Argument{
public:
	int k;
	string dataset;
	double epsilon;
	string model;
	double T;
	bool subgraph;
	int subsize;
	string submodel;
}

#endif

