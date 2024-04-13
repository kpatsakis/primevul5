TEST_F(QueryPlannerTest, SkipEvaluatesBeforeFetchForIndexedOr) {
    addIndex(fromjson("{a: 1}"));

    runQuerySkipNToReturn(fromjson("{$or: [{a: 5}, {a: 7}]}"), 8, 0);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{skip: {n: 8, node: "
        "{cscan: {dir: 1, filter: {$or: [{a: 5}, {a: 7}]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {skip: {n: 8, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}}}");
}