TEST_F(QueryPlannerTest, SkipAndSoftLimit) {
    addIndex(BSON("x" << 1));

    runQuerySkipNToReturn(BSON("x" << BSON("$lte" << 4)), 7, 2);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{skip: {n: 7, node: "
        "{cscan: {dir: 1, filter: {x: {$lte: 4}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {skip: {n: 7, node: "
        "{ixscan: {filter: null, pattern: {x: 1}}}}}}}");
}