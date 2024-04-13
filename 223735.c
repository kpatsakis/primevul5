intrusive_ptr<Expression> ExpressionConvert::parse(ExpressionContext* const expCtx,
                                                   BSONElement expr,
                                                   const VariablesParseState& vps) {
    uassert(ErrorCodes::FailedToParse,
            str::stream() << "$convert expects an object of named arguments but found: "
                          << typeName(expr.type()),
            expr.type() == BSONType::Object);

    boost::intrusive_ptr<Expression> input;
    boost::intrusive_ptr<Expression> to;
    boost::intrusive_ptr<Expression> onError;
    boost::intrusive_ptr<Expression> onNull;
    for (auto&& elem : expr.embeddedObject()) {
        const auto field = elem.fieldNameStringData();
        if (field == "input"_sd) {
            input = parseOperand(expCtx, elem, vps);
        } else if (field == "to"_sd) {
            to = parseOperand(expCtx, elem, vps);
        } else if (field == "onError"_sd) {
            onError = parseOperand(expCtx, elem, vps);
        } else if (field == "onNull"_sd) {
            onNull = parseOperand(expCtx, elem, vps);
        } else {
            uasserted(ErrorCodes::FailedToParse,
                      str::stream()
                          << "$convert found an unknown argument: " << elem.fieldNameStringData());
        }
    }

    uassert(ErrorCodes::FailedToParse, "Missing 'input' parameter to $convert", input);
    uassert(ErrorCodes::FailedToParse, "Missing 'to' parameter to $convert", to);

    return new ExpressionConvert(
        expCtx, std::move(input), std::move(to), std::move(onError), std::move(onNull));
}