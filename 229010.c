void InMatchExpression::_doSetCollator(const CollatorInterface* collator) {
    _collator = collator;
    _eltCmp = BSONElementComparator(BSONElementComparator::FieldNamesMode::kIgnore, _collator);

    if (!std::is_sorted(_originalEqualityVector.begin(),
                        _originalEqualityVector.end(),
                        _eltCmp.makeLessThan())) {
        // Re-sort the list of equalities according to our current comparator. This is necessary to
        // work around https://svn.boost.org/trac10/ticket/13140.
        std::sort(
            _originalEqualityVector.begin(), _originalEqualityVector.end(), _eltCmp.makeLessThan());
    }

    // We need to re-compute '_equalitySet', since our set comparator has changed.
    _equalitySet = _eltCmp.makeBSONEltFlatSet(_originalEqualityVector);
}