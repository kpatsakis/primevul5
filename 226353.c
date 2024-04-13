TEST_F(QueryPlannerTest, TwoNegatedRanges) {
    addIndex(BSON("i" << 1));
    runQuery(
        fromjson("{$and: [{i: {$not: {$lte: 'b'}}}, "
                 "{i: {$not: {$gte: 'f'}}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {i:1}, "
        "bounds: {i: [['MinKey','',true,false], "
        "['b','f',false,false], "
        "[{},'MaxKey',true,true]]}}}}}");
}