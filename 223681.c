Value ExpressionReplaceBase::serialize(bool explain) const {
    return Value(Document{{getOpName(),
                           Document{{"input", _input->serialize(explain)},
                                    {"find", _find->serialize(explain)},
                                    {"replacement", _replacement->serialize(explain)}}}});
}