TEST_F(QueryPlannerTest, ContainedOrMultiplePredicates) {
    addIndex(BSON("c" << 1 << "a" << 1 << "b" << 1));
    addIndex(BSON("d" << 1 << "b" << 1 << "a" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {b: 6}, {$or: [{c: 7}, {d: 8}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {c: 1, a: 1, b: 1}, bounds: {c: [[7, 7, true, true]], a: [[5, 5, true, "
        "true]], b: [[6, 6, true, true]]}}},"
        "{ixscan: {pattern: {d: 1, b: 1, a: 1}, bounds: {d: [[8, 8, true, true]], b: [[6, 6, true, "
        "true]], a: [[5, 5, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}