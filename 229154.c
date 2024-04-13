TEST_F(QueryPlannerTest, NegatedElemMatchValueInArray) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {$in: [[1]]}}}}"));
    assertHasOnlyCollscan();
}