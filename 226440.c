TEST_F(QueryPlannerTest, CantUseCompound) {
    addIndex(BSON("x" << 1 << "y" << 1));
    runQuery(fromjson("{ y: 10}"));

    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{cscan: {dir: 1, filter: {y: 10}}}");
}