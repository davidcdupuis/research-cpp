#define HEAD_INFO

#include "sfmt/SFMT.h"
#include "head.h"

class Argument{
public:
    int k;
    string dataset;
    double epsilon;
    string model;
    double T;
};

#include "graph.h"
#include "infgraph.h"
#include "imm.h"



void run_with_parameter(InfGraph &g, const Argument & arg)
{
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << arg.dataset << " k=" << arg.k << " epsilon=" << arg.epsilon <<   " " << arg.model << endl;

        Imm::InfluenceMaximize(g, arg);

        // save g.seedSet here => nethept_k100e0.01.txt
        string file = "../data/" + arg.dataset + "/imm/" + arg.dataset + "_k" + to_string(arg.k) + "e" + to_string(arg.epsilon) + ".txt";
        cout << "Saving influence scores to " << file << endl;
        ofstream seedSetFile;
        seedSetFile.open(file);
        for(int node: g.seedSet){
          seedSetFile << node << endl;
        }
        INFO(g.seedSet);
        // use saved seed set to compute influence score using MC simulations
        INFO(g.InfluenceHyperGraph());
    Timer::show();
}

void Run(int argn, char **argv)
{
    Argument arg;


    for (int i = 0; i < argn; i++)
    {
        if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1)
        {
            cout << "./tim -dataset *** -epsilon *** -k ***  -model IC|LT|TR|CONT " << endl;
            return ;
        }
        if (argv[i] == string("-dataset"))
            arg.dataset = argv[i + 1];
        if (argv[i] == string("-epsilon"))
            arg.epsilon = atof(argv[i + 1]);
        if (argv[i] == string("-T"))
            arg.T = atof(argv[i + 1]);
        if (argv[i] == string("-k"))
            arg.k = atoi(argv[i + 1]);
        if (argv[i] == string("-model"))
            arg.model = argv[i + 1];
    }
    ASSERT(arg.dataset != "");
    ASSERT(arg.model == "IC" || arg.model == "LT" || arg.model == "TR" || arg.model=="CONT");

    string graph_file;
    if (arg.model == "IC"){
        //graph_file = arg.dataset + "graph_wc.inf";
	graph_file = "../data/" + arg.dataset + "/" + arg.dataset +"_wc.inf";
    }else if (arg.model == "LT")
        graph_file = arg.dataset + "graph_lt.inf";
    else if (arg.model == "TR")
        graph_file = arg.dataset + "graph_tr.inf";
    else if (arg.model == "CONT")
        graph_file = arg.dataset + "graph_cont.inf";
    else
        ASSERT(false);

    InfGraph g(arg.dataset, graph_file);


    if (arg.model == "IC")
        g.setInfuModel(InfGraph::IC);
    else if (arg.model == "LT")
        g.setInfuModel(InfGraph::LT);
    else if (arg.model == "TR")
        g.setInfuModel(InfGraph::IC);
    else if (arg.model == "CONT")
        g.setInfuModel(InfGraph::CONT);
    else
        ASSERT(false);

    INFO(arg.T);

    run_with_parameter(g, arg);
}


int main(int argn, char **argv)
{
    __head_version = "v1";
    OutputInfo info(argn, argv);


    Run( argn, argv );
}
