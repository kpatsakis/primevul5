ExpressionMap::ExpressionMap(ExpressionContext* const expCtx,
                             const string& varName,
                             Variables::Id varId,
                             intrusive_ptr<Expression> input,
                             intrusive_ptr<Expression> each)
    : Expression(expCtx, {std::move(input), std::move(each)}),
      _varName(varName),
      _varId(varId),
      _input(_children[0]),
      _each(_children[1]) {}