Status InMatchExpression::setEqualities(std::vector<BSONElement> equalities) {
    for (auto&& equality : equalities) {
        if (equality.type() == BSONType::RegEx) {
            return Status(ErrorCodes::BadValue, "InMatchExpression equality cannot be a regex");
        }
        if (equality.type() == BSONType::Undefined) {
            return Status(ErrorCodes::BadValue, "InMatchExpression equality cannot be undefined");
        }

        if (equality.type() == BSONType::jstNULL) {
            _hasNull = true;
        } else if (equality.type() == BSONType::Array && equality.Obj().isEmpty()) {
            _hasEmptyArray = true;
        }
    }

    _originalEqualityVector = std::move(equalities);

    if (!std::is_sorted(_originalEqualityVector.begin(),
                        _originalEqualityVector.end(),
                        _eltCmp.makeLessThan())) {
        // Sort the list of equalities to work around https://svn.boost.org/trac10/ticket/13140.
        std::sort(
            _originalEqualityVector.begin(), _originalEqualityVector.end(), _eltCmp.makeLessThan());
    }

    _equalitySet = _eltCmp.makeBSONEltFlatSet(_originalEqualityVector);

    return Status::OK();
}