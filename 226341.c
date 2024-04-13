TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCollscanIfIndexIsSparse) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    constexpr bool isMultikey = false;
    constexpr bool isSparse = true;
    addIndex(BSON("a" << 1), isMultikey, isSparse);
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1, b: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{cscan: {dir: 1}}}}");
}