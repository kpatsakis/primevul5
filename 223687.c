intrusive_ptr<ExpressionFieldPath> ExpressionFieldPath::create(ExpressionContext* const expCtx,
                                                               const string& fieldPath) {
    return new ExpressionFieldPath(expCtx, "CURRENT." + fieldPath, Variables::kRootId);
}