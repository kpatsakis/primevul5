intrusive_ptr<ExpressionConstant> ExpressionConstant::create(ExpressionContext* const expCtx,
                                                             const Value& value) {
    intrusive_ptr<ExpressionConstant> pEC(new ExpressionConstant(expCtx, value));
    return pEC;
}