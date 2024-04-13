intrusive_ptr<Expression> ExpressionPow::create(ExpressionContext* const expCtx,
                                                Value base,
                                                Value exp) {
    intrusive_ptr<ExpressionPow> expr(new ExpressionPow(expCtx));
    expr->_children.push_back(
        ExpressionConstant::create(expr->getExpressionContext(), std::move(base)));
    expr->_children.push_back(
        ExpressionConstant::create(expr->getExpressionContext(), std::move(exp)));
    return expr;
}