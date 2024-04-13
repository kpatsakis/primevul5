TEST_F(QueryPlannerTest, LessThan) {
    addIndex(BSON("x" << 1));

    runQuery(BSON("x" << BSON("$lt" << 5)));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: {$lt: 5}}}}");
    assertSolutionExists("{fetch: {filter: null, node: {ixscan: {pattern: {x: 1}}}}}");
}