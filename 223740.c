intrusive_ptr<Expression> ExpressionZip::parse(ExpressionContext* const expCtx,
                                               BSONElement expr,
                                               const VariablesParseState& vps) {
    uassert(34460,
            str::stream() << "$zip only supports an object as an argument, found "
                          << typeName(expr.type()),
            expr.type() == Object);

    auto useLongestLength = false;
    std::vector<boost::intrusive_ptr<Expression>> children;
    // We need to ensure defaults appear after inputs so we build them seperately and then
    // concatenate them.
    std::vector<boost::intrusive_ptr<Expression>> tempDefaultChildren;

    for (auto&& elem : expr.Obj()) {
        const auto field = elem.fieldNameStringData();
        if (field == "inputs") {
            uassert(34461,
                    str::stream() << "inputs must be an array of expressions, found "
                                  << typeName(elem.type()),
                    elem.type() == Array);
            for (auto&& subExpr : elem.Array()) {
                children.push_back(parseOperand(expCtx, subExpr, vps));
            }
        } else if (field == "defaults") {
            uassert(34462,
                    str::stream() << "defaults must be an array of expressions, found "
                                  << typeName(elem.type()),
                    elem.type() == Array);
            for (auto&& subExpr : elem.Array()) {
                tempDefaultChildren.push_back(parseOperand(expCtx, subExpr, vps));
            }
        } else if (field == "useLongestLength") {
            uassert(34463,
                    str::stream() << "useLongestLength must be a bool, found "
                                  << typeName(expr.type()),
                    elem.type() == Bool);
            useLongestLength = elem.Bool();
        } else {
            uasserted(34464,
                      str::stream() << "$zip found an unknown argument: " << elem.fieldName());
        }
    }

    auto numInputs = children.size();
    std::move(tempDefaultChildren.begin(), tempDefaultChildren.end(), std::back_inserter(children));

    std::vector<std::reference_wrapper<boost::intrusive_ptr<Expression>>> inputs;
    std::vector<std::reference_wrapper<boost::intrusive_ptr<Expression>>> defaults;
    for (auto&& child : children) {
        if (numInputs == 0) {
            defaults.push_back(child);
        } else {
            inputs.push_back(child);
            numInputs--;
        }
    }

    uassert(34465, "$zip requires at least one input array", !inputs.empty());
    uassert(34466,
            "cannot specify defaults unless useLongestLength is true",
            (useLongestLength || defaults.empty()));
    uassert(34467,
            "defaults and inputs must have the same length",
            (defaults.empty() || defaults.size() == inputs.size()));

    return new ExpressionZip(
        expCtx, useLongestLength, std::move(children), std::move(inputs), std::move(defaults));
}