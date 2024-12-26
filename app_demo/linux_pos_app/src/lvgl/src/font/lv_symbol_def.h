#ifndef LV_SYMBOL_DEF_H
#define LV_SYMBOL_DEF_H
/*clang-format off*/

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_conf_internal.h"

/*-------------------------------
 * Symbols from FontAwesome font
 *-----------------------------*/
//61452, 61453, 61457,61523, 61524,61560,61559,61724,62016,62017, 62018, 62019, 62020, 62087,62810,63650,61523,61524

/*In the font converter use this list as range:
    61441, 61448, 61451, 61452, 61453, 61457, 61459, 61461, 61465, 61468,
    61473, 61478, 61479, 61480, 61502, 61512, 61515, 61516, 61517, 61521,
    61522, 61523, 61524, 61543, 61544, 61550, 61552, 61553, 61556, 61559,
    61560, 61561, 61563, 61587, 61589, 61636, 61637, 61639, 61671, 61674,
    61683, 61724, 61732, 61787, 61931, 62016, 62017, 62018, 62019, 62020,
    62087, 62099, 62212, 62189, 62810, 63426, 63650
*/

#define LV_SYMBOL_OK              "\xef\x80\x8c" /*61452, 0xF00C*/
#define LV_SYMBOL_CLOSE           "\xef\x80\x8d" /*61453, 0xF00D*/
#define LV_SYMBOL_POWER           "\xef\x80\x91" /*61457, 0xF011*/
#define LV_SYMBOL_LEFT            "\xef\x81\x93" /*61523, 0xF053*/
#define LV_SYMBOL_RIGHT           "\xef\x81\x94" /*61524, 0xF054*/
#define LV_SYMBOL_UP              "\xef\x81\xb7" /*61559, 0xF077*/
#define LV_SYMBOL_DOWN            "\xef\x81\xb8" /*61560, 0xF078*/
#define LV_SYMBOL_KEYBOARD        "\xEE\xA4\x85" /*59653, 0xe905*/
#define LV_SYMBOL_BATTERY_FULL    "\xef\x89\x80" /*62016, 0xF240*/
#define LV_SYMBOL_BATTERY_3       "\xef\x89\x81" /*62017, 0xF241*/
#define LV_SYMBOL_BATTERY_2       "\xef\x89\x82" /*62018, 0xF242*/
#define LV_SYMBOL_BATTERY_1       "\xef\x89\x83" /*62019, 0xF243*/
#define LV_SYMBOL_BATTERY_EMPTY   "\xef\x89\x84" /*62020, 0xF244*/
#define LV_SYMBOL_USB             "\xef\x8a\x87" /*62087, 0xF287*/
#define LV_SYMBOL_BACKSPACE       "\xef\x95\x9A" /*62810, 0xF55A*/
#define LV_SYMBOL_NEW_LINE        "\xef\xA2\xA2" /*63650, 0xF8A2*/

#define LV_SYMBOL_GPRS_1            "\xEE\xA9\x92" /*61458, 0xEa52*/
#define LV_SYMBOL_GPRS_2            "\xEE\xA9\x94" /*61458, 0xEa54*/
#define LV_SYMBOL_GPRS_3            "\xEE\xA9\x95" /*61458, 0xEa55*/
#define LV_SYMBOL_GPRS_4            "\xEE\xA4\x80" /*61458, 0xE900*/
#define LV_SYMBOL_GPRS_5            "\xEE\xA9\x93" /*61458, 0xEa53*/


#define LV_SYMBOL_WIFI_1            "\xEE\xA4\x83" /*61931, 0xe903*/
#define LV_SYMBOL_WIFI_4            "\xEE\xA4\x80" /*61931, 0xe900*/
#define LV_SYMBOL_WIFI_3            "\xEE\xA4\x81" /*61931, 0xe901*/
#define LV_SYMBOL_WIFI_2            "\xEE\xA4\x82" /*61931, 0xe902*/
/** Invalid symbol at (U+F8FF). If written before a string then `lv_img` will show it as a label*/


#define LV_SYMBOL_DUMMY           "\xEF\xA3\xBF"

/*-------------------------------
 * Symbols from "normal" font
 *-----------------------------*/
#define LV_SYMBOL_BULLET          "\xE2\x80\xA2" /*20042, 0x2022*/

/*
 * The following list is generated using
 * cat src/font/lv_symbol_def.h | sed -E -n 's/^#define\s+LV_(SYMBOL_\w+).*".*$/    _LV_STR_\1,/p'
 */
enum {
    _LV_STR_SYMBOL_OK,
    _LV_STR_SYMBOL_CLOSE,
    _LV_STR_SYMBOL_POWER,
    _LV_STR_SYMBOL_LEFT,
    _LV_STR_SYMBOL_RIGHT,
    _LV_STR_SYMBOL_UP,
    _LV_STR_SYMBOL_DOWN,
    _LV_STR_SYMBOL_KEYBOARD,
    _LV_STR_SYMBOL_BATTERY_FULL,
    _LV_STR_SYMBOL_BATTERY_3,
    _LV_STR_SYMBOL_BATTERY_2,
    _LV_STR_SYMBOL_BATTERY_1,
    _LV_STR_SYMBOL_BATTERY_EMPTY,
    _LV_STR_SYMBOL_USB,
    _LV_STR_SYMBOL_BACKSPACE,
    _LV_STR_SYMBOL_NEW_LINE,
    _LV_STR_SYMBOL_DUMMY,
    _LV_STR_SYMBOL_BULLET,
};

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYMBOL_DEF_H*/
