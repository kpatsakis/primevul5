TEST_F(QueryPlannerTest, NegationMod) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$mod: [2, 1]}}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}