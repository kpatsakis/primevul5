TEST_F(QueryPlannerTest, EmptyQueryWithProjectionUsesCollscanIfIndexIsPartial) {
    params.options = QueryPlannerParams::GENERATE_COVERED_IXSCANS;
    AlwaysFalseMatchExpression matchExpr;
    addIndex(BSON("a" << 1), &matchExpr);
    runQueryAsCommand(fromjson("{find: 'testns', projection: {_id: 0, a: 1}}"));
    assertNumSolutions(1);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{cscan: {dir: 1}}}}");
}