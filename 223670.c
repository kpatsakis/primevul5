    static Value performFormatDouble(ExpressionContext* const expCtx, Value inputValue) {
        double doubleValue = inputValue.getDouble();

        if (std::isinf(doubleValue)) {
            return Value(std::signbit(doubleValue) ? "-Infinity"_sd : "Infinity"_sd);
        } else if (std::isnan(doubleValue)) {
            return Value("NaN"_sd);
        } else if (doubleValue == 0.0 && std::signbit(doubleValue)) {
            return Value("-0"_sd);
        } else {
            return Value(static_cast<std::string>(str::stream() << doubleValue));
        }
    }