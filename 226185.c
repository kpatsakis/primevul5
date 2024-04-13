TEST_F(QueryPlannerTest, ContainedOrOfAndCollapseIdenticalScansTwoFilters) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{c: 1, $or: [{a:1, b:2, d:3}, {a:1, b:2, e:4}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {c: 1}, node: {fetch: {filter: {$or:[{e:4},{d:3}]},"
        "node: {ixscan: {pattern: {a: 1, b: 1}, filter: null,"
        "bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]]}}}}}}}");
}