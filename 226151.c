TEST_F(QueryPlannerTest, EqCanUseHashedIndexWithRegex) {
    addIndex(BSON("a"
                  << "hashed"));
    runQuery(fromjson("{a: {$eq: /abc/}}"));
    ASSERT_EQUALS(getNumSolutions(), 2U);
}