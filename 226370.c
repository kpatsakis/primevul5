TEST_F(QueryPlannerTest, SkipEvaluatesAfterFetchWithPredicate) {
    addIndex(fromjson("{a: 1}"));

    runQuerySkipNToReturn(fromjson("{a: 5, b: 7}"), 8, 0);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{skip: {n: 8, node: {cscan: {dir: 1, filter: {a: 5, b: 7}}}}}");

    // When a plan includes a fetch with no predicate, the skip should execute first, so we avoid
    // fetching a document that we will always discard. When the fetch does have a predicate (as in
    // this case), however, that optimization would be invalid; we need to fetch the document and
    // evaluate the filter to determine if the document should count towards the number of skipped
    // documents.
    assertSolutionExists(
        "{skip: {n: 8, node: {fetch: {filter: {b: 7}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}}}");
}