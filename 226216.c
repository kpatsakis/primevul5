TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCollscanIfIndexIsMultikey) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    constexpr bool isMultikey = true;
    addIndex(BSON("a" << 1 << "b" << 1), isMultikey);
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1, b: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{cscan: {dir: 1}}}}");
}