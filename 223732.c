boost::intrusive_ptr<Expression> ExpressionSwitch::parse(ExpressionContext* const expCtx,
                                                         BSONElement expr,
                                                         const VariablesParseState& vps) {
    uassert(40060,
            str::stream() << "$switch requires an object as an argument, found: "
                          << typeName(expr.type()),
            expr.type() == Object);

    boost::intrusive_ptr<Expression> expDefault;
    std::vector<boost::intrusive_ptr<Expression>> children;
    for (auto&& elem : expr.Obj()) {
        auto field = elem.fieldNameStringData();

        if (field == "branches") {
            // Parse each branch separately.
            uassert(40061,
                    str::stream() << "$switch expected an array for 'branches', found: "
                                  << typeName(elem.type()),
                    elem.type() == Array);

            for (auto&& branch : elem.Array()) {
                uassert(40062,
                        str::stream() << "$switch expected each branch to be an object, found: "
                                      << typeName(branch.type()),
                        branch.type() == Object);

                boost::intrusive_ptr<Expression> switchCase, switchThen;

                for (auto&& branchElement : branch.Obj()) {
                    auto branchField = branchElement.fieldNameStringData();

                    if (branchField == "case") {
                        switchCase = parseOperand(expCtx, branchElement, vps);
                    } else if (branchField == "then") {
                        switchThen = parseOperand(expCtx, branchElement, vps);
                    } else {
                        uasserted(40063,
                                  str::stream() << "$switch found an unknown argument to a branch: "
                                                << branchField);
                    }
                }

                uassert(40064, "$switch requires each branch have a 'case' expression", switchCase);
                uassert(
                    40065, "$switch requires each branch have a 'then' expression.", switchThen);

                children.push_back(std::move(switchCase));
                children.push_back(std::move(switchThen));
            }
        } else if (field == "default") {
            // Optional, arbitrary expression.
            expDefault = parseOperand(expCtx, elem, vps);
        } else {
            uasserted(40067, str::stream() << "$switch found an unknown argument: " << field);
        }
    }
    children.push_back(std::move(expDefault));
    // Obtain references to the case and branch expressions two-by-two from the children vector,
    // ignore the last.
    std::vector<ExpressionPair> branches;
    boost::optional<boost::intrusive_ptr<Expression>&> first;
    for (auto&& child : children) {
        if (first) {
            branches.emplace_back(*first, child);
            first = boost::none;
        } else {
            first = child;
        }
    }

    uassert(40068, "$switch requires at least one branch.", !branches.empty());

    return new ExpressionSwitch(expCtx, std::move(children), std::move(branches));
}