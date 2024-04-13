TEST_F(QueryPlannerTest, AndWithNestedNE) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{a: {$gt: -1, $lt: 1, $ne: 0}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [[-1,0,false,false], "
        "[0,1,false,false]]}}}}}");
}