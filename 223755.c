intrusive_ptr<Expression> ExpressionTrim::parse(ExpressionContext* const expCtx,
                                                BSONElement expr,
                                                const VariablesParseState& vps) {
    const auto name = expr.fieldNameStringData();
    TrimType trimType = TrimType::kBoth;
    if (name == "$ltrim"_sd) {
        trimType = TrimType::kLeft;
    } else if (name == "$rtrim"_sd) {
        trimType = TrimType::kRight;
    } else {
        invariant(name == "$trim"_sd);
    }
    uassert(50696,
            str::stream() << name << " only supports an object as an argument, found "
                          << typeName(expr.type()),
            expr.type() == Object);

    boost::intrusive_ptr<Expression> input;
    boost::intrusive_ptr<Expression> characters;
    for (auto&& elem : expr.Obj()) {
        const auto field = elem.fieldNameStringData();
        if (field == "input"_sd) {
            input = parseOperand(expCtx, elem, vps);
        } else if (field == "chars"_sd) {
            characters = parseOperand(expCtx, elem, vps);
        } else {
            uasserted(50694,
                      str::stream() << name << " found an unknown argument: " << elem.fieldName());
        }
    }
    uassert(50695, str::stream() << name << " requires an 'input' field", input);

    return new ExpressionTrim(expCtx, trimType, name, input, characters);
}