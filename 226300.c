TEST_F(QueryPlannerTest, SparseIndexPreferCompoundIndexForSort) {
    addIndex(fromjson("{a: 1}"), false, true);
    addIndex(fromjson("{a: 1, b: 1}"));
    runQuerySortProj(BSONObj(), fromjson("{a: 1}"), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1, b: 1}}}}}");
}