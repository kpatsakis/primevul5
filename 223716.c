Value ExpressionDateToString::evaluate(const Document& root, Variables* variables) const {
    const Value date = _date->evaluate(root, variables);
    Value formatValue;

    // Eagerly validate the format parameter, ignoring if nullish since the input date nullish
    // behavior takes precedence.
    if (_format) {
        formatValue = _format->evaluate(root, variables);
        if (!formatValue.nullish()) {
            uassert(18533,
                    str::stream() << "$dateToString requires that 'format' be a string, found: "
                                  << typeName(formatValue.getType()) << " with value "
                                  << formatValue.toString(),
                    formatValue.getType() == BSONType::String);

            TimeZone::validateToStringFormat(formatValue.getStringData());
        }
    }

    // Evaluate the timezone parameter before checking for nullish input, as this will throw an
    // exception for an invalid timezone string.
    auto timeZone =
        makeTimeZone(getExpressionContext()->timeZoneDatabase, root, _timeZone.get(), variables);

    if (date.nullish()) {
        return _onNull ? _onNull->evaluate(root, variables) : Value(BSONNULL);
    }

    if (!timeZone) {
        return Value(BSONNULL);
    }

    if (_format) {
        if (formatValue.nullish()) {
            return Value(BSONNULL);
        }

        return Value(uassertStatusOK(
            timeZone->formatDate(formatValue.getStringData(), date.coerceToDate())));
    }

    return Value(
        uassertStatusOK(timeZone->formatDate(Value::kISOFormatString, date.coerceToDate())));
}