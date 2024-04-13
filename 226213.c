TEST_F(QueryPlannerTest, SortSkipSoftLimit) {
    runQuerySortProjSkipNToReturn(BSONObj(), fromjson("{a: 1}"), BSONObj(), 2, 3);
    assertNumSolutions(1U);
    assertSolutionExists(
        "{skip: {n: 2, node: "
        "{sort: {pattern: {a: 1}, limit: 5, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}}}");
}