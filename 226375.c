TEST_F(QueryPlannerTest, BasicSkipNoIndex) {
    addIndex(BSON("a" << 1));

    runQuerySkipNToReturn(BSON("x" << 5), 3, 0);

    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{skip: {n: 3, node: {cscan: {dir: 1, filter: {x: 5}}}}}");
}