TEST_F(QueryPlannerTest, NegatedElemMatchObjectInArray) {
    addIndex(BSON("i.j" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {j: {$in: [[1]]}}}}}"));
    assertHasOnlyCollscan();
}