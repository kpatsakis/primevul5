    static Value performCastDoubleToLong(ExpressionContext* const expCtx, Value inputValue) {
        double inputDouble = inputValue.getDouble();
        validateDoubleValueIsFinite(inputDouble);

        uassert(ErrorCodes::ConversionFailure,
                str::stream()
                    << "Conversion would overflow target type in $convert with no onError value: "
                    << inputDouble,
                inputDouble >= std::numeric_limits<long long>::lowest() &&
                    inputDouble < BSONElement::kLongLongMaxPlusOneAsDouble);

        return Value(static_cast<long long>(inputDouble));
    }