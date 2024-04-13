TEST_F(QueryPlannerTest, BasicSoftLimitNoIndex) {
    addIndex(BSON("a" << 1));

    runQuerySkipNToReturn(BSON("x" << 5), 0, 3);

    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: 5}}}");
}