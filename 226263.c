TEST_F(QueryPlannerTest, EqualityIndexScanWithTrailingFields) {
    addIndex(BSON("x" << 1 << "y" << 1));

    runQuery(BSON("x" << 5));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: 5}}}");
    assertSolutionExists("{fetch: {filter: null, node: {ixscan: {pattern: {x: 1, y: 1}}}}}");
}