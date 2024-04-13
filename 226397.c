TEST_F(QueryPlannerTest, SortElimTrailingFieldsReverse) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1 << "d" << 1));
    runQuerySortProj(fromjson("{a: 5, b: 6}"), BSON("c" << -1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {c: -1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1, filter: {a: 5, b: 6}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, dir: -1, pattern: {a: 1, b: 1, c: 1, d: 1}}}}}");
}