#ifndef __IMM_H__
#define __IMM_H__

#include "head.h"
#include "InfGraph.h"
#include "Arguments.h"
#include "Timer.h"
#include "Math.h"

class IMM
{
public:
	static void InfluenceMaximize(InfGraph &g, const Arguments &arg);

private:
	static double sampling(InfGraph &g, const Arguments & arg);
	static double nodeSelection(InfGraph &g, const Arguments & arg, double OPT_prime);

};

#endif
