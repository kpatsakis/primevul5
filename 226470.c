TEST_F(QueryPlannerTest, BoundsTypeMinKeyMaxKey) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    addIndex(BSON("a" << 1));

    runQuery(fromjson("{a: {$type: -1}}"));
    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1}, bounds:"
        "{a: [['MinKey','MinKey',true,true]]}}}}}");

    runQuery(fromjson("{a: {$type: 127}}"));
    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1}, bounds:"
        "{a: [['MaxKey','MaxKey',true,true]]}}}}}");
}