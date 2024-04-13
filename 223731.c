Value ExpressionRandom::serialize(const bool explain) const {
    return Value(DOC(getOpName() << Document()));
}