TEST_F(QueryPlannerTest, CantUseTextIndexToProvideSort) {
    addIndex(BSON("x" << 1 << "_fts"
                      << "text"
                      << "_ftsx"
                      << 1));
    runQuerySortProj(BSONObj(), BSON("x" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{sort: {pattern: {x: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1, filter: {}}}}}}}");
}