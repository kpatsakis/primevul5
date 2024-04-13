Value ExpressionBsonSize::evaluate(const Document& root, Variables* variables) const {
    Value arg = _children[0]->evaluate(root, variables);

    if (arg.nullish())
        return Value(BSONNULL);

    uassert(31393,
            str::stream() << "$bsonSize requires a document input, found: "
                          << typeName(arg.getType()),
            arg.getType() == BSONType::Object);

    return Value(arg.getDocument().toBson().objsize());
}