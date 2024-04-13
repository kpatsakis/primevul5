TEST_F(QueryPlannerTest, MinValid) {
    addIndex(BSON("a" << 1));
    runQueryHintMinMax(BSONObj(), BSONObj(), fromjson("{a: 1}"), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {filter: null, pattern: {a: 1}}}}}");
}