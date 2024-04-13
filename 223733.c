intrusive_ptr<Expression> Expression::parseExpression(ExpressionContext* const expCtx,
                                                      BSONObj obj,
                                                      const VariablesParseState& vps) {
    uassert(15983,
            str::stream() << "An object representing an expression must have exactly one "
                             "field: "
                          << obj.toString(),
            obj.nFields() == 1);

    // Look up the parser associated with the expression name.
    const char* opName = obj.firstElementFieldName();
    auto it = parserMap.find(opName);
    uassert(ErrorCodes::InvalidPipelineOperator,
            str::stream() << "Unrecognized expression '" << opName << "'",
            it != parserMap.end());

    // Make sure we are allowed to use this expression under the current feature compatibility
    // version.
    auto& entry = it->second;
    uassert(ErrorCodes::QueryFeatureNotAllowed,
            // We would like to include the current version and the required minimum version in this
            // error message, but using FeatureCompatibilityVersion::toString() would introduce a
            // dependency cycle (see SERVER-31968).
            str::stream() << opName
                          << " is not allowed in the current feature compatibility version. See "
                          << feature_compatibility_version_documentation::kCompatibilityLink
                          << " for more information.",
            !expCtx->maxFeatureCompatibilityVersion || !entry.requiredMinVersion ||
                (*entry.requiredMinVersion <= *expCtx->maxFeatureCompatibilityVersion));
    return entry.parser(expCtx, obj.firstElement(), vps);
}