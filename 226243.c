TEST_F(QueryPlannerTest, MinBadHint) {
    addIndex(BSON("b" << 1));
    runInvalidQueryHintMinMax(BSONObj(), fromjson("{b: 1}"), fromjson("{a: 1}"), BSONObj());
}