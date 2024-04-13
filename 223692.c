Expression::Parser makeConversionAlias(const StringData shortcutName, BSONType toType) {
    return [=](ExpressionContext* const expCtx,
               BSONElement elem,
               const VariablesParseState& vps) -> intrusive_ptr<Expression> {
        // Use parseArguments to allow for a singleton array, or the unwrapped version.
        auto operands = ExpressionNary::parseArguments(expCtx, elem, vps);

        uassert(50723,
                str::stream() << shortcutName << " requires a single argument, got "
                              << operands.size(),
                operands.size() == 1);
        return ExpressionConvert::create(expCtx, std::move(operands[0]), toType);
    };
}