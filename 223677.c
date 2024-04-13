ExpressionCoerceToBool::ExpressionCoerceToBool(ExpressionContext* const expCtx,
                                               intrusive_ptr<Expression> pExpression)
    : Expression(expCtx, {std::move(pExpression)}), pExpression(_children[0]) {}