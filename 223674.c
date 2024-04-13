intrusive_ptr<Expression> ExpressionMeta::parse(ExpressionContext* const expCtx,
                                                BSONElement expr,
                                                const VariablesParseState& vpsIn) {
    uassert(17307, "$meta only supports string arguments", expr.type() == String);

    const auto iter = kMetaNameToMetaType.find(expr.valueStringData());
    if (iter != kMetaNameToMetaType.end()) {
        return new ExpressionMeta(expCtx, iter->second);
    } else {
        uasserted(17308, "Unsupported argument to $meta: " + expr.String());
    }
}