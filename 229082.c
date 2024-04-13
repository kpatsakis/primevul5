TEST_F(QueryPlannerTest, ElemMatchValueNegationEqArray) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$elemMatch: {$not: {$eq: [1]}}}}"));
    assertHasOnlyCollscan();
}