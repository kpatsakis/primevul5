TEST_F(QueryPlannerTest, BasicLimitWithIndex) {
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuerySkipNToReturn(BSON("a" << 5), 0, -5);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{limit: {n: 5, node: {cscan: {dir: 1, filter: {a: 5}}}}}");
    assertSolutionExists(
        "{limit: {n: 5, node: {fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}}}");
}