TEST_F(QueryPlannerTest, MinWithoutIndex) {
    runInvalidQueryHintMinMax(BSONObj(), BSONObj(), fromjson("{a: 1}"), BSONObj());
}