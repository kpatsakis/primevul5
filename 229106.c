TEST_F(QueryPlannerTest, ElemMatchObjectNegationEqArray) {
    addIndex(BSON("i.j" << 1));
    runQuery(fromjson("{i: {$elemMatch: {j: {$ne: [1]}}}}"));
    assertHasOnlyCollscan();
}