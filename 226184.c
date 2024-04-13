TEST_F(QueryPlannerTest, MaxMinNoMatchingIndexDir) {
    addIndex(BSON("a" << -1));
    runInvalidQueryHintMinMax(BSONObj(), fromjson("{a: 2}"), BSONObj(), fromjson("{a: 8}"));
}