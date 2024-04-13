TEST_F(QueryPlannerTest, NegationAndIndexOnEquality) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{$and: [{a: 1}, {b: {$ne: 1}}]}"), BSONObj(), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1},"
        "bounds: {a: [[1,1,true,true]]}}}}}");
}