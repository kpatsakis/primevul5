TEST_F(QueryPlannerTest, EqualityIndexScan) {
    addIndex(BSON("x" << 1));

    runQuery(BSON("x" << 5));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: 5}}}");
    assertSolutionExists("{fetch: {filter: null, node: {ixscan: {pattern: {x: 1}}}}}");
}