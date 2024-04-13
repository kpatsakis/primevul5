TEST_F(QueryPlannerTest, EnumerateNestedOr) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));

    runQuery(fromjson("{d: 1, $or: [{a: 1, b: 1}, {c: 1}]}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{fetch: {filter: {d: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {b: 1}, node: {ixscan: {pattern: {a: 1}}}}},"
        "{ixscan: {pattern: {c: 1}}}]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {d: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {a: 1}, node: {ixscan: {pattern: {b: 1}}}}},"
        "{ixscan: {pattern: {c: 1}}}]}}}}");
}