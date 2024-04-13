TEST_F(QueryPlannerTest, SortLimit) {
    // Negative limit indicates hard limit - see query_request.cpp
    runQuerySortProjSkipNToReturn(BSONObj(), fromjson("{a: 1}"), BSONObj(), 0, -3);
    assertNumSolutions(1U);
    assertSolutionExists(
        "{sort: {pattern: {a: 1}, limit: 3, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}");
}