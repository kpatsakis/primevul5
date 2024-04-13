ExpressionLet::ExpressionLet(ExpressionContext* const expCtx,
                             VariableMap&& vars,
                             std::vector<boost::intrusive_ptr<Expression>> children,
                             std::vector<Variables::Id> orderedVariableIds)
    : Expression(expCtx, std::move(children)),
      _variables(std::move(vars)),
      _orderedVariableIds(std::move(orderedVariableIds)),
      _subExpression(_children.back()) {}