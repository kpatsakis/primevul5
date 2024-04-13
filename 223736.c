    Optimized(ExpressionContext* const expCtx,
              const ValueSet& cachedRhsSet,
              const ExpressionVector& operands)
        : ExpressionSetIsSubset(expCtx), _cachedRhsSet(cachedRhsSet) {
        _children = operands;
    }