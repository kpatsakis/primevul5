boost::intrusive_ptr<Expression> ExpressionConvert::create(ExpressionContext* const expCtx,
                                                           boost::intrusive_ptr<Expression> input,
                                                           BSONType toType) {
    return new ExpressionConvert(
        expCtx,
        std::move(input),
        ExpressionConstant::create(expCtx, Value(StringData(typeName(toType)))),
        nullptr,
        nullptr);
}