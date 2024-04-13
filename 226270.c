TEST_F(QueryPlannerTest, HintValidWithPredicate) {
    addIndex(BSON("a" << 1));
    runQueryHint(fromjson("{a: {$gt: 1}}"), fromjson("{a: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {filter: null, pattern: {a: 1}}}}}");
}