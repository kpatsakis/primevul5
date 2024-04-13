InMatchExpression::InMatchExpression(StringData path)
    : LeafMatchExpression(MATCH_IN, path),
      _eltCmp(BSONElementComparator::FieldNamesMode::kIgnore, _collator),
      _equalitySet(_eltCmp.makeBSONEltFlatSet(_originalEqualityVector)) {}