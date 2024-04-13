ExpressionFieldPath::ExpressionFieldPath(ExpressionContext* const expCtx,
                                         const string& theFieldPath,
                                         Variables::Id variable)
    : Expression(expCtx), _fieldPath(theFieldPath), _variable(variable) {}