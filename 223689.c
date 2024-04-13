intrusive_ptr<Expression> Expression::parseOperand(ExpressionContext* const expCtx,
                                                   BSONElement exprElement,
                                                   const VariablesParseState& vps) {
    BSONType type = exprElement.type();

    if (type == String && exprElement.valuestr()[0] == '$') {
        /* if we got here, this is a field path expression */
        return ExpressionFieldPath::parse(expCtx, exprElement.str(), vps);
    } else if (type == Object) {
        return Expression::parseObject(expCtx, exprElement.Obj(), vps);
    } else if (type == Array) {
        return ExpressionArray::parse(expCtx, exprElement, vps);
    } else {
        return ExpressionConstant::parse(expCtx, exprElement, vps);
    }
}