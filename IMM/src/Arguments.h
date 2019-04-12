#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#include "sfmt/SFMT.h"
#include "head.h"
#include <string>

class Arguments
{
public:
	int k = 50;
	int subsize;
	double epsilon = 0.1;
	double T;
	std::string dataset;
	std::string model = "IC";
	std::string submodel;
	bool subgraph = false;

	void readArguments(int argn, char ** argv);
	void printArguments();
};

#endif
