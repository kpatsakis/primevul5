TEST_F(QueryPlannerTest, InCantUseHashedIndexWithRegex) {
    addIndex(BSON("a"
                  << "hashed"));
    runQuery(fromjson("{a: {$in: [/abc/]}}"));
    ASSERT_EQUALS(getNumSolutions(), 1U);
}