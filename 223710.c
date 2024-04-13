boost::intrusive_ptr<ExpressionObject> ExpressionObject::create(
    ExpressionContext* const expCtx,
    std::vector<std::pair<std::string, boost::intrusive_ptr<Expression>>>&&
        expressionsWithChildrenInPlace) {
    std::vector<boost::intrusive_ptr<Expression>> children;
    std::vector<std::pair<std::string, boost::intrusive_ptr<Expression>&>> expressions;
    for (auto& [unused, expression] : expressionsWithChildrenInPlace)
        // These 'push_back's must complete before we insert references to the 'children' vector
        // into the 'expressions' vector since 'push_back' invalidates references.
        children.push_back(std::move(expression));
    std::vector<boost::intrusive_ptr<Expression>>::size_type index = 0;
    for (auto& [fieldName, unused] : expressionsWithChildrenInPlace) {
        expressions.emplace_back(fieldName, children[index]);
        ++index;
    }
    // It is safe to 'std::move' 'children' since the standard guarantees the references are stable.
    return new ExpressionObject(expCtx, std::move(children), std::move(expressions));
}