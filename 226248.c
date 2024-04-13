TEST_F(QueryPlannerTest, NoKeepWithMergeSort) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;

    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProj(fromjson("{a: {$in: [1, 2]}}"), BSON("b" << 1), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {mergeSort: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1},"
        "bounds: {a: [[1,1,true,true]], b: [['MinKey','MaxKey',true,true]]}}},"
        "{ixscan: {pattern: {a: 1, b: 1},"
        "bounds: {a: [[2,2,true,true]], b: [['MinKey','MaxKey',true,true]]}}}]}}}}");
}