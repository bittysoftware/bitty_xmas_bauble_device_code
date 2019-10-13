#include "MicroBit.h"
#include <stdlib.h> 

MicroBit uBit;
MicroBitImage pattern(5,5);

// State report is used to tell the client the current light and audio selections
const int STATE_REPORT = 9010;
// State control allows the client to ask what the current light and audio selections are so it can get its UI in sync with the micro:bit state
const int STATE_QUERY = 9009;
// Behaviour events control the display and audio behaviour of the microbit and have a value whose first octet is a light sequence selector and second octet is an audio sequence selector
const int BEHAVIOUR_CONTROL = 9007;
const int MAX_LIGHT_EVENT=6;
const int MAX_AUDIO_EVENT=3;
int selected_light_seq=0;
int selected_audio_seq=0;
int playing_audio_seq=0;
int display_on=0;
int audio_on=0;
int num_rnd_pixels = 4;

const int MERRY_XMAS           = 1;
const int FLASH                = 2;
const int RANDOM_PIXELS_FLASH  = 3;
const int RIPPLE               = 4;
const int SPIRAL               = 5;
const int CHECKER              = 6;
const int STOP                 = 254;

const int QUERY_STATE          = 1;

// The period of the PWM determines the tone that will be played; the duty cycle changes the volume.
const int JINGLE_BELLS         = 1;
const int SILENT_NIGHT         = 2;
const int DING_DONG            = 3;

int bluetooth_connected = 0;

int ripples[4][25] = {
    {0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0}, 
    {0,0,0,0,0,  0,0,0,0,0,  0,0,1,0,0,  0,0,0,0,0,  0,0,0,0,0}, 
    {0,0,0,0,0,  0,1,1,1,0,  0,1,0,1,0,  0,1,1,1,0,  0,0,0,0,0},
    {1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1} 
};

int spiral[25] = {
    12, 13, 8, 7, 6,   11, 16, 17, 18, 19,   14, 9, 4, 3, 2,   1, 0, 5, 10, 15,   20, 21, 22, 23, 24
};

int sleep_time=100;
int flash_state=1;
int inx=0;
int delta = 1;
int pixel_value=255;

// BPM tempo
int tempo =  120;

// frequencies
// unused
//     C = 262, CSharp = 277, D = 294, Eb = 311, E = 330, F = 349, FSharp = 370, G = 392, GSharp = 415, A = 440, Bb = 466, B = 494, 
//    C3 = 131, CSharp3 = 139, D3 = 147, Eb3 = 156, E3 = 165, F3 = 175, FSharp3 = 185, G3 = 196, GSharp3 = 208, A3 = 220, Bb3 = 233, B3 = 247,

enum Note {
    C4 = 262, CSharp4 = 277, D4 = 294, Eb4 = 311, E4 = 330, F4 = 349, FSharp4 = 370, G4 = 392, GSharp4 = 415, A4 = 440, Bb4 = 466, B4 = 494,
    C5 = 523, CSharp5 = 555, D5 = 587, Eb5 = 622, E5 = 659, F5 = 698, FSharp5 = 740, G5 = 784, GSharp5 = 831, A5 = 880, Bb5 = 932, B5 = 989,
};

// rest
#define RE        0
// demisemiquaver (1/32)
//#define N64       1
// demisemiquaver (1/32)
#define N32       2
// demisemiquaver - dotted
#define N32D      3
// semiquaver (1/16)
#define N16       4
// semiquaver - dotted
#define N16D      6
// quaver     (1/8 )
#define N08       8
// quaver - dotted
#define N08D     12
// crotchet   (1/4)
#define N04      16
// crotchet - dotted
#define N04D     24
// minim      (1/2)
#define N02      32
// minim - dotted
#define N02D     48
// semibreve  (1/1)
#define N01      64
// semibreve  (1/1)
#define N01D     96
// end of tune
#define TUNE_END -1

int N64_duration_ms;

const int jingle_bells[][2] = {
    {E5,N16}, {RE,N16}, {E5,N16}, {RE,N16}, {E5,N16}, {RE,N16}, {RE,N16}, {RE,N16},  
    {E5,N16}, {RE,N16}, {E5,N16}, {RE,N16}, {E5,N16}, {RE,N16}, {RE,N16}, {RE,N16},
    {E5,N16}, {RE,N16}, {G5,N16}, {RE,N16}, {C5,N08}, {RE,N16}, {D5,N32}, {RE,N32}, {E5,N08}, {RE,N16}, {RE,N16}, {RE,N16}, {RE,N16}, {RE,N16}, {RE,N16},
    {F5,N16}, {RE,N16}, {F5,N16}, {RE,N16}, {F5,N16}, {RE,N16}, {F5,N32}, {RE,N32}, {F5,N32}, {RE,N32}, {F5,N16}, {RE,N16}, 
    {E5,N16}, {RE,N16}, {E5,N16}, {RE,N16}, {E5,N32}, {RE,N32}, {E5,N32}, {RE,N32}, {E5,N32}, {RE,N32}, {RE,N16}, 
    {D5,N16}, {RE,N16}, {D5,N16}, {RE,N16}, {E5,N16}, {RE,N16}, {D5,N16}, {RE,N16}, {RE,N16}, {RE,N16}, {G5,N16}, {RE,N04}, 
    {TUNE_END, 00} 
};

const int silent_night[][2] = {
    {G4,N04D},{A4,N08}, {G4,N04},   
    {E4,N02D},   
    {G4,N04D},{A4,N08}, {G4,N04},   
    {E4,N02D}, 
    {D5,N02}, {D5,N04},
    {B4,N02D},
    {C5,N02}, {C5,N04},{G4,N02D},
    {A4,N02}, {A4,N04},
    {C5,N04D},{B4,N08},{A4,N04},
    {G4,N04D},{A4,N08},{G4,N04},{E4,N02D},
    {A4,N02}, {A4,N04},
    {C5,N04D},{B4,N08},{A4,N04},
    {G4,N04D},{A4,N08},{G4,N04},
    {E4,N02D},
    {D5,N02}, {D5,N04},
    {F5,N04D},{D5,N08},{B4,N04},
    {C5,N02D},{E5,N02D},
    {C5,N04}, {G4,N04},{E4,N04},
    {G4,N04D},{F4,N08},{D4,N04},
    {C4,N02D},{C4,N02D},
    {TUNE_END, 00} 
};

const int ding_dong[][2] = {
    {F4,N04}, {F4,N04},{G4,N08},{F4,N08},{E4,N08},{D4,N08},{C4,N02D},{C4,N04},
    {D4,N04}, {F4,N04},{F4,N04},{E4,N04},{F4,N02},{F4,N02},
    {F4,N04},{F4,N04},{G4,N08},{F4,N08},{E4,N08},{D4,N08},{C4,N02D},{C4,N04},
    {D4,N04}, {F4,N04},{F4,N04},{E4,N04},{F4,N02},{F4,N02},

    {C5,N04D},{Bb4,N08},{A4,N08},{Bb4,N08},{C5,N08},{A4,N08},
    {Bb4,N04D},{A4,N08},{G4,N08},{A4,N08},{Bb4,N08},{G4,N08},
    {A4,N04D},{G4,N08},{F4,N08},{G4,N08},{A4,N08},{F4,N08},
    {G4,N04D},{F4,N08},{E4,N08},{F4,N08},{G4,N08},{E4,N08},
    {F4,N04D},{E4,N08},{D4,N08},{E4,N08},{F4,N08},{D4,N08},

    {E4,N04D},{D4,N08},{C4,N04},{C4,N04},
    {D4,N04}, {F4,N04},{F4,N04},{E4,N04},
    {F4,N02},{F4,N04},{RE,N04},
    {TUNE_END, 00} 
};

void calculateNoteDuration() {
    N64_duration_ms = ( 60.0 / tempo / 16.0 * 1000.0);    
} 

void playTone(int frequency, int duration_ms) {
    if (frequency <= 0) {
        uBit.io.pin[0].setAnalogValue(0);
    } else {
        uBit.io.pin[0].setAnalogValue(512);
        uBit.io.pin[0].setAnalogPeriodUs(1000000/frequency);
    }
    if (duration_ms > 0) {
        fiber_sleep(duration_ms);
        uBit.io.pin[0].setAnalogValue(0);
        wait_ms(5);
    }
}

void rest(int duration_ms) {
    playTone(0, duration_ms);
}

void audioOff() {
    uBit.io.pin[0].setAnalogValue(0);
}

void playTune(const int tune_data[][2]) {
    printf("play_tune\n");
    calculateNoteDuration();
    int i=0;
    while (tune_data[i][0] != TUNE_END) {
        if (selected_audio_seq != playing_audio_seq) {
            return;
        } 
        int duration = N64_duration_ms * tune_data[i][1];
//        printf("duration=%d\n\n",duration);
        if (tune_data[i][0] != RE) {
            playTone(tune_data[i][0] , duration);
        } else {
            rest(duration);
        }
        i++;
        if (selected_audio_seq != playing_audio_seq) {
            return;
        } 
    }
}

void lightMerryXmas() {
    uBit.display.scroll("Merry Xmas");
}

void lightFlash() {
    int i;
    for (i=0;i<25;i++) {
        pattern.setPixelValue(i % 5,floor(i / 5), flash_state * 255);
    }
    uBit.display.image.paste(pattern);
    flash_state = !flash_state;
}

void lightRandomPixelsFlash() {
    if (flash_state == 1) {
        pattern.clear();
        int i=0;
        for (i=0;i<num_rnd_pixels;i++) {
            uint8_t pixel = uBit.random(25);
            pattern.setPixelValue(pixel % 5,floor(pixel / 5), 255);
        }
        flash_state = 0;
    } else {
        pattern.clear();
        flash_state = 1;
    }
    uBit.display.image.paste(pattern);
}

void lightRipple() {
    int i=0;
    for (i=0;i<25;i++) {
        pattern.setPixelValue(i % 5,floor(i / 5), ripples[inx][i]);
    }
    uBit.display.image.paste(pattern);
    inx = inx + delta;
    if (inx == 3 || inx == 0) {
        delta = delta * -1;
    }
}

void lightSpiral() {
    int x = spiral[inx] % 5;
    int y = floor(spiral[inx] / 5);
    pattern.setPixelValue(x,y, pixel_value);
    uBit.display.image.paste(pattern);
    inx = inx + delta;
    if (inx == 25 || inx == -1) {
        delta = delta * -1;
        inx = inx + delta;
        if (pixel_value == 255) {
            pixel_value = 0;
        } else {
            pixel_value = 255;
        }
    }
}

void lightAlternatingCheckerPattern() {
// every other pixel on to begin with. Then toggle state of each pixel on | off periodically. 
    int i;
    for (i=0;i<25;i++) {
        int x = spiral[i] % 5;
        int y = floor(spiral[i] / 5);
        pattern.setPixelValue(x,y, flash_state * 255);
        flash_state = !flash_state;
    }
    uBit.display.image.paste(pattern);
}

void stopLight() {
    if (display_on == 1) {
        uBit.display.stopAnimation();
    }
}

void audioLoop() {
    printf("audioLoop\n\n");
    while (audio_on == 1) {
       switch (selected_audio_seq) {
         case STOP:
           printf("audioLoop.STOP\n");
           audio_on = 0;
           break;
         case JINGLE_BELLS:
           printf("audioLoop.JINGLE_BELLS\n");
           audio_on = 1;
           tempo = 120;
           playing_audio_seq = JINGLE_BELLS;
           playTune(jingle_bells);
           break;
         case SILENT_NIGHT:
           printf("audioLoop.SILENT_NIGHT\n");
           audio_on = 1;
           tempo = 120;
           playing_audio_seq = SILENT_NIGHT;
           playTune(silent_night);
           break;
         case DING_DONG:
           printf("audioLoop.DING_DONG\n");
           audio_on = 1;
           tempo = 160;
           playing_audio_seq = DING_DONG;
           playTune(ding_dong);
           break;
       }
    }
    printf("audioLoop exiting\n");
}

void animationLoop() {
    // printf("animationLoop %d\n\n",selected_light_seq);
    // exit if no animation requested to save power
    while(display_on == 1) {
       printf("animationLoop continuing %d\n\n",selected_light_seq);
       switch (selected_light_seq) {
         case STOP:
           stopLight();
           display_on = 0;
           break;
        case MERRY_XMAS:
           display_on = 1;
           lightMerryXmas();
           break;
        case FLASH:
           display_on = 1;
           lightFlash();
           break;
        case RANDOM_PIXELS_FLASH:
           display_on = 1;
           lightRandomPixelsFlash();
           break;
        case RIPPLE:
           display_on = 1;
           lightRipple();
           break;
        case SPIRAL:
           display_on = 1;
           lightSpiral();
           break;
        case CHECKER:
           display_on = 1;
           lightAlternatingCheckerPattern();
           break;
       }
       uBit.sleep(sleep_time); 
    }
    printf("animationLoop exiting\n");
}

void onControllerEvent(MicroBitEvent e)
{
    printf("onControllerEvent %d.%d\n",e.source,e.value);

    if ( e.source == STATE_QUERY) {
        // prime the Microbit Event characteristic with the current display/audio values so that they
        // are returned when the client issues a read against the characteristic
        // remember: current_selections is a 16 bit number... when transmitted over BLE it will be done so in Little Endian format
        // This is why it might look like the audio and light selection values are the wrong way around here. They're not :-)
        uint16_t current_selections = (selected_audio_seq << 8) + selected_light_seq;
        MicroBitEvent(STATE_REPORT, current_selections);
        return;
    }

    // 0 means no change being requested
    const uint8_t audio_sequence = (uint8_t)((e.value & 0xFF00) >> 8);
    const uint8_t light_sequence = (uint8_t)(e.value & 0x00FF);
	printf("onControllerEvent light=%d audio=%d\n",light_sequence,audio_sequence);
	printf("onControllerEvent selected_light=%d\n",selected_light_seq);

    if (light_sequence != 0 && light_sequence != selected_light_seq) {
        printf("light sequence selected\n");
        if (display_on == 0) { 
            display_on = 1;
            // we only create the animation loop if an animation has been requested (to save power)
            printf("starting animation loop\n");
            create_fiber(animationLoop);
        }
        sleep_time = 1000;
        if (light_sequence == RANDOM_PIXELS_FLASH) {
            sleep_time = 100;
        } else if (light_sequence == RIPPLE) {
            sleep_time = 400;
            delta = 1;
            inx = 0;
        } else if (light_sequence == SPIRAL) {
            sleep_time = 50;
            inx = 0;
            delta = 1;
            pixel_value=255;
        }
    } 
    uBit.display.stopAnimation();
    pattern.clear();
    if (audio_sequence > 0) {
        selected_audio_seq = audio_sequence;
        if (audio_on == 0) { 
            audio_on = 1;
            create_fiber(audioLoop);
        }
    }
    if (light_sequence != 0) {
        selected_light_seq = light_sequence;
    }
}

void onButton(MicroBitEvent e)
{
    // "emergency stop" - no need to dig out your phone and connect just to stop the music from annoying people!
    // Either  button will stop both sound and animation
    printf("onButton\n");
    if (e.source == MICROBIT_ID_BUTTON_A || e.source == MICROBIT_ID_BUTTON_B) {
        // stop current light and/or audio
        selected_light_seq = STOP;
        selected_audio_seq = STOP;
        uBit.display.stopAnimation();
        uint16_t stopped = (STOP << 8) + STOP;
        printf("raising state report event: %d\n",stopped);
        MicroBitEvent(STATE_REPORT, stopped);
        return;
    }
}

void onConnected(MicroBitEvent)
{
    printf("onConnected\n");
    uBit.display.stopAnimation();
    uBit.display.print("C");
    bluetooth_connected = 1;
}

void onDisconnected(MicroBitEvent)
{
    printf("onDisconnected\n");
    bluetooth_connected = 0;
}

int main()
{
    uBit.init();
    uBit.seedRandom();
    calculateNoteDuration();
//    printf("N64_duration_ms=%d\n\n",N64_duration_ms);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButton);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButton);
    uBit.messageBus.listen(BEHAVIOUR_CONTROL, 0, onControllerEvent); 
    uBit.messageBus.listen(STATE_QUERY, 0, onControllerEvent); 

    selected_light_seq=0;
    selected_audio_seq=0;
    display_on=0;
    audio_on=0;

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}
