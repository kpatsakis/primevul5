TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCollscanIfNoCoveredIxscans) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    addIndex(BSON("a" << 1));
    runQueryAsCommand(fromjson("{find: 'testns', projection: {a: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {a: 1}, node:"
        "{cscan: {dir: 1}}}}");
}