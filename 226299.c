TEST_F(QueryPlannerTest, NegationIndexForSort) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: {$ne: 1}}"), fromjson("{a: 1}"), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1}, "
        "bounds: {a: [['MinKey',1,true,false], "
        "[1,'MaxKey',false,true]]}}}}}");
}