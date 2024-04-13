TEST_F(QueryPlannerTest, NegationElemMatchObject) {
    addIndex(BSON("i.j" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {j: 1}}}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}