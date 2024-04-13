intrusive_ptr<Expression> ExpressionReplaceBase::optimize() {
    _input = _input->optimize();
    _find = _find->optimize();
    _replacement = _replacement->optimize();
    return this;
}