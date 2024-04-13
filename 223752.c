ExpressionDateToString::ExpressionDateToString(ExpressionContext* const expCtx,
                                               intrusive_ptr<Expression> date,
                                               intrusive_ptr<Expression> format,
                                               intrusive_ptr<Expression> timeZone,
                                               intrusive_ptr<Expression> onNull)
    : Expression(expCtx,
                 {std::move(format), std::move(date), std::move(timeZone), std::move(onNull)}),
      _format(_children[0]),
      _date(_children[1]),
      _timeZone(_children[2]),
      _onNull(_children[3]) {}