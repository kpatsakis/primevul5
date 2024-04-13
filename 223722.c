    static Value performCastLongToInt(ExpressionContext* const expCtx, Value inputValue) {
        long long longValue = inputValue.getLong();

        uassert(ErrorCodes::ConversionFailure,
                str::stream()
                    << "Conversion would overflow target type in $convert with no onError value: ",
                longValue >= std::numeric_limits<int>::min() &&
                    longValue <= std::numeric_limits<int>::max());

        return Value(static_cast<int>(longValue));
    }