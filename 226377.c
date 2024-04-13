TEST_F(QueryPlannerTest, NinCantUseMultikeyIndex) {
    // true means multikey
    addIndex(BSON("a" << 1), true);
    runQuery(fromjson("{a: {$nin: [4, /foobar/]}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}