TEST_F(QueryPlannerTest, ContainedOrMultikeyCannotCombineTrailingFields) {
    const bool multikey = true;
    addIndex(BSON("b" << 1 << "a" << 1), multikey);
    addIndex(BSON("c" << 1));

    runQuery(
        fromjson("{$and: [{a: {$gte: 0}}, {$or: [{$and: [{a: {$lte: 10}}, {b: 6}]}, {c: 7}]}]}"));
    assertNumSolutions(2);
    std::vector<std::string> alternates;
    alternates.push_back(
        "{fetch: {filter: {a: {$gte: 0}}, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[-Infinity, 10, "
        "true, true]]}}},"
        "{ixscan: {pattern: {c: 1}, bounds: {c: [[7, 7, true, true]]}}}"
        "]}}}}");
    alternates.push_back(
        "{fetch: {filter: {a: {$gte: 0}}, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[0, Infinity, "
        "true, true]]}}},"
        "{ixscan: {pattern: {c: 1}, bounds: {c: [[7, 7, true, true]]}}}"
        "]}}}}");
    assertHasOneSolutionOf(alternates);
    assertSolutionExists("{cscan: {dir: 1}}}}");
}