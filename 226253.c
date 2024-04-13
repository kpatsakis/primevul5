TEST_F(QueryPlannerTest, OrCollapsesToSingleScan3) {
    addIndex(BSON("a" << 1));
    runQueryHint(fromjson("{$or: [{a:1},{a:3}]}"), fromjson("{a:1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [[1,1,true,true], [3,3,true,true]]}}}}}");
}