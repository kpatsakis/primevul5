TEST_F(QueryPlannerTest, NegationRegexPrefix) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: /^a/}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}