TEST_F(QueryPlannerTest, HintValidWithSort) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuerySortHint(fromjson("{a: 100, b: 200}"), fromjson("{b: 1}"), fromjson("{a: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {filter: {b: 200}, "
        "node: {ixscan: {filter: null, pattern: {a: 1}}}}}}}}}");
}