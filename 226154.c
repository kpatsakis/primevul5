TEST_F(QueryPlannerTest, BasicSort) {
    addIndex(BSON("x" << 1));
    runQuerySortProj(BSONObj(), BSON("x" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {x: 1}, limit: 0, node: {sortKeyGen:"
        "{node: {cscan: {dir: 1, filter: {}}}}}}}");
}