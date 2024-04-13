TEST_F(QueryPlannerTest, SortSkipLimit) {
    runQuerySortProjSkipNToReturn(BSONObj(), fromjson("{a: 1}"), BSONObj(), 2, -3);
    assertNumSolutions(1U);
    // Limit in sort node should be adjusted by skip count
    assertSolutionExists(
        "{skip: {n: 2, node: "
        "{sort: {pattern: {a: 1}, limit: 5, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}}}");
}