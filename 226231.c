TEST_F(QueryPlannerTest, GreaterThanEqual) {
    addIndex(BSON("x" << 1));

    runQuery(BSON("x" << BSON("$gte" << 5)));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: {$gte: 5}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
}