TEST_F(QueryPlannerTest, NegationInArray) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$in: [1, [1, 2]]}}}"));

    assertHasOnlyCollscan();
}