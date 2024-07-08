// To do: Declare this variable if you want use the battery percentage
//static int g_BattPercetInterval = LIMIT_BATT_PERCENT_INERVAL;
#include "appinc.h"

#define BAT_DISP_BLOCK_H (10)
#define BAT_DISP_BLOCK_W (7)

#define BAT_DISP_BORDER_H (BAT_DISP_BLOCK_H)
#define BAT_DISP_BORDER_W (4*BAT_DISP_BLOCK_W)

#define BAT_DISP_START_X (320 - 3 - 5 - BAT_DISP_BORDER_W - 2 )
#define BAT_DISP_START_Y (2)

#define BAT_DISP_END_X (BAT_DISP_START_X + 2 + BAT_DISP_BORDER_W)
#define BAT_DISP_END_Y (BAT_DISP_START_Y + 2 + BAT_DISP_BLOCK_H)

static int glb_battry_charge_flag = 0;
int glb_bat_value = 0;

void ShowBattery_Border(DPORT_COLOR_T color)
{
    POINT_T tBegin;
    int i = 0;
    unsigned char disp_char[BAT_DISP_BLOCK_W * BAT_DISP_BLOCK_H * 2 + 1] = { 0 };
    short color_n = 0x00;
    switch (color)
    {
        case EM_COLOR_RED:
            color_n = 0xF800;
            break;
        case EM_COLOR_BLACK:
            color_n = 0x0000;
            break;
        default:
            break;
    }

    // memset(disp_char, 0xFF, sizeof(disp_char));
    // tBegin.X = BAT_DISP_START_X;
    // tBegin.Y = BAT_DISP_START_Y;
    // Disp_vShowImage(disp_char, BAT_DISP_BORDER_W * BAT_DISP_BORDER_H * 2, tBegin, BAT_DISP_BORDER_W, BAT_DISP_BORDER_H);

    memset(disp_char, 0xFF, sizeof(disp_char));
    for (i = 0;i < sizeof(disp_char);)
    {
        disp_char[i] = color_n & 0xFF;
        disp_char[i + 1] = (color_n>>8) & 0xFF;
        i += 2;
    }

    #if 1
    tBegin.X = BAT_DISP_START_X;
    tBegin.Y = BAT_DISP_START_Y;
    Disp_vShowImage(disp_char, 2 * (BAT_DISP_BORDER_W+4) * 2, tBegin, BAT_DISP_BORDER_W+4, 2);

    tBegin.X = BAT_DISP_START_X;
    tBegin.Y = BAT_DISP_END_Y;
    Disp_vShowImage(disp_char, 2 * (BAT_DISP_BORDER_W+4) * 2, tBegin, BAT_DISP_BORDER_W+4, 2);

    /*****************/
    tBegin.X = BAT_DISP_END_X + 2;
    tBegin.Y = 5;
    Disp_vShowImage(disp_char, 4 * 8 * 2, tBegin, 4, 8);
    /*****************/

    tBegin.X = BAT_DISP_START_X;
    tBegin.Y = BAT_DISP_START_Y;
    Disp_vShowImage(disp_char, 2 * (BAT_DISP_BORDER_H+4) * 2, tBegin, 2, BAT_DISP_BORDER_H+4);

    tBegin.X = BAT_DISP_END_X;
    tBegin.Y = BAT_DISP_START_Y;
    Disp_vShowImage(disp_char, 2 * (BAT_DISP_BORDER_H+4)  * 2, tBegin, 2, BAT_DISP_BORDER_H+4);

    #endif
}


void ShowBattery_ClearPercent(int p)
{
    POINT_T tBegin;
    unsigned char disp_char[BAT_DISP_BORDER_W * BAT_DISP_BLOCK_H * 2 + 1] = { 0 };

    p = p > BAT_DISP_BORDER_W ? BAT_DISP_BORDER_W : p;
    memset(disp_char, 0xFF, sizeof(disp_char));
    tBegin.X = BAT_DISP_START_X + 2 + p;
    tBegin.Y = BAT_DISP_START_Y+2;
    
    Disp_vShowImage(disp_char, BAT_DISP_BORDER_W * BAT_DISP_BORDER_H * 2, tBegin, BAT_DISP_BORDER_W-p, BAT_DISP_BORDER_H);
}


void ShowBattery_Percent(int p)
{
    POINT_T tBegin;
    unsigned char disp_char[BAT_DISP_BORDER_H * BAT_DISP_BORDER_W * 2 + 1] = { 0 };
    int w = 0, y = 0;

    p = p > BAT_DISP_BORDER_W ? BAT_DISP_BORDER_W : p;
    w = p;
    y = BAT_DISP_BORDER_H;
    tBegin.X = BAT_DISP_START_X + 2;
    tBegin.Y = BAT_DISP_START_Y + 2;
    
    memset(disp_char, 0x00, sizeof(disp_char));
    Disp_vShowImage(disp_char, w * y * 2, tBegin, w, y);
}

int Get_Battry_Percent(void)
{
    int percent = OsGetBatCap();
    //OsLog(LOG_ERROR, "Dspread: Get_Battry_Percent = %d", percent);
    return percent;
}

void ShowBattery(int lvl)
{
    POINT_T tBegin;
    int i = 0, w = 0, y = 0;
    unsigned char disp_char[BAT_DISP_BORDER_H * BAT_DISP_BORDER_W * 2 + 1] = { 0 };

    memset(disp_char, 0x00, sizeof(disp_char));

    if (lvl == BATTERY_LEVEL_0)
    {
        for (i = 0;i < sizeof(disp_char);)
        {
            disp_char[i] = 0x00;
            disp_char[i + 1] = 0xF8;//red color
            i += 2;
        }
        ShowBattery_Border(EM_COLOR_BLACK);
        tBegin.X = BAT_DISP_START_X + 2;
        tBegin.Y = BAT_DISP_START_Y+2+1;
        Disp_vShowImage(disp_char, 2*BAT_DISP_BLOCK_H*2, tBegin, 2, BAT_DISP_BLOCK_H);
    }
    else if (lvl == BATTERY_LEVEL_ABSENT)
    {
        ShowBattery_Border(EM_COLOR_RED);
        ShowBattery_Percent(0);       
    }
    else if (lvl == BATTERY_LEVEL_CHARGE)
    {
        if (glb_battry_charge_flag == 0)
        {
            glb_bat_value = Get_Battry_Percent() * BAT_DISP_BORDER_W / 100;
        }        

        ShowBattery_Border(EM_COLOR_BLACK);
        ShowBattery_Percent(glb_bat_value);
        glb_bat_value++;
        if (glb_bat_value > BAT_DISP_BORDER_W)
        {
            glb_bat_value = Get_Battry_Percent() * BAT_DISP_BORDER_W / 100;
            ShowBattery_ClearPercent(glb_bat_value);
        }
        glb_battry_charge_flag = 1;
        return;
    }
    else if (lvl == BATTERY_LEVEL_COMPLETE)
    {
        memset(disp_char, 0x00, sizeof(disp_char));
        tBegin.X = BAT_DISP_START_X+2;
        tBegin.Y = BAT_DISP_START_Y+2;
        Disp_vShowImage(disp_char, BAT_DISP_BORDER_W * BAT_DISP_BORDER_H * 2, tBegin, BAT_DISP_BORDER_W, BAT_DISP_BORDER_H);
    }
    else
    {
        
        ShowBattery_Border(EM_COLOR_BLACK);
        //ShowBattery_ClearPercent(BAT_DISP_BORDER_W);
        ShowBattery_Percent(0);
        ShowBattery_Percent(Get_Battry_Percent() * BAT_DISP_BORDER_W / 100);
    }

    glb_battry_charge_flag = 0;

}

/*----------------------------------------------------------------------------
|   Function Name:
|       CheckBattery
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void CheckBattery(void)
{
    int bat_val = OsCheckBattery();

    //bat_val = BATTERY_LEVEL_ABSENT;
    //OsLog(LOG_ERROR, "CheckBattery retvalue %d", bat_val);

    ShowBattery(bat_val);

    return;
}