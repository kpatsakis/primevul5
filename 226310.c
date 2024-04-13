TEST_F(QueryPlannerTest, SortSkip) {
    runQuerySortProjSkipNToReturn(BSONObj(), fromjson("{a: 1}"), BSONObj(), 2, 0);
    assertNumSolutions(1U);
    // If only skip is provided, do not limit sort.
    assertSolutionExists(
        "{skip: {n: 2, node: "
        "{sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}}}");
}