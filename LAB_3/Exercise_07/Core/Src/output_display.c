#include "output_display.h"

static uint8_t digits[4] = {0U, 0U, 0U, 1U};
static uint8_t scan_digit = 0U;

static void all_digits_off(void)
{
    HAL_GPIO_WritePin(
        EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET
    );
}

static void set_segments(uint8_t digit)
{
    /* Common anode: LOW = ON */
    static const uint8_t seg[10][7] = {
        {0,0,0,0,0,0,1}, /* 0 */
        {1,0,0,1,1,1,1}, /* 1 */
        {0,0,1,0,0,1,0}, /* 2 */
        {0,0,0,0,1,1,0}, /* 3 */
        {1,0,0,1,1,0,0}, /* 4 */
        {0,1,0,0,1,0,0}, /* 5 */
        {0,1,0,0,0,0,0}, /* 6 */
        {0,0,0,1,1,1,1}, /* 7 */
        {0,0,0,0,0,0,0}, /* 8 */
        {0,0,0,0,1,0,0}  /* 9 */
    };

    if (digit > 9U) {
        digit = 0U;
    }

    HAL_GPIO_WritePin(
        SEG_A_GPIO_Port, SEG_A_Pin,
        (GPIO_PinState)seg[digit][0]
    );

    HAL_GPIO_WritePin(
        SEG_B_GPIO_Port, SEG_B_Pin,
        (GPIO_PinState)seg[digit][1]
    );

    HAL_GPIO_WritePin(
        SEG_C_GPIO_Port, SEG_C_Pin,
        (GPIO_PinState)seg[digit][2]
    );

    HAL_GPIO_WritePin(
        SEG_D_GPIO_Port, SEG_D_Pin,
        (GPIO_PinState)seg[digit][3]
    );

    HAL_GPIO_WritePin(
        SEG_E_GPIO_Port, SEG_E_Pin,
        (GPIO_PinState)seg[digit][4]
    );

    HAL_GPIO_WritePin(
        SEG_F_GPIO_Port, SEG_F_Pin,
        (GPIO_PinState)seg[digit][5]
    );

    HAL_GPIO_WritePin(
        SEG_G_GPIO_Port, SEG_G_Pin,
        (GPIO_PinState)seg[digit][6]
    );
}

/* Display two numbers: 00..99 */
void display_show_values(uint8_t left, uint8_t right)
{
    if (left > 99U) left = 99U;
    if (right > 99U) right = 99U;

    digits[0] = left / 10U;
    digits[1] = left % 10U;

    digits[2] = right / 10U;
    digits[3] = right % 10U;
}

void display_init(void)
{
    all_digits_off();
    scan_digit = 0U;

    display_show_values(0U, 1U);
}

/* Scan one digit every call */
void display_scan_next(void)
{
    all_digits_off();

    set_segments(digits[scan_digit]);

    switch (scan_digit) {
        case 0:
            HAL_GPIO_WritePin(
                EN0_GPIO_Port, EN0_Pin,
                GPIO_PIN_RESET
            );
            break;

        case 1:
            HAL_GPIO_WritePin(
                EN1_GPIO_Port, EN1_Pin,
                GPIO_PIN_RESET
            );
            break;

        case 2:
            HAL_GPIO_WritePin(
                EN2_GPIO_Port, EN2_Pin,
                GPIO_PIN_RESET
            );
            break;

        default:
            HAL_GPIO_WritePin(
                EN3_GPIO_Port, EN3_Pin,
                GPIO_PIN_RESET
            );
            break;
    }

    scan_digit = (uint8_t)((scan_digit + 1U) % 4U);
}