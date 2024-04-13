TEST_F(QueryPlannerTest, BasicSkipWithIndex) {
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuerySkipNToReturn(BSON("a" << 5), 8, 0);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{skip: {n: 8, node: {cscan: {dir: 1, filter: {a: 5}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {skip: {n: 8, node: "
        "{ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}}}");
}