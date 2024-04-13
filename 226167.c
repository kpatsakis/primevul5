TEST_F(QueryPlannerTest, HintInvalid) {
    addIndex(BSON("a" << 1));
    runInvalidQueryHint(BSONObj(), fromjson("{b: 1}"));
}