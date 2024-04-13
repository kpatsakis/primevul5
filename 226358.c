TEST_F(QueryPlannerTest, ExprEqCannotUseMultikeyFieldOfIndex) {
    MultikeyPaths multikeyPaths{{0U}};
    addIndex(BSON("a.b" << 1), multikeyPaths);
    runQuery(fromjson("{'a.b': {$_internalExprEq: 1}}"));
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1, filter: {'a.b': {$_internalExprEq: 1}}}}");
}