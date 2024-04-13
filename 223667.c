boost::intrusive_ptr<Expression> ExpressionRegexFindAll::parse(ExpressionContext* const expCtx,
                                                               BSONElement expr,
                                                               const VariablesParseState& vpsIn) {
    auto opName = "$regexFindAll"_sd;
    auto [input, regex, options] = CommonRegexParse(expCtx, expr, vpsIn, opName);
    return new ExpressionRegexFindAll(
        expCtx, std::move(input), std::move(regex), std::move(options), opName);
}