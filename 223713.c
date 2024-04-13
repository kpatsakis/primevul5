intrusive_ptr<Expression> ExpressionReduce::parse(ExpressionContext* const expCtx,
                                                  BSONElement expr,
                                                  const VariablesParseState& vps) {
    uassert(40075,
            str::stream() << "$reduce requires an object as an argument, found: "
                          << typeName(expr.type()),
            expr.type() == Object);


    // vpsSub is used only to parse 'in', which must have access to $$this and $$value.
    VariablesParseState vpsSub(vps);
    auto thisVar = vpsSub.defineVariable("this");
    auto valueVar = vpsSub.defineVariable("value");

    boost::intrusive_ptr<Expression> input;
    boost::intrusive_ptr<Expression> initial;
    boost::intrusive_ptr<Expression> in;
    for (auto&& elem : expr.Obj()) {
        auto field = elem.fieldNameStringData();

        if (field == "input") {
            input = parseOperand(expCtx, elem, vps);
        } else if (field == "initialValue") {
            initial = parseOperand(expCtx, elem, vps);
        } else if (field == "in") {
            in = parseOperand(expCtx, elem, vpsSub);
        } else {
            uasserted(40076, str::stream() << "$reduce found an unknown argument: " << field);
        }
    }

    uassert(40077, "$reduce requires 'input' to be specified", input);
    uassert(40078, "$reduce requires 'initialValue' to be specified", initial);
    uassert(40079, "$reduce requires 'in' to be specified", in);

    return new ExpressionReduce(
        expCtx, std::move(input), std::move(initial), std::move(in), thisVar, valueVar);
}