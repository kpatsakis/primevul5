TEST_F(QueryPlannerTest, SparseIndexForQuery) {
    addIndex(fromjson("{a: 1}"), false, true);
    runQuerySortProj(fromjson("{a: 1}"), BSONObj(), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {a: 1}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1}}}}}");
}