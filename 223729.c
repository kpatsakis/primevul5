    static Value performCastDecimalToDouble(ExpressionContext* const expCtx, Value inputValue) {
        Decimal128 inputDecimal = inputValue.getDecimal();

        std::uint32_t signalingFlags = Decimal128::SignalingFlag::kNoFlag;
        double result =
            inputDecimal.toDouble(&signalingFlags, Decimal128::RoundingMode::kRoundTiesToEven);

        uassert(ErrorCodes::ConversionFailure,
                str::stream()
                    << "Conversion would overflow target type in $convert with no onError value: "
                    << inputDecimal.toString(),
                signalingFlags == Decimal128::SignalingFlag::kNoFlag ||
                    signalingFlags == Decimal128::SignalingFlag::kInexact);

        return Value(result);
    }