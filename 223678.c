intrusive_ptr<Expression> ExpressionRandom::parse(ExpressionContext* const expCtx,
                                                  BSONElement exprElement,
                                                  const VariablesParseState& vps) {
    uassert(3040500,
            "$rand not allowed inside collection validators",
            !expCtx->isParsingCollectionValidator);

    uassert(3040501, "$rand does not currently accept arguments", exprElement.Obj().isEmpty());

    return new ExpressionRandom(expCtx);
}