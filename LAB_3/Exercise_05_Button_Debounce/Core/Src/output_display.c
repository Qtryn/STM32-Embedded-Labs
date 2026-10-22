#include "main.h"
#include "output_display.h"

void display7SEG(uint8_t num)
{
    static const uint8_t seg[10][7] =
    {
        /* A B C D E F G */
        {0,0,0,0,0,0,1},   // 0
        {1,0,0,1,1,1,1},   // 1
        {0,0,1,0,0,1,0},   // 2
        {0,0,0,0,1,1,0},   // 3
        {1,0,0,1,1,0,0},   // 4
        {0,1,0,0,1,0,0},   // 5
        {0,1,0,0,0,0,0},   // 6
        {0,0,0,1,1,1,1},   // 7
        {0,0,0,0,0,0,0},   // 8
        {0,0,0,0,1,0,0}    // 9
    };

    if (num > 9)
    {
        num = 0;
    }

    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, seg[num][0]);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, seg[num][1]);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, seg[num][2]);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, seg[num][3]);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, seg[num][4]);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, seg[num][5]);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, seg[num][6]);
}

void turnOffAll7SEG(void)
{
    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET);
}