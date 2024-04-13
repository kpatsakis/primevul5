TEST_F(QueryPlannerTest, HintValid) {
    addIndex(BSON("a" << 1));
    runQueryHint(BSONObj(), fromjson("{a: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {filter: null, pattern: {a: 1}}}}}");
}