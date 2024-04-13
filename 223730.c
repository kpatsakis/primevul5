ExpressionConstant::ExpressionConstant(ExpressionContext* const expCtx, const Value& value)
    : Expression(expCtx), _value(value) {}