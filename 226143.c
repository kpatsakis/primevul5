TEST_F(QueryPlannerTest, NegationTypeOperator) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$type: 16}}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}