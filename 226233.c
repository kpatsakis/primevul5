TEST_F(QueryPlannerTest, EnumerateNestedOr2) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));
    addIndex(BSON("d" << 1));
    addIndex(BSON("e" << 1));
    addIndex(BSON("f" << 1));

    runQuery(fromjson("{a: 1, b: 1, $or: [{c: 1, d: 1}, {e: 1, f: 1}]}"));

    assertNumSolutions(6U);

    // Four possibilities from indexing the $or.
    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {d: 1}, node: {ixscan: {pattern: {c: 1}}}}},"
        "{fetch: {filter: {f: 1}, node: {ixscan: {pattern: {e: 1}}}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {c: 1}, node: {ixscan: {pattern: {d: 1}}}}},"
        "{fetch: {filter: {f: 1}, node: {ixscan: {pattern: {e: 1}}}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {d: 1}, node: {ixscan: {pattern: {c: 1}}}}},"
        "{fetch: {filter: {e: 1}, node: {ixscan: {pattern: {f: 1}}}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {c: 1}, node: {ixscan: {pattern: {d: 1}}}}},"
        "{fetch: {filter: {e: 1}, node: {ixscan: {pattern: {f: 1}}}}}"
        "]}}}}");

    // Two possibilties from outside the $or.
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {a: 1}}}}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {b: 1}}}}}");
}