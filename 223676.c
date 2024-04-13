intrusive_ptr<ExpressionCoerceToBool> ExpressionCoerceToBool::create(
    ExpressionContext* const expCtx, intrusive_ptr<Expression> pExpression) {
    return new ExpressionCoerceToBool(expCtx, std::move(pExpression));
}