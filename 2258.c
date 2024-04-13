  bool operator()(const Handle& h1, const Handle& h2) const {
    return h1.SameHandle(h2);
  }