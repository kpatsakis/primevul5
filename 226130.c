TEST_F(QueryPlannerTest, DoublyContainedOr) {
    addIndex(BSON("b" << 1 << "a" << 1));
    addIndex(BSON("c" << 1 << "a" << 1));
    addIndex(BSON("d" << 1));

    runQuery(
        fromjson("{$and: [{$or: [{$and: [{a: 5}, {$or: [{b: 6}, {c: 7}]}]}, {d: 8}]}, {e: 9}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: {e: 9}, node: {or: {nodes: ["
        "{or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[5, 5, true, "
        "true]]}}},"
        "{ixscan: {pattern: {c: 1, a: 1}, bounds: {c: [[7, 7, true, true]], a: [[5, 5, true, "
        "true]]}}}]}},"
        "{ixscan: {pattern: {d: 1}, bounds: {d: [[8, 8, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}