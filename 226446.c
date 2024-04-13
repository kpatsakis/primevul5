TEST_F(QueryPlannerTest, RootedOrOfAndCollapseScansExistingOrFilter) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{$or: [{a:1, b:2, $or: [{c:3}, {d:4}]}, {a:1, b:2, e:5}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{e:5},{c:3},{d:4}]}, node: {ixscan: "
        "{filter: null, pattern: {a: 1, b: 1}, "
        "bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]]}}}}}");
}