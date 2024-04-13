TEST_F(QueryPlannerTest, MaxValid) {
    addIndex(BSON("a" << 1));
    runQueryHintMinMax(BSONObj(), BSONObj(), BSONObj(), fromjson("{a: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {filter: null, pattern: {a: 1}}}}}");
}