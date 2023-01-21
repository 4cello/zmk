#include <zephyr/bluetooth/bluetooth.h>

int startup_listener(const zmk_event_t *eh) {
    bt_set_appearance(0x04C3);
    bt_set_name("bli bla blub");
}