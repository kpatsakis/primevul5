Value arrayElemAt(const ExpressionNary* self, Value array, Value indexArg) {
    if (array.nullish() || indexArg.nullish()) {
        return Value(BSONNULL);
    }

    size_t arity = self->getOperandList().size();
    uassert(28689,
            str::stream() << self->getOpName() << "'s "
                          << (arity == 1 ? "argument" : "first argument")
                          << " must be an array, but is " << typeName(array.getType()),
            array.isArray());
    uassert(28690,
            str::stream() << self->getOpName() << "'s second argument must be a numeric value,"
                          << " but is " << typeName(indexArg.getType()),
            indexArg.numeric());
    uassert(28691,
            str::stream() << self->getOpName() << "'s second argument must be representable as"
                          << " a 32-bit integer: " << indexArg.coerceToDouble(),
            indexArg.integral());

    long long i = indexArg.coerceToLong();
    if (i < 0 && static_cast<size_t>(std::abs(i)) > array.getArrayLength()) {
        // Positive indices that are too large are handled automatically by Value.
        return Value();
    } else if (i < 0) {
        // Index from the back of the array.
        i = array.getArrayLength() + i;
    }
    const size_t index = static_cast<size_t>(i);
    return array[index];
}