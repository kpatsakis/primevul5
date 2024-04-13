TEST_F(QueryPlannerTest, NonTopLevelIndexedNegation) {
    addIndex(BSON("state" << 1));
    addIndex(BSON("is_draft" << 1));
    addIndex(BSON("published_date" << 1));
    addIndex(BSON("newsroom_id" << 1));

    BSONObj queryObj = fromjson(
        "{$and:[{$or:[{is_draft:false},{creator_id:1}]},"
        "{$or:[{state:3,is_draft:false},"
        "{published_date:{$ne:null}}]},"
        "{newsroom_id:{$in:[1]}}]}");
    runQuery(queryObj);
}