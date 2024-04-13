TEST_F(QueryPlannerTest, TooManyToExplodeOr) {
    addIndex(BSON("a" << 1 << "e" << 1));
    addIndex(BSON("b" << 1 << "e" << 1));
    addIndex(BSON("c" << 1 << "e" << 1));
    addIndex(BSON("d" << 1 << "e" << 1));
    runQuerySortProj(fromjson("{$or: [{a: {$in: [1,2,3,4,5,6]},"
                              "b: {$in: [1,2,3,4,5,6]}},"
                              "{c: {$in: [1,2,3,4,5,6]},"
                              "d: {$in: [1,2,3,4,5,6]}}]}"),
                     BSON("e" << 1),
                     BSONObj());

    // We cap the # of ixscans we're willing to create, so we don't get explosion. Instead
    // we get 5 different solutions which all use a blocking sort.
    assertNumSolutions(5U);
    assertSolutionExists(
        "{sort: {pattern: {e: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {e: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{or: {nodes: ["
        "{fetch: {node: {ixscan: {pattern: {a: 1, e: 1}}}}},"
        "{fetch: {node: {ixscan: {pattern: {c: 1, e: 1}}}}}]}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {e: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{or: {nodes: ["
        "{fetch: {node: {ixscan: {pattern: {b: 1, e: 1}}}}},"
        "{fetch: {node: {ixscan: {pattern: {c: 1, e: 1}}}}}]}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {e: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{or: {nodes: ["
        "{fetch: {node: {ixscan: {pattern: {a: 1, e: 1}}}}},"
        "{fetch: {node: {ixscan: {pattern: {d: 1, e: 1}}}}}]}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {e: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{or: {nodes: ["
        "{fetch: {node: {ixscan: {pattern: {b: 1, e: 1}}}}},"
        "{fetch: {node: {ixscan: {pattern: {d: 1, e: 1}}}}}]}}}}}}");
}