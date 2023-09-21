#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb_hid.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdio.h>
#include "version.h"
#include "Keycodes.h"

uint8_t Key_code = 0x04;
uint8_t keyCodes[] = {
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
    0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
    0x22, 0x23, 0x24, 0x25, 0x26, 0x27
};

char All[36] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0'};

// Uncomment to be able to make a screenshot
//#define USB_HID_AUTOFIRE_SCREENSHOT

typedef enum {
  EventTypeInput,
}
EventType;

typedef struct {
  union {
    InputEvent input;
  };
  EventType type;
}
UsbMouseEvent;

bool btn_autofire = false;
uint32_t autofire_delay = 10;
char current1[] = "0";
char * current = "xxx";
char MainSelect[] = "a";
char current3[] = "b";
int prev = 36;
int next = 1;
int minchar = 0;
int maxchar = 36; //length of array
int selected = 0;

static void usb_hid_autofire_render_callback(Canvas * canvas, void * ctx) {
  UNUSED(ctx);
  char autofire_delay_str[12];
  itoa(autofire_delay, autofire_delay_str, 10);

  canvas_clear(canvas);

  canvas_set_font(canvas, FontPrimary);
  canvas_draw_str(canvas, 0, 10, "LET/NUM AUTOCLICKER"); //16 charecters long
  canvas_draw_str(canvas, 0, 34, btn_autofire ? "<active>" : "<inactive>");
  canvas_draw_str(canvas, 5, 55, current1);
  canvas_draw_str(canvas, 60, 55, MainSelect);
  canvas_draw_str(canvas, 120, 55, current3);

  canvas_set_font(canvas, FontSecondary);
  canvas_draw_str(canvas, 55, 34, "up/down = time");
  canvas_draw_str(canvas, 0, 22, "press [ok] for on/off");
  canvas_draw_str(canvas, 0, 45, "delay [ms]:               ");
  canvas_draw_str(canvas, 50, 46, autofire_delay_str);
  canvas_draw_str(canvas, 0, 63, "Left/Right for LET/NUM");
}

static void usb_hid_autofire_input_callback(InputEvent * input_event, void * ctx) {
  FuriMessageQueue * event_queue = ctx;

  UsbMouseEvent event;
  event.type = EventTypeInput;
  event.input = * input_event;
  furi_message_queue_put(event_queue, & event, FuriWaitForever);
}

int32_t usb_hid_autofire_app(void * p) {
  UNUSED(p);
  FuriMessageQueue * event_queue = furi_message_queue_alloc(8, sizeof(UsbMouseEvent));
  furi_check(event_queue);
  ViewPort * view_port = view_port_alloc();

  FuriHalUsbInterface * usb_mode_prev = furi_hal_usb_get_config();
  #ifndef USB_HID_AUTOFIRE_SCREENSHOT
  furi_hal_usb_unlock();
  furi_check(furi_hal_usb_set_config( & usb_hid, NULL) == true);
  #endif

  view_port_draw_callback_set(view_port, usb_hid_autofire_render_callback, NULL);
  view_port_input_callback_set(view_port, usb_hid_autofire_input_callback, event_queue);

  // Open GUI and register view_port
  Gui * gui = furi_record_open(RECORD_GUI);
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  UsbMouseEvent event;
  while (1) {
    FuriStatus event_status = furi_message_queue_get(event_queue, & event, 50);

    if (event_status == FuriStatusOk) {
      if (event.type == EventTypeInput) {
        if (event.input.key == InputKeyBack) {
          break;
        }

        if (event.input.type != InputTypeRelease) {
          continue;
        }

        switch (event.input.key) {
        case InputKeyOk:
btn_autofire = !btn_autofire;
            if (btn_autofire == true) {
                
            }
            else if (btn_autofire == false) {
                
            }

            
          break;
        case InputKeyUp:
            
            autofire_delay += 10;
            
          break;
        case InputKeyDown:
          if (autofire_delay > 0) {
            autofire_delay -= 10;
          }
          break;
        case InputKeyLeft:

          selected = selected - 1;
          prev = selected;
          next = next - 1;
              //if (selected >= 36) {
               // selected = minchar; 
              //}
              if (selected < 0) {
                selected = 35;
                  prev = 35;
                  next = minchar;
              }
              if (selected == minchar) {
                prev = maxchar; 
              }
              if (selected == 34) {
                next = 35; 
              }
          current1[0]   =  All[prev - 1];
          MainSelect[0] =  All[selected];
          current3[0]   =  All[next];
              if (selected >= 0 && selected <= 35) {
                Key_code = keyCodes[selected];
              } 
            
          break;
        case InputKeyRight:
            
          selected = selected + 1;
          prev = selected;
          next = next + 1;
              if (selected >= 36) {
                selected = minchar; 
              }
              if (selected == minchar) {
                prev = maxchar; 
              }
              if (selected == 35) {
                next = minchar; 
              }
          current1[0]   =  All[prev - 1];
          MainSelect[0] =  All[selected];
          current3[0]   =  All[next];
              if (selected >= 0 && selected <= 35) {
                Key_code = keyCodes[selected];
              } 
            
          break;
        default:
          break;
        }
      }
    }

    if (btn_autofire) {
      furi_hal_hid_kb_press(Key_code);
      // TODO: Don't wait, but use the timer directly to just don't send the release event (see furi_hal_cortex_delay_us)
      furi_delay_us(autofire_delay * 500);
      furi_hal_hid_kb_release(Key_code);
      furi_delay_us(autofire_delay * 500);
    }


    view_port_update(view_port);
  }

  furi_hal_usb_set_config(usb_mode_prev, NULL);

  // remove & free all stuff created by app
  gui_remove_view_port(gui, view_port);
  view_port_free(view_port);
  furi_message_queue_free(event_queue);

  return 0;
}
