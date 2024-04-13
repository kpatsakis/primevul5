intrusive_ptr<Expression> ExpressionCompare::parse(ExpressionContext* const expCtx,
                                                   BSONElement bsonExpr,
                                                   const VariablesParseState& vps,
                                                   CmpOp op) {
    intrusive_ptr<ExpressionCompare> expr = new ExpressionCompare(expCtx, op);
    ExpressionVector args = parseArguments(expCtx, bsonExpr, vps);
    expr->validateArguments(args);
    expr->_children = args;
    return expr;
}