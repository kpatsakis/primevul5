TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCoveredIxscanIfEnabled) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    addIndex(BSON("a" << 1));
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{ixscan: {filter: null, pattern: {a: 1},"
        "bounds: {a: [['MinKey', 'MaxKey', true, true]]}}}}}");
}