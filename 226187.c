TEST_F(QueryPlannerTest, ExprEqCanUseIndex) {
    params.options &= ~QueryPlannerParams::INCLUDE_COLLSCAN;
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{a: {$_internalExprEq: 1}}"));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a: 1}, bounds: {a: "
        "[[1,1,true,true]]}}}}}");
}