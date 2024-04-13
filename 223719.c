double ExpressionRandom::getRandomValue() const {
    return kMinValue + (kMaxValue - kMinValue) * threadLocalRNG.nextCanonicalDouble();
}