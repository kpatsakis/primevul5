TEST_F(QueryPlannerTest, ElemMatchObjectNegationInArray) {
    addIndex(BSON("i.j" << 1));
    runQuery(fromjson("{i: {$elemMatch: {j: {$not: {$in: [[1]]}}}}}"));
    assertHasOnlyCollscan();
}