  ~DisjointSet() {
    for (auto rep : nodes_) {
      delete rep.second;
    }
  }