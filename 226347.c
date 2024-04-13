TEST_F(QueryPlannerTest, ReverseScanForSort) {
    addIndex(BSON("_id" << 1));
    runQuerySortProj(BSONObj(), fromjson("{_id: -1}"), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {_id: -1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {_id: 1}}}}}");
}