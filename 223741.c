intrusive_ptr<ExpressionObject> ExpressionObject::parse(ExpressionContext* const expCtx,
                                                        BSONObj obj,
                                                        const VariablesParseState& vps) {
    // Make sure we don't have any duplicate field names.
    stdx::unordered_set<string> specifiedFields;

    std::vector<boost::intrusive_ptr<Expression>> children;
    vector<pair<string, intrusive_ptr<Expression>&>> expressions;
    for (auto&& elem : obj) {
        // Make sure this element has a valid field name. Use StringData here so that we can detect
        // if the field name contains a null byte.
        FieldPath::uassertValidFieldName(elem.fieldNameStringData());

        auto fieldName = elem.fieldName();
        uassert(16406,
                str::stream() << "duplicate field name specified in object literal: "
                              << obj.toString(),
                specifiedFields.find(fieldName) == specifiedFields.end());
        specifiedFields.insert(fieldName);
        children.push_back(parseOperand(expCtx, elem, vps));
    }

    std::vector<boost::intrusive_ptr<Expression>>::size_type index = 0;
    for (auto&& elem : obj) {
        expressions.emplace_back(elem.fieldName(), children[index]);
        ++index;
    }

    return new ExpressionObject{expCtx, std::move(children), std::move(expressions)};
}