TEST_F(QueryPlannerTest, OrElemMatchObjectBeneathAnd) {
    // true means multikey
    addIndex(BSON("a.b" << 1), true);
    runQuery(
        fromjson("{$or: [{'a.b': 0, a: {$elemMatch: {b: {$lte: 1}}}},"
                 "{a: {$elemMatch: {b: {$gte: 4}}}}]}"));

    assertNumSolutions(3U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {$and:[{a:{$elemMatch:{b:{$lte:1}}}},{'a.b':0}]},"
        "node: {ixscan: {filter: null, pattern: {'a.b': 1}, "
        "bounds: {'a.b': [[-Infinity,1,true,true]]}}}}},"
        "{fetch: {filter: {a:{$elemMatch:{b:{$gte:4}}}}, node: "
        "{ixscan: {filter: null, pattern: {'a.b': 1},"
        "bounds: {'a.b': [[4,Infinity,true,true]]}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {a:{$elemMatch:{b:{$lte:1}}}},"
        "node: {ixscan: {filter: null, pattern: {'a.b': 1}, "
        "bounds: {'a.b': [[0,0,true,true]]}}}}},"
        "{fetch: {filter: {a:{$elemMatch:{b:{$gte:4}}}}, node: "
        "{ixscan: {filter: null, pattern: {'a.b': 1},"
        "bounds: {'a.b': [[4,Infinity,true,true]]}}}}}]}}");
}