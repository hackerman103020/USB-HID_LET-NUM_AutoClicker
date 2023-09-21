#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb_hid.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdio.h>
#include "version.h"
#include "Keycodes.h"
//#define KEY_A 0x04 // Keyboard a and A
#define TESTT = "6.9"

// Uncomment to be able to make a screenshot
//#define USB_HID_AUTOFIRE_SCREENSHOT

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} UsbMouseEvent;

bool btn_left_autofire = false;
bool btn_right_autofire = false;
bool btn_select = false;
bool ison = false;
char All[36] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0'};

int btn_sel = 0; //     0 = off     1 = right     -1 = left
//int selectedAll = 0;
uint32_t autofire_delay = 10;
//char letter[3] = {'a','b','c'};
char * words = "ABC";
char * qwert = "XYZ";
char current[] = "xxx";
char current1[] = "0";
char MainSelect[] = "a";
char current3[] = "b";
int prev = 36;
int next = 1;
int minchar = 0;  
int maxchar = 36; //length of array
int selected = 0; 
//current[1] = '_';

static void usb_hid_autofire_render_callback(Canvas* canvas, void* ctx) {
    //strcpy(words , "p.1");
    UNUSED(ctx);
    char autofire_delay_str[12];
    //std::string pi = "pi is " + std::to_string(3.1415926);
    itoa(autofire_delay, autofire_delay_str, 10);
    //sprintf(autofire_delay_str, "%lu", autofire_delay);


    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "USB HID RightClick"); //16 charecters long
    canvas_draw_str(canvas, 0, 34, btn_left_autofire ? "<left>" : "<right>");
    canvas_draw_str(canvas, 50, 34, ison ? "<active>" : "<inactive>");
    canvas_draw_str(canvas, 5, 54, current1);
    canvas_draw_str(canvas, 60, 54, MainSelect);
    canvas_draw_str(canvas, 120, 54, current3);
    
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 100, 10, "v");
    canvas_draw_str(canvas, 106, 10, current);
    canvas_draw_str(canvas, 0, 22, "Press [ok] for auto clicking");
    canvas_draw_str(canvas, 0, 45, "delay [ms]:               [down] = off");
    canvas_draw_str(canvas, 50, 46, autofire_delay_str);
    canvas_draw_str(canvas, 0, 63, "Press [back] to exit");
}

static void usb_hid_autofire_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;

    UsbMouseEvent event;
    event.type = EventTypeInput;
    event.input = *input_event;
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t usb_hid_autofire_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(UsbMouseEvent));
    furi_check(event_queue);
    ViewPort* view_port = view_port_alloc();

    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
#ifndef USB_HID_AUTOFIRE_SCREENSHOT
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);
#endif

    view_port_draw_callback_set(view_port, usb_hid_autofire_render_callback, NULL);
    view_port_input_callback_set(view_port, usb_hid_autofire_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    UsbMouseEvent event;
    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 50);

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack) {
                    break;
                }

                if(event.input.type != InputTypeRelease) {
                    continue;
                }

                switch(event.input.key) {
                    case InputKeyOk:
                       // btn_left_autofire = !btn_left_autofire;
                       // btn_right_autofire = false;
                        if(btn_sel == 0) {
                            btn_sel = -1; 
                            btn_left_autofire = true; 
                            ison = true; 
                          //  selectedAll = selectedAll + 1;
                         //   strcpy(words , qwert);
                                current[0] = 'O';
                                current[1] = 'K';
                                current[2] = '_';
                        }
                        break;
                    case InputKeyUp: 
                        btn_sel = (btn_sel * -1); // 1 = right       2 = left
                            current[0] = 'U';
                            current[1] = 'P';
                            current[2] = '_';
                        if(btn_sel == -1) {
                            btn_right_autofire = false;
                            btn_left_autofire = true;
                            ison = true;
                        }
                        if(btn_sel == 1) {
                            btn_left_autofire = false;
                            btn_right_autofire = true;
                            ison = true;
                        }
                        break; 
                        case InputKeyDown: 
                        btn_sel = 0; 
                        btn_right_autofire = false;
                        btn_left_autofire = false;
                        ison = false;
                           // selectedAll = selectedAll + 1;
                            current1[0] = All[prev - 1];
                            MainSelect[0] = All[selected];
                            current3[0] = All[next];
                            if (selected == 2 ) {
                            current[0] = 'n';
                            current[1] = 'i';
                            current[2] = 'g'; 
                                                           
                            }
        selected = selected + 1 ; 
        prev     =   selected;
        // if (selected < 35 && selected >= minchar) {
           next     =   next + 1 ;  
        //}
        if (selected >= 36)
        {
            selected = minchar;  //if trying to go past last char, loop back around to 0
        }
        if (selected == minchar) 
        {
            prev = maxchar; //if on first char, then prev is the last char of array
        }
        if (selected == 35)
        {
    next = minchar;  //if on last char, then next char is first char of array
        }
                        break;
                    case InputKeyLeft:
                        if(autofire_delay > 0) {
                            autofire_delay -= 10;
                        }
                            current[0] = 'L';
                            current[1] = 'F';
                            current[2] = 'T';
                        break;
                    case InputKeyRight:
                        autofire_delay += 10;
                            current[0] = 'R';
                            current[1] = 'H';
                            current[2] = 'T';
                        break;
                    default:
                        break;
                }
            }
        }

        if(btn_left_autofire) {
            //furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
           // furi_hal_hid_kb_press(HID_KEYBOARD_DELETE);
             furi_hal_hid_kb_press(KEY_A);
            // TODO: Don't wait, but use the timer directly to just don't send the release event (see furi_hal_cortex_delay_us)
            furi_delay_us(autofire_delay * 500);
            //furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
            // furi_hal_hid_kb_release(HID_KEYBOARD_DELETE);
            furi_hal_hid_kb_release(KEY_A);
            furi_delay_us(autofire_delay * 500); 
        

        }
        if(btn_right_autofire) {
            //furi_hal_hid_mouse_press(HID_MOUSE_BTN_RIGHT);
            furi_hal_hid_kb_press(KEY_B);
            // TODO: Don't wait, but use the timer directly to just don't send the release event (see furi_hal_cortex_delay_us)
            furi_delay_us(autofire_delay * 500);
           // furi_hal_hid_mouse_release(HID_MOUSE_BTN_RIGHT);
            furi_hal_hid_kb_release(KEY_B);
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
