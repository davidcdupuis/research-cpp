#ifndef __IMM_H__
#define __IMM_H__

class IMM{
public:
	static void InfluenceMaximize(InfGraph &g, const Argument &arg);

private:
	static double step1(InfGraph &g, const Argument & arg);
	static double step2(InfGraph &g, const Argument & arg, double OPT_prime);

};

#endif

