TEST_F(QueryPlannerTest, NegatedRangeStrGT) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{i: {$not: {$gt: 'a'}}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {i:1}, "
        "bounds: {i: [['MinKey','a',true,true], "
        "[{},'MaxKey',true,true]]}}}}}");
}