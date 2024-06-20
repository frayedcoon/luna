#include "app/terminal.h"
#include "driver/pcd8544.h"
#include "lib/string.h"

#define LCD_CMD_PRINT  "print"
#define LCD_CMD_DRAW   "draw"
#define LCD_CMD_CLEAR  "clear"
#define LCD_CMD_INIT   "init"

#define LCD_IMAGE_PONY "pony"

/**
 * lcd driver configuration
 */
static const pcd8544_config lcd_config = {
    .port                    = PCD8544_PORT,

    .rst_pin                 = PCD8544_PIN_RST,
    .ce_pin                  = PCD8544_PIN_CE,
    .dc_pin                  = PCD8544_PIN_DC,
    .din_pin                 = PCD8544_PIN_DIN,
    .clk_pin                 = PCD8544_PIN_CLK,

    .contrast                = 60,
    .mux_rate                = PCD8544_MUX_RATE_HUNDRED,
    .display_mode            = PCD8544_DISPLAY_MODE_NORMAL
};

/**
 * pony bitmap for pcd8544 lcd display
 */
static const uint8_t pony[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0xc0, 0x60, 0x74, 0xf4, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe,
    0xfc, 0xf8, 0xe0, 0xe0, 0x30, 0x60, 0xc0, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x31, 0x3c, 0x1f, 0xcf,
    0x4f, 0x7f, 0x03, 0x03, 0x03, 0x01, 0xf9, 0xe9,
    0x0b, 0xfb, 0x17, 0x01, 0x01, 0x00, 0x03, 0x81,
    0xec, 0xe7, 0xf9, 0x0f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0xc0, 0xc0, 0xe0, 0xc0, 0xc0,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x04, 0x0a,
    0x0a, 0x08, 0x11, 0xf1, 0x11, 0x11, 0x10, 0x18,
    0x38, 0x1c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xf8,
    0x20, 0x20, 0x20, 0x20, 0x30, 0x10, 0x18, 0x08,
    0x18, 0x18, 0x7c, 0xfe, 0x3e, 0x3f, 0x7f, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
    0xfc, 0xf0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x80,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
    0x38, 0x20, 0xc0, 0x80, 0x80, 0xc3, 0x0f, 0x1f,
    0x12, 0x13, 0xb7, 0x23, 0x20, 0x20, 0xa0, 0x30,
    0x18, 0x08, 0x0c, 0x8e, 0x80, 0xe0, 0x30, 0x19,
    0x0f, 0x00, 0x00, 0x01, 0x01, 0x07, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf,
    0x1f, 0x1f, 0x1f, 0x1f, 0x07, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff,
    0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01,
    0x01, 0x1f, 0xf1, 0x0f, 0x78, 0xc0, 0x01, 0x03,
    0x03, 0x06, 0x1c, 0x70, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x1f, 0x7f, 0xff, 0xff, 0x03,
    0x07, 0x0f, 0x1f, 0x3f, 0x3e, 0x3c, 0x1c, 0x1c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x1e, 0x73, 0x79, 0x4c, 0x46, 0x43, 0x40, 0x40,
    0x40, 0x7e, 0x03, 0x00, 0x1c, 0x1f, 0x11, 0x70,
    0x7c, 0x47, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7c,
    0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static int lcd_cmd_handler(list_ifc *args) {
    int size = args->size(args);
    if (size > 3 || size < 2) {
        return -1;
    }

    pcd8544_iface *lcd_iface = pcd8544_iface_get();

    const list_node *node = args->get_front(args)->nxt;

    if (strcmp((char*) node->ptr, LCD_CMD_PRINT) == 0) {
        if (!lcd_iface) {
            return -1;
        }
        if (size == 3) {
            node = node->nxt;
            lcd_iface->puts(lcd_iface, (char*) node->ptr);
            return 0;
        }
    } else if (strcmp((char*) node->ptr, LCD_CMD_DRAW) == 0) {
        if (!lcd_iface) {
            return -1;
        }
        if (size == 3) {
            node = node->nxt;
            if (strcmp((char*) node->ptr, LCD_IMAGE_PONY) == 0) {
                lcd_iface->draw(lcd_iface, pony, sizeof(pony)/sizeof(pony[0]));
                return 0;
            }
        }
    } else if (strcmp((char*) node->ptr, LCD_CMD_CLEAR) == 0) {
        if (!lcd_iface) {
            return -1;
        }
        if (size == 2) {
            lcd_iface->clear(lcd_iface);
            return 0;
        }
    } else if (strcmp((char*) node->ptr, LCD_CMD_INIT) == 0) {
        if (size == 2) {
            pcd8544_init(&lcd_config);
            return 0;
        }
    }

    return -1;
}

TERMINAL_CMD(lcd, lcd_cmd_handler);