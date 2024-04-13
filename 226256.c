TEST_F(QueryPlannerTest, CannotIntersectSubnodes) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));
    addIndex(BSON("d" << 1));

    runQuery(fromjson("{$or: [{a: 1}, {b: 1}], $or: [{c: 1}, {d: 1}]}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{fetch: {filter: {$or: [{c: 1}, {d: 1}]}, node: {or: {nodes: ["
        "{ixscan: {filter: null, pattern: {a: 1}}},"
        "{ixscan: {filter: null, pattern: {b: 1}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{a: 1}, {b: 1}]}, node: {or: {nodes: ["
        "{ixscan: {filter: null, pattern: {c: 1}}},"
        "{ixscan: {filter: null, pattern: {d: 1}}}"
        "]}}}}");
}