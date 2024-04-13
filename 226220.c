TEST_F(QueryPlannerTest, NegationCantUseSparseIndex) {
    // false means not multikey, true means sparse
    addIndex(BSON("i" << 1), false, true);
    runQuery(fromjson("{i: {$ne: 4}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}