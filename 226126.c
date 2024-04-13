TEST_F(QueryPlannerTest, MaxWithoutIndex) {
    runInvalidQueryHintMinMax(BSONObj(), BSONObj(), BSONObj(), fromjson("{a: 1}"));
}