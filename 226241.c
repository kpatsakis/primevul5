TEST_F(QueryPlannerTest, RootedOrOfAndCollapseIndenticalScans) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{$or: [{a:1, b:2}, {a:1, b:2}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a: 1, b: 1}},"
        "bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]]},"
        "filter: null}}}");
}