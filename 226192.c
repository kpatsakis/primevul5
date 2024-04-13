TEST_F(QueryPlannerTest, BasicSoftLimitWithIndex) {
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuerySkipNToReturn(BSON("a" << 5), 0, 5);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {a: 5}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}");
}