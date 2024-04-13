    static Value parseStringToNumber(ExpressionContext* const expCtx, Value inputValue) {
        auto stringValue = inputValue.getStringData();
        targetType result;

        // Reject any strings in hex format. This check is needed because the
        // NumberParser::parse call below allows an input hex string prefixed by '0x' when
        // parsing to a double.
        uassert(ErrorCodes::ConversionFailure,
                str::stream() << "Illegal hexadecimal input in $convert with no onError value: "
                              << stringValue,
                !stringValue.startsWith("0x"));

        Status parseStatus = NumberParser().base(base)(stringValue, &result);
        uassert(ErrorCodes::ConversionFailure,
                str::stream() << "Failed to parse number '" << stringValue
                              << "' in $convert with no onError value: " << parseStatus.reason(),
                parseStatus.isOK());

        return Value(result);
    }