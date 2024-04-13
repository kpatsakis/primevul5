TEST_F(QueryPlannerTest, OrCollapsesToSingleScan) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a:{$gt:2}}, {a:{$gt:0}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [[0,Infinity,false,true]]}}}}}");
}