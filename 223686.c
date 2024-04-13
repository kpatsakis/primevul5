ExpressionConvert::ExpressionConvert(ExpressionContext* const expCtx,
                                     boost::intrusive_ptr<Expression> input,
                                     boost::intrusive_ptr<Expression> to,
                                     boost::intrusive_ptr<Expression> onError,
                                     boost::intrusive_ptr<Expression> onNull)
    : Expression(expCtx, {std::move(input), std::move(to), std::move(onError), std::move(onNull)}),
      _input(_children[0]),
      _to(_children[1]),
      _onError(_children[2]),
      _onNull(_children[3]) {}