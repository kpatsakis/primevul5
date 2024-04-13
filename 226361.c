TEST_F(QueryPlannerTest, ExprEqCanUseSparseIndex) {
    params.options &= ~QueryPlannerParams::INCLUDE_COLLSCAN;
    addIndex(fromjson("{a: 1}"), false, true);
    runQuery(fromjson("{a: {$_internalExprEq: 1}}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1}, bounds: {a: [[1,1,true,true]]}}}}}");
}