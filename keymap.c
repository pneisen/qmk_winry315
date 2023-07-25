// Custom QMK firmware keymap for the winry315 macro pad. 

#include QMK_KEYBOARD_H

// Defines names for use in layer keycodes and the keymap
enum layer_names {
  _BASE,
  _SOUNDS,
  _OTHER,
};

enum keycodes {
  KC_CYCLE_LAYERS = QK_USER,
};

#define LAYER_CYCLE_START 0
#define LAYER_CYCLE_END 2

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // Base
  [_BASE] = LAYOUT_top(
                       KC_F15,      KC_MUTE,     KC_F18,
      KC_F19,          KC_F20,      KC_F21,      KC_F22,      KC_F23,
      KC_F24,          LCTL(KC_F1), LCTL(KC_F2), LCTL(KC_F3), LCTL(KC_F4),
      KC_CYCLE_LAYERS, LCTL(KC_F5), KC_SLEP,     LCTL(KC_F7), LCTL(KC_F8)
      ),

  // Soundboard configuration
  [_SOUNDS] = LAYOUT_top(
                    KC_TRNS,      KC_TRNS,      KC_TRNS,
      LCTL(KC_F9),  LCTL(KC_F10), LCTL(KC_F11), LCTL(KC_F12), LCTL(KC_F13),
      LCTL(KC_F14), LCTL(KC_F15), LCTL(KC_F16), LCTL(KC_F18), LCTL(KC_F19),
      KC_TRNS,      LCTL(KC_F20), LCTL(KC_F21), LCTL(KC_F22), LCTL(KC_F23)
      ),

  // Other configuration
  [_OTHER] = LAYOUT_top(
               KC_TRNS, KC_TRNS, KC_TRNS,
      QK_BOOT, QK_RBT,  KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
      ),
};
// clang-format on

void keyboard_post_init_user(void) {
  // Set solid color white on init.
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(HSV_WHITE);
}

bool encoder_update_user(uint8_t index, bool clockwise) {
  uint8_t layer = get_highest_layer(layer_state | default_layer_state);

  switch (index) {
    case 0:
      // Left encoder
      // Delay added to allow OBS to intercept the keypress
      tap_code_delay(clockwise ? KC_F13 : KC_F14, 25);
      break;

    case 1:
      // Center encoder
      if (layer == _OTHER) {
        if (clockwise) {
          rgb_matrix_increase_val_noeeprom();
        } else {
          rgb_matrix_decrease_val_noeeprom();
        }
      } else {
        // Send 2 volume commands to make volume changes faster.
        int num = 2;
        int i = 0;

        if (clockwise) {
          for (i=0; i<num; i++) {
            tap_code_delay(KC_VOLU, 10);
          }
        } else {
          for (i=0; i<num; i++) {
            tap_code_delay(KC_VOLD, 10);
          }
        }
      }
      break;

    case 2:
      // Right encoder
      // Delay added to allow OBS to intercept the keypress
      tap_code_delay(clockwise ? KC_F16 : KC_F17, 25);
      break;
  }
  return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_CYCLE_LAYERS:
      if (!record->event.pressed) {
        // Handle the keycode and do nothing if this isn't a press.
        return false;
      }

      uint8_t current_layer = get_highest_layer(layer_state);

      // Make sure we are in range.
      if (current_layer > LAYER_CYCLE_END || current_layer < LAYER_CYCLE_START) {
        return false;
      }

      uint8_t next_layer = current_layer + 1;
      if (next_layer > LAYER_CYCLE_END) {
        next_layer = LAYER_CYCLE_START;
      }
      layer_move(next_layer);
      return false;

      // Process other keycodes normally.
    default:
      return true;
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
  uint8_t current_val = rgb_matrix_get_val();

  switch (get_highest_layer(state)) {
    case _SOUNDS:
      // Blue
      rgb_matrix_sethsv_noeeprom(169, 255, current_val);
      break;
    case _OTHER:
      // Cyan
      rgb_matrix_sethsv_noeeprom(127, 255, current_val);
      break;
    default:
      // White
      rgb_matrix_sethsv_noeeprom(0, 0, current_val);

      break;
  }
  return state;
}
