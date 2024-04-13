    Optimized(ExpressionContext* const expCtx,
              const ValueUnorderedMap<vector<int>>& indexMap,
              const ExpressionVector& operands)
        : ExpressionIndexOfArray(expCtx), _indexMap(std::move(indexMap)) {
        _children = operands;
    }