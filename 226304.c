TEST_F(QueryPlannerTest,
       EmptyQueryWithProjectionDoesNotUseCoveredIxscanOnDotttedNonMultikeyIndexIfDisabled) {
    params.options &= ~QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    addIndex(BSON("a.b" << 1));
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, 'a.b': 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, 'a.b': 1}, node: "
        "{cscan: {dir: 1}}}}");
}