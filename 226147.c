TEST_F(QueryPlannerTest, SolutionSetStableWhenOrEnumerationLimitIsReached) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    addIndex(BSON("d" << 1));
    addIndex(BSON("e" << 1));
    addIndex(BSON("f" << 1));
    addIndex(BSON("f" << 1 << "y" << 1));
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));
    addIndex(BSON("c" << 1 << "x" << 1));

    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {$or: [{a: 1, b: 1, c: 1}, {d: 1, e: 1, f: 1}]}}"));

    assertNumSolutions(10U);

    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {b: {$eq: 1}, c: {$eq: 1} }, node: {ixscan: {pattern: {a: "
        "1}}}}}, {fetch: {filter: {e: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {d: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, c: {$eq: 1} }, node: {ixscan: {pattern: {b: "
        "1}}}}}, {fetch: {filter: {e: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {d: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, b: {$eq: 1} }, node: {ixscan: {pattern: {c: "
        "1}}}}}, {fetch: {filter: {e: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {d: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, b: {$eq: 1} }, node: {ixscan: {pattern: {c: "
        "1, x: 1}}}}}, {fetch: {filter: {e: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {d: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {b: {$eq: 1}, c: {$eq: 1} }, node: {ixscan: {pattern: {a: "
        "1}}}}}, {fetch: {filter: {d: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {e: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, c: {$eq: 1} }, node: {ixscan: {pattern: {b: "
        "1}}}}}, {fetch: {filter: {d: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {e: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, b: {$eq: 1} }, node: {ixscan: {pattern: {c: "
        "1}}}}}, {fetch: {filter: {d: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {e: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, b: {$eq: 1} }, node: {ixscan: {pattern: {c: "
        "1, x: 1}}}}}, {fetch: {filter: {d: {$eq: 1}, f: {$eq: 1} }, node: {ixscan: {pattern: {e: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {b: {$eq: 1}, c: {$eq: 1} }, node: {ixscan: {pattern: {a: "
        "1}}}}}, {fetch: {filter: {d: {$eq: 1}, e: {$eq: 1} }, node: {ixscan: {pattern: {f: "
        "1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {a: {$eq: 1}, c: {$eq: 1} }, node: {ixscan: {pattern: {b: "
        "1}}}}}, {fetch: {filter: {d: {$eq: 1}, e: {$eq: 1} }, node: {ixscan: {pattern: {f: "
        "1}}}}}]}}");
}