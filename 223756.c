boost::intrusive_ptr<ExpressionCompare> ExpressionCompare::create(
    ExpressionContext* const expCtx,
    CmpOp cmpOp,
    const boost::intrusive_ptr<Expression>& exprLeft,
    const boost::intrusive_ptr<Expression>& exprRight) {
    boost::intrusive_ptr<ExpressionCompare> expr = new ExpressionCompare(expCtx, cmpOp);
    expr->_children = {exprLeft, exprRight};
    return expr;
}