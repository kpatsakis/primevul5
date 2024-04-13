TEST_F(QueryPlannerTest, IntersectCompoundInsteadUnusedField) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    runQuery(fromjson("{a: 1, b: 1}"));

    assertNumSolutions(3U);
    assertSolutionExists(
        "{fetch: {filter: {b:1}, node: "
        "{ixscan: {filter: null, pattern: {a:1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a:1}, node: "
        "{ixscan: {filter: null, pattern: {b:1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {a:1,b:1,c:1}}}}}");
}