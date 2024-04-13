DEATH_TEST(GteOp, InvalidEooOperand, "Invariant failure _rhs") {
    BSONObj operand;
    GTEMatchExpression gte("", operand.firstElement());
}