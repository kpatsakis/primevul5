TEST_F(QueryPlannerTest, NegationElemMatchObject2) {
    addIndex(BSON("i.j" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {j: {$ne: 1}}}}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}