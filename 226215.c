TEST_F(QueryPlannerTest, NegationAndIndexOnEqualityAndNegationBranches) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuerySortProj(fromjson("{$and: [{a: 1}, {b: 2}, {b: {$ne: 1}}]}"), BSONObj(), BSONObj());

    assertNumSolutions(3U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1}, "
        "bounds: {a: [[1,1,true,true]]}}}}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {b: 1}, "
        "bounds: {b: [[2,2,true,true]]}}}}}");
}