# RTIM (Real-Time Influence Maximization)

    dataset_name:
      params.txt:
        - dataset_name
        pre_process:
          - # simulations
          - max_depth
        live:
          - limit seed size
          - stream_used

      inf_scores.csv:
        store list of users and associated influence scores

      live_seed_set.csv:
        store list of users selected during live stage
