ExpressionObject::ExpressionObject(ExpressionContext* const expCtx,
                                   std::vector<boost::intrusive_ptr<Expression>> _children,
                                   vector<pair<string, intrusive_ptr<Expression>&>>&& expressions)
    : Expression(expCtx, std::move(_children)), _expressions(std::move(expressions)) {}