TEST_F(QueryPlannerTest, ContainedOrCombineLeadingFields) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1 << "a" << 1));

    runQuery(fromjson("{$and: [{a: {$gte: 0}}, {$or: [{a: {$lte: 10}}, {b: 6}]}]}"));
    assertNumSolutions(3);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1}, bounds: {a: [[0, 10, true, true]]}}},"
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[0, Infinity, "
        "true, true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{a: {$lte: 10}}, {b: 6}]}, node: "
        "{ixscan: {pattern: {a: 1}, bounds: {a: [[0, Infinity, true, true]]}}}"
        "}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}