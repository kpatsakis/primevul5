TEST_F(QueryPlannerTest, MergeSortReverseScanOneIndexNotExplodeForSort) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("a" << -1 << "b" << -1));
    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {$or: [{a: 1, b: 1}, {a: {$lt: 0}}]}, sort: {a: -1}}"));

    assertNumSolutions(5U);
    assertSolutionExists(
        "{sort: {pattern: {a: -1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: -1, b: -1}, dir: 1}}, {ixscan: {pattern: {a: 1}, dir: -1}}]}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{fetch: {filter: {b: 1}, node: {ixscan: {pattern: {a: 1}, dir: -1}}}}, {ixscan: "
        "{pattern: {a: 1}, dir: -1}}]}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{fetch: {filter: {b: 1}, node: {ixscan: {pattern: {a: 1}, dir: -1}}}}, {ixscan: "
        "{pattern: {a: -1, b: -1}, dir: 1}}]}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: -1, b: -1}, dir: 1}}, {ixscan: {pattern: {a: -1, b: -1}, dir: "
        "1}}]}}}}");
}