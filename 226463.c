TEST_F(QueryPlannerTest, ExistsFalseSparseIndex) {
    addIndex(BSON("x" << 1), false, true);

    runQuery(fromjson("{x: {$exists: false}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}