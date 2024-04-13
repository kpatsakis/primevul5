boost::intrusive_ptr<Expression> ExpressionRegexMatch::parse(ExpressionContext* const expCtx,
                                                             BSONElement expr,
                                                             const VariablesParseState& vpsIn) {
    auto opName = "$regexMatch"_sd;
    auto [input, regex, options] = CommonRegexParse(expCtx, expr, vpsIn, opName);
    return new ExpressionRegexMatch(
        expCtx, std::move(input), std::move(regex), std::move(options), opName);
}