boost::intrusive_ptr<Expression> ExpressionRegexFind::parse(ExpressionContext* const expCtx,
                                                            BSONElement expr,
                                                            const VariablesParseState& vpsIn) {
    auto opName = "$regexFind"_sd;
    auto [input, regex, options] = CommonRegexParse(expCtx, expr, vpsIn, opName);
    return new ExpressionRegexFind(
        expCtx, std::move(input), std::move(regex), std::move(options), opName);
}