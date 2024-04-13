intrusive_ptr<Expression> ExpressionTrunc::parse(ExpressionContext* const expCtx,
                                                 BSONElement elem,
                                                 const VariablesParseState& vps) {
    return ExpressionRangedArity<ExpressionTrunc, 1, 2>::parse(expCtx, elem, vps);
}