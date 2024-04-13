Value ExpressionReplaceBase::evaluate(const Document& root, Variables* variables) const {
    Value input = _input->evaluate(root, variables);
    Value find = _find->evaluate(root, variables);
    Value replacement = _replacement->evaluate(root, variables);

    // Throw an error if any arg is non-string, non-nullish.
    uassert(51746,
            str::stream() << getOpName()
                          << " requires that 'input' be a string, found: " << input.toString(),
            input.getType() == BSONType::String || input.nullish());
    uassert(51745,
            str::stream() << getOpName()
                          << " requires that 'find' be a string, found: " << find.toString(),
            find.getType() == BSONType::String || find.nullish());
    uassert(51744,
            str::stream() << getOpName() << " requires that 'replacement' be a string, found: "
                          << replacement.toString(),
            replacement.getType() == BSONType::String || replacement.nullish());

    // Return null if any arg is nullish.
    if (input.nullish())
        return Value(BSONNULL);
    if (find.nullish())
        return Value(BSONNULL);
    if (replacement.nullish())
        return Value(BSONNULL);

    return _doEval(input.getStringData(), find.getStringData(), replacement.getStringData());
}