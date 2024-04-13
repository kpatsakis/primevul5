    static Value parseStringToOID(ExpressionContext* const expCtx, Value inputValue) {
        try {
            return Value(OID::createFromString(inputValue.getStringData()));
        } catch (const DBException& ex) {
            // Rethrow any caught exception as a conversion failure such that 'onError' is evaluated
            // and returned.
            uasserted(ErrorCodes::ConversionFailure,
                      str::stream() << "Failed to parse objectId '" << inputValue.getString()
                                    << "' in $convert with no onError value: " << ex.reason());
        }
    }