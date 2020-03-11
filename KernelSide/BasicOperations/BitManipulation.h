#pragma once

#define SET_BIT(input, bitPosition) *input = *input | (1UL << bitPosition);

#define CLEAR_BIT(input, bitPosition) *input = *input & ~(1UL << bitPosition);

#define GET_BIT(input, bitPosition) (input >> bitPosition) & 1;