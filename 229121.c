TEST_F(QueryPlannerTest, NegatedElemMatchValueEqArray) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {$eq: [1]}}}}"));
    assertHasOnlyCollscan();
}