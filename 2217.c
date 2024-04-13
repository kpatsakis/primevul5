    bool operator()(const NodeAndId& lhs, const NodeAndId& rhs) const {
      return lhs.second < rhs.second;
    }