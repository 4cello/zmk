#include <logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/layer_state_changed.h>

void display_profile(uint8_t value);

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

int ble_event_listener(const zmk_event_t *eh) {
    struct zmk_ble_active_profile_changed *ev = as_zmk_ble_active_profile_changed(eh);
    if (ev == NULL) {
        return -ENOTSUP;
    }
    LOG_DBG("Setting BLE profile: %d", ev->index);
    display_profile(ev->index);
    return 0;
}

ZMK_LISTENER(ble, ble_event_listener);
ZMK_SUBSCRIPTION(ble, zmk_ble_active_profile_changed);
