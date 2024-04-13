ExpressionDateToParts::ExpressionDateToParts(ExpressionContext* const expCtx,
                                             intrusive_ptr<Expression> date,
                                             intrusive_ptr<Expression> timeZone,
                                             intrusive_ptr<Expression> iso8601)
    : Expression(expCtx, {std::move(date), std::move(timeZone), std::move(iso8601)}),
      _date(_children[0]),
      _timeZone(_children[1]),
      _iso8601(_children[2]) {}