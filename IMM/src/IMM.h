#ifndef __IMM_H__
#define __IMM_H__

#include "head.h"
#include "InfGraph.h"
#include "Arguments.h"
#include "Timer.h"
// #include "Math.h"
#include <math.h>

class IMM
{
public:
	static void InfluenceMaximize(InfGraph &g, const Arguments &arg);
	void saveLog(const Arguments &arg, std::string fileName, std::string startDatetime, std::string endDatetime, double duration, int seedSize, double estScore);
	void saveCSV(const Arguments &arg, std::string fileName, std::string startDatetime, std::string endDatetime, double duration, int seedSize, double estScore);

private:
	static double sampling(InfGraph &g, const Arguments & arg);
	static double nodeSelection(InfGraph &g, const Arguments & arg, double OPT_prime);

};

#endif
