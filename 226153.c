TEST_F(QueryPlannerTest, ContainedOrCombineWithAnd) {
    addIndex(BSON("b" << 1 << "a" << 1));
    addIndex(BSON("c" << 1 << "d" << 1 << "a" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {$or: [{b: 6}, {$and: [{c: 7}, {d: 8}]}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[5, 5, true, "
        "true]]}}},"
        "{ixscan: {pattern: {c: 1, d: 1, a: 1}, bounds: {c: [[7, 7, true, true]], d: [[8, 8, true, "
        "true]], a: [[5, 5, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}