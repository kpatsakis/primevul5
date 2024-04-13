intrusive_ptr<Expression> ExpressionDateToString::parse(ExpressionContext* const expCtx,
                                                        BSONElement expr,
                                                        const VariablesParseState& vps) {
    verify(expr.fieldNameStringData() == "$dateToString");

    uassert(18629,
            "$dateToString only supports an object as its argument",
            expr.type() == BSONType::Object);

    BSONElement formatElem, dateElem, timeZoneElem, onNullElem;
    for (auto&& arg : expr.embeddedObject()) {
        auto field = arg.fieldNameStringData();

        if (field == "format"_sd) {
            formatElem = arg;
        } else if (field == "date"_sd) {
            dateElem = arg;
        } else if (field == "timezone"_sd) {
            timeZoneElem = arg;
        } else if (field == "onNull"_sd) {
            onNullElem = arg;
        } else {
            uasserted(18534,
                      str::stream()
                          << "Unrecognized argument to $dateToString: " << arg.fieldName());
        }
    }

    uassert(18628, "Missing 'date' parameter to $dateToString", !dateElem.eoo());

    return new ExpressionDateToString(expCtx,
                                      parseOperand(expCtx, dateElem, vps),
                                      formatElem ? parseOperand(expCtx, formatElem, vps) : nullptr,
                                      timeZoneElem ? parseOperand(expCtx, timeZoneElem, vps)
                                                   : nullptr,
                                      onNullElem ? parseOperand(expCtx, onNullElem, vps) : nullptr);
}