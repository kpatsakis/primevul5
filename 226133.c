TEST_F(QueryPlannerTest, OrOnlyOneBranchCanUseIndex) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a:1}, {b:2}]}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}