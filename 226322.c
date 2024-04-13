TEST_F(QueryPlannerTest, EmptyQueryWithProjectionDoesNotUseCoveredIxscanOnCompoundIndexIfDisabled) {
    params.options &= ~QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1, c: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, c: 1}, node: "
        "{cscan: {dir: 1}}}}");
}