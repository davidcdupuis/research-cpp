#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "sfmt/SFMT.h"
#include "head.h"
#include "Arguments.h"

typedef double (*pf)(int, int);

class Graph{
public:
	std::string dataset;
	int n, m, k;
	std::vector<int> inDeg;
	std::vector< std::vector<int> > gT;
	std::vector< std::vector<double> > probT;
	std::string folder;
	std::string graph_file;
	std::vector<bool> hasnode;

	Graph(std::string name, std::string folder, std::string graph_file);

	enum InfluModel {IC, LT, CONT};
	InfluModel influModel;

	void setInfluModel(InfluModel p);
	void readAttributes();
	void addEdge(int a, int b, double p);
	void readGraph();
	void readGraphBin();
	void printInfo();
};

#endif
