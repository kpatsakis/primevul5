TEST_F(QueryPlannerTest, OrOnlyOneBranchCanUseIndexHinted) {
    addIndex(BSON("a" << 1));
    runQueryHint(fromjson("{$or: [{a:1}, {b:2}]}"), fromjson("{a:1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: {$or:[{a:1},{b:2}]}, node: {ixscan: "
        "{pattern: {a:1}, bounds: "
        "{a: [['MinKey','MaxKey',true,true]]}}}}}");
}