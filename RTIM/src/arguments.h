class Arguments{
public:
  int k; // size of seed set if defined
  string dataset; //name of dataset
  string model;
  string stage; // pre|live|evaluate
  int mc_depth; // max depth for monte carlo simulations
  int mc_sim; // number of simulations for monte carlo simulations

  /* Function to get arguments from command line
  */
  void getArguments(int argn, char **argv){
    for (int i = 0; i < argn; i++)
    {
        // print help
        if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1)
        {
            cout << "./rtim -dataset *** -epsilon *** -k ***  -model IC|LT|TR|CONT " << endl;
            return ;
        }

        // get arguments
        if (argv[i] == string("-dataset"))
            this.dataset = argv[i + 1];
        if (argv[i] == string("-stage"))
            this.stage = argv[i + 1];
        if (argv[i] == string("-depth"))
            this.mc_depth = argv[i + 1]
        if (argv[i] == string("-sim"))
            this.mc_sim = argv[i + 1]
        if (argv[i] == string("-epsilon"))
            this.epsilon = atof(argv[i + 1]);
        if (argv[i] == string("-T"))
            this.T = atof(argv[i + 1]);
        if (argv[i] == string("-k"))
            this.k = atoi(argv[i + 1]);
        if (argv[i] == string("-model"))
            this.model = argv[i + 1];
    }
    ASSERT(this.dataset != "");
    ASSERT(this.model == "IC" || this.model == "LT" || this.model == "TR" || this.model=="CONT");

    // string graph_file;
    // if (this.model == "IC")
    //     graph_file = this.dataset + "graph_ic.inf";
    // else if (this.model == "LT")
    //     graph_file = this.dataset + "graph_lt.inf";
    // else if (this.model == "TR")
    //     graph_file = this.dataset + "graph_tr.inf";
    // else if (this.model == "CONT")
    //     graph_file = this.dataset + "graph_cont.inf";
    // else
    //     ASSERT(false);

    //InfGraph g(this.dataset, graph_file);

    // if (this.model == "IC")
    //     g.setInfuModel(InfGraph::IC);
    // else if (this.model == "LT")
    //     g.setInfuModel(InfGraph::LT);
    // else if (this.model == "TR")
    //     g.setInfuModel(InfGraph::IC);
    // else if (this.model == "CONT")
    //     g.setInfuModel(InfGraph::CONT);
    // else
    //     ASSERT(false);

    // INFO(this.T);
  }

  /* Function to print arguments out to command line
  */
  void printArguments(){
    cout << "------------- Arguments --------------" << endl;
    cout << "Dataset: " << this.dataset << endl;
    cout << "Model: " << this.model << endl;
    cout << "Stage: " << "" << endl;
    if(this.stage == "pre"){
      cout << "Max Depth: " << this.mc_depth << endl;
      cout << "Simulations: " << this.mc_sim << endl;
    }else if (this.stage == "live"){
      cout << "Function: " << this.live_function << endl;
      cout << "Max Depth: " << this.mc_depth << endl;
    }else if(this.stage == "evaluate"){
      cout << "Simulations: 10000" << endl;
    }else{
      cout << "Error stage not defined" << endl;
      exit(1);
    }
    cout << "Size of seed set: " << this.k << endl;
  }
};
