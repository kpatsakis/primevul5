TEST_F(QueryPlannerTest, MaxBadHint) {
    addIndex(BSON("b" << 1));
    runInvalidQueryHintMinMax(BSONObj(), fromjson("{b: 1}"), BSONObj(), fromjson("{a: 1}"));
}