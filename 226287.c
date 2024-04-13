TEST_F(QueryPlannerTest, NestedContainedOr) {
    addIndex(BSON("b" << 1 << "a" << 1));
    addIndex(BSON("d" << 1 << "a" << 1));
    addIndex(BSON("e" << 1 << "a" << 1));

    runQuery(
        fromjson("{$and: [{a: 5}, {$or: [{b: 6}, {$and: [{c: 7}, {$or: [{d: 8}, {e: 9}]}]}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[5, 5, true, "
        "true]]}}},"
        "{fetch: {filter: {c: 7}, node: {or: {nodes: ["
        "{ixscan: {pattern: {d: 1, a: 1}, bounds: {d: [[8, 8, true, true]], a: [[5, 5, true, "
        "true]]}}},"
        "{ixscan: {pattern: {e: 1, a: 1}, bounds: {e: [[9, 9, true, true]], a: [[5, 5, true, "
        "true]]}}}"
        "]}}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}