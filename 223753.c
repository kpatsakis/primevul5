intrusive_ptr<Expression> ExpressionReplaceOne::parse(ExpressionContext* const expCtx,
                                                      BSONElement expr,
                                                      const VariablesParseState& vps) {
    auto [input, find, replacement] = parseExpressionReplaceBase(opName, expCtx, expr, vps);
    return make_intrusive<ExpressionReplaceOne>(
        expCtx, std::move(input), std::move(find), std::move(replacement));
}