intrusive_ptr<Expression> ExpressionLet::parse(ExpressionContext* const expCtx,
                                               BSONElement expr,
                                               const VariablesParseState& vpsIn) {
    verify(expr.fieldNameStringData() == "$let");

    uassert(16874, "$let only supports an object as its argument", expr.type() == Object);
    const BSONObj args = expr.embeddedObject();

    // varsElem must be parsed before inElem regardless of BSON order.
    BSONElement varsElem;
    BSONElement inElem;
    for (auto&& arg : args) {
        if (arg.fieldNameStringData() == "vars") {
            varsElem = arg;
        } else if (arg.fieldNameStringData() == "in") {
            inElem = arg;
        } else {
            uasserted(16875,
                      str::stream() << "Unrecognized parameter to $let: " << arg.fieldName());
        }
    }

    uassert(16876, "Missing 'vars' parameter to $let", !varsElem.eoo());
    uassert(16877, "Missing 'in' parameter to $let", !inElem.eoo());

    // parse "vars"
    VariablesParseState vpsSub(vpsIn);  // vpsSub gets our vars, vpsIn doesn't.
    VariableMap vars;
    std::vector<boost::intrusive_ptr<Expression>> children;
    auto&& varsObj = varsElem.embeddedObjectUserCheck();
    for (auto&& varElem : varsObj)
        children.push_back(parseOperand(expCtx, varElem, vpsIn));

    // Make a place in the vector for "in".
    auto& inPtr = children.emplace_back(nullptr);

    std::vector<boost::intrusive_ptr<Expression>>::size_type index = 0;
    std::vector<Variables::Id> orderedVariableIds;
    for (auto&& varElem : varsObj) {
        const string varName = varElem.fieldName();
        Variables::validateNameForUserWrite(varName);
        Variables::Id id = vpsSub.defineVariable(varName);

        orderedVariableIds.push_back(id);

        vars.emplace(id, NameAndExpression{varName, children[index]});  // only has outer vars
        ++index;
    }

    // parse "in"
    inPtr = parseOperand(expCtx, inElem, vpsSub);  // has our vars

    return new ExpressionLet(
        expCtx, std::move(vars), std::move(children), std::move(orderedVariableIds));
}