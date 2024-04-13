  bool EquivalentShapesAndTypes(const std::vector<ShapeAndType>& st1,
                                const std::vector<ShapeAndType>& st2) const {
    if (st1.size() != st2.size()) {
      return false;
    }
    for (int i = 0, st1_size = st1.size(); i < st1_size; ++i) {
      const ShapeAndType& s1 = st1[i];
      const ShapeAndType& s2 = st2[i];
      if (s1.dtype != s2.dtype) {
        return false;
      }
      if (!EquivalentShapes(s1.shape, s2.shape)) {
        return false;
      }
    }
    return true;
  }