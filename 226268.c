TEST_F(QueryPlannerTest, NegationAndIndexOnInequality) {
    addIndex(BSON("b" << 1));
    runQuerySortProj(fromjson("{$and: [{a: 1}, {b: {$ne: 1}}]}"), BSONObj(), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {a:1}, node: {ixscan: {pattern: {b:1}, "
        "bounds: {b: [['MinKey',1,true,false], "
        "[1,'MaxKey',false,true]]}}}}}");
}