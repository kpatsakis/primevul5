Value ExpressionArrayElemAt::evaluate(const Document& root, Variables* variables) const {
    const Value array = _children[0]->evaluate(root, variables);
    const Value indexArg = _children[1]->evaluate(root, variables);
    return arrayElemAt(this, array, indexArg);
}