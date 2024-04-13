intrusive_ptr<Expression> ExpressionCond::parse(ExpressionContext* const expCtx,
                                                BSONElement expr,
                                                const VariablesParseState& vps) {
    if (expr.type() != Object) {
        return Base::parse(expCtx, expr, vps);
    }
    verify(expr.fieldNameStringData() == "$cond");

    intrusive_ptr<ExpressionCond> ret = new ExpressionCond(expCtx);
    ret->_children.resize(3);

    const BSONObj args = expr.embeddedObject();
    BSONForEach(arg, args) {
        if (arg.fieldNameStringData() == "if") {
            ret->_children[0] = parseOperand(expCtx, arg, vps);
        } else if (arg.fieldNameStringData() == "then") {
            ret->_children[1] = parseOperand(expCtx, arg, vps);
        } else if (arg.fieldNameStringData() == "else") {
            ret->_children[2] = parseOperand(expCtx, arg, vps);
        } else {
            uasserted(17083,
                      str::stream() << "Unrecognized parameter to $cond: " << arg.fieldName());
        }
    }

    uassert(17080, "Missing 'if' parameter to $cond", ret->_children[0]);
    uassert(17081, "Missing 'then' parameter to $cond", ret->_children[1]);
    uassert(17082, "Missing 'else' parameter to $cond", ret->_children[2]);

    return ret;
}