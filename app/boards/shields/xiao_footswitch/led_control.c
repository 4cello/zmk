/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

/**
 * LED Code is a slightly adapted version of ergodone's.
 * https://github.com/ergodone/zmk/tree/kinesisble/app/boards/shields/kinesisble
*/

#define LED_R_NODE DT_NODELABEL(ledr)
#define LED_G_NODE DT_NODELABEL(ledg)
#define LED_B_NODE DT_NODELABEL(ledb)

struct led {
    const struct device *gpio_dev;
    const char *gpio_dev_name;
    const char *gpio_pin_name;
    unsigned int gpio_pin;
    unsigned int gpio_flags;
};

void display_profile(uint8_t profile);

enum { LED_R, LED_G, LED_B };
struct led leds[] = {[LED_R] =
                         {
                             .gpio_dev = NULL,
                             .gpio_dev_name = DT_GPIO_LABEL(LED_R_NODE, gpios),
                             .gpio_pin_name = DT_LABEL(LED_R_NODE),
                             .gpio_pin = DT_GPIO_PIN(LED_R_NODE, gpios),
                             .gpio_flags = GPIO_OUTPUT | DT_GPIO_FLAGS(LED_R_NODE, gpios),
                         },
                     [LED_G] =
                         {
                             .gpio_dev = NULL,
                             .gpio_dev_name = DT_GPIO_LABEL(LED_G_NODE, gpios),
                             .gpio_pin_name = DT_LABEL(LED_G_NODE),
                             .gpio_pin = DT_GPIO_PIN(LED_G_NODE, gpios),
                             .gpio_flags = GPIO_OUTPUT | DT_GPIO_FLAGS(LED_G_NODE, gpios),
                         },
                     [LED_B] =
                         {
                             .gpio_dev = NULL,
                             .gpio_dev_name = DT_GPIO_LABEL(LED_B_NODE, gpios),
                             .gpio_pin_name = DT_LABEL(LED_B_NODE),
                             .gpio_pin = DT_GPIO_PIN(LED_B_NODE, gpios),
                             .gpio_flags = GPIO_OUTPUT | DT_GPIO_FLAGS(LED_B_NODE, gpios),
                         }};

static int led_init(const struct device *dev) {
    LOG_DBG("Initializing LEDs");
    for (int i = 0; i < (sizeof(leds) / sizeof(struct led)); i++) {
        leds[i].gpio_dev = device_get_binding(leds[i].gpio_dev_name);
        LOG_DBG("Initializing LED: %d: %s", i, leds[i].gpio_pin_name);
        if (leds[i].gpio_dev == NULL) {
            printk("Error: didn't find %s device\n", leds[i].gpio_dev_name);
            return -EIO;
        };

        int ret = gpio_pin_configure(leds[i].gpio_dev, leds[i].gpio_pin, leds[i].gpio_flags);
        if (ret != 0) {
            printk("Error %d: failed to configure pin %d '%s'\n", ret, leds[i].gpio_pin,
                   leds[i].gpio_pin_name);
            return -EIO;
        }
    }
    return 1;
}

SYS_INIT(led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static inline void ledON(const struct led *led) { gpio_pin_set(led->gpio_dev, led->gpio_pin, 1); }

static inline void ledOFF(const struct led *led) { gpio_pin_set(led->gpio_dev, led->gpio_pin, 0); }

static void led_all_OFF() {
    for (int i = 0; i < (sizeof(leds) / sizeof(struct led)); i++) {
        gpio_pin_set(leds[i].gpio_dev, leds[i].gpio_pin, 0);
    }
};

#define LEVEL_LED_SLEEP_PERIOD 1000

void display_profile(uint8_t profile) {
    switch (profile) {
        case 0:
            ledON(&leds[0]);
            break;
        case 1:
            ledON(&leds[1]);
            break;
        case 2:
            ledON(&leds[2]);
            break;
        case 3:
            ledON(&leds[0]);
            ledON(&leds[1]);
            break;
        case 4:
            ledON(&leds[1]);
            ledON(&leds[2]);
            break;
    }
    k_msleep(LEVEL_LED_SLEEP_PERIOD);
    led_all_OFF();
};