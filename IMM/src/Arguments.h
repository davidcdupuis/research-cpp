#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#include "sfmt/SFMT.h"
#include "head.h"
#include <string>

class Arguments
{
public:
	int k;
	std::string dataset;
	double epsilon;
	std::string model;
	double T;
	bool subgraph;
	int subsize;
	std::string submodel;

	void readArguments(int argn, char ** argv);
	void printArguments();
};

#endif
