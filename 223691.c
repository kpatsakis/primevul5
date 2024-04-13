Value ExpressionSplit::evaluate(const Document& root, Variables* variables) const {
    Value inputArg = _children[0]->evaluate(root, variables);
    Value separatorArg = _children[1]->evaluate(root, variables);

    if (inputArg.nullish() || separatorArg.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40085,
            str::stream() << "$split requires an expression that evaluates to a string as a first "
                             "argument, found: "
                          << typeName(inputArg.getType()),
            inputArg.getType() == BSONType::String);
    uassert(40086,
            str::stream() << "$split requires an expression that evaluates to a string as a second "
                             "argument, found: "
                          << typeName(separatorArg.getType()),
            separatorArg.getType() == BSONType::String);

    StringData input = inputArg.getStringData();
    StringData separator = separatorArg.getStringData();

    uassert(40087, "$split requires a non-empty separator", !separator.empty());

    std::vector<Value> output;

    const char* needle = separator.rawData();
    const char* const needleEnd = needle + separator.size();
    const char* remainingHaystack = input.rawData();
    const char* const haystackEnd = remainingHaystack + input.size();

    const char* it = remainingHaystack;
    while ((it = std::search(remainingHaystack, haystackEnd, needle, needleEnd)) != haystackEnd) {
        StringData sd(remainingHaystack, it - remainingHaystack);
        output.push_back(Value(sd));
        remainingHaystack = it + separator.size();
    }

    StringData splitString(remainingHaystack, input.size() - (remainingHaystack - input.rawData()));
    output.push_back(Value(splitString));
    return Value(std::move(output));
}