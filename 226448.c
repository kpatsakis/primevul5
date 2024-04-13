TEST_F(QueryPlannerTest, IntersectElemMatch) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a.b" << 1));
    addIndex(BSON("a.c" << 1));
    runQuery(fromjson("{a : {$elemMatch: {b:1, c:1}}}"));
    assertSolutionExists(
        "{fetch: {filter: {a:{$elemMatch:{b:1, c:1}}},"
        "node: {andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {'a.b':1}}},"
        "{ixscan: {filter: null, pattern: {'a.c':1}}}]}}}}");
}