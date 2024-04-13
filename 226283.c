TEST_F(QueryPlannerTest, EmptyQueryWithProjectionDoesNotConsiderNonHintedIndices) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    addIndex(BSON("a" << 1));
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1}, hint: {_id: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {_id: 1}, "
        "bounds: {_id: [['MinKey', 'MaxKey', true, true]]}}}}}}}");
}