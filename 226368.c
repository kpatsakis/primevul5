TEST_F(QueryPlannerTest, ExprEqCanUseHashedIndexWithRegex) {
    params.options &= ~QueryPlannerParams::INCLUDE_COLLSCAN;
    addIndex(BSON("a"
                  << "hashed"));
    runQuery(fromjson("{a: {$_internalExprEq: /abc/}}"));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{fetch: {filter: {a: {$_internalExprEq: /abc/}}, node: {ixscan: {filter: null, pattern: "
        "{a: 'hashed'}}}}}");
}