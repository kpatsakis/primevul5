TEST_F(QueryPlannerTest, ExprEqCanUseHashedIndex) {
    params.options &= ~QueryPlannerParams::INCLUDE_COLLSCAN;
    addIndex(BSON("a"
                  << "hashed"));
    runQuery(fromjson("{a: {$_internalExprEq: 1}}"));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{fetch: {filter: {a: {$_internalExprEq: 1}}, node: {ixscan: {filter: null, pattern: {a: "
        "'hashed'}}}}}");
}