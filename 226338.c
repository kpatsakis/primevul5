TEST_F(QueryPlannerTest, ContainedOrOfAndCollapseIndenticalScansWithFilter2) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{c: 1, $or: [{a:{$gte:1,$lte:1}, b:2}, {a:1, b:2, d:3}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {c: 1}, node: {fetch: {filter: null, node: "
        "{ixscan: {pattern: {a: 1, b: 1}},"
        "bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]]},"
        "filter: null}}}}}");
}