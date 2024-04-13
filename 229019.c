TEST_F(QueryPlannerTest, NegatedElemMatchObjectEqArray) {
    addIndex(BSON("i.j" << 1));
    runQuery(fromjson("{i: {$not: {$elemMatch: {j: [1]}}}}"));
    assertHasOnlyCollscan();
}