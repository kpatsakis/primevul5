    static Value performCastNumberToDate(ExpressionContext* const expCtx, Value inputValue) {
        long long millisSinceEpoch;

        switch (inputValue.getType()) {
            case BSONType::NumberLong:
                millisSinceEpoch = inputValue.getLong();
                break;
            case BSONType::NumberDouble:
                millisSinceEpoch = performCastDoubleToLong(expCtx, inputValue).getLong();
                break;
            case BSONType::NumberDecimal:
                millisSinceEpoch =
                    performCastDecimalToInt(BSONType::NumberLong, inputValue).getLong();
                break;
            default:
                MONGO_UNREACHABLE;
        }

        return Value(Date_t::fromMillisSinceEpoch(millisSinceEpoch));
    }