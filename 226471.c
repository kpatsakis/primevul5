TEST_F(QueryPlannerTest, NonTopLevelIndexedNegationMinQuery) {
    addIndex(BSON("state" << 1));
    addIndex(BSON("is_draft" << 1));
    addIndex(BSON("published_date" << 1));

    // This is the min query to reproduce SERVER-13714
    BSONObj queryObj = fromjson("{$or:[{state:1, is_draft:1}, {published_date:{$ne: 1}}]}");
    runQuery(queryObj);
}