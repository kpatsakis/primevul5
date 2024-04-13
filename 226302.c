TEST_F(QueryPlannerTest, HintedBlockingSortIndexFilteredOut) {
    params.options = QueryPlannerParams::NO_BLOCKING_SORT;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {a: 1, b: 1}, sort: {b: 1}, hint: {a: 1}}"));
    assertNumSolutions(0U);
}