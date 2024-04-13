TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCollscanIfIndexIsGeo) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    addIndex(BSON("a"
                  << "2dsphere"));
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{cscan: {dir: 1}}}}");
}