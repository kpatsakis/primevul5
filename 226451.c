TEST_F(QueryPlannerTest, SparseIndexHintForSort) {
    addIndex(fromjson("{a: 1}"), false, true);
    runQuerySortHint(BSONObj(), fromjson("{a: 1}"), fromjson("{a: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1}}}}}");
}