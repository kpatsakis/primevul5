TEST_F(QueryPlannerTest, HintedNotCoveredProjectionIndexFilteredOut) {
    params.options = QueryPlannerParams::NO_UNCOVERED_PROJECTIONS;
    addIndex(BSON("a" << 1));
    addIndex(BSON("a" << 1 << "b" << 1));
    runQueryAsCommand(fromjson(
        "{find: 'testns', filter: {a: 1}, projection: {a: 1, b: 1, _id: 0}, hint: {a: 1}}"));
    assertNumSolutions(0U);
}