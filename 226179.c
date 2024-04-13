TEST_F(QueryPlannerTest, NegationElemMatchValue) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {$gt: 3, $lt: 10}}}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}