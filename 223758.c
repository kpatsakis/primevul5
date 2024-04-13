Value ExpressionRandom::evaluate(const Document& root, Variables* variables) const {
    return Value(getRandomValue());
}