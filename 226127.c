TEST_F(QueryPlannerTest, NoMutationsForSort) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    runQuerySortProj(fromjson(""), fromjson("{a:1}"), BSONObj());
    assertSolutionExists(
        "{sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
}