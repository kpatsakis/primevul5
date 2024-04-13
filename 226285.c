TEST_F(QueryPlannerTest, SparseIndexIgnoreForSort) {
    addIndex(fromjson("{a: 1}"), false, true);
    runQuerySortProj(BSONObj(), fromjson("{a: 1}"), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
}