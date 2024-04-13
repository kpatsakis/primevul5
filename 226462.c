TEST_F(QueryPlannerTest, MinMaxSameValue) {
    addIndex(BSON("a" << 1));
    runInvalidQueryHintMinMax(BSONObj(), BSONObj(), fromjson("{a: 1}"), fromjson("{a: 1}"));
}