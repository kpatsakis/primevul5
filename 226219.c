TEST_F(QueryPlannerTest, KeyPatternOverflowsInt) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << -2147483649LL));

    runQuerySortProj(fromjson("{a: {$gte: 3, $lte: 5}}"), fromjson("{a: 1}"), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a: -2147483649}, "
        "bounds: {a: [[3, 5, true, true]]}}}}}");
}