Value ExpressionBinarySize::evaluate(const Document& root, Variables* variables) const {
    Value arg = _children[0]->evaluate(root, variables);
    if (arg.nullish()) {
        return Value(BSONNULL);
    }

    uassert(51276,
            str::stream() << "$binarySize requires a string or BinData argument, found: "
                          << typeName(arg.getType()),
            arg.getType() == BSONType::BinData || arg.getType() == BSONType::String);

    if (arg.getType() == BSONType::String) {
        return strLenBytes(arg.getStringData());
    }

    BSONBinData binData = arg.getBinData();
    return Value(binData.length);
}