std::unique_ptr<MatchExpression> InMatchExpression::shallowClone() const {
    auto next = stdx::make_unique<InMatchExpression>(path());
    next->setCollator(_collator);
    if (getTag()) {
        next->setTag(getTag()->clone());
    }
    next->_hasNull = _hasNull;
    next->_hasEmptyArray = _hasEmptyArray;
    next->_equalitySet = _equalitySet;
    next->_originalEqualityVector = _originalEqualityVector;
    for (auto&& regex : _regexes) {
        std::unique_ptr<RegexMatchExpression> clonedRegex(
            static_cast<RegexMatchExpression*>(regex->shallowClone().release()));
        next->_regexes.push_back(std::move(clonedRegex));
    }
    return std::move(next);
}