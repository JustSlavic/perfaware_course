#define IEEE754_32BIT_MANTISSA_MASK 0x7fffff
#define IEEE754_32BIT_MANTISSA_NBITS 23
#define IEEE754_32BIT_MANTISSA_SHIFT 0

#define IEEE754_32BIT_EXPONENT_MASK 0xff
#define IEEE754_32BIT_EXPONENT_NBITS 8
#define IEEE754_32BIT_EXPONENT_SHIFT 23

#define IEEE754_32BIT_SIGN_BIT_MASK 0x1
#define IEEE754_32BIT_SIGN_BIT_NBITS 1
#define IEEE754_32BIT_SIGN_BIT_SHIFT 31

#define IEEE754_32BIT_GET_SIGN_BIT(x) (((x) >> IEEE754_32BIT_SIGN_BIT_SHIFT) & IEEE754_32BIT_SIGN_BIT_MASK)
#define IEEE754_32BIT_GET_EXPONENT(x) (((x) >> IEEE754_32BIT_EXPONENT_SHIFT) & IEEE754_32BIT_EXPONENT_MASK)
#define IEEE754_32BIT_GET_MANTISSA(x) ((x) & IEEE754_32BIT_MANTISSA_MASK)

static int8 digit_lut[29][29] =
{
    { 5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,3,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,1,5,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,7,8,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,3,9,0,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,1,9,5,3,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,9,7,6,5,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,4,8,8,2,8,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,2,4,4,1,4,0,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,1,2,2,0,7,0,3,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,6,1,0,3,5,1,5,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,3,0,5,1,7,5,7,8,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,1,5,2,5,8,7,8,9,0,6,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,7,6,2,9,3,9,4,5,3,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,3,8,1,4,6,9,7,2,6,5,6,2,5,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,1,9,0,7,3,4,8,6,3,2,8,1,2,5,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,9,5,3,6,7,4,3,1,6,4,0,6,2,5,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,4,7,6,8,3,7,1,5,8,2,0,3,1,2,5,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,2,3,8,4,1,8,5,7,9,1,0,1,5,6,2,5,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,1,1,9,2,0,9,2,8,9,5,5,0,7,8,1,2,5,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,5,9,6,0,4,6,4,4,7,7,5,3,9,0,6,2,5,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,2,9,8,0,2,3,2,2,3,8,7,6,9,5,3,1,2,5,0,0,0,0 },
    { 0,0,0,0,0,0,0,1,4,9,0,1,1,6,1,1,9,3,8,4,7,6,5,6,2,5,0,0,0 },
    { 0,0,0,0,0,0,0,0,7,4,5,0,5,8,0,5,9,6,9,2,3,8,2,8,1,2,5,0,0 },
    { 0,0,0,0,0,0,0,0,3,7,2,5,2,9,0,2,9,8,4,6,1,9,1,4,0,6,2,5,0 },
    { 0,0,0,0,0,0,0,0,1,8,6,2,6,4,5,1,4,9,2,3,0,9,5,7,0,3,1,2,5 },
};

void print_binary(char *memory, uint32 nbytes)
{
    for (uint32 i = nbytes - 1; i < nbytes; i--)
    {
        uint32 byte = memory[i];
        uint32 mask = 0x80;
        while (mask)
        {
            printf("%d", (byte & mask) > 0);
            mask = mask >> 1;
        }
        printf(" ");
    }
}

void ieee754_32bit(float x)
{
    uint32 input_bits;
    memcpy((void *) &input_bits, (void *) &x, 4);

    uint32 S = IEEE754_32BIT_GET_SIGN_BIT(input_bits);
    uint32 E = IEEE754_32BIT_GET_EXPONENT(input_bits);
    uint32 M = IEEE754_32BIT_GET_MANTISSA(input_bits);
    int32 exponent = E - 127;
    int32 integral_part = 0;
    if (exponent > 0)
    {
        integral_part = (input_bits >> (23 - exponent)) & ((1 << exponent) - 1);
        integral_part = integral_part | (1 << exponent); // implicit 1
    }

    int32 i = 0;
    uint32 mask = 1;
    uint32 fractional_digits[ARRAY_COUNT(digit_lut)] = {};
    uint32 fractional_bit_max = 23 - (exponent > 0 ? exponent : 0);
    while (mask < (1 << fractional_bit_max))
    {
        uint32 bit = (input_bits & mask) > 0;
        if (bit)
        {
            uint32 lut_index = 23 - exponent - i - 1;
            if (0 <= lut_index && lut_index < ARRAY_COUNT(digit_lut))
            {
                for (int j = 0; j < ARRAY_COUNT(fractional_digits); j++)
                {
                    fractional_digits[j] += digit_lut[lut_index][j];
                }
            }
            else
            {
                printf("Error: I was trying to get row (index=%d) in the lut table.\n", lut_index);
                printf("       You should expand the lut table to be able to print higher exponent floating values.\n");
                printf("       Current printing number: ");
                print_binary((void *) &input_bits, 4);
                printf("\n");
                printf("       Exponent = %d\n", exponent);
            }
        }
        mask = mask << 1;
        i++;
    }
    if (exponent < 0)
    {
        uint32 lut_index = -exponent-1;
        if (0 <= lut_index && lut_index < 23)
        {
            for (int j = 0; j < ARRAY_COUNT(fractional_digits); j++)
            {
                fractional_digits[j] += digit_lut[lut_index][j];
            }
        }
        else
        {
            printf("Error: I was trying to get row (index=%d) in the lut table.\n", lut_index);
            printf("       You should expand the lut table to be able to print higher exponent floating values.\n");
            printf("       Current printing number: ");
            print_binary((void *) &input_bits, 4);
            printf("\n");
            printf("       Exponent = %d\n", exponent);
        }
    }

    for (int j = ARRAY_COUNT(fractional_digits) - 1; j > 1; j--)
    {
        fractional_digits[j - 1] += fractional_digits[j] / 10;
        fractional_digits[j] = fractional_digits[j] % 10;
    }

    printf("Final result: ");
    if (S > 0) printf("-");
    printf("%d", integral_part);
    printf(".");
    for (int j = 0; j < ARRAY_COUNT(fractional_digits); j++)
    {
        printf("%d", fractional_digits[j]);
    }
    printf("\n");
}

int main()
{
    float x = -0.000000000000000000000000000000000000000000001f;
    ieee754_32bit(x);

    // printf("%f\n", x);
    printf("printf:       %20.150f\n", x);
    // ieee754_32bit(0.5f + 0.0625f);
    return 0;
}
