TEST_F(QueryPlannerTest, SortElimTrailingFields) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    runQuerySortProj(fromjson("{a: 5}"), BSON("b" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1, filter: {a: 5}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1, b: 1, c: 1}}}}}");
}