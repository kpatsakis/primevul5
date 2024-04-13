TEST_F(QueryPlannerTest, MutationsFromFetchWithSort) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: 5}"), fromjson("{b:1}"), BSONObj());
    assertSolutionExists(
        "{sort: {pattern: {b:1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {node: {ixscan: {pattern: {a:1}}}}}}}}}");
}