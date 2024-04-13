TEST_F(QueryPlannerTest, HintOnNonUniqueIndex) {
    params.options = QueryPlannerParams::INDEX_INTERSECTION;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1),
             false,  // multikey
             false,  // sparse,
             true);  // unique

    runQueryHint(fromjson("{a: 1, b: 1}"), BSON("a" << 1));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: {b: 1}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
}