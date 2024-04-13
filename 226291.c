TEST_F(QueryPlannerTest, OrCollapsesToSingleScan2) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a:{$lt:2}}, {a:{$lt:4}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [[-Infinity,4,true,false]]}}}}}");
}