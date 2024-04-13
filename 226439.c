TEST_F(QueryPlannerTest, NorWithSingleChildCanUseIndexAfterComplementingBounds) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$nor: [{a: {$lt: 3}}]}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a: 1}, bounds:"
        "{a: [['MinKey', -Infinity, true, false], [3, 'MaxKey', true, true]]}}}}}");
}