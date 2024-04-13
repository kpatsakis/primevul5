auto CommonRegexParse(ExpressionContext* const expCtx,
                      BSONElement expr,
                      const VariablesParseState& vpsIn,
                      StringData opName) {
    uassert(51103,
            str::stream() << opName
                          << " expects an object of named arguments but found: " << expr.type(),
            expr.type() == BSONType::Object);

    struct {
        boost::intrusive_ptr<Expression> input;
        boost::intrusive_ptr<Expression> regex;
        boost::intrusive_ptr<Expression> options;
    } parsed;
    for (auto&& elem : expr.embeddedObject()) {
        const auto field = elem.fieldNameStringData();
        if (field == "input"_sd) {
            parsed.input = Expression::parseOperand(expCtx, elem, vpsIn);
        } else if (field == "regex"_sd) {
            parsed.regex = Expression::parseOperand(expCtx, elem, vpsIn);
        } else if (field == "options"_sd) {
            parsed.options = Expression::parseOperand(expCtx, elem, vpsIn);
        } else {
            uasserted(31024,
                      str::stream() << opName << " found an unknown argument: "
                                    << elem.fieldNameStringData());
        }
    }
    uassert(31022, str::stream() << opName << " requires 'input' parameter", parsed.input);
    uassert(31023, str::stream() << opName << " requires 'regex' parameter", parsed.regex);

    return parsed;
}