TEST_F(QueryPlannerTest, IntersectCompoundInsteadThreeCompoundIndices) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("c" << 1 << "d" << 1));
    addIndex(BSON("a" << 1 << "c" << -1 << "b" << -1 << "d" << 1));
    runQuery(fromjson("{a: 1, b: 1, c: 1, d: 1}"));

    assertNumSolutions(3U);
    assertSolutionExists(
        "{fetch: {filter: {$and: [{c:1},{d:1}]}, node: "
        "{ixscan: {filter: null, pattern: {a:1,b:1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a:1},{b:1}]}, node: "
        "{ixscan: {filter: null, pattern: {c:1,d:1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {a:1,c:-1,b:-1,d:1}}}}}");
}