TEST_F(QueryPlannerTest, FloatingPointInKeyPattern) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << -0.1));

    runQuerySortProj(fromjson("{a: {$gte: 3, $lte: 5}}"), fromjson("{a: 1}"), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a: -0.1}, "
        "bounds: {a: [[3, 5, true, true]]}}}}}");
}