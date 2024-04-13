TEST_F(QueryPlannerTest, ElemMatchValueNegationInArray) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$elemMatch: {$not: {$in: [[1]]}}}}"));
    assertHasOnlyCollscan();
}