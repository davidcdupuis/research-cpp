class Graph{

  /* Function to read attributes.txt
  */
  void readAttributes(){
    ifstream cin((folder + "attribute.txt").c_str());
    ASSERT(!cin == false);
    string s;
    while (cin >> s)
    {
        if (s.substr(0, 2) == "n=")
        {
            n = atoi(s.substr(2).c_str());
            continue;
        }
        if (s.substr(0, 2) == "m=")
        {
            m = atoi(s.substr(2).c_str());
            continue;
        }
        ASSERT(false);
    }
    TRACE(n, m);
    cin.close();
  }

  /* Function to import graph from file
  */
  void loadGraph(){
    FILE *fin = fopen((graph_file).c_str(), "r");
    ASSERT(fin != false);
    int readCnt = 0;
    for (int i = 0; i < m; i++)
    {
        readCnt ++;
        int a, b;
        double p;
        int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
        ASSERTT(c == 3, a, b, p, c);

        //TRACE_LINE(a, b);
        // check if node ids within graph array range
        ASSERT( a < n );
        ASSERT( b < n );
        hasnode[a] = true;
        hasnode[b] = true;
        add_edge(a, b, p);
    }
    TRACE_LINE_END();
    int s = 0;
    for (int i = 0; i < n; i++)
        if (hasnode[i])
            s++;
    INFO(s);
    ASSERT(readCnt == m);
    fclose(fin);
  }

  /* Function to calculate influence score of seed set
  */
  void influenceScore(){

  }

};
