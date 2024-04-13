TEST_F(QueryPlannerTest, LessThanEqual) {
    addIndex(BSON("x" << 1));

    runQuery(BSON("x" << BSON("$lte" << 5)));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: {$lte: 5}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
}