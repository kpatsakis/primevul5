ExpressionFilter::ExpressionFilter(ExpressionContext* const expCtx,
                                   string varName,
                                   Variables::Id varId,
                                   intrusive_ptr<Expression> input,
                                   intrusive_ptr<Expression> filter)
    : Expression(expCtx, {std::move(input), std::move(filter)}),
      _varName(std::move(varName)),
      _varId(varId),
      _input(_children[0]),
      _filter(_children[1]) {}