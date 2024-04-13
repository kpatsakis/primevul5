parseExpressionReplaceBase(const char* opName,
                           ExpressionContext* const expCtx,
                           BSONElement expr,
                           const VariablesParseState& vps) {

    uassert(51751,
            str::stream() << opName
                          << " requires an object as an argument, found: " << typeName(expr.type()),
            expr.type() == Object);

    intrusive_ptr<Expression> input;
    intrusive_ptr<Expression> find;
    intrusive_ptr<Expression> replacement;
    for (auto&& elem : expr.Obj()) {
        auto field = elem.fieldNameStringData();

        if (field == "input"_sd) {
            input = Expression::parseOperand(expCtx, elem, vps);
        } else if (field == "find"_sd) {
            find = Expression::parseOperand(expCtx, elem, vps);
        } else if (field == "replacement"_sd) {
            replacement = Expression::parseOperand(expCtx, elem, vps);
        } else {
            uasserted(51750, str::stream() << opName << " found an unknown argument: " << field);
        }
    }

    uassert(51749, str::stream() << opName << " requires 'input' to be specified", input);
    uassert(51748, str::stream() << opName << " requires 'find' to be specified", find);
    uassert(
        51747, str::stream() << opName << " requires 'replacement' to be specified", replacement);

    return {input, find, replacement};
}