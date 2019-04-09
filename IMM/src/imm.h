#ifndef __IMM_H__
#define __IMM_H__

#include "head.h"
#include "InfGraph.h"
#include "Arguments.h"
#include "Timer.h"
#include "Math.h"
#include "OutputInfo.h"

class IMM{
public:
	static void InfluenceMaximize(InfGraph &g, const Argument &arg);

private:
	static double sampling(InfGraph &g, const Argument & arg);
	static double nodeSelection(InfGraph &g, const Argument & arg, double OPT_prime);

};

#endif
