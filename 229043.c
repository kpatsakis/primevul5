TEST_F(QueryPlannerTest, NegationEqArray) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$eq: [1, 2]}}}"));

    assertHasOnlyCollscan();
}