bool Expression::isExpressionName(StringData name) {
    return parserMap.find(name) != parserMap.end();
}