Value ExpressionFirst::evaluate(const Document& root, Variables* variables) const {
    const Value array = _children[0]->evaluate(root, variables);
    return arrayElemAt(this, array, Value(0));
}