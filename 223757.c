Value ExpressionStrLenBytes::evaluate(const Document& root, Variables* variables) const {
    Value str(_children[0]->evaluate(root, variables));

    uassert(34473,
            str::stream() << "$strLenBytes requires a string argument, found: "
                          << typeName(str.getType()),
            str.getType() == BSONType::String);

    return strLenBytes(str.getStringData());
}