ExpressionMeta::ExpressionMeta(ExpressionContext* const expCtx, MetaType metaType)
    : Expression(expCtx), _metaType(metaType) {}