TEST_F(QueryPlannerTest, NegationTopLevel) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: {$ne: 1}}"), BSONObj(), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [['MinKey',1,true,false], "
        "[1,'MaxKey',false,true]]}}}}}");
}