#include QMK_KEYBOARD_H

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

#ifdef OLED_DRIVER_ENABLE
static uint32_t oled_timer = 0;
#endif

extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
enum layers {
  _QWERTY,
  _LOWER,
  _RAISE,
  _ADJUST
};

// Custom keycodes for layer keys
// Dual function escape with left command
#define KC_LGESC LGUI_T(KC_ESC)

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  ADJUST,
  RGBRST,
  KC_RACL // right alt / colon
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT(
  //,-----------------------------------------.                  ,---------------------------------------------.
     KC_TAB,  KC_Q,  KC_W,  KC_E,  KC_R,  KC_T,                   KC_Y,  KC_U,   KC_I,  KC_O,   KC_P, KC_BSPC,
  //|------+------+------+------+------+------|                  |------+------+-------+------+-------+--------|
    KC_ESC,  KC_A,  KC_S,  KC_D,  KC_F,  KC_G,                    KC_H,  KC_J,   KC_K,  KC_L,KC_SCLN, KC_QUOT,
  //|------+------+------+------+------+------|                  |------+------+-------+------+-------+--------|
    KC_LSFT,  KC_Z,  KC_X,  KC_C,  KC_V,  KC_B,                   KC_N,  KC_M,KC_COMM,KC_DOT,KC_SLSH,  KC_ENT,
  //|------+------+------+------+------+------+--------|  |------+------+------+-------+------+-------+--------|
                               KC_LGESC,LOWER, RCTL_T(KC_SPC),  KC_SPC, RAISE, KC_RALT
                              //`----------------------'  `--------------------'
  ),

  [_LOWER] = LAYOUT(
  //,---------------------------------------------.                ,---------------------------------------------.
     KC_GRV,  KC_1,   KC_2,   KC_3,   KC_4,   KC_5,                   KC_6,  KC_7,   KC_8,  KC_9,   KC_0, KC_BSPC,
  //|------+------+-------+-------+-------+-------|                |------+------+-------+------+-------+--------|
     KC_DEL, KC_NO,KC_LEFT,  KC_UP,KC_RGHT,  KC_NO,                KC_HOME,KC_PGUP,KC_END, KC_NO,  KC_NO,   KC_NO,
  //|------+------+-------+-------+-------+-------|                |------+------+-------+------+-------+--------|
    KC_TRNS, KC_NO,  KC_NO,KC_DOWN,  KC_NO,  KC_NO,                KC_HOME,KC_PGDN,KC_TRNS,KC_TRNS,KC_NO,  KC_ENT,
  //|------+------+-------+-------+-------+-------+------|  |------+------+------+-------+------+-------+--------|
                                    KC_LGUI, LOWER,KC_SPC,   KC_SPC, RAISE,KC_RALT
                                  //`--------------------'  `--------------------'
  ),

  [_RAISE] = LAYOUT(
  //,-----------------------------------------.                ,-----------------------------------------.
    KC_TILD,KC_EXLM,KC_AT,KC_HASH,KC_DLR,KC_PERC,              KC_CIRC,KC_AMPR,KC_ASTR,KC_LPRN,KC_RPRN,KC_BSPC,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
     KC_DEL, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,                KC_MINS,KC_PLUS,KC_LCBR,KC_RCBR,KC_PIPE,KC_GRV,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
    KC_TRNS, KC_F6, KC_F7, KC_F8, KC_F9,KC_F10,                KC_UNDS,KC_EQL,KC_LBRC,KC_RBRC,KC_BSLS,KC_TILD,
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                KC_LGUI, LOWER,KC_SPC,   KC_SPC, RAISE,KC_RALT
                              //`--------------------'  `--------------------'
  ),

  [_ADJUST] = LAYOUT(
  //,-----------------------------------------.                ,----------------------------------------------.
      RESET,RGBRST, KC_NO, KC_NO, KC_NO, KC_NO,                  KC_NO,KC_MUTE,  KC_NO,  KC_NO,KC_PSCR,  KC_NO,
  //|------+------+------+------+------+------|                |------+-------+-------+-------+-------+-------|
    RGB_TOG,RGB_HUI,RGB_SAI,RGB_VAI,RGB_SPI,KC_NO,            KC_PAUSE,KC_VOLU,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
  //|------+------+------+------+------+------|                |------+-------+-------+-------+-------+-------|
    RGB_MOD,RGB_HUD,RGB_SAD,RGB_VAD,RGB_SPD,KC_NO,             KC_SLCK,KC_VOLD,  KC_NO,  KC_NO,  KC_NO,RGB_RMOD,
  //|------+------+------+------+------+------+------|  |------+------+-------+-------+-------+-------+-------|
                                KC_LGUI, LOWER,KC_SPC,   KC_SPC, RAISE,KC_RALT
                              //`--------------------'  `--------------------'
  )
};

int RGB_current_mode;

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
}

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }

void render_space(void) {
    oled_write_P(PSTR("     "), false);
}

void render_mod_status_gui_alt(uint8_t modifiers) {
    static const char PROGMEM gui_off_1[] = {0x85, 0x86, 0};
    static const char PROGMEM gui_off_2[] = {0xa5, 0xa6, 0};
    static const char PROGMEM gui_on_1[] = {0x8d, 0x8e, 0};
    static const char PROGMEM gui_on_2[] = {0xad, 0xae, 0};

    static const char PROGMEM alt_off_1[] = {0x87, 0x88, 0};
    static const char PROGMEM alt_off_2[] = {0xa7, 0xa8, 0};
    static const char PROGMEM alt_on_1[] = {0x8f, 0x90, 0};
    static const char PROGMEM alt_on_2[] = {0xaf, 0xb0, 0};

    // fillers between the modifier icons bleed into the icon frames
    static const char PROGMEM off_off_1[] = {0xc5, 0};
    static const char PROGMEM off_off_2[] = {0xc6, 0};
    static const char PROGMEM on_off_1[] = {0xc7, 0};
    static const char PROGMEM on_off_2[] = {0xc8, 0};
    static const char PROGMEM off_on_1[] = {0xc9, 0};
    static const char PROGMEM off_on_2[] = {0xca, 0};
    static const char PROGMEM on_on_1[] = {0xcb, 0};
    static const char PROGMEM on_on_2[] = {0xcc, 0};

    if(modifiers & MOD_MASK_GUI) {
        oled_write_P(gui_on_1, false);
    } else {
        oled_write_P(gui_off_1, false);
    }

    if ((modifiers & MOD_MASK_GUI) && (modifiers & MOD_MASK_ALT)) {
        oled_write_P(on_on_1, false);
    } else if(modifiers & MOD_MASK_GUI) {
        oled_write_P(on_off_1, false);
    } else if(modifiers & MOD_MASK_ALT) {
        oled_write_P(off_on_1, false);
    } else {
        oled_write_P(off_off_1, false);
    }

    if(modifiers & MOD_MASK_ALT) {
        oled_write_P(alt_on_1, false);
    } else {
        oled_write_P(alt_off_1, false);
    }

    if(modifiers & MOD_MASK_GUI) {
        oled_write_P(gui_on_2, false);
    } else {
        oled_write_P(gui_off_2, false);
    }

    if (modifiers & MOD_MASK_GUI & MOD_MASK_ALT) {
        oled_write_P(on_on_2, false);
    } else if(modifiers & MOD_MASK_GUI) {
        oled_write_P(on_off_2, false);
    } else if(modifiers & MOD_MASK_ALT) {
        oled_write_P(off_on_2, false);
    } else {
        oled_write_P(off_off_2, false);
    }

    if(modifiers & MOD_MASK_ALT) {
        oled_write_P(alt_on_2, false);
    } else {
        oled_write_P(alt_off_2, false);
    }
}

void render_mod_status_ctrl_shift(uint8_t modifiers) {
    static const char PROGMEM ctrl_off_1[] = {0x89, 0x8a, 0};
    static const char PROGMEM ctrl_off_2[] = {0xa9, 0xaa, 0};
    static const char PROGMEM ctrl_on_1[] = {0x91, 0x92, 0};
    static const char PROGMEM ctrl_on_2[] = {0xb1, 0xb2, 0};

    static const char PROGMEM shift_off_1[] = {0x8b, 0x8c, 0};
    static const char PROGMEM shift_off_2[] = {0xab, 0xac, 0};
    static const char PROGMEM shift_on_1[] = {0xcd, 0xce, 0};
    static const char PROGMEM shift_on_2[] = {0xcf, 0xd0, 0};

    // fillers between the modifier icons bleed into the icon frames
    static const char PROGMEM off_off_1[] = {0xc5, 0};
    static const char PROGMEM off_off_2[] = {0xc6, 0};
    static const char PROGMEM on_off_1[] = {0xc7, 0};
    static const char PROGMEM on_off_2[] = {0xc8, 0};
    static const char PROGMEM off_on_1[] = {0xc9, 0};
    static const char PROGMEM off_on_2[] = {0xca, 0};
    static const char PROGMEM on_on_1[] = {0xcb, 0};
    static const char PROGMEM on_on_2[] = {0xcc, 0};

    if(modifiers & MOD_MASK_CTRL) {
        oled_write_P(ctrl_on_1, false);
    } else {
        oled_write_P(ctrl_off_1, false);
    }

    if ((modifiers & MOD_MASK_CTRL) && (modifiers & MOD_MASK_SHIFT)) {
        oled_write_P(on_on_1, false);
    } else if(modifiers & MOD_MASK_CTRL) {
        oled_write_P(on_off_1, false);
    } else if(modifiers & MOD_MASK_SHIFT) {
        oled_write_P(off_on_1, false);
    } else {
        oled_write_P(off_off_1, false);
    }

    if(modifiers & MOD_MASK_SHIFT) {
        oled_write_P(shift_on_1, false);
    } else {
        oled_write_P(shift_off_1, false);
    }

    if(modifiers & MOD_MASK_CTRL) {
        oled_write_P(ctrl_on_2, false);
    } else {
        oled_write_P(ctrl_off_2, false);
    }

    if (modifiers & MOD_MASK_CTRL & MOD_MASK_SHIFT) {
        oled_write_P(on_on_2, false);
    } else if(modifiers & MOD_MASK_CTRL) {
        oled_write_P(on_off_2, false);
    } else if(modifiers & MOD_MASK_SHIFT) {
        oled_write_P(off_on_2, false);
    } else {
        oled_write_P(off_off_2, false);
    }

    if(modifiers & MOD_MASK_SHIFT) {
        oled_write_P(shift_on_2, false);
    } else {
        oled_write_P(shift_off_2, false);
    }
}

void render_logo(void) {
    static const char PROGMEM corne_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0};
    oled_write_P(corne_logo, false);
	oled_write_P(PSTR("     "), false);
    oled_write_P(PSTR("cyb-7"), false);
}

// image map is vertical 32x128px, set rendering to vertical
void render_logo_r(void) {
    static const char PROGMEM hosaka[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xd8, 0xd8, 0x00, 0x00, 0xd8, 0xd8, 0xd8, 0x10, 0x10, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xd0, 0xd0, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xc0, 0x50, 0x1c, 0xde, 0xde, 0x02, 0x00, 0xc0, 0xc0, 0xce, 0xce, 0xce, 0xc8, 0xc8, 
0xd8, 0xd8, 0xd8, 0xc8, 0xc8, 0xc8, 0xce, 0xce, 0xce, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0xdb, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0x50, 0x18, 0x08, 0x03, 
0xdb, 0xdb, 0xdb, 0x03, 0x03, 0x1a, 0x18, 0x58, 0xd0, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x06, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x20, 
0x20, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x08, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0xf7, 0xf7, 0x03, 0x03, 0x03, 0x03, 0x00, 0xf7, 0xf7, 0x07, 
0x37, 0xf7, 0xc7, 0x07, 0x07, 0x07, 0x07, 0x07, 0xe7, 0xf7, 0x37, 0x07, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xc0, 0xc0, 0xd0, 0x50, 0x1e, 0x1e, 0x18, 0x0c, 0x84, 0xc0, 0xdc, 0x1e, 0x02, 0x00, 
0x00, 0x80, 0xc6, 0xce, 0x5c, 0x58, 0xde, 0xc6, 0xc2, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x02, 0x00, 0x00, 0x06, 0x06, 
0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x06, 0x06, 0x00, 0x00, 0x80, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0x30, 0x30, 0x30, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x60, 0x60, 0x60, 0x00, 0xe0, 0xe0, 0x00, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0x4c, 0x4d, 0x4d, 0x4d, 0x0c, 0x7d, 0x3c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x84, 0xc4, 0xe4, 0x00, 0x67, 0x67, 0x00, 0xe4, 0xc4, 0x84, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0x0e, 0x1f, 0x13, 0x13, 0x03, 0x1f, 0x0e, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0x8f, 0x18, 0x00, 0x10, 0x90, 0x00, 0x98, 0x8f, 0x07, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0x8e, 0xd9, 0xdc, 0xce, 0xc0, 0xd3, 0x8e, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xcf, 0x9f, 0x98, 0x00, 0x0e, 0x07, 0x00, 0x01, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0x33, 0x34, 0x30, 0x37, 0x30, 0xf4, 0xf3, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xb1, 0x9f, 0x9f, 0x00, 0x99, 0x8b, 0x00, 0x8f, 0x86, 0x86, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0xb9, 0xfd, 0x8d, 0x8d, 0x0c, 0xfd, 0xb8, 0x80, 0x80, 0x80, 0x00, 
0x00, 0xb1, 0x19, 0x0d, 0x00, 0x07, 0x07, 0x00, 0x0d, 0x19, 0x31, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0xbc, 0xa6, 0xa6, 0xbc, 0x80, 0xb2, 0x1c, 0xa0, 0x80, 0x00, 0x00, 
0x00, 0x63, 0x3f, 0x3f, 0x00, 0x32, 0x16, 0x00, 0x1e, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0xff, 0x00, 
0x00, 0x3f, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00};
    oled_write_raw_P(hosaka, sizeof(hosaka));
}

void render_layer_state(void) {
    static const char PROGMEM default_layer[] = {
        0x20, 0x94, 0x95, 0x96, 0x20,
        0x20, 0xb4, 0xb5, 0xb6, 0x20,
        0x20, 0xd4, 0xd5, 0xd6, 0x20, 0};
    static const char PROGMEM raise_layer[] = {
        0x20, 0x97, 0x98, 0x99, 0x20,
        0x20, 0xb7, 0xb8, 0xb9, 0x20,
        0x20, 0xd7, 0xd8, 0xd9, 0x20, 0};
    static const char PROGMEM lower_layer[] = {
        0x20, 0x9a, 0x9b, 0x9c, 0x20,
        0x20, 0xba, 0xbb, 0xbc, 0x20,
        0x20, 0xda, 0xdb, 0xdc, 0x20, 0};
    static const char PROGMEM adjust_layer[] = {
        0x20, 0x9d, 0x9e, 0x9f, 0x20,
        0x20, 0xbd, 0xbe, 0xbf, 0x20,
        0x20, 0xdd, 0xde, 0xdf, 0x20, 0};
    if(layer_state_is(_ADJUST)) {
        oled_write_P(adjust_layer, false);
    } else if(layer_state_is(_LOWER)) {
        oled_write_P(lower_layer, false);
    } else if(layer_state_is(_RAISE)) {
        oled_write_P(raise_layer, false);
    } else {
        oled_write_P(default_layer, false);
    }
}

void render_status_main(void) {
    render_logo();
    render_space();
    render_layer_state();
    render_space();
    render_mod_status_gui_alt(get_mods()|get_oneshot_mods());
    render_mod_status_ctrl_shift(get_mods()|get_oneshot_mods());
}
// this is for the second screen
void render_status_secondary(void) {
    render_logo_r();
}

void oled_task_user(void) {
    if (timer_elapsed32(oled_timer) > 30000) {
        oled_off();
        return;
    }
#ifndef SPLIT_KEYBOARD
    else { oled_on(); }
#endif

    if (is_master) {
        render_status_main();  // Renders the current keyboard state (layer, lock, caps, scroll, etc)
    } else {
        render_status_secondary();
    }
}

#endif
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
#ifdef OLED_DRIVER_ENABLE
        oled_timer = timer_read32();
#endif
    // set_timelog();
  }
  static uint16_t my_colon_timer;

  switch (keycode) {
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case ADJUST:
        if (record->event.pressed) {
          layer_on(_ADJUST);
        } else {
          layer_off(_ADJUST);
        }
        return false;
    case KC_RACL:
        if (record->event.pressed) {
          my_colon_timer = timer_read();
          register_code(KC_RALT);
        } else {
          unregister_code(KC_RALT);
          if (timer_elapsed(my_colon_timer) < TAPPING_TERM) {
            SEND_STRING(":"); // Change the character(s) to be sent on tap here
          }
        }
        return false;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      #ifdef RGB_MATRIX_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgb_matrix_default();
          rgb_matrix_enable();
        }
      #endif
      break;
  }
  return true;
}

#ifdef RGB_MATRIX_ENABLE

void suspend_power_down_keymap(void) {
    rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_keymap(void) {
    rgb_matrix_set_suspend_state(false);
}

#endif
