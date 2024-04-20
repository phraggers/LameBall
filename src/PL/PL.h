/*============================
  Phraglib
  Main Phraglib header
  PL.h
=============================*/
#ifndef _P_L_H
#ifdef __cplusplus
extern "C" {
#endif

    /* ==== USAGE: ====
    #include <PL/PL.h>
    - add "PL/PL.c" to your compiler input (eg: "cl yourapp.cpp PL/PL.c")
    ** (note: I'm thinking of changing it to a stb style single header with IMPLEMENTATION define -
        because it currently can't use any cpp features when the file ends in *.c) **
    - PL handles main, creates standard OpenGL window, starts gameloop
    - Don't define main(), instead define: void PL_Startup(), void PL_Frame(), void PL_ErrorCallback().
    - PL_ErrorCallback is called when an error occurs.
    - PL_Startup is called once before main loop.
    - PL_Frame is called every frame after input/event handling, before window update.
    - PL_Quit() closes the window and quits
    */

    /*=======================
       Platform Detection
    =======================
       PL_WINDOWS
       PL_MACOS
       PL_LINUX
    */

#if defined(WIN32) || \
defined(_WIN32) || \
defined(__WIN32__) || \
defined(__NT__) || \
defined(_WIN64)
#define PL_WINDOWS
#ifdef _MSC_VER
#define PL_WINDOWS_MSVC
#endif
#define GP_MAX_COUNT 4 // Xinput.h XUSER_MAX_COUNT
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#elif defined(TARGET_OS_MAC)
#define PL_MACOS
#elif defined(__linux__)
#define PL_LINUX
#else
#error "Unsupported Compiler"
#endif
#if defined(_DEBUG) || \
defined(DEBUG) || \
!defined(NDEBUG)
#define PL_DEBUG 1
#endif

    /*================================
         Phraglib Types
    =================================
    */

#include <stdint.h>
#include <limits.h>
    typedef int8_t i8;
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;

#ifndef __cplusplus
#include <stdbool.h>
#endif
    typedef bool b8;
    typedef i32 b32;

#include <float.h>
    typedef float r32;
    typedef double r64;

    typedef void* ptr;
    typedef char* cstr;

    /*=======================
         Helper Macros
    ========================*/
    // convert kilobytes to bytes
#define KB(v) ((v)*1024LL)
    // convert megabytes to bytes
#define MB(v) (KB(v)*1024LL)
    // convert gigabytes to bytes
#define GB(v) (MB(v)*1024LL)
    // break on expression being false (only when PL_DEBUG defined)
#if PL_DEBUG
#define PL_Assert(exp) if(!(exp)) {*(int*)0=0;}
#else
#define PL_Assert(exp)
#endif
    // get number of elements in array
#define PL_ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))
    // swap ptr A and B values of type
#define PL_SwapPtr(type, a, b) {type *tmp=b; b=a; a=tmp;}
    // re-interpret data bits(v) as new type(t)
#define PL_Reinterpret(t,v) (*((const t*)(v)))

    typedef struct
    {
        i32 x,y,w,h;
    } PL_Rect;

    // ====== Functions User must implement: =======

    // called before running loop
    void PL_Startup(void);
    // called every frame
    void PL_Frame(void);
    // ErrorCallback gets called when ErrorString is set.
    // Note that this can be called before all systems are initialized so don't use any other PL functions.
    void PL_ErrorCallback(void);

    /*============================
       PL General Functions
    =============================*/
    // get handle to user mem block
    ptr PL_GetUserMemory(void);
    // set user mem size, default 256MB (min 256mb)
    b32 PL_SetUserMemorySize(u64 size);
    // returns current size of user mem block
    u64 PL_GetUserMemorySize(void);
    // closes window and quits running
    void PL_Quit(void);
    // returns handle to error string (1024 bytes, gets filled when an error occurs, null terminated)
    cstr PL_GetErrorString(void); // previous error gets cleared when new error occurs.
    // sets error string (1024 max length, overwrites any existing error, will call ErrorCallback)
    void PL_SetErrorString(const cstr format, ...);

    /*============================
       Memory Management
    =============================*/
    // allocate block of memory on heap, returns pointer to new block
    ptr PL_Alloc(u64 size);
    // allocate block of memory on heap and zero, returns pointer to new block
    ptr PL_Alloc0(u64 size);
    // re-allocate existing block, returns pointer to new block
    ptr PL_ReAlloc(ptr oldMem, u64 newSize);
    // re-allocate existing block and zero, returns pointer to new block
    ptr PL_ReAlloc0(ptr oldMem, u64 newSize);
    // free previously allocated block, returns success
    b32 PL_Free(ptr mem);

    /*============================
       String & Byte Helpers
    =============================*/
    // writes 0s at mem block
    void PL_MemZero(ptr dst, u64 size);
    // writes value at memblock, returns ptr to last byte written + 1 (for further operations)
    ptr PL_MemSet(ptr dst, u8 value, u64 size);
    // copy size bytes from src to dst, returns num bytes copied
    u64 PL_MemCpy(ptr src, ptr dst, u64 size);
    // returns length of null terminated c string
    u64 PL_StrLen(cstr str);
    // copy str for len or src reads 0, null terminates dst, returns num bytes copied
    u64 PL_StrCpy(cstr src, cstr dst, u64 len);
    // adds src to end of dst, null terminates, returns new dst length
    u64 PL_StrCat(cstr src, cstr dst, u64 len);
    // print variables to string
    void PL_StrVar(cstr dst, u64 maxlen, const cstr format, ...);

    /*============================
            File IO
    =============================*/
    // File handle
    typedef struct
    {
        ptr handle;
        i64 size;
    } PL_File;

    // open a file, returns file handle, handle and size both 0 on error
    PL_File PL_FileOpen(const cstr path);
    // close an opened file handle
    void PL_FileClose(PL_File *file);
    // read size bytes from file offset bytes into buffer. size = 0 will copy the whole file. returns bytes read
    u64 PL_FileRead(PL_File *file, u64 offset, ptr dst, u64 size);
    // write size bytes from buffer to file at offset. overwrites existing contents, returns bytes written
    u64 PL_FileWrite(PL_File *file, u64 offset, ptr src, u64 size);
    // write size bytes from buffer to end of file. returns bytes written
    u64 PL_FileWriteAppend(PL_File *file, ptr src, u64 size);

    /*===========================
      Formatted String Printing
    ===========================*/
    // print to stdout
    void PL_Print(cstr format, ...);
    // print to stderr
    void PL_PrintErr(cstr format, ...);
    // print to open file at offset
    void PL_PrintFile(PL_File *file, u64 offset, cstr format, ...);
    // print to end of open file
    void PL_PrintFileAppend(PL_File *file, cstr format, ...);

    /*=======================
        Maths Functions
    =======================*/
    i32 PL_sign(i32 v); // sign of
    r32 PL_abs(r32 v); // absolute value
    i32 PL_round_i32(r32 v); // round r32 to nearest i32
    u32 PL_round_u32(r32 v); // round r32 to nearest u32
    i32 PL_trunc_i32(r32 v); // truncate r32 to i32
    i32 PL_floor_i32(r32 v); // floor r32 to i32
    i32 PL_ceil_i32(r32 v); // ceil r32 to i32
    r32 PL_div(r32 n, r32 d); // divide n/d, if d==0, returns 0.0f

    r64 PL_norm64(r64 v, r64 min, r64 max); // normalize 0.0 - 1.0 (64bit)
    r32 PL_norm32(r32 v, r32 min, r32 max); // normalize 0.0 - 1.0 (32bit)

    r32 PL_clamp(r32 v, r32 min, r32 max); // clamp between min and max
    r32 PL_clampMin(r32 v, r32 min); // clamp to >= min
    r32 PL_clampMax(r32 v, r32 max); // clamp to <= max
    r32 PL_clampN(r32 v); // clamp to normalized value >= 0.0, <= 1.0

    u32 PL_rotl(u32 v, i32 s); // rotate bits left (s times)
    u32 PL_rotr(u32 v, i32 s); // rotate bits right (s times)

    r32 PL_lerp(r32 a, r32 b, r32 t); // linear interpolation
    r32 PL_qlerp(r32 a, r32 b, r32 t); // quick lerp (fewer CPU cycles but less precise)

    r64 PL_pi64(void); // returns 64bit pi
    r32 PL_pi32(void); // returns 32bit pi
    r64 PL_tau64(void); // returns 64bit 2*pi
    r32 PL_tau32(void); // returns 32bit 2*pi

    r32 PL_sqrt(r32 v); // square root
    r32 PL_square(r32 v); // v*v
    r32 PL_pow32(r32 v, r32 s); // v^s
    r32 PL_sin(r32 a);
    r32 PL_cos(r32 a);
    r32 PL_atan2(r32 y, r32 x);

    /*======== Vectors ==========*/

    // ------ v2 ------ //

    typedef union
    {
        r32 i[2];
        struct {r32 a,b;};
        struct {r32 x,y;};
        struct {r32 u,v;};
    } v2;

    // constructors
    v2 v2i(i32 a, i32 b);
    v2 v2u(u32 a, u32 b);
    v2 v2r(r32 a, r32 b);

    // operators
    v2 v2add(v2 a, v2 b);
    v2 v2sub(v2 a, v2 b);
    v2 v2mul(v2 a, r32 v);
    v2 v2div(v2 a, r32 v);

    v2 v2perp(v2 a); // get perpendicular vector
    v2 v2hadamard(v2 a, v2 b); // hadamard product
    r32 v2dot(v2 a, v2 b); // dot/inner product
    r32 v2lengthsq(v2 a); // a^2
    r32 v2length(v2 a); // sqrt(a^2)

#ifdef __cplusplus
}
inline v2 operator+(v2 a, v2 b) {return {a.i[0]+b.i[0], a.i[1]+b.i[1]};}
inline v2 &operator+=(v2 &a, v2 b) {a = a+b; return a;}
inline v2 operator-(v2 a) {return {-a.i[0], -a.i[1]};}
inline v2 operator-(v2 a, v2 b) {return {a.i[0]-b.i[0], a.i[1]-b.i[1]};}
inline v2 &operator-=(v2 &a, v2 b) {a = a-b; return a;}
inline v2 operator*(r32 a, v2 b) {return {b.i[0]*a, b.i[1]*a};}
inline v2 operator*(v2 b, r32 a) {v2 result = a*b; return result;}
inline v2 &operator*=(v2 &b, r32 a) {b = b*a; return b;}
inline v2 operator/(v2 b, r32 a) {return {b.i[0]/a, b.i[1]/a};}
inline v2 &operator/=(v2 &b, r32 a) {b = b/a; return b;}
extern "C" {
#endif

    // ------ v3 ------ //

    typedef union
    {
        r32 i[3];
        union {struct{r32 x,y;}; v2 xy; r32 z;};
        struct {r32 PAD0; v2 yz;};
        struct {r32 u,v,w;};
        struct {r32 r,g,b;};
    } v3;

    // constructors
    v3 v3i(i32 a, i32 b, i32 c);
    v3 v3u(u32 a, u32 b, u32 c);
    v3 v3r(r32 a, r32 b, r32 c);

    // operators
    v3 v3add(v3 a, v3 b);
    v3 v3sub(v3 a, v3 b);
    v3 v3mul(v3 a, r32 v);
    v3 v3div(v3 a, r32 v);

    v3 v3hadamard(v3 a, v3 b); // hadamard product
    r32 v3dot(v3 a, v3 b); // dot/inner product
	r32 v3lengthsq(v3 a); // a^2
    r32 v3length(v3 a); // sqrt(a^2)

#ifdef __cplusplus
}
inline v3 operator+(v3 a, v3 b) {return {a.i[0]+b.i[0], a.i[1]+b.i[1], a.i[2]+b.i[2]};}
inline v3 &operator+=(v3 &a, v3 b) {a = a+b; return a;}
inline v3 operator-(v3 a) {return {-a.i[0], -a.i[1], -a.i[2]};}
inline v3 operator-(v3 a, v3 b) {return {a.i[0]-b.i[0], a.i[1]-b.i[1], a.i[2]-b.i[2]};}
inline v3 &operator-=(v3 &a, v3 b) {a = a-b; return a;}
inline v3 operator*(r32 a, v3 b) {return {b.i[0]*a, b.i[1]*a, b.i[2]*a};}
inline v3 operator*(v3 b, r32 a) {v3 result = a*b; return result;}
inline v3 &operator*=(v3 &b, r32 a) {b = b*a; return b;}
inline v3 operator/(v3 b, r32 a) {return {b.i[0]/a, b.i[1]/a, b.i[2]/a};}
inline v3 &operator/=(v3 &b, r32 a) {b = b/a; return b;}
extern "C" {
#endif

    // ------ v4 ------ //

    typedef union
    {
        r32 i[4];
        union {struct {r32 x,y,z;}; v3 xyz; r32 w;};
        union {struct {r32 r,g,b;}; v3 rgb; r32 a;};
        struct {r32 u,v,s,t;};
    } v4;

    // constructors
    v4 v4i(i32 a, i32 b, i32 c, i32 d);
    v4 v4u(u32 a, u32 b, u32 c, u32 d);
    v4 v4r(r32 a, r32 b, r32 c, r32 d);

    // operators
    v4 v4add(v4 a, v4 b);
    v4 v4sub(v4 a, v4 b);
    v4 v4mul(v4 a, r32 v);
    v4 v4div(v4 a, r32 v);

    v4 v4hadamard(v4 a, v4 b); // hadamard product
    r32 v4dot(v4 a, v4 b); // dot/inner product
	r32 v4lengthsq(v4 a); // a^2
    r32 v4length(v4 a); // sqrt(a^2)

    // C++ operators
#ifdef __cplusplus
}
inline v4 operator+(v4 a, v4 b) {return {a.i[0]+b.i[0], a.i[1]+b.i[1], a.i[2]+b.i[2], a.i[3]+b.i[3]};}
inline v4 &operator+=(v4 &a, v4 b) {a = a+b; return a;}
inline v4 operator-(v4 a) {return {-a.i[0], -a.i[1], -a.i[2], -a.i[3]};}
inline v4 operator-(v4 a, v4 b) {return {a.i[0]-b.i[0], a.i[1]-b.i[1], a.i[2]-b.i[2], a.i[3]-b.i[3]};}
inline v4 &operator-=(v4 &a, v4 b) {a = a-b; return a;}
inline v4 operator*(r32 a, v4 b) {return {b.i[0]*a, b.i[1]*a, b.i[2]*a, b.i[3]*a};}
inline v4 operator*(v4 b, r32 a) {v4 result = a*b; return result;}
inline v4 &operator*=(v4 &b, r32 a) {b = b*a; return b;}
inline v4 operator/(v4 b, r32 a) {return {b.i[0]/a, b.i[1]/a, b.i[2]/a, b.i[3]/a};}
inline v4 &operator/=(v4 &b, r32 a) {b = b/a; return b;}
extern "C" {
#endif

    /*===== RNG and Hashing =========*/
    // get hashed u32 from input (inputSize = sizeof input)
    u32 PL_Hash32(ptr input, u64 inputSize);
    // auto sizeof macro (just pass address of var, no cast)
#define PL_HASH32(v) PL_Hash32((ptr)v, sizeof *(v))
    // get psuedo random normalized number (0.0 to 1.0)
    r32 PL_Rand(void);
    // OPTIONAL: seed PL_Rand using any sized type (PL_Rand will otherwise seed using time 0)
    void PL_SeedRand(ptr value, u64 valueSize); // eg: int seed = 1234; PL_SeedRand(&seed, sizeof(int));
    // auto sizeof macro (just pass address of var, no cast)
#define PL_SEEDRAND(v) PL_SeedRand((ptr)v, sizeof *(v))
    // Rand macro helpers to cast to PL int types 8-32
#define PL_RANDI8 ((i8)PL_round_i32(((PL_Rand()*(r32)UINT8_MAX)-((r32)UINT8_MAX/2))))
#define PL_RANDU8 ((u8)PL_round_u32((PL_Rand()*(r32)UINT8_MAX)))
#define PL_RANDI16 ((i16)PL_round_i32(((PL_Rand()*UINT16_MAX)-((r32)UINT16_MAX/2))))
#define PL_RANDU16 ((u16)PL_round_u32((PL_Rand()*(r32)UINT16_MAX)))
#define PL_RANDI32 (PL_round_i32(((PL_Rand()*UINT32_MAX)-((r32)UINT32_MAX/2))))
#define PL_RANDU32 (PL_round_u32((PL_Rand()*(r32)UINT32_MAX)))

    /*================
      Timing
    ================*/
    typedef struct
    {
        u16 year; // 4 digits
        u16 month; // 1-12
        u16 day; // 1-31
        u16 wday; // 1:monday, 7:sunday
        u16 hr; // 0-23
        u16 min; // 0-59
        u16 sec; // 0-59
        u16 ms; // 0-999
    } PL_Clock;

    // Clock updated every frame
    PL_Clock *PL_GetClock(void);

    typedef struct
    {
        u64 frames; // total frames since startup
        r64 tLastFrame; // seconds elapsed last frame
        r64 tAvgFrame; // average seconds elapsed per frame since start
    } PL_Timer;

    // Timer updated every frame
    PL_Timer *PL_GetTimer(void);
    // returns timerperf value, pass this value to PL_TimerElapsed
    u64 PL_TimerStart(void);
    // time in seconds since TimerStart
    r64 PL_TimerElapsed(u64 timerperf);

    /*======================
      Input Definitions
    ======================*/
    typedef enum
    {
        K_NONE = 0,

        K_ESC, // escape
        K_PRTSC, // print screen
        K_SCRLK, // scroll lock
        K_PAUSE, // pause
        K_NUM, // num lock
        K_INS, // insert
        K_DEL, // delete
        K_HOME, // home
        K_END, // end
        K_PGUP, // page up
        K_PGDN, // page down

        K_F1,
        K_F2,
        K_F3,
        K_F4,
        K_F5,
        K_F6,
        K_F7,
        K_F8,
        K_F9,
        K_F10,
        K_F11,
        K_F12,

        K_UP,
        K_DOWN,
        K_LEFT,
        K_RIGHT,

        K_CAPS,
        K_BACKSPACE,
        K_LSHIFT,
        K_RSHIFT,
        K_LCTRL,
        K_RCTRL,
        K_LALT,
        K_RALT,
        K_LWIN, // left windows key
        K_RWIN, // right windows key
        K_MENU, // key usually right of RALT or RWIN
        // usually same as right-click contextual menu at mouse pos 0,0

        K_TAB = '\t',
        K_TILDE = '`',
        K_MINUS = '-',
        K_EQUALS = '=',
        K_RETURN = '\n',
        K_SPACE = ' ',
        K_LBRACKET = '[',
        K_RBRACKET = ']',
        K_SEMICOLON = ';',
        K_APOSTROPHE = '\'',
        K_BACKSLASH = '\\',
        K_COMMA = ',',
        K_PERIOD = '.',
        K_SLASH = '/',

        K_A = 'a',
        K_B = 'b',
        K_C = 'c',
        K_D = 'd',
        K_E = 'e',
        K_F = 'f',
        K_G = 'g',
        K_H = 'h',
        K_I = 'i',
        K_J = 'j',
        K_K = 'k',
        K_L = 'l',
        K_M = 'm',
        K_N = 'n',
        K_O = 'o',
        K_P = 'p',
        K_Q = 'q',
        K_R = 'r',
        K_S = 's',
        K_T = 't',
        K_U = 'u',
        K_V = 'v',
        K_W = 'w',
        K_X = 'x',
        K_Y = 'y',
        K_Z = 'z',

        K_1 = '1',
        K_2 = '2',
        K_3 = '3',
        K_4 = '4',
        K_5 = '5',
        K_6 = '6',
        K_7 = '7',
        K_8 = '8',
        K_9 = '9',
        K_0 = '0',

        KP_SLASH, // keypad /
        KP_MULTIPLY,  // keypad *
        KP_MINUS, // keypad -
        KP_PLUS, // keypad +
        KP_RETURN,
        KP_PERIOD,

        KP_0,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,

        KM_MUTE, // media key mute
        KM_VOLDOWN, // media key volume down
        KM_VOLUP, // media key volume up
        KM_PLAYPAUSE, // media key play/pause
        KM_STOP, // media key stop
        KM_PREV, // media key rewind
        KM_NEXT, // media key forward

        //not (yet?) implemented: browser keys, applaunch keys

        K_MAX
    } PL_KEYCODE;

    typedef enum
    {
        MB_NONE = 0,
        MB_LEFT,
        MB_RIGHT,
        MB_MIDDLE,
        MB_X1,
        MB_X2,

        MB_MAX
    } PL_MOUSEBTNCODE;

    typedef enum
    {
        GP_NONE = 0,
        GP_A,
        GP_B,
        GP_X,
        GP_Y,
        GP_START,
        GP_BACK,
        GP_DPAD_UP,
        GP_DPAD_DOWN,
        GP_DPAD_LEFT,
        GP_DPAD_RIGHT,
        GP_LEFTSHOULDER,
        GP_RIGHTSHOULDER,
        GP_L3,
        GP_R3,

        GP_MAX
    } PL_GAMEPADBTNCODE;

    typedef struct
    {
        r64 tDown; // seconds held down
        b32 isDown; // is currently down
        b32 wasDown; // was down previous frame
        b32 downTick; // was first pressed down this frame
        b32 upTick; // was first released this frame
    } PL_ButtonState;

    typedef struct
    {
        r32 rx,ry; // normalized pos -1 to 1 (0,0 is window centre)
        int px,py; // pixel pos (relative to window: 0,0 is top left)
        PL_ButtonState buttons[MB_MAX];
        int wheel; // <0: down clicks, >0: up clicks
    } PL_Mouse;

    typedef struct
    {
        r32 x, y; // normalized stick state -1 to 1 (0,0: centre)
    } PL_GamepadStickState;

    typedef struct
    {
        b32 isConnected;
        PL_ButtonState buttons[GP_MAX];
        PL_GamepadStickState LS, RS; // sticks -1.0 to 1.0
        r32 LT,RT; // triggers 0.0 - 1.0
        r32 LVib, RVib; // vibration 0.0 - 1.0
    } PL_Gamepad;

    typedef struct
    {
        PL_ButtonState keyboard[K_MAX];
        PL_Mouse mouse;
        PL_Gamepad gamepads[GP_MAX_COUNT];
    } PL_Input;

    // returns handles to the input structs
    PL_Input* PL_GetInput(void);
    PL_Mouse* PL_GetMouse(void);
    PL_Gamepad* PL_GetGamepad(i32 id);
    PL_ButtonState* PL_GetKeyState(PL_KEYCODE key);

    /*=======================
        Audio
    =======================*/
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BYTES_PER_SAMPLE 4 // sizeof(i16)*2
#define AUDIO_BUFFER_SIZE AUDIO_SAMPLE_RATE * AUDIO_BYTES_PER_SAMPLE // 1 sec buffer
    typedef struct
    {
        u64 playCursor; // sound driver last reported play pos within buffer
        u8 buffer[AUDIO_BUFFER_SIZE]; // circular play buffer
    } PL_Audio;

    // returns handle to audio struct
    PL_Audio* PL_GetAudio(void);
    // get handle to audio buffer as void*
    ptr PL_GetAudioBuffer(void);

    /*=======================
        Window
    =======================*/
    typedef struct
    {
        PL_Rect dim;
        i32 framerate;
        b32 vsync;
        b32 fullscreen;
        b32 focus;
    } PL_Window;

    // returns handle to window state struct
    PL_Window *PL_GetWindow(void);
    // set window title
    void PL_SetWindowTitle(const cstr format, ...);
    // set window screen pos (any param set to -1 will be unchanged)
    void PL_SetWindowPos(i32 x, i32 y, i32 w, i32 h);
    // set desired framerate (no effect if vsync is enabled)
    void PL_SetWindowFramerate(i32 framerate);
    // enable vsync true/false
    void PL_SetWindowVSync(b32 vsync);
    // set window fullscreen true or false, no change if it is already that state
    void PL_SetWindowFullscreen(b32 fullscreen);
    // if window is not fullscreen, fullscreen it, and vice-versa, returns new state
    b32 PL_ToggleWindowFullscreen(void);

    // OS message box popups
    void PL_MsgBox(const cstr title, cstr format, ...);
    void PL_MsgBoxInfo(const cstr title, cstr format, ...);
    void PL_MsgBoxError(const cstr title, cstr format, ...);

    /*=====================
      OpenGL Helpers
    =====================*/
    // get which GL version's functions are loaded (as float, e.g: '4.4')
    r32 PL_GetGLVersion(void);
    // compile frag & vert shader, create program, return glProgramID (errors reported to PL_ErrorString)
    u32 PL_GLCreateProgram(cstr vertexShaderSrc, cstr fragmentShaderSrc);

    /*=====================
       OpenGL
    =====================*/
    typedef unsigned int GLenum;
    typedef unsigned char GLboolean;
    typedef unsigned int GLbitfield;
    typedef void GLvoid;
    typedef int8_t GLbyte;
    typedef uint8_t GLubyte;
    typedef int16_t GLshort;
    typedef uint16_t GLushort;
    typedef int GLint;
    typedef unsigned int GLuint;
    typedef int32_t GLclampx;
    typedef int GLsizei;
    typedef float GLfloat;
    typedef float GLclampf;
    typedef double GLdouble;
    typedef double GLclampd;
    typedef void *GLeglClientBufferEXT;
    typedef void *GLeglImageOES;
    typedef char GLchar;
    typedef char GLcharARB;
#ifdef __APPLE__
    typedef void *GLhandleARB;
#else
    typedef unsigned int GLhandleARB;
#endif

    typedef uint16_t GLhalf;
    typedef uint16_t GLhalfARB;
    typedef int32_t GLfixed;
    typedef intptr_t GLintptr;
    typedef intptr_t GLintptrARB;
    typedef int64_t GLsizeiptr;
    typedef int64_t GLsizeiptrARB;
    typedef int64_t GLint64;
    typedef int64_t GLint64EXT;
    typedef uint64_t GLuint64;
    typedef uint64_t GLuint64EXT;

    typedef struct __GLsync *GLsync;
    struct _cl_context;
    struct _cl_event;
    typedef void (*GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
    typedef void (*GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
    typedef void (*GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
    typedef void (*GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);
    typedef unsigned short GLhalfNV;
    typedef GLintptr GLvdpauSurfaceNV;
    typedef void (*GLVULKANPROCNV)(void);

#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_NONE 0
#define GL_FRONT_LEFT 0x0400
#define GL_FRONT_RIGHT 0x0401
#define GL_BACK_LEFT 0x0402
#define GL_BACK_RIGHT 0x0403
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_LEFT 0x0406
#define GL_RIGHT 0x0407
#define GL_FRONT_AND_BACK 0x0408
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_POINT_SIZE 0x0B11
#define GL_POINT_SIZE_RANGE 0x0B12
#define GL_POINT_SIZE_GRANULARITY 0x0B13
#define GL_LINE_SMOOTH 0x0B20
#define GL_LINE_WIDTH 0x0B21
#define GL_LINE_WIDTH_RANGE 0x0B22
#define GL_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_POLYGON_MODE 0x0B40
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_CULL_FACE 0x0B44
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_TEST 0x0B71
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_STENCIL_TEST 0x0B90
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_VIEWPORT 0x0BA2
#define GL_DITHER 0x0BD0
#define GL_BLEND_DST 0x0BE0
#define GL_BLEND_SRC 0x0BE1
#define GL_BLEND 0x0BE2
#define GL_LOGIC_OP_MODE 0x0BF0
#define GL_DRAW_BUFFER 0x0C01
#define GL_READ_BUFFER 0x0C02
#define GL_SCISSOR_BOX 0x0C10
#define GL_SCISSOR_TEST 0x0C11
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_DOUBLEBUFFER 0x0C32
#define GL_STEREO 0x0C33
#define GL_LINE_SMOOTH_HINT 0x0C52
#define GL_POLYGON_SMOOTH_HINT 0x0C53
#define GL_UNPACK_SWAP_BYTES 0x0CF0
#define GL_UNPACK_LSB_FIRST 0x0CF1
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_SWAP_BYTES 0x0D00
#define GL_PACK_LSB_FIRST 0x0D01
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_DONT_CARE 0x1100
#define GL_FASTEST 0x1101
#define GL_NICEST 0x1102
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_CLEAR 0x1500
#define GL_AND 0x1501
#define GL_AND_REVERSE 0x1502
#define GL_COPY 0x1503
#define GL_AND_INVERTED 0x1504
#define GL_NOOP 0x1505
#define GL_XOR 0x1506
#define GL_OR 0x1507
#define GL_NOR 0x1508
#define GL_EQUIV 0x1509
#define GL_INVERT 0x150A
#define GL_OR_REVERSE 0x150B
#define GL_COPY_INVERTED 0x150C
#define GL_OR_INVERTED 0x150D
#define GL_NAND 0x150E
#define GL_SET 0x150F
#define GL_TEXTURE 0x1702
#define GL_COLOR 0x1800
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_STENCIL_INDEX 0x1901
#define GL_DEPTH_COMPONENT 0x1902
#define GL_RED 0x1903
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_REPEAT 0x2901
#define GL_CURRENT_BIT 0x00000001
#define GL_POINT_BIT 0x00000002
#define GL_LINE_BIT 0x00000004
#define GL_POLYGON_BIT 0x00000008
#define GL_POLYGON_STIPPLE_BIT 0x00000010
#define GL_PIXEL_MODE_BIT 0x00000020
#define GL_LIGHTING_BIT 0x00000040
#define GL_FOG_BIT 0x00000080
#define GL_ACCUM_BUFFER_BIT 0x00000200
#define GL_VIEWPORT_BIT 0x00000800
#define GL_TRANSFORM_BIT 0x00001000
#define GL_ENABLE_BIT 0x00002000
#define GL_HINT_BIT 0x00008000
#define GL_EVAL_BIT 0x00010000
#define GL_LIST_BIT 0x00020000
#define GL_TEXTURE_BIT 0x00040000
#define GL_SCISSOR_BIT 0x00080000
#define GL_ALL_ATTRIB_BITS 0xFFFFFFFF
#define GL_QUAD_STRIP 0x0008
#define GL_POLYGON 0x0009
#define GL_ACCUM 0x0100
#define GL_LOAD 0x0101
#define GL_RETURN 0x0102
#define GL_MULT 0x0103
#define GL_ADD 0x0104
#define GL_AUX0 0x0409
#define GL_AUX1 0x040A
#define GL_AUX2 0x040B
#define GL_AUX3 0x040C
#define GL_2D 0x0600
#define GL_3D 0x0601
#define GL_3D_COLOR 0x0602
#define GL_3D_COLOR_TEXTURE 0x0603
#define GL_4D_COLOR_TEXTURE 0x0604
#define GL_PASS_THROUGH_TOKEN 0x0700
#define GL_POINT_TOKEN 0x0701
#define GL_LINE_TOKEN 0x0702
#define GL_POLYGON_TOKEN 0x0703
#define GL_BITMAP_TOKEN 0x0704
#define GL_DRAW_PIXEL_TOKEN 0x0705
#define GL_COPY_PIXEL_TOKEN 0x0706
#define GL_LINE_RESET_TOKEN 0x0707
#define GL_EXP 0x0800
#define GL_EXP2 0x0801
#define GL_COEFF 0x0A00
#define GL_ORDER 0x0A01
#define GL_DOMAIN 0x0A02
#define GL_PIXEL_MAP_I_TO_I 0x0C70
#define GL_PIXEL_MAP_S_TO_S 0x0C71
#define GL_PIXEL_MAP_I_TO_R 0x0C72
#define GL_PIXEL_MAP_I_TO_G 0x0C73
#define GL_PIXEL_MAP_I_TO_B 0x0C74
#define GL_PIXEL_MAP_I_TO_A 0x0C75
#define GL_PIXEL_MAP_R_TO_R 0x0C76
#define GL_PIXEL_MAP_G_TO_G 0x0C77
#define GL_PIXEL_MAP_B_TO_B 0x0C78
#define GL_PIXEL_MAP_A_TO_A 0x0C79
#define GL_CURRENT_COLOR 0x0B00
#define GL_CURRENT_INDEX 0x0B01
#define GL_CURRENT_NORMAL 0x0B02
#define GL_CURRENT_TEXTURE_COORDS 0x0B03
#define GL_CURRENT_RASTER_COLOR 0x0B04
#define GL_CURRENT_RASTER_INDEX 0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS 0x0B06
#define GL_CURRENT_RASTER_POSITION 0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID 0x0B08
#define GL_CURRENT_RASTER_DISTANCE 0x0B09
#define GL_POINT_SMOOTH 0x0B10
#define GL_LINE_STIPPLE 0x0B24
#define GL_LINE_STIPPLE_PATTERN 0x0B25
#define GL_LINE_STIPPLE_REPEAT 0x0B26
#define GL_LIST_MODE 0x0B30
#define GL_MAX_LIST_NESTING 0x0B31
#define GL_LIST_BASE 0x0B32
#define GL_LIST_INDEX 0x0B33
#define GL_POLYGON_STIPPLE 0x0B42
#define GL_EDGE_FLAG 0x0B43
#define GL_LIGHTING 0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE 0x0B52
#define GL_LIGHT_MODEL_AMBIENT 0x0B53
#define GL_SHADE_MODEL 0x0B54
#define GL_COLOR_MATERIAL_FACE 0x0B55
#define GL_COLOR_MATERIAL_PARAMETER 0x0B56
#define GL_COLOR_MATERIAL 0x0B57
#define GL_FOG 0x0B60
#define GL_FOG_INDEX 0x0B61
#define GL_FOG_DENSITY 0x0B62
#define GL_FOG_START 0x0B63
#define GL_FOG_END 0x0B64
#define GL_FOG_MODE 0x0B65
#define GL_FOG_COLOR 0x0B66
#define GL_ACCUM_CLEAR_VALUE 0x0B80
#define GL_MATRIX_MODE 0x0BA0
#define GL_NORMALIZE 0x0BA1
#define GL_MODELVIEW_STACK_DEPTH 0x0BA3
#define GL_PROJECTION_STACK_DEPTH 0x0BA4
#define GL_TEXTURE_STACK_DEPTH 0x0BA5
#define GL_MODELVIEW_MATRIX 0x0BA6
#define GL_PROJECTION_MATRIX 0x0BA7
#define GL_TEXTURE_MATRIX 0x0BA8
#define GL_ATTRIB_STACK_DEPTH 0x0BB0
#define GL_ALPHA_TEST 0x0BC0
#define GL_ALPHA_TEST_FUNC 0x0BC1
#define GL_ALPHA_TEST_REF 0x0BC2
#define GL_LOGIC_OP 0x0BF1
#define GL_AUX_BUFFERS 0x0C00
#define GL_INDEX_CLEAR_VALUE 0x0C20
#define GL_INDEX_WRITEMASK 0x0C21
#define GL_INDEX_MODE 0x0C30
#define GL_RGBA_MODE 0x0C31
#define GL_RENDER_MODE 0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT 0x0C50
#define GL_POINT_SMOOTH_HINT 0x0C51
#define GL_FOG_HINT 0x0C54
#define GL_TEXTURE_GEN_S 0x0C60
#define GL_TEXTURE_GEN_T 0x0C61
#define GL_TEXTURE_GEN_R 0x0C62
#define GL_TEXTURE_GEN_Q 0x0C63
#define GL_PIXEL_MAP_I_TO_I_SIZE 0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE 0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE 0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE 0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE 0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE 0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE 0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE 0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE 0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE 0x0CB9
#define GL_MAP_COLOR 0x0D10
#define GL_MAP_STENCIL 0x0D11
#define GL_INDEX_SHIFT 0x0D12
#define GL_INDEX_OFFSET 0x0D13
#define GL_RED_SCALE 0x0D14
#define GL_RED_BIAS 0x0D15
#define GL_ZOOM_X 0x0D16
#define GL_ZOOM_Y 0x0D17
#define GL_GREEN_SCALE 0x0D18
#define GL_GREEN_BIAS 0x0D19
#define GL_BLUE_SCALE 0x0D1A
#define GL_BLUE_BIAS 0x0D1B
#define GL_ALPHA_SCALE 0x0D1C
#define GL_ALPHA_BIAS 0x0D1D
#define GL_DEPTH_SCALE 0x0D1E
#define GL_DEPTH_BIAS 0x0D1F
#define GL_MAX_EVAL_ORDER 0x0D30
#define GL_MAX_LIGHTS 0x0D31
#define GL_MAX_CLIP_PLANES 0x0D32
#define GL_MAX_PIXEL_MAP_TABLE 0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH 0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH 0x0D36
#define GL_MAX_NAME_STACK_DEPTH 0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH 0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH 0x0D39
#define GL_INDEX_BITS 0x0D51
#define GL_RED_BITS 0x0D52
#define GL_GREEN_BITS 0x0D53
#define GL_BLUE_BITS 0x0D54
#define GL_ALPHA_BITS 0x0D55
#define GL_DEPTH_BITS 0x0D56
#define GL_STENCIL_BITS 0x0D57
#define GL_ACCUM_RED_BITS 0x0D58
#define GL_ACCUM_GREEN_BITS 0x0D59
#define GL_ACCUM_BLUE_BITS 0x0D5A
#define GL_ACCUM_ALPHA_BITS 0x0D5B
#define GL_NAME_STACK_DEPTH 0x0D70
#define GL_AUTO_NORMAL 0x0D80
#define GL_MAP1_COLOR_4 0x0D90
#define GL_MAP1_INDEX 0x0D91
#define GL_MAP1_NORMAL 0x0D92
#define GL_MAP1_TEXTURE_COORD_1 0x0D93
#define GL_MAP1_TEXTURE_COORD_2 0x0D94
#define GL_MAP1_TEXTURE_COORD_3 0x0D95
#define GL_MAP1_TEXTURE_COORD_4 0x0D96
#define GL_MAP1_VERTEX_3 0x0D97
#define GL_MAP1_VERTEX_4 0x0D98
#define GL_MAP2_COLOR_4 0x0DB0
#define GL_MAP2_INDEX 0x0DB1
#define GL_MAP2_NORMAL 0x0DB2
#define GL_MAP2_TEXTURE_COORD_1 0x0DB3
#define GL_MAP2_TEXTURE_COORD_2 0x0DB4
#define GL_MAP2_TEXTURE_COORD_3 0x0DB5
#define GL_MAP2_TEXTURE_COORD_4 0x0DB6
#define GL_MAP2_VERTEX_3 0x0DB7
#define GL_MAP2_VERTEX_4 0x0DB8
#define GL_MAP1_GRID_DOMAIN 0x0DD0
#define GL_MAP1_GRID_SEGMENTS 0x0DD1
#define GL_MAP2_GRID_DOMAIN 0x0DD2
#define GL_MAP2_GRID_SEGMENTS 0x0DD3
#define GL_TEXTURE_COMPONENTS 0x1003
#define GL_TEXTURE_BORDER 0x1005
#define GL_AMBIENT 0x1200
#define GL_DIFFUSE 0x1201
#define GL_SPECULAR 0x1202
#define GL_POSITION 0x1203
#define GL_SPOT_DIRECTION 0x1204
#define GL_SPOT_EXPONENT 0x1205
#define GL_SPOT_CUTOFF 0x1206
#define GL_CONSTANT_ATTENUATION 0x1207
#define GL_LINEAR_ATTENUATION 0x1208
#define GL_QUADRATIC_ATTENUATION 0x1209
#define GL_COMPILE 0x1300
#define GL_COMPILE_AND_EXECUTE 0x1301
#define GL_2_BYTES 0x1407
#define GL_3_BYTES 0x1408
#define GL_4_BYTES 0x1409
#define GL_EMISSION 0x1600
#define GL_SHININESS 0x1601
#define GL_AMBIENT_AND_DIFFUSE 0x1602
#define GL_COLOR_INDEXES 0x1603
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_COLOR_INDEX 0x1900
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_BITMAP 0x1A00
#define GL_RENDER 0x1C00
#define GL_FEEDBACK 0x1C01
#define GL_SELECT 0x1C02
#define GL_FLAT 0x1D00
#define GL_SMOOTH 0x1D01
#define GL_S 0x2000
#define GL_T 0x2001
#define GL_R 0x2002
#define GL_Q 0x2003
#define GL_MODULATE 0x2100
#define GL_DECAL 0x2101
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_TEXTURE_ENV_COLOR 0x2201
#define GL_TEXTURE_ENV 0x2300
#define GL_EYE_LINEAR 0x2400
#define GL_OBJECT_LINEAR 0x2401
#define GL_SPHERE_MAP 0x2402
#define GL_TEXTURE_GEN_MODE 0x2500
#define GL_OBJECT_PLANE 0x2501
#define GL_EYE_PLANE 0x2502
#define GL_CLAMP 0x2900
#define GL_CLIP_PLANE0 0x3000
#define GL_CLIP_PLANE1 0x3001
#define GL_CLIP_PLANE2 0x3002
#define GL_CLIP_PLANE3 0x3003
#define GL_CLIP_PLANE4 0x3004
#define GL_CLIP_PLANE5 0x3005
#define GL_LIGHT0 0x4000
#define GL_LIGHT1 0x4001
#define GL_LIGHT2 0x4002
#define GL_LIGHT3 0x4003
#define GL_LIGHT4 0x4004
#define GL_LIGHT5 0x4005
#define GL_LIGHT6 0x4006
#define GL_LIGHT7 0x4007
#define GL_COLOR_LOGIC_OP 0x0BF2
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_TEXTURE_BINDING_1D 0x8068
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_DOUBLE 0x140A
#define GL_PROXY_TEXTURE_1D 0x8063
#define GL_PROXY_TEXTURE_2D 0x8064
#define GL_R3_G3_B2 0x2A10
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB8 0x8051
#define GL_RGB10 0x8052
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGBA2 0x8055
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGBA8 0x8058
#define GL_RGB10_A2 0x8059
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_CLIENT_PIXEL_STORE_BIT 0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS 0xFFFFFFFF
#define GL_VERTEX_ARRAY_POINTER 0x808E
#define GL_NORMAL_ARRAY_POINTER 0x808F
#define GL_COLOR_ARRAY_POINTER 0x8090
#define GL_INDEX_ARRAY_POINTER 0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER 0x8093
#define GL_FEEDBACK_BUFFER_POINTER 0x0DF0
#define GL_SELECTION_BUFFER_POINTER 0x0DF3
#define GL_CLIENT_ATTRIB_STACK_DEPTH 0x0BB1
#define GL_INDEX_LOGIC_OP 0x0BF1
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH 0x0D3B
#define GL_FEEDBACK_BUFFER_SIZE 0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE 0x0DF2
#define GL_SELECTION_BUFFER_SIZE 0x0DF4
#define GL_VERTEX_ARRAY 0x8074
#define GL_NORMAL_ARRAY 0x8075
#define GL_COLOR_ARRAY 0x8076
#define GL_INDEX_ARRAY 0x8077
#define GL_TEXTURE_COORD_ARRAY 0x8078
#define GL_EDGE_FLAG_ARRAY 0x8079
#define GL_VERTEX_ARRAY_SIZE 0x807A
#define GL_VERTEX_ARRAY_TYPE 0x807B
#define GL_VERTEX_ARRAY_STRIDE 0x807C
#define GL_NORMAL_ARRAY_TYPE 0x807E
#define GL_NORMAL_ARRAY_STRIDE 0x807F
#define GL_COLOR_ARRAY_SIZE 0x8081
#define GL_COLOR_ARRAY_TYPE 0x8082
#define GL_COLOR_ARRAY_STRIDE 0x8083
#define GL_INDEX_ARRAY_TYPE 0x8085
#define GL_INDEX_ARRAY_STRIDE 0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE 0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE 0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE 0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE 0x808C
#define GL_TEXTURE_LUMINANCE_SIZE 0x8060
#define GL_TEXTURE_INTENSITY_SIZE 0x8061
#define GL_TEXTURE_PRIORITY 0x8066
#define GL_TEXTURE_RESIDENT 0x8067
#define GL_ALPHA4 0x803B
#define GL_ALPHA8 0x803C
#define GL_ALPHA12 0x803D
#define GL_ALPHA16 0x803E
#define GL_LUMINANCE4 0x803F
#define GL_LUMINANCE8 0x8040
#define GL_LUMINANCE12 0x8041
#define GL_LUMINANCE16 0x8042
#define GL_LUMINANCE4_ALPHA4 0x8043
#define GL_LUMINANCE6_ALPHA2 0x8044
#define GL_LUMINANCE8_ALPHA8 0x8045
#define GL_LUMINANCE12_ALPHA4 0x8046
#define GL_LUMINANCE12_ALPHA12 0x8047
#define GL_LUMINANCE16_ALPHA16 0x8048
#define GL_INTENSITY 0x8049
#define GL_INTENSITY4 0x804A
#define GL_INTENSITY8 0x804B
#define GL_INTENSITY12 0x804C
#define GL_INTENSITY16 0x804D
#define GL_V2F 0x2A20
#define GL_V3F 0x2A21
#define GL_C4UB_V2F 0x2A22
#define GL_C4UB_V3F 0x2A23
#define GL_C3F_V3F 0x2A24
#define GL_N3F_V3F 0x2A25
#define GL_C4F_N3F_V3F 0x2A26
#define GL_T2F_V3F 0x2A27
#define GL_T4F_V4F 0x2A28
#define GL_T2F_C4UB_V3F 0x2A29
#define GL_T2F_C3F_V3F 0x2A2A
#define GL_T2F_N3F_V3F 0x2A2B
#define GL_T2F_C4F_N3F_V3F 0x2A2C
#define GL_T4F_C4F_N3F_V4F 0x2A2D
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_TEXTURE_3D 0x806F
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_RESCALE_NORMAL 0x803A
#define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
#define GL_SINGLE_COLOR 0x81F9
#define GL_SEPARATE_SPECULAR_COLOR 0x81FA
#define GL_ALIASED_POINT_SIZE_RANGE 0x846D
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_MULTISAMPLE 0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE 0x809F
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLES 0x80A9
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP 0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_COMPRESSED_RGB 0x84ED
#define GL_COMPRESSED_RGBA 0x84EE
#define GL_TEXTURE_COMPRESSION_HINT 0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
#define GL_TEXTURE_COMPRESSED 0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLIENT_ACTIVE_TEXTURE 0x84E1
#define GL_MAX_TEXTURE_UNITS 0x84E2
#define GL_TRANSPOSE_MODELVIEW_MATRIX 0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX 0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX 0x84E6
#define GL_MULTISAMPLE_BIT 0x20000000
#define GL_NORMAL_MAP 0x8511
#define GL_REFLECTION_MAP 0x8512
#define GL_COMPRESSED_ALPHA 0x84E9
#define GL_COMPRESSED_LUMINANCE 0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA 0x84EB
#define GL_COMPRESSED_INTENSITY 0x84EC
#define GL_COMBINE 0x8570
#define GL_COMBINE_RGB 0x8571
#define GL_COMBINE_ALPHA 0x8572
#define GL_SOURCE0_RGB 0x8580
#define GL_SOURCE1_RGB 0x8581
#define GL_SOURCE2_RGB 0x8582
#define GL_SOURCE0_ALPHA 0x8588
#define GL_SOURCE1_ALPHA 0x8589
#define GL_SOURCE2_ALPHA 0x858A
#define GL_OPERAND0_RGB 0x8590
#define GL_OPERAND1_RGB 0x8591
#define GL_OPERAND2_RGB 0x8592
#define GL_OPERAND0_ALPHA 0x8598
#define GL_OPERAND1_ALPHA 0x8599
#define GL_OPERAND2_ALPHA 0x859A
#define GL_RGB_SCALE 0x8573
#define GL_ADD_SIGNED 0x8574
#define GL_INTERPOLATE 0x8575
#define GL_SUBTRACT 0x84E7
#define GL_CONSTANT 0x8576
#define GL_PRIMARY_COLOR 0x8577
#define GL_PREVIOUS 0x8578
#define GL_DOT3_RGB 0x86AE
#define GL_DOT3_RGBA 0x86AF
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_MIRRORED_REPEAT 0x8370
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_POINT_SIZE_MIN 0x8126
#define GL_POINT_SIZE_MAX 0x8127
#define GL_POINT_DISTANCE_ATTENUATION 0x8129
#define GL_GENERATE_MIPMAP 0x8191
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_FOG_COORDINATE_SOURCE 0x8450
#define GL_FOG_COORDINATE 0x8451
#define GL_FRAGMENT_DEPTH 0x8452
#define GL_CURRENT_FOG_COORDINATE 0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE 0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE 0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER 0x8456
#define GL_FOG_COORDINATE_ARRAY 0x8457
#define GL_COLOR_SUM 0x8458
#define GL_CURRENT_SECONDARY_COLOR 0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE 0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE 0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE 0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER 0x845D
#define GL_SECONDARY_COLOR_ARRAY 0x845E
#define GL_TEXTURE_FILTER_CONTROL 0x8500
#define GL_DEPTH_TEXTURE_MODE 0x884B
#define GL_COMPARE_R_TO_TEXTURE 0x884E
#define GL_BLEND_COLOR 0x8005
#define GL_BLEND_EQUATION 0x8009
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_FUNC_SUBTRACT 0x800A
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_CURRENT_QUERY 0x8865
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_BUFFER_ACCESS 0x88BB
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_SAMPLES_PASSED 0x8914
#define GL_SRC1_ALPHA 0x8589
#define GL_VERTEX_ARRAY_BUFFER_BINDING 0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING 0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING 0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING 0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING 0x889E
#define GL_FOG_COORD_SRC 0x8450
#define GL_FOG_COORD 0x8451
#define GL_CURRENT_FOG_COORD 0x8453
#define GL_FOG_COORD_ARRAY_TYPE 0x8454
#define GL_FOG_COORD_ARRAY_STRIDE 0x8455
#define GL_FOG_COORD_ARRAY_POINTER 0x8456
#define GL_FOG_COORD_ARRAY 0x8457
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING 0x889D
#define GL_SRC0_RGB 0x8580
#define GL_SRC1_RGB 0x8581
#define GL_SRC2_RGB 0x8582
#define GL_SRC0_ALPHA 0x8588
#define GL_SRC2_ALPHA 0x858A
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_DRAW_BUFFER14 0x8833
#define GL_DRAW_BUFFER15 0x8834
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_MAX_VARYING_FLOATS 0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE 0x8B4F
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_3D 0x8B5F
#define GL_SAMPLER_CUBE 0x8B60
#define GL_SAMPLER_1D_SHADOW 0x8B61
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_DELETE_STATUS 0x8B80
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#define GL_LOWER_LEFT 0x8CA1
#define GL_UPPER_LEFT 0x8CA2
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_VERTEX_PROGRAM_TWO_SIDE 0x8643
#define GL_POINT_SPRITE 0x8861
#define GL_COORD_REPLACE 0x8862
#define GL_MAX_TEXTURE_COORDS 0x8871
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB_ALPHA 0x8C42
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_COMPRESSED_SRGB 0x8C48
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
#define GL_CURRENT_RASTER_SECONDARY_COLOR 0x845F
#define GL_SLUMINANCE_ALPHA 0x8C44
#define GL_SLUMINANCE8_ALPHA8 0x8C45
#define GL_SLUMINANCE 0x8C46
#define GL_SLUMINANCE8 0x8C47
#define GL_COMPRESSED_SLUMINANCE 0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA 0x8C4B
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#define GL_CLIP_DISTANCE0 0x3000
#define GL_CLIP_DISTANCE1 0x3001
#define GL_CLIP_DISTANCE2 0x3002
#define GL_CLIP_DISTANCE3 0x3003
#define GL_CLIP_DISTANCE4 0x3004
#define GL_CLIP_DISTANCE5 0x3005
#define GL_CLIP_DISTANCE6 0x3006
#define GL_CLIP_DISTANCE7 0x3007
#define GL_MAX_CLIP_DISTANCES 0x0D32
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_NUM_EXTENSIONS 0x821D
#define GL_CONTEXT_FLAGS 0x821E
#define GL_COMPRESSED_RED 0x8225
#define GL_COMPRESSED_RG 0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815
#define GL_RGBA16F 0x881A
#define GL_RGB16F 0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
#define GL_CLAMP_READ_COLOR 0x891C
#define GL_FIXED_ONLY 0x891D
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#define GL_TEXTURE_1D_ARRAY 0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY 0x8C19
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY 0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY 0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_RGB9_E5 0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
#define GL_TEXTURE_SHARED_SIZE 0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED 0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS 0x8C8C
#define GL_SEPARATE_ATTRIBS 0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RED_INTEGER 0x8D94
#define GL_GREEN_INTEGER 0x8D95
#define GL_BLUE_INTEGER 0x8D96
#define GL_RGB_INTEGER 0x8D98
#define GL_RGBA_INTEGER 0x8D99
#define GL_BGR_INTEGER 0x8D9A
#define GL_BGRA_INTEGER 0x8D9B
#define GL_SAMPLER_1D_ARRAY 0x8DC0
#define GL_SAMPLER_2D_ARRAY 0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW 0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
#define GL_SAMPLER_CUBE_SHADOW 0x8DC5
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_INT_SAMPLER_1D 0x8DC9
#define GL_INT_SAMPLER_2D 0x8DCA
#define GL_INT_SAMPLER_3D 0x8DCB
#define GL_INT_SAMPLER_CUBE 0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY 0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D 0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
#define GL_QUERY_WAIT 0x8E13
#define GL_QUERY_NO_WAIT 0x8E14
#define GL_QUERY_BY_REGION_WAIT 0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT 0x8E16
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#define GL_BUFFER_MAP_LENGTH 0x9120
#define GL_BUFFER_MAP_OFFSET 0x9121
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT 0x8218
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_DEPTH_STENCIL 0x84F9
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_TEXTURE_STENCIL_SIZE 0x88F1
#define GL_TEXTURE_RED_TYPE 0x8C10
#define GL_TEXTURE_GREEN_TYPE 0x8C11
#define GL_TEXTURE_BLUE_TYPE 0x8C12
#define GL_TEXTURE_ALPHA_TYPE 0x8C13
#define GL_TEXTURE_DEPTH_TYPE 0x8C16
#define GL_UNSIGNED_NORMALIZED 0x8C17
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_RENDERBUFFER_SAMPLES 0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_STENCIL_INDEX1 0x8D46
#define GL_STENCIL_INDEX4 0x8D47
#define GL_STENCIL_INDEX8 0x8D48
#define GL_STENCIL_INDEX16 0x8D49
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES 0x8D57
#define GL_INDEX 0x8222
#define GL_TEXTURE_LUMINANCE_TYPE 0x8C14
#define GL_TEXTURE_INTENSITY_TYPE 0x8C15
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_HALF_FLOAT 0x140B
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_CLAMP_VERTEX_COLOR 0x891A
#define GL_CLAMP_FRAGMENT_COLOR 0x891B
#define GL_ALPHA_INTEGER 0x8D97
#define GL_SAMPLER_2D_RECT 0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
#define GL_SAMPLER_BUFFER 0x8DC2
#define GL_INT_SAMPLER_2D_RECT 0x8DCD
#define GL_INT_SAMPLER_BUFFER 0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
#define GL_TEXTURE_BINDING_BUFFER 0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGB8_SNORM 0x8F96
#define GL_RGBA8_SNORM 0x8F97
#define GL_R16_SNORM 0x8F98
#define GL_RG16_SNORM 0x8F99
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGBA16_SNORM 0x8F9B
#define GL_SIGNED_NORMALIZED 0x8F9C
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS 0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS 0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS 0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_UNIFORM_TYPE 0x8A37
#define GL_UNIFORM_SIZE 0x8A38
#define GL_UNIFORM_NAME_LENGTH 0x8A39
#define GL_UNIFORM_BLOCK_INDEX 0x8A3A
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE 0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE 0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR 0x8A3E
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH 0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX 0xFFFFFFFF
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY 0x000A
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_GEOMETRY_VERTICES_OUT 0x8916
#define GL_GEOMETRY_INPUT_TYPE 0x8917
#define GL_GEOMETRY_OUTPUT_TYPE 0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_DEPTH_CLAMP 0x864F
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION 0x8E4D
#define GL_LAST_VERTEX_CONVENTION 0x8E4E
#define GL_PROVOKING_VERTEX 0x8E4F
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#define GL_MAX_SERVER_WAIT_TIMEOUT 0x9111
#define GL_OBJECT_TYPE 0x9112
#define GL_SYNC_CONDITION 0x9113
#define GL_SYNC_STATUS 0x9114
#define GL_SYNC_FLAGS 0x9115
#define GL_SYNC_FENCE 0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_UNSIGNALED 0x9118
#define GL_SIGNALED 0x9119
#define GL_ALREADY_SIGNALED 0x911A
#define GL_TIMEOUT_EXPIRED 0x911B
#define GL_CONDITION_SATISFIED 0x911C
#define GL_WAIT_FAILED 0x911D
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFF
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#define GL_SAMPLE_POSITION 0x8E50
#define GL_SAMPLE_MASK 0x8E51
#define GL_SAMPLE_MASK_VALUE 0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE 0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES 0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE 0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
#define GL_MAX_INTEGER_SAMPLES 0x9110
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
#define GL_SRC1_COLOR 0x88F9
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS 0x88FC
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_SAMPLER_BINDING 0x8919
#define GL_RGB10_A2UI 0x906F
#define GL_TEXTURE_SWIZZLE_R 0x8E42
#define GL_TEXTURE_SWIZZLE_G 0x8E43
#define GL_TEXTURE_SWIZZLE_B 0x8E44
#define GL_TEXTURE_SWIZZLE_A 0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA 0x8E46
#define GL_TIME_ELAPSED 0x88BF
#define GL_TIMESTAMP 0x8E28
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_SAMPLE_SHADING 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE 0x8C37
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_TEXTURE_CUBE_MAP_ARRAY 0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY 0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY 0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
#define GL_DRAW_INDIRECT_BUFFER 0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING 0x8F43
#define GL_GEOMETRY_SHADER_INVOCATIONS 0x887F
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
#define GL_MAX_VERTEX_STREAMS 0x8E71
#define GL_DOUBLE_VEC2 0x8FFC
#define GL_DOUBLE_VEC3 0x8FFD
#define GL_DOUBLE_VEC4 0x8FFE
#define GL_DOUBLE_MAT2 0x8F46
#define GL_DOUBLE_MAT3 0x8F47
#define GL_DOUBLE_MAT4 0x8F48
#define GL_DOUBLE_MAT2x3 0x8F49
#define GL_DOUBLE_MAT2x4 0x8F4A
#define GL_DOUBLE_MAT3x2 0x8F4B
#define GL_DOUBLE_MAT3x4 0x8F4C
#define GL_DOUBLE_MAT4x2 0x8F4D
#define GL_DOUBLE_MAT4x3 0x8F4E
#define GL_ACTIVE_SUBROUTINES 0x8DE5
#define GL_ACTIVE_SUBROUTINE_UNIFORMS 0x8DE6
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH 0x8E48
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_MAX_SUBROUTINES 0x8DE7
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_NUM_COMPATIBLE_SUBROUTINES 0x8E4A
#define GL_COMPATIBLE_SUBROUTINES 0x8E4B
#define GL_PATCHES 0x000E
#define GL_PATCH_VERTICES 0x8E72
#define GL_PATCH_DEFAULT_INNER_LEVEL 0x8E73
#define GL_PATCH_DEFAULT_OUTER_LEVEL 0x8E74
#define GL_TESS_CONTROL_OUTPUT_VERTICES 0x8E75
#define GL_TESS_GEN_MODE 0x8E76
#define GL_TESS_GEN_SPACING 0x8E77
#define GL_TESS_GEN_VERTEX_ORDER 0x8E78
#define GL_TESS_GEN_POINT_MODE 0x8E79
#define GL_ISOLINES 0x8E7A
#define GL_FRACTIONAL_ODD 0x8E7B
#define GL_FRACTIONAL_EVEN 0x8E7C
#define GL_MAX_PATCH_VERTICES 0x8E7D
#define GL_MAX_TESS_GEN_LEVEL 0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS 0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_TESS_EVALUATION_SHADER 0x8E87
#define GL_TESS_CONTROL_SHADER 0x8E88
#define GL_TRANSFORM_FEEDBACK 0x8E22
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED 0x8E23
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_BINDING 0x8E25
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
#define GL_FIXED 0x140C
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_LOW_FLOAT 0x8DF0
#define GL_MEDIUM_FLOAT 0x8DF1
#define GL_HIGH_FLOAT 0x8DF2
#define GL_LOW_INT 0x8DF3
#define GL_MEDIUM_INT 0x8DF4
#define GL_HIGH_INT 0x8DF5
#define GL_SHADER_COMPILER 0x8DFA
#define GL_SHADER_BINARY_FORMATS 0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_RGB565 0x8D62
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_PROGRAM_BINARY_LENGTH 0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS 0x87FE
#define GL_PROGRAM_BINARY_FORMATS 0x87FF
#define GL_VERTEX_SHADER_BIT 0x00000001
#define GL_FRAGMENT_SHADER_BIT 0x00000002
#define GL_GEOMETRY_SHADER_BIT 0x00000004
#define GL_TESS_CONTROL_SHADER_BIT 0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT 0x00000010
#define GL_ALL_SHADER_BITS 0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE 0x8258
#define GL_ACTIVE_PROGRAM 0x8259
#define GL_PROGRAM_PIPELINE_BINDING 0x825A
#define GL_MAX_VIEWPORTS 0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS 0x825C
#define GL_VIEWPORT_BOUNDS_RANGE 0x825D
#define GL_LAYER_PROVOKING_VERTEX 0x825E
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX 0x825F
#define GL_UNDEFINED_VERTEX 0x8260
#define GL_COPY_READ_BUFFER_BINDING 0x8F36
#define GL_COPY_WRITE_BUFFER_BINDING 0x8F37
#define GL_TRANSFORM_FEEDBACK_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_PAUSED 0x8E23
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH 0x9127
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH 0x9129
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE 0x912A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH 0x912B
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT 0x912C
#define GL_PACK_COMPRESSED_BLOCK_DEPTH 0x912D
#define GL_PACK_COMPRESSED_BLOCK_SIZE 0x912E
#define GL_NUM_SAMPLE_COUNTS 0x9380
#define GL_MIN_MAP_BUFFER_ALIGNMENT 0x90BC
#define GL_ATOMIC_COUNTER_BUFFER 0x92C0
#define GL_ATOMIC_COUNTER_BUFFER_BINDING 0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START 0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE 0x92C3
#define GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE 0x92C4
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS 0x92C5
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES 0x92C6
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER 0x92C7
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER 0x92C8
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER 0x92C9
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER 0x92CA
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER 0x92CB
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS 0x92CE
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS 0x92CF
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS 0x92D2
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS 0x92D4
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS 0x92D5
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS 0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS 0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS 0x92D9
#define GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX 0x92DA
#define GL_UNSIGNED_INT_ATOMIC_COUNTER 0x92DB
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT 0x00000002
#define GL_UNIFORM_BARRIER_BIT 0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT 0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT 0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT 0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT 0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT 0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT 0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT 0x00001000
#define GL_ALL_BARRIER_BITS 0xFFFFFFFF
#define GL_MAX_IMAGE_UNITS 0x8F38
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS 0x8F39
#define GL_IMAGE_BINDING_NAME 0x8F3A
#define GL_IMAGE_BINDING_LEVEL 0x8F3B
#define GL_IMAGE_BINDING_LAYERED 0x8F3C
#define GL_IMAGE_BINDING_LAYER 0x8F3D
#define GL_IMAGE_BINDING_ACCESS 0x8F3E
#define GL_IMAGE_1D 0x904C
#define GL_IMAGE_2D 0x904D
#define GL_IMAGE_3D 0x904E
#define GL_IMAGE_2D_RECT 0x904F
#define GL_IMAGE_CUBE 0x9050
#define GL_IMAGE_BUFFER 0x9051
#define GL_IMAGE_1D_ARRAY 0x9052
#define GL_IMAGE_2D_ARRAY 0x9053
#define GL_IMAGE_CUBE_MAP_ARRAY 0x9054
#define GL_IMAGE_2D_MULTISAMPLE 0x9055
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY 0x9056
#define GL_INT_IMAGE_1D 0x9057
#define GL_INT_IMAGE_2D 0x9058
#define GL_INT_IMAGE_3D 0x9059
#define GL_INT_IMAGE_2D_RECT 0x905A
#define GL_INT_IMAGE_CUBE 0x905B
#define GL_INT_IMAGE_BUFFER 0x905C
#define GL_INT_IMAGE_1D_ARRAY 0x905D
#define GL_INT_IMAGE_2D_ARRAY 0x905E
#define GL_INT_IMAGE_CUBE_MAP_ARRAY 0x905F
#define GL_INT_IMAGE_2D_MULTISAMPLE 0x9060
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x9061
#define GL_UNSIGNED_INT_IMAGE_1D 0x9062
#define GL_UNSIGNED_INT_IMAGE_2D 0x9063
#define GL_UNSIGNED_INT_IMAGE_3D 0x9064
#define GL_UNSIGNED_INT_IMAGE_2D_RECT 0x9065
#define GL_UNSIGNED_INT_IMAGE_CUBE 0x9066
#define GL_UNSIGNED_INT_IMAGE_BUFFER 0x9067
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY 0x9068
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY 0x9069
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY 0x906A
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 0x906B
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x906C
#define GL_MAX_IMAGE_SAMPLES 0x906D
#define GL_IMAGE_BINDING_FORMAT 0x906E
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_MAX_VERTEX_IMAGE_UNIFORMS 0x90CA
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS 0x90CC
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS 0x90CD
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS 0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS 0x90CF
#define GL_COMPRESSED_RGBA_BPTC_UNORM 0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F
#define GL_TEXTURE_IMMUTABLE_FORMAT 0x912F
#define GL_NUM_SHADING_LANGUAGE_VERSIONS 0x82E9
#define GL_VERTEX_ATTRIB_ARRAY_LONG 0x874E
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#define GL_COMPRESSED_SRGB8_ETC2 0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_COMPRESSED_R11_EAC 0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC 0x9271
#define GL_COMPRESSED_RG11_EAC 0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC 0x9273
#define GL_PRIMITIVE_RESTART_FIXED_INDEX 0x8D69
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_MAX_ELEMENT_INDEX 0x8D6B
#define GL_COMPUTE_SHADER 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS 0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS 0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS 0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT 0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE 0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE 0x8267
#define GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER 0x90EC
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER 0x90ED
#define GL_DISPATCH_INDIRECT_BUFFER 0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT 0x00000020
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
#define GL_BUFFER 0x82E0
#define GL_SHADER 0x82E1
#define GL_PROGRAM 0x82E2
#define GL_QUERY 0x82E3
#define GL_PROGRAM_PIPELINE 0x82E4
#define GL_SAMPLER 0x82E6
#define GL_MAX_LABEL_LENGTH 0x82E8
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_MAX_UNIFORM_LOCATIONS 0x826E
#define GL_FRAMEBUFFER_DEFAULT_WIDTH 0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT 0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS 0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES 0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH 0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT 0x9316
#define GL_MAX_FRAMEBUFFER_LAYERS 0x9317
#define GL_MAX_FRAMEBUFFER_SAMPLES 0x9318
#define GL_INTERNALFORMAT_SUPPORTED 0x826F
#define GL_INTERNALFORMAT_PREFERRED 0x8270
#define GL_INTERNALFORMAT_RED_SIZE 0x8271
#define GL_INTERNALFORMAT_GREEN_SIZE 0x8272
#define GL_INTERNALFORMAT_BLUE_SIZE 0x8273
#define GL_INTERNALFORMAT_ALPHA_SIZE 0x8274
#define GL_INTERNALFORMAT_DEPTH_SIZE 0x8275
#define GL_INTERNALFORMAT_STENCIL_SIZE 0x8276
#define GL_INTERNALFORMAT_SHARED_SIZE 0x8277
#define GL_INTERNALFORMAT_RED_TYPE 0x8278
#define GL_INTERNALFORMAT_GREEN_TYPE 0x8279
#define GL_INTERNALFORMAT_BLUE_TYPE 0x827A
#define GL_INTERNALFORMAT_ALPHA_TYPE 0x827B
#define GL_INTERNALFORMAT_DEPTH_TYPE 0x827C
#define GL_INTERNALFORMAT_STENCIL_TYPE 0x827D
#define GL_MAX_WIDTH 0x827E
#define GL_MAX_HEIGHT 0x827F
#define GL_MAX_DEPTH 0x8280
#define GL_MAX_LAYERS 0x8281
#define GL_MAX_COMBINED_DIMENSIONS 0x8282
#define GL_COLOR_COMPONENTS 0x8283
#define GL_DEPTH_COMPONENTS 0x8284
#define GL_STENCIL_COMPONENTS 0x8285
#define GL_COLOR_RENDERABLE 0x8286
#define GL_DEPTH_RENDERABLE 0x8287
#define GL_STENCIL_RENDERABLE 0x8288
#define GL_FRAMEBUFFER_RENDERABLE 0x8289
#define GL_FRAMEBUFFER_RENDERABLE_LAYERED 0x828A
#define GL_FRAMEBUFFER_BLEND 0x828B
#define GL_READ_PIXELS 0x828C
#define GL_READ_PIXELS_FORMAT 0x828D
#define GL_READ_PIXELS_TYPE 0x828E
#define GL_TEXTURE_IMAGE_FORMAT 0x828F
#define GL_TEXTURE_IMAGE_TYPE 0x8290
#define GL_GET_TEXTURE_IMAGE_FORMAT 0x8291
#define GL_GET_TEXTURE_IMAGE_TYPE 0x8292
#define GL_MIPMAP 0x8293
#define GL_MANUAL_GENERATE_MIPMAP 0x8294
#define GL_AUTO_GENERATE_MIPMAP 0x8295
#define GL_COLOR_ENCODING 0x8296
#define GL_SRGB_READ 0x8297
#define GL_SRGB_WRITE 0x8298
#define GL_FILTER 0x829A
#define GL_VERTEX_TEXTURE 0x829B
#define GL_TESS_CONTROL_TEXTURE 0x829C
#define GL_TESS_EVALUATION_TEXTURE 0x829D
#define GL_GEOMETRY_TEXTURE 0x829E
#define GL_FRAGMENT_TEXTURE 0x829F
#define GL_COMPUTE_TEXTURE 0x82A0
#define GL_TEXTURE_SHADOW 0x82A1
#define GL_TEXTURE_GATHER 0x82A2
#define GL_TEXTURE_GATHER_SHADOW 0x82A3
#define GL_SHADER_IMAGE_LOAD 0x82A4
#define GL_SHADER_IMAGE_STORE 0x82A5
#define GL_SHADER_IMAGE_ATOMIC 0x82A6
#define GL_IMAGE_TEXEL_SIZE 0x82A7
#define GL_IMAGE_COMPATIBILITY_CLASS 0x82A8
#define GL_IMAGE_PIXEL_FORMAT 0x82A9
#define GL_IMAGE_PIXEL_TYPE 0x82AA
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST 0x82AC
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST 0x82AD
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE 0x82AE
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE 0x82AF
#define GL_TEXTURE_COMPRESSED_BLOCK_WIDTH 0x82B1
#define GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT 0x82B2
#define GL_TEXTURE_COMPRESSED_BLOCK_SIZE 0x82B3
#define GL_CLEAR_BUFFER 0x82B4
#define GL_TEXTURE_VIEW 0x82B5
#define GL_VIEW_COMPATIBILITY_CLASS 0x82B6
#define GL_FULL_SUPPORT 0x82B7
#define GL_CAVEAT_SUPPORT 0x82B8
#define GL_IMAGE_CLASS_4_X_32 0x82B9
#define GL_IMAGE_CLASS_2_X_32 0x82BA
#define GL_IMAGE_CLASS_1_X_32 0x82BB
#define GL_IMAGE_CLASS_4_X_16 0x82BC
#define GL_IMAGE_CLASS_2_X_16 0x82BD
#define GL_IMAGE_CLASS_1_X_16 0x82BE
#define GL_IMAGE_CLASS_4_X_8 0x82BF
#define GL_IMAGE_CLASS_2_X_8 0x82C0
#define GL_IMAGE_CLASS_1_X_8 0x82C1
#define GL_IMAGE_CLASS_11_11_10 0x82C2
#define GL_IMAGE_CLASS_10_10_10_2 0x82C3
#define GL_VIEW_CLASS_128_BITS 0x82C4
#define GL_VIEW_CLASS_96_BITS 0x82C5
#define GL_VIEW_CLASS_64_BITS 0x82C6
#define GL_VIEW_CLASS_48_BITS 0x82C7
#define GL_VIEW_CLASS_32_BITS 0x82C8
#define GL_VIEW_CLASS_24_BITS 0x82C9
#define GL_VIEW_CLASS_16_BITS 0x82CA
#define GL_VIEW_CLASS_8_BITS 0x82CB
#define GL_VIEW_CLASS_S3TC_DXT1_RGB 0x82CC
#define GL_VIEW_CLASS_S3TC_DXT1_RGBA 0x82CD
#define GL_VIEW_CLASS_S3TC_DXT3_RGBA 0x82CE
#define GL_VIEW_CLASS_S3TC_DXT5_RGBA 0x82CF
#define GL_VIEW_CLASS_RGTC1_RED 0x82D0
#define GL_VIEW_CLASS_RGTC2_RG 0x82D1
#define GL_VIEW_CLASS_BPTC_UNORM 0x82D2
#define GL_VIEW_CLASS_BPTC_FLOAT 0x82D3
#define GL_UNIFORM 0x92E1
#define GL_UNIFORM_BLOCK 0x92E2
#define GL_PROGRAM_INPUT 0x92E3
#define GL_PROGRAM_OUTPUT 0x92E4
#define GL_BUFFER_VARIABLE 0x92E5
#define GL_SHADER_STORAGE_BLOCK 0x92E6
#define GL_VERTEX_SUBROUTINE 0x92E8
#define GL_TESS_CONTROL_SUBROUTINE 0x92E9
#define GL_TESS_EVALUATION_SUBROUTINE 0x92EA
#define GL_GEOMETRY_SUBROUTINE 0x92EB
#define GL_FRAGMENT_SUBROUTINE 0x92EC
#define GL_COMPUTE_SUBROUTINE 0x92ED
#define GL_VERTEX_SUBROUTINE_UNIFORM 0x92EE
#define GL_TESS_CONTROL_SUBROUTINE_UNIFORM 0x92EF
#define GL_TESS_EVALUATION_SUBROUTINE_UNIFORM 0x92F0
#define GL_GEOMETRY_SUBROUTINE_UNIFORM 0x92F1
#define GL_FRAGMENT_SUBROUTINE_UNIFORM 0x92F2
#define GL_COMPUTE_SUBROUTINE_UNIFORM 0x92F3
#define GL_TRANSFORM_FEEDBACK_VARYING 0x92F4
#define GL_ACTIVE_RESOURCES 0x92F5
#define GL_MAX_NAME_LENGTH 0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES 0x92F7
#define GL_MAX_NUM_COMPATIBLE_SUBROUTINES 0x92F8
#define GL_NAME_LENGTH 0x92F9
#define GL_TYPE 0x92FA
#define GL_ARRAY_SIZE 0x92FB
#define GL_OFFSET 0x92FC
#define GL_BLOCK_INDEX 0x92FD
#define GL_ARRAY_STRIDE 0x92FE
#define GL_MATRIX_STRIDE 0x92FF
#define GL_IS_ROW_MAJOR 0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX 0x9301
#define GL_BUFFER_BINDING 0x9302
#define GL_BUFFER_DATA_SIZE 0x9303
#define GL_NUM_ACTIVE_VARIABLES 0x9304
#define GL_ACTIVE_VARIABLES 0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER 0x9306
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER 0x9308
#define GL_REFERENCED_BY_GEOMETRY_SHADER 0x9309
#define GL_REFERENCED_BY_FRAGMENT_SHADER 0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER 0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE 0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE 0x930D
#define GL_LOCATION 0x930E
#define GL_LOCATION_INDEX 0x930F
#define GL_IS_PER_PATCH 0x92E7
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING 0x90D3
#define GL_SHADER_STORAGE_BUFFER_START 0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE 0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE 0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_DEPTH_STENCIL_TEXTURE_MODE 0x90EA
#define GL_TEXTURE_BUFFER_OFFSET 0x919D
#define GL_TEXTURE_BUFFER_SIZE 0x919E
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT 0x919F
#define GL_TEXTURE_VIEW_MIN_LEVEL 0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS 0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER 0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS 0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS 0x82DF
#define GL_VERTEX_ATTRIB_BINDING 0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D5
#define GL_VERTEX_BINDING_DIVISOR 0x82D6
#define GL_VERTEX_BINDING_OFFSET 0x82D7
#define GL_VERTEX_BINDING_STRIDE 0x82D8
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS 0x82DA
#define GL_VERTEX_BINDING_BUFFER 0x8F4F
#define GL_DISPLAY_LIST 0x82E7
#define GL_MAX_VERTEX_ATTRIB_STRIDE 0x82E5
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_TEXTURE_BUFFER_BINDING 0x8C2A
#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_MAP_COHERENT_BIT 0x0080
#define GL_DYNAMIC_STORAGE_BIT 0x0100
#define GL_CLIENT_STORAGE_BIT 0x0200
#define GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT 0x00004000
#define GL_BUFFER_IMMUTABLE_STORAGE 0x821F
#define GL_BUFFER_STORAGE_FLAGS 0x8220
#define GL_CLEAR_TEXTURE 0x9365
#define GL_LOCATION_COMPONENT 0x934A
#define GL_TRANSFORM_FEEDBACK_BUFFER_INDEX 0x934B
#define GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE 0x934C
#define GL_QUERY_BUFFER 0x9192
#define GL_QUERY_BUFFER_BARRIER_BIT 0x00008000
#define GL_QUERY_BUFFER_BINDING 0x9193
#define GL_QUERY_RESULT_NO_WAIT 0x9194
#define GL_MIRROR_CLAMP_TO_EDGE 0x8743
#define GL_CONTEXT_LOST 0x0507
#define GL_NEGATIVE_ONE_TO_ONE 0x935E
#define GL_ZERO_TO_ONE 0x935F
#define GL_CLIP_ORIGIN 0x935C
#define GL_CLIP_DEPTH_MODE 0x935D
#define GL_QUERY_WAIT_INVERTED 0x8E17
#define GL_QUERY_NO_WAIT_INVERTED 0x8E18
#define GL_QUERY_BY_REGION_WAIT_INVERTED 0x8E19
#define GL_QUERY_BY_REGION_NO_WAIT_INVERTED 0x8E1A
#define GL_MAX_CULL_DISTANCES 0x82F9
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES 0x82FA
#define GL_TEXTURE_TARGET 0x1006
#define GL_QUERY_TARGET 0x82EA
#define GL_GUILTY_CONTEXT_RESET 0x8253
#define GL_INNOCENT_CONTEXT_RESET 0x8254
#define GL_UNKNOWN_CONTEXT_RESET 0x8255
#define GL_RESET_NOTIFICATION_STRATEGY 0x8256
#define GL_LOSE_CONTEXT_ON_RESET 0x8252
#define GL_NO_RESET_NOTIFICATION 0x8261
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT 0x00000004
#define GL_COLOR_TABLE 0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE 0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE 0x80D2
#define GL_PROXY_COLOR_TABLE 0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE 0x80D5
#define GL_CONVOLUTION_1D 0x8010
#define GL_CONVOLUTION_2D 0x8011
#define GL_SEPARABLE_2D 0x8012
#define GL_HISTOGRAM 0x8024
#define GL_PROXY_HISTOGRAM 0x8025
#define GL_MINMAX 0x802E
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC

    // GL 1.0
    typedef void (*pfn_glCullFace)(GLenum mode);
    pfn_glCullFace glCullFace;
    typedef void (*pfn_glFrontFace)(GLenum mode);
    pfn_glFrontFace glFrontFace;
    typedef void (*pfn_glHint)(GLenum target, GLenum mode);
    pfn_glHint glHint;
    typedef void (*pfn_glLineWidth)(GLfloat width);
    pfn_glLineWidth glLineWidth;
    typedef void (*pfn_glPointSize)(GLfloat size);
    pfn_glPointSize glPointSize;
    typedef void (*pfn_glPolygonMode)(GLenum face, GLenum mode);
    pfn_glPolygonMode glPolygonMode;
    typedef void (*pfn_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glScissor glScissor;
    typedef void (*pfn_glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
    pfn_glTexParameterf glTexParameterf;
    typedef void (*pfn_glTexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
    pfn_glTexParameterfv glTexParameterfv;
    typedef void (*pfn_glTexParameteri)(GLenum target, GLenum pname, GLint param);
    pfn_glTexParameteri glTexParameteri;
    typedef void (*pfn_glTexParameteriv)(GLenum target, GLenum pname, const GLint *params);
    pfn_glTexParameteriv glTexParameteriv;
    typedef void (*pfn_glTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
    pfn_glTexImage1D glTexImage1D;
    typedef void (*pfn_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
    pfn_glTexImage2D glTexImage2D;
    typedef void (*pfn_glDrawBuffer)(GLenum buf);
    pfn_glDrawBuffer glDrawBuffer;
    typedef void (*pfn_glClear)(GLbitfield mask);
    pfn_glClear glClear;
    typedef void (*pfn_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    pfn_glClearColor glClearColor;
    typedef void (*pfn_glClearStencil)(GLint s);
    pfn_glClearStencil glClearStencil;
    typedef void (*pfn_glClearDepth)(GLdouble depth);
    pfn_glClearDepth glClearDepth;
    typedef void (*pfn_glStencilMask)(GLuint mask);
    pfn_glStencilMask glStencilMask;
    typedef void (*pfn_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    pfn_glColorMask glColorMask;
    typedef void (*pfn_glDepthMask)(GLboolean flag);
    pfn_glDepthMask glDepthMask;
    typedef void (*pfn_glDisable)(GLenum cap);
    pfn_glDisable glDisable;
    typedef void (*pfn_glEnable)(GLenum cap);
    pfn_glEnable glEnable;
    typedef void (*pfn_glFinish)(void);
    pfn_glFinish glFinish;
    typedef void (*pfn_glFlush)(void);
    pfn_glFlush glFlush;
    typedef void (*pfn_glBlendFunc)(GLenum sfactor, GLenum dfactor);
    pfn_glBlendFunc glBlendFunc;
    typedef void (*pfn_glLogicOp)(GLenum opcode);
    pfn_glLogicOp glLogicOp;
    typedef void (*pfn_glStencilFunc)(GLenum func, GLint ref, GLuint mask);
    pfn_glStencilFunc glStencilFunc;
    typedef void (*pfn_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
    pfn_glStencilOp glStencilOp;
    typedef void (*pfn_glDepthFunc)(GLenum func);
    pfn_glDepthFunc glDepthFunc;
    typedef void (*pfn_glPixelStoref)(GLenum pname, GLfloat param);
    pfn_glPixelStoref glPixelStoref;
    typedef void (*pfn_glPixelStorei)(GLenum pname, GLint param);
    pfn_glPixelStorei glPixelStorei;
    typedef void (*pfn_glReadBuffer)(GLenum src);
    pfn_glReadBuffer glReadBuffer;
    typedef void (*pfn_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
    pfn_glReadPixels glReadPixels;
    typedef void (*pfn_glGetBooleanv)(GLenum pname, GLboolean *data);
    pfn_glGetBooleanv glGetBooleanv;
    typedef void (*pfn_glGetDoublev)(GLenum pname, GLdouble *data);
    pfn_glGetDoublev glGetDoublev;
    typedef GLenum (*pfn_glGetError)(void);
    pfn_glGetError glGetError;
    typedef void (*pfn_glGetFloatv)(GLenum pname, GLfloat *data);
    pfn_glGetFloatv glGetFloatv;
    typedef void (*pfn_glGetIntegerv)(GLenum pname, GLint *data);
    pfn_glGetIntegerv glGetIntegerv;
    typedef const GLubyte * (*pfn_glGetString)(GLenum name);
    pfn_glGetString glGetString;
    typedef void (*pfn_glGetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
    pfn_glGetTexImage glGetTexImage;
    typedef void (*pfn_glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
    pfn_glGetTexParameterfv glGetTexParameterfv;
    typedef void (*pfn_glGetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
    pfn_glGetTexParameteriv glGetTexParameteriv;
    typedef void (*pfn_glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat *params);
    pfn_glGetTexLevelParameterfv glGetTexLevelParameterfv;
    typedef void (*pfn_glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint *params);
    pfn_glGetTexLevelParameteriv glGetTexLevelParameteriv;
    typedef GLboolean (*pfn_glIsEnabled)(GLenum cap);
    pfn_glIsEnabled glIsEnabled;
    typedef void (*pfn_glDepthRange)(GLdouble n, GLdouble f);
    pfn_glDepthRange glDepthRange;
    typedef void (*pfn_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glViewport glViewport;

    // GL 1.1
    typedef void (*pfn_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    pfn_glDrawArrays glDrawArrays;
    typedef void (*pfn_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);
    pfn_glDrawElements glDrawElements;
    typedef void (*pfn_glPolygonOffset)(GLfloat factor, GLfloat units);
    pfn_glPolygonOffset glPolygonOffset;
    typedef void (*pfn_glCopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
    pfn_glCopyTexImage1D glCopyTexImage1D;
    typedef void (*pfn_glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    pfn_glCopyTexImage2D glCopyTexImage2D;
    typedef void (*pfn_glCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
    pfn_glCopyTexSubImage1D glCopyTexSubImage1D;
    typedef void (*pfn_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glCopyTexSubImage2D glCopyTexSubImage2D;
    typedef void (*pfn_glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
    pfn_glTexSubImage1D glTexSubImage1D;
    typedef void (*pfn_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
    pfn_glTexSubImage2D glTexSubImage2D;
    typedef void (*pfn_glBindTexture)(GLenum target, GLuint texture);
    pfn_glBindTexture glBindTexture;
    typedef void (*pfn_glDeleteTextures)(GLsizei n, const GLuint *textures);
    pfn_glDeleteTextures glDeleteTextures;
    typedef void (*pfn_glGenTextures)(GLsizei n, GLuint *textures);
    pfn_glGenTextures glGenTextures;
    typedef GLboolean (*pfn_glIsTexture)(GLuint texture);
    pfn_glIsTexture glIsTexture;

    // GL 1.2
    typedef void (*pfn_glDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
    pfn_glDrawRangeElements glDrawRangeElements;
    typedef void (*pfn_glTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
    pfn_glTexImage3D glTexImage3D;
    typedef void (*pfn_glTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
    pfn_glTexSubImage3D glTexSubImage3D;
    typedef void (*pfn_glCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glCopyTexSubImage3D glCopyTexSubImage3D;

    // GL 1.3
    typedef void (*pfn_glActiveTexture)(GLenum texture);
    pfn_glActiveTexture glActiveTexture;
    typedef void (*pfn_glSampleCoverage)(GLfloat value, GLboolean invert);
    pfn_glSampleCoverage glSampleCoverage;
    typedef void (*pfn_glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
    pfn_glCompressedTexImage3D glCompressedTexImage3D;
    typedef void (*pfn_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
    pfn_glCompressedTexImage2D glCompressedTexImage2D;
    typedef void (*pfn_glCompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
    pfn_glCompressedTexImage1D glCompressedTexImage1D;
    typedef void (*pfn_glCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
    pfn_glCompressedTexSubImage3D glCompressedTexSubImage3D;
    typedef void (*pfn_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
    pfn_glCompressedTexSubImage2D glCompressedTexSubImage2D;
    typedef void (*pfn_glCompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
    pfn_glCompressedTexSubImage1D glCompressedTexSubImage1D;
    typedef void (*pfn_glGetCompressedTexImage)(GLenum target, GLint level, void *img);
    pfn_glGetCompressedTexImage glGetCompressedTexImage;

    // GL 1.4
    typedef void (*pfn_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
    pfn_glBlendFuncSeparate glBlendFuncSeparate;
    typedef void (*pfn_glMultiDrawArrays)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
    pfn_glMultiDrawArrays glMultiDrawArrays;
    typedef void (*pfn_glMultiDrawElements)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
    pfn_glMultiDrawElements glMultiDrawElements;
    typedef void (*pfn_glPointParameterf)(GLenum pname, GLfloat param);
    pfn_glPointParameterf glPointParameterf;
    typedef void (*pfn_glPointParameterfv)(GLenum pname, const GLfloat *params);
    pfn_glPointParameterfv glPointParameterfv;
    typedef void (*pfn_glPointParameteri)(GLenum pname, GLint param);
    pfn_glPointParameteri glPointParameteri;
    typedef void (*pfn_glPointParameteriv)(GLenum pname, const GLint *params);
    pfn_glPointParameteriv glPointParameteriv;
    typedef void (*pfn_glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    pfn_glBlendColor glBlendColor;
    typedef void (*pfn_glBlendEquation)(GLenum mode);
    pfn_glBlendEquation glBlendEquation;

    // GL 1.5
    typedef void (*pfn_glGenQueries)(GLsizei n, GLuint *ids);
    pfn_glGenQueries glGenQueries;
    typedef void (*pfn_glDeleteQueries)(GLsizei n, const GLuint *ids);
    pfn_glDeleteQueries glDeleteQueries;
    typedef GLboolean (*pfn_glIsQuery)(GLuint id);
    pfn_glIsQuery glIsQuery;
    typedef void (*pfn_glBeginQuery)(GLenum target, GLuint id);
    pfn_glBeginQuery glBeginQuery;
    typedef void (*pfn_glEndQuery)(GLenum target);
    pfn_glEndQuery glEndQuery;
    typedef void (*pfn_glGetQueryiv)(GLenum target, GLenum pname, GLint *params);
    pfn_glGetQueryiv glGetQueryiv;
    typedef void (*pfn_glGetQueryObjectiv)(GLuint id, GLenum pname, GLint *params);
    pfn_glGetQueryObjectiv glGetQueryObjectiv;
    typedef void (*pfn_glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint *params);
    pfn_glGetQueryObjectuiv glGetQueryObjectuiv;
    typedef void (*pfn_glBindBuffer)(GLenum target, GLuint buffer);
    pfn_glBindBuffer glBindBuffer;
    typedef void (*pfn_glDeleteBuffers)(GLsizei n, const GLuint *buffers);
    pfn_glDeleteBuffers glDeleteBuffers;
    typedef void (*pfn_glGenBuffers)(GLsizei n, GLuint *buffers);
    pfn_glGenBuffers glGenBuffers;
    typedef GLboolean (*pfn_glIsBuffer)(GLuint buffer);
    pfn_glIsBuffer glIsBuffer;
    typedef void (*pfn_glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
    pfn_glBufferData glBufferData;
    typedef void (*pfn_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
    pfn_glBufferSubData glBufferSubData;
    typedef void (*pfn_glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
    pfn_glGetBufferSubData glGetBufferSubData;
    typedef void * (*pfn_glMapBuffer)(GLenum target, GLenum access);
    pfn_glMapBuffer glMapBuffer;
    typedef GLboolean (*pfn_glUnmapBuffer)(GLenum target);
    pfn_glUnmapBuffer glUnmapBuffer;
    typedef void (*pfn_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
    pfn_glGetBufferParameteriv glGetBufferParameteriv;
    typedef void (*pfn_glGetBufferPointerv)(GLenum target, GLenum pname, void **params);
    pfn_glGetBufferPointerv glGetBufferPointerv;

    // GL 2.0
    typedef void (*pfn_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
    pfn_glBlendEquationSeparate glBlendEquationSeparate;
    typedef void (*pfn_glDrawBuffers)(GLsizei n, const GLenum *bufs);
    pfn_glDrawBuffers glDrawBuffers;
    typedef void (*pfn_glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
    pfn_glStencilOpSeparate glStencilOpSeparate;
    typedef void (*pfn_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
    pfn_glStencilFuncSeparate glStencilFuncSeparate;
    typedef void (*pfn_glStencilMaskSeparate)(GLenum face, GLuint mask);
    pfn_glStencilMaskSeparate glStencilMaskSeparate;
    typedef void (*pfn_glAttachShader)(GLuint program, GLuint shader);
    pfn_glAttachShader glAttachShader;
    typedef void (*pfn_glBindAttribLocation)(GLuint program, GLuint index, const GLchar *name);
    pfn_glBindAttribLocation glBindAttribLocation;
    typedef void (*pfn_glCompileShader)(GLuint shader);
    pfn_glCompileShader glCompileShader;
    typedef GLuint (*pfn_glCreateProgram)(void);
    pfn_glCreateProgram glCreateProgram;
    typedef GLuint (*pfn_glCreateShader)(GLenum type);
    pfn_glCreateShader glCreateShader;
    typedef void (*pfn_glDeleteProgram)(GLuint program);
    pfn_glDeleteProgram glDeleteProgram;
    typedef void (*pfn_glDeleteShader)(GLuint shader);
    pfn_glDeleteShader glDeleteShader;
    typedef void (*pfn_glDetachShader)(GLuint program, GLuint shader);
    pfn_glDetachShader glDetachShader;
    typedef void (*pfn_glDisableVertexAttribArray)(GLuint index);
    pfn_glDisableVertexAttribArray glDisableVertexAttribArray;
    typedef void (*pfn_glEnableVertexAttribArray)(GLuint index);
    pfn_glEnableVertexAttribArray glEnableVertexAttribArray;
    typedef void (*pfn_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
    pfn_glGetActiveAttrib glGetActiveAttrib;
    typedef void (*pfn_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
    pfn_glGetActiveUniform glGetActiveUniform;
    typedef void (*pfn_glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
    pfn_glGetAttachedShaders glGetAttachedShaders;
    typedef GLint (*pfn_glGetAttribLocation)(GLuint program, const GLchar *name);
    pfn_glGetAttribLocation glGetAttribLocation;
    typedef void (*pfn_glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
    pfn_glGetProgramiv glGetProgramiv;
    typedef void (*pfn_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    pfn_glGetProgramInfoLog glGetProgramInfoLog;
    typedef void (*pfn_glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
    pfn_glGetShaderiv glGetShaderiv;
    typedef void (*pfn_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    pfn_glGetShaderInfoLog glGetShaderInfoLog;
    typedef void (*pfn_glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
    pfn_glGetShaderSource glGetShaderSource;
    typedef GLint (*pfn_glGetUniformLocation)(GLuint program, const GLchar *name);
    pfn_glGetUniformLocation glGetUniformLocation;
    typedef void (*pfn_glGetUniformfv)(GLuint program, GLint location, GLfloat *params);
    pfn_glGetUniformfv glGetUniformfv;
    typedef void (*pfn_glGetUniformiv)(GLuint program, GLint location, GLint *params);
    pfn_glGetUniformiv glGetUniformiv;
    typedef void (*pfn_glGetVertexAttribdv)(GLuint index, GLenum pname, GLdouble *params);
    pfn_glGetVertexAttribdv glGetVertexAttribdv;
    typedef void (*pfn_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat *params);
    pfn_glGetVertexAttribfv glGetVertexAttribfv;
    typedef void (*pfn_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint *params);
    pfn_glGetVertexAttribiv glGetVertexAttribiv;
    typedef void (*pfn_glGetVertexAttribPointerv)(GLuint index, GLenum pname, void **pointer);
    pfn_glGetVertexAttribPointerv glGetVertexAttribPointerv;
    typedef GLboolean (*pfn_glIsProgram)(GLuint program);
    pfn_glIsProgram glIsProgram;
    typedef GLboolean (*pfn_glIsShader)(GLuint shader);
    pfn_glIsShader glIsShader;
    typedef void (*pfn_glLinkProgram)(GLuint program);
    pfn_glLinkProgram glLinkProgram;
    typedef void (*pfn_glShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
    pfn_glShaderSource glShaderSource;
    typedef void (*pfn_glUseProgram)(GLuint program);
    pfn_glUseProgram glUseProgram;
    typedef void (*pfn_glUniform1f)(GLint location, GLfloat v0);
    pfn_glUniform1f glUniform1f;
    typedef void (*pfn_glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
    pfn_glUniform2f glUniform2f;
    typedef void (*pfn_glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    pfn_glUniform3f glUniform3f;
    typedef void (*pfn_glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    pfn_glUniform4f glUniform4f;
    typedef void (*pfn_glUniform1i)(GLint location, GLint v0);
    pfn_glUniform1i glUniform1i;
    typedef void (*pfn_glUniform2i)(GLint location, GLint v0, GLint v1);
    pfn_glUniform2i glUniform2i;
    typedef void (*pfn_glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
    pfn_glUniform3i glUniform3i;
    typedef void (*pfn_glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    pfn_glUniform4i glUniform4i;
    typedef void (*pfn_glUniform1fv)(GLint location, GLsizei count, const GLfloat *value);
    pfn_glUniform1fv glUniform1fv;
    typedef void (*pfn_glUniform2fv)(GLint location, GLsizei count, const GLfloat *value);
    pfn_glUniform2fv glUniform2fv;
    typedef void (*pfn_glUniform3fv)(GLint location, GLsizei count, const GLfloat *value);
    pfn_glUniform3fv glUniform3fv;
    typedef void (*pfn_glUniform4fv)(GLint location, GLsizei count, const GLfloat *value);
    pfn_glUniform4fv glUniform4fv;
    typedef void (*pfn_glUniform1iv)(GLint location, GLsizei count, const GLint *value);
    pfn_glUniform1iv glUniform1iv;
    typedef void (*pfn_glUniform2iv)(GLint location, GLsizei count, const GLint *value);
    pfn_glUniform2iv glUniform2iv;
    typedef void (*pfn_glUniform3iv)(GLint location, GLsizei count, const GLint *value);
    pfn_glUniform3iv glUniform3iv;
    typedef void (*pfn_glUniform4iv)(GLint location, GLsizei count, const GLint *value);
    pfn_glUniform4iv glUniform4iv;
    typedef void (*pfn_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix2fv glUniformMatrix2fv;
    typedef void (*pfn_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix3fv glUniformMatrix3fv;
    typedef void (*pfn_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix4fv glUniformMatrix4fv;
    typedef void (*pfn_glValidateProgram)(GLuint program);
    pfn_glValidateProgram glValidateProgram;
    typedef void (*pfn_glVertexAttrib1d)(GLuint index, GLdouble x);
    pfn_glVertexAttrib1d glVertexAttrib1d;
    typedef void (*pfn_glVertexAttrib1dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttrib1dv glVertexAttrib1dv;
    typedef void (*pfn_glVertexAttrib1f)(GLuint index, GLfloat x);
    pfn_glVertexAttrib1f glVertexAttrib1f;
    typedef void (*pfn_glVertexAttrib1fv)(GLuint index, const GLfloat *v);
    pfn_glVertexAttrib1fv glVertexAttrib1fv;
    typedef void (*pfn_glVertexAttrib1s)(GLuint index, GLshort x);
    pfn_glVertexAttrib1s glVertexAttrib1s;
    typedef void (*pfn_glVertexAttrib1sv)(GLuint index, const GLshort *v);
    pfn_glVertexAttrib1sv glVertexAttrib1sv;
    typedef void (*pfn_glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
    pfn_glVertexAttrib2d glVertexAttrib2d;
    typedef void (*pfn_glVertexAttrib2dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttrib2dv glVertexAttrib2dv;
    typedef void (*pfn_glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
    pfn_glVertexAttrib2f glVertexAttrib2f;
    typedef void (*pfn_glVertexAttrib2fv)(GLuint index, const GLfloat *v);
    pfn_glVertexAttrib2fv glVertexAttrib2fv;
    typedef void (*pfn_glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
    pfn_glVertexAttrib2s glVertexAttrib2s;
    typedef void (*pfn_glVertexAttrib2sv)(GLuint index, const GLshort *v);
    pfn_glVertexAttrib2sv glVertexAttrib2sv;
    typedef void (*pfn_glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
    pfn_glVertexAttrib3d glVertexAttrib3d;
    typedef void (*pfn_glVertexAttrib3dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttrib3dv glVertexAttrib3dv;
    typedef void (*pfn_glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
    pfn_glVertexAttrib3f glVertexAttrib3f;
    typedef void (*pfn_glVertexAttrib3fv)(GLuint index, const GLfloat *v);
    pfn_glVertexAttrib3fv glVertexAttrib3fv;
    typedef void (*pfn_glVertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z);
    pfn_glVertexAttrib3s glVertexAttrib3s;
    typedef void (*pfn_glVertexAttrib3sv)(GLuint index, const GLshort *v);
    pfn_glVertexAttrib3sv glVertexAttrib3sv;
    typedef void (*pfn_glVertexAttrib4Nbv)(GLuint index, const GLbyte *v);
    pfn_glVertexAttrib4Nbv glVertexAttrib4Nbv;
    typedef void (*pfn_glVertexAttrib4Niv)(GLuint index, const GLint *v);
    pfn_glVertexAttrib4Niv glVertexAttrib4Niv;
    typedef void (*pfn_glVertexAttrib4Nsv)(GLuint index, const GLshort *v);
    pfn_glVertexAttrib4Nsv glVertexAttrib4Nsv;
    typedef void (*pfn_glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
    pfn_glVertexAttrib4Nub glVertexAttrib4Nub;
    typedef void (*pfn_glVertexAttrib4Nubv)(GLuint index, const GLubyte *v);
    pfn_glVertexAttrib4Nubv glVertexAttrib4Nubv;
    typedef void (*pfn_glVertexAttrib4Nuiv)(GLuint index, const GLuint *v);
    pfn_glVertexAttrib4Nuiv glVertexAttrib4Nuiv;
    typedef void (*pfn_glVertexAttrib4Nusv)(GLuint index, const GLushort *v);
    pfn_glVertexAttrib4Nusv glVertexAttrib4Nusv;
    typedef void (*pfn_glVertexAttrib4bv)(GLuint index, const GLbyte *v);
    pfn_glVertexAttrib4bv glVertexAttrib4bv;
    typedef void (*pfn_glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    pfn_glVertexAttrib4d glVertexAttrib4d;
    typedef void (*pfn_glVertexAttrib4dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttrib4dv glVertexAttrib4dv;
    typedef void (*pfn_glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    pfn_glVertexAttrib4f glVertexAttrib4f;
    typedef void (*pfn_glVertexAttrib4fv)(GLuint index, const GLfloat *v);
    pfn_glVertexAttrib4fv glVertexAttrib4fv;
    typedef void (*pfn_glVertexAttrib4iv)(GLuint index, const GLint *v);
    pfn_glVertexAttrib4iv glVertexAttrib4iv;
    typedef void (*pfn_glVertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
    pfn_glVertexAttrib4s glVertexAttrib4s;
    typedef void (*pfn_glVertexAttrib4sv)(GLuint index, const GLshort *v);
    pfn_glVertexAttrib4sv glVertexAttrib4sv;
    typedef void (*pfn_glVertexAttrib4ubv)(GLuint index, const GLubyte *v);
    pfn_glVertexAttrib4ubv glVertexAttrib4ubv;
    typedef void (*pfn_glVertexAttrib4uiv)(GLuint index, const GLuint *v);
    pfn_glVertexAttrib4uiv glVertexAttrib4uiv;
    typedef void (*pfn_glVertexAttrib4usv)(GLuint index, const GLushort *v);
    pfn_glVertexAttrib4usv glVertexAttrib4usv;
    typedef void (*pfn_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
    pfn_glVertexAttribPointer glVertexAttribPointer;

    // GL 2.1
    typedef void (*pfn_glUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix2x3fv glUniformMatrix2x3fv;
    typedef void (*pfn_glUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix3x2fv glUniformMatrix3x2fv;
    typedef void (*pfn_glUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix2x4fv glUniformMatrix2x4fv;
    typedef void (*pfn_glUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix4x2fv glUniformMatrix4x2fv;
    typedef void (*pfn_glUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix3x4fv glUniformMatrix3x4fv;
    typedef void (*pfn_glUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glUniformMatrix4x3fv glUniformMatrix4x3fv;

    // GL 3.0
    typedef void (*pfn_glColorMaski)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
    pfn_glColorMaski glColorMaski;
    typedef void (*pfn_glGetBooleani_v)(GLenum target, GLuint index, GLboolean *data);
    pfn_glGetBooleani_v glGetBooleani_v;
    typedef void (*pfn_glGetIntegeri_v)(GLenum target, GLuint index, GLint *data);
    pfn_glGetIntegeri_v glGetIntegeri_v;
    typedef void (*pfn_glEnablei)(GLenum target, GLuint index);
    pfn_glEnablei glEnablei;
    typedef void (*pfn_glDisablei)(GLenum target, GLuint index);
    pfn_glDisablei glDisablei;
    typedef GLboolean (*pfn_glIsEnabledi)(GLenum target, GLuint index);
    pfn_glIsEnabledi glIsEnabledi;
    typedef void (*pfn_glBeginTransformFeedback)(GLenum primitiveMode);
    pfn_glBeginTransformFeedback glBeginTransformFeedback;
    typedef void (*pfn_glEndTransformFeedback)(void);
    pfn_glEndTransformFeedback glEndTransformFeedback;
    typedef void (*pfn_glBindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
    pfn_glBindBufferRange glBindBufferRange;
    typedef void (*pfn_glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
    pfn_glBindBufferBase glBindBufferBase;
    typedef void (*pfn_glTransformFeedbackVaryings)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
    pfn_glTransformFeedbackVaryings glTransformFeedbackVaryings;
    typedef void (*pfn_glGetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
    pfn_glGetTransformFeedbackVarying glGetTransformFeedbackVarying;
    typedef void (*pfn_glClampColor)(GLenum target, GLenum clamp);
    pfn_glClampColor glClampColor;
    typedef void (*pfn_glBeginConditionalRender)(GLuint id, GLenum mode);
    pfn_glBeginConditionalRender glBeginConditionalRender;
    typedef void (*pfn_glEndConditionalRender)(void);
    pfn_glEndConditionalRender glEndConditionalRender;
    typedef void (*pfn_glVertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
    pfn_glVertexAttribIPointer glVertexAttribIPointer;
    typedef void (*pfn_glGetVertexAttribIiv)(GLuint index, GLenum pname, GLint *params);
    pfn_glGetVertexAttribIiv glGetVertexAttribIiv;
    typedef void (*pfn_glGetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint *params);
    pfn_glGetVertexAttribIuiv glGetVertexAttribIuiv;
    typedef void (*pfn_glVertexAttribI1i)(GLuint index, GLint x);
    pfn_glVertexAttribI1i glVertexAttribI1i;
    typedef void (*pfn_glVertexAttribI2i)(GLuint index, GLint x, GLint y);
    pfn_glVertexAttribI2i glVertexAttribI2i;
    typedef void (*pfn_glVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
    pfn_glVertexAttribI3i glVertexAttribI3i;
    typedef void (*pfn_glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w);
    pfn_glVertexAttribI4i glVertexAttribI4i;
    typedef void (*pfn_glVertexAttribI1ui)(GLuint index, GLuint x);
    pfn_glVertexAttribI1ui glVertexAttribI1ui;
    typedef void (*pfn_glVertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
    pfn_glVertexAttribI2ui glVertexAttribI2ui;
    typedef void (*pfn_glVertexAttribI3ui)(GLuint index, GLuint x, GLuint y, GLuint z);
    pfn_glVertexAttribI3ui glVertexAttribI3ui;
    typedef void (*pfn_glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
    pfn_glVertexAttribI4ui glVertexAttribI4ui;
    typedef void (*pfn_glVertexAttribI1iv)(GLuint index, const GLint *v);
    pfn_glVertexAttribI1iv glVertexAttribI1iv;
    typedef void (*pfn_glVertexAttribI2iv)(GLuint index, const GLint *v);
    pfn_glVertexAttribI2iv glVertexAttribI2iv;
    typedef void (*pfn_glVertexAttribI3iv)(GLuint index, const GLint *v);
    pfn_glVertexAttribI3iv glVertexAttribI3iv;
    typedef void (*pfn_glVertexAttribI4iv)(GLuint index, const GLint *v);
    pfn_glVertexAttribI4iv glVertexAttribI4iv;
    typedef void (*pfn_glVertexAttribI1uiv)(GLuint index, const GLuint *v);
    pfn_glVertexAttribI1uiv glVertexAttribI1uiv;
    typedef void (*pfn_glVertexAttribI2uiv)(GLuint index, const GLuint *v);
    pfn_glVertexAttribI2uiv glVertexAttribI2uiv;
    typedef void (*pfn_glVertexAttribI3uiv)(GLuint index, const GLuint *v);
    pfn_glVertexAttribI3uiv glVertexAttribI3uiv;
    typedef void (*pfn_glVertexAttribI4uiv)(GLuint index, const GLuint *v);
    pfn_glVertexAttribI4uiv glVertexAttribI4uiv;
    typedef void (*pfn_glVertexAttribI4bv)(GLuint index, const GLbyte *v);
    pfn_glVertexAttribI4bv glVertexAttribI4bv;
    typedef void (*pfn_glVertexAttribI4sv)(GLuint index, const GLshort *v);
    pfn_glVertexAttribI4sv glVertexAttribI4sv;
    typedef void (*pfn_glVertexAttribI4ubv)(GLuint index, const GLubyte *v);
    pfn_glVertexAttribI4ubv glVertexAttribI4ubv;
    typedef void (*pfn_glVertexAttribI4usv)(GLuint index, const GLushort *v);
    pfn_glVertexAttribI4usv glVertexAttribI4usv;
    typedef void (*pfn_glGetUniformuiv)(GLuint program, GLint location, GLuint *params);
    pfn_glGetUniformuiv glGetUniformuiv;
    typedef void (*pfn_glBindFragDataLocation)(GLuint program, GLuint color, const GLchar *name);
    pfn_glBindFragDataLocation glBindFragDataLocation;
    typedef GLint (*pfn_glGetFragDataLocation)(GLuint program, const GLchar *name);
    pfn_glGetFragDataLocation glGetFragDataLocation;
    typedef void (*pfn_glUniform1ui)(GLint location, GLuint v0);
    pfn_glUniform1ui glUniform1ui;
    typedef void (*pfn_glUniform2ui)(GLint location, GLuint v0, GLuint v1);
    pfn_glUniform2ui glUniform2ui;
    typedef void (*pfn_glUniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2);
    pfn_glUniform3ui glUniform3ui;
    typedef void (*pfn_glUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
    pfn_glUniform4ui glUniform4ui;
    typedef void (*pfn_glUniform1uiv)(GLint location, GLsizei count, const GLuint *value);
    pfn_glUniform1uiv glUniform1uiv;
    typedef void (*pfn_glUniform2uiv)(GLint location, GLsizei count, const GLuint *value);
    pfn_glUniform2uiv glUniform2uiv;
    typedef void (*pfn_glUniform3uiv)(GLint location, GLsizei count, const GLuint *value);
    pfn_glUniform3uiv glUniform3uiv;
    typedef void (*pfn_glUniform4uiv)(GLint location, GLsizei count, const GLuint *value);
    pfn_glUniform4uiv glUniform4uiv;
    typedef void (*pfn_glTexParameterIiv)(GLenum target, GLenum pname, const GLint *params);
    pfn_glTexParameterIiv glTexParameterIiv;
    typedef void (*pfn_glTexParameterIuiv)(GLenum target, GLenum pname, const GLuint *params);
    pfn_glTexParameterIuiv glTexParameterIuiv;
    typedef void (*pfn_glGetTexParameterIiv)(GLenum target, GLenum pname, GLint *params);
    pfn_glGetTexParameterIiv glGetTexParameterIiv;
    typedef void (*pfn_glGetTexParameterIuiv)(GLenum target, GLenum pname, GLuint *params);
    pfn_glGetTexParameterIuiv glGetTexParameterIuiv;
    typedef void (*pfn_glClearBufferiv)(GLenum buffer, GLint drawbuffer, const GLint *value);
    pfn_glClearBufferiv glClearBufferiv;
    typedef void (*pfn_glClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint *value);
    pfn_glClearBufferuiv glClearBufferuiv;
    typedef void (*pfn_glClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat *value);
    pfn_glClearBufferfv glClearBufferfv;
    typedef void (*pfn_glClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
    pfn_glClearBufferfi glClearBufferfi;
    typedef const GLubyte * (*pfn_glGetStringi)(GLenum name, GLuint index);
    pfn_glGetStringi glGetStringi;
    typedef GLboolean (*pfn_glIsRenderbuffer)(GLuint renderbuffer);
    pfn_glIsRenderbuffer glIsRenderbuffer;
    typedef void (*pfn_glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
    pfn_glBindRenderbuffer glBindRenderbuffer;
    typedef void (*pfn_glDeleteRenderbuffers)(GLsizei n, const GLuint *renderbuffers);
    pfn_glDeleteRenderbuffers glDeleteRenderbuffers;
    typedef void (*pfn_glGenRenderbuffers)(GLsizei n, GLuint *renderbuffers);
    pfn_glGenRenderbuffers glGenRenderbuffers;
    typedef void (*pfn_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    pfn_glRenderbufferStorage glRenderbufferStorage;
    typedef void (*pfn_glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint *params);
    pfn_glGetRenderbufferParameteriv glGetRenderbufferParameteriv;
    typedef GLboolean (*pfn_glIsFramebuffer)(GLuint framebuffer);
    pfn_glIsFramebuffer glIsFramebuffer;
    typedef void (*pfn_glBindFramebuffer)(GLenum target, GLuint framebuffer);
    pfn_glBindFramebuffer glBindFramebuffer;
    typedef void (*pfn_glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
    pfn_glDeleteFramebuffers glDeleteFramebuffers;
    typedef void (*pfn_glGenFramebuffers)(GLsizei n, GLuint *framebuffers);
    pfn_glGenFramebuffers glGenFramebuffers;
    typedef GLenum (*pfn_glCheckFramebufferStatus)(GLenum target);
    pfn_glCheckFramebufferStatus glCheckFramebufferStatus;
    typedef void (*pfn_glFramebufferTexture1D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    pfn_glFramebufferTexture1D glFramebufferTexture1D;
    typedef void (*pfn_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    pfn_glFramebufferTexture2D glFramebufferTexture2D;
    typedef void (*pfn_glFramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
    pfn_glFramebufferTexture3D glFramebufferTexture3D;
    typedef void (*pfn_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    pfn_glFramebufferRenderbuffer glFramebufferRenderbuffer;
    typedef void (*pfn_glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
    pfn_glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameteriv;
    typedef void (*pfn_glGenerateMipmap)(GLenum target);
    pfn_glGenerateMipmap glGenerateMipmap;
    typedef void (*pfn_glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    pfn_glBlitFramebuffer glBlitFramebuffer;
    typedef void (*pfn_glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
    pfn_glRenderbufferStorageMultisample glRenderbufferStorageMultisample;
    typedef void (*pfn_glFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
    pfn_glFramebufferTextureLayer glFramebufferTextureLayer;
    typedef void * (*pfn_glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
    pfn_glMapBufferRange glMapBufferRange;
    typedef void (*pfn_glFlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length);
    pfn_glFlushMappedBufferRange glFlushMappedBufferRange;
    typedef void (*pfn_glBindVertexArray)(GLuint array);
    pfn_glBindVertexArray glBindVertexArray;
    typedef void (*pfn_glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
    pfn_glDeleteVertexArrays glDeleteVertexArrays;
    typedef void (*pfn_glGenVertexArrays)(GLsizei n, GLuint *arrays);
    pfn_glGenVertexArrays glGenVertexArrays;
    typedef GLboolean (*pfn_glIsVertexArray)(GLuint array);
    pfn_glIsVertexArray glIsVertexArray;

    // GL 3.1
    typedef void (*pfn_glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
    pfn_glDrawArraysInstanced glDrawArraysInstanced;
    typedef void (*pfn_glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
    pfn_glDrawElementsInstanced glDrawElementsInstanced;
    typedef void (*pfn_glTexBuffer)(GLenum target, GLenum internalformat, GLuint buffer);
    pfn_glTexBuffer glTexBuffer;
    typedef void (*pfn_glPrimitiveRestartIndex)(GLuint index);
    pfn_glPrimitiveRestartIndex glPrimitiveRestartIndex;
    typedef void (*pfn_glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
    pfn_glCopyBufferSubData glCopyBufferSubData;
    typedef void (*pfn_glGetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
    pfn_glGetUniformIndices glGetUniformIndices;
    typedef void (*pfn_glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
    pfn_glGetActiveUniformsiv glGetActiveUniformsiv;
    typedef void (*pfn_glGetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
    pfn_glGetActiveUniformName glGetActiveUniformName;
    typedef GLuint (*pfn_glGetUniformBlockIndex)(GLuint program, const GLchar *uniformBlockName);
    pfn_glGetUniformBlockIndex glGetUniformBlockIndex;
    typedef void (*pfn_glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
    pfn_glGetActiveUniformBlockiv glGetActiveUniformBlockiv;
    typedef void (*pfn_glGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
    pfn_glGetActiveUniformBlockName glGetActiveUniformBlockName;
    typedef void (*pfn_glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
    pfn_glUniformBlockBinding glUniformBlockBinding;

    // GL 3.2
    typedef void (*pfn_glDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
    pfn_glDrawElementsBaseVertex glDrawElementsBaseVertex;
    typedef void (*pfn_glDrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
    pfn_glDrawRangeElementsBaseVertex glDrawRangeElementsBaseVertex;
    typedef void (*pfn_glDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
    pfn_glDrawElementsInstancedBaseVertex glDrawElementsInstancedBaseVertex;
    typedef void (*pfn_glMultiDrawElementsBaseVertex)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
    pfn_glMultiDrawElementsBaseVertex glMultiDrawElementsBaseVertex;
    typedef void (*pfn_glProvokingVertex)(GLenum mode);
    pfn_glProvokingVertex glProvokingVertex;
    typedef GLsync (*pfn_glFenceSync)(GLenum condition, GLbitfield flags);
    pfn_glFenceSync glFenceSync;
    typedef GLboolean (*pfn_glIsSync)(GLsync sync);
    pfn_glIsSync glIsSync;
    typedef void (*pfn_glDeleteSync)(GLsync sync);
    pfn_glDeleteSync glDeleteSync;
    typedef GLenum (*pfn_glClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
    pfn_glClientWaitSync glClientWaitSync;
    typedef void (*pfn_glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
    pfn_glWaitSync glWaitSync;
    typedef void (*pfn_glGetInteger64v)(GLenum pname, GLint64 *data);
    pfn_glGetInteger64v glGetInteger64v;
    typedef void (*pfn_glGetSynciv)(GLsync sync, GLenum pname, GLsizei count, GLsizei *length, GLint *values);
    pfn_glGetSynciv glGetSynciv;
    typedef void (*pfn_glGetInteger64i_v)(GLenum target, GLuint index, GLint64 *data);
    pfn_glGetInteger64i_v glGetInteger64i_v;
    typedef void (*pfn_glGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64 *params);
    pfn_glGetBufferParameteri64v glGetBufferParameteri64v;
    typedef void (*pfn_glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
    pfn_glFramebufferTexture glFramebufferTexture;
    typedef void (*pfn_glTexImage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
    pfn_glTexImage2DMultisample glTexImage2DMultisample;
    typedef void (*pfn_glTexImage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
    pfn_glTexImage3DMultisample glTexImage3DMultisample;
    typedef void (*pfn_glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat *val);
    pfn_glGetMultisamplefv glGetMultisamplefv;
    typedef void (*pfn_glSampleMaski)(GLuint maskNumber, GLbitfield mask);
    pfn_glSampleMaski glSampleMaski;

    // GL 3.3
    typedef void (*pfn_glBindFragDataLocationIndexed)(GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
    pfn_glBindFragDataLocationIndexed glBindFragDataLocationIndexed;
    typedef GLint (*pfn_glGetFragDataIndex)(GLuint program, const GLchar *name);
    pfn_glGetFragDataIndex glGetFragDataIndex;
    typedef void (*pfn_glGenSamplers)(GLsizei count, GLuint *samplers);
    pfn_glGenSamplers glGenSamplers;
    typedef void (*pfn_glDeleteSamplers)(GLsizei count, const GLuint *samplers);
    pfn_glDeleteSamplers glDeleteSamplers;
    typedef GLboolean (*pfn_glIsSampler)(GLuint sampler);
    pfn_glIsSampler glIsSampler;
    typedef void (*pfn_glBindSampler)(GLuint unit, GLuint sampler);
    pfn_glBindSampler glBindSampler;
    typedef void (*pfn_glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
    pfn_glSamplerParameteri glSamplerParameteri;
    typedef void (*pfn_glSamplerParameteriv)(GLuint sampler, GLenum pname, const GLint *param);
    pfn_glSamplerParameteriv glSamplerParameteriv;
    typedef void (*pfn_glSamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param);
    pfn_glSamplerParameterf glSamplerParameterf;
    typedef void (*pfn_glSamplerParameterfv)(GLuint sampler, GLenum pname, const GLfloat *param);
    pfn_glSamplerParameterfv glSamplerParameterfv;
    typedef void (*pfn_glSamplerParameterIiv)(GLuint sampler, GLenum pname, const GLint *param);
    pfn_glSamplerParameterIiv glSamplerParameterIiv;
    typedef void (*pfn_glSamplerParameterIuiv)(GLuint sampler, GLenum pname, const GLuint *param);
    pfn_glSamplerParameterIuiv glSamplerParameterIuiv;
    typedef void (*pfn_glGetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint *params);
    pfn_glGetSamplerParameteriv glGetSamplerParameteriv;
    typedef void (*pfn_glGetSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint *params);
    pfn_glGetSamplerParameterIiv glGetSamplerParameterIiv;
    typedef void (*pfn_glGetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat *params);
    pfn_glGetSamplerParameterfv glGetSamplerParameterfv;
    typedef void (*pfn_glGetSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint *params);
    pfn_glGetSamplerParameterIuiv glGetSamplerParameterIuiv;
    typedef void (*pfn_glQueryCounter)(GLuint id, GLenum target);
    pfn_glQueryCounter glQueryCounter;
    typedef void (*pfn_glGetQueryObjecti64v)(GLuint id, GLenum pname, GLint64 *params);
    pfn_glGetQueryObjecti64v glGetQueryObjecti64v;
    typedef void (*pfn_glGetQueryObjectui64v)(GLuint id, GLenum pname, GLuint64 *params);
    pfn_glGetQueryObjectui64v glGetQueryObjectui64v;
    typedef void (*pfn_glVertexAttribDivisor)(GLuint index, GLuint divisor);
    pfn_glVertexAttribDivisor glVertexAttribDivisor;
    typedef void (*pfn_glVertexAttribP1ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
    pfn_glVertexAttribP1ui glVertexAttribP1ui;
    typedef void (*pfn_glVertexAttribP1uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
    pfn_glVertexAttribP1uiv glVertexAttribP1uiv;
    typedef void (*pfn_glVertexAttribP2ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
    pfn_glVertexAttribP2ui glVertexAttribP2ui;
    typedef void (*pfn_glVertexAttribP2uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
    pfn_glVertexAttribP2uiv glVertexAttribP2uiv;
    typedef void (*pfn_glVertexAttribP3ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
    pfn_glVertexAttribP3ui glVertexAttribP3ui;
    typedef void (*pfn_glVertexAttribP3uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
    pfn_glVertexAttribP3uiv glVertexAttribP3uiv;
    typedef void (*pfn_glVertexAttribP4ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
    pfn_glVertexAttribP4ui glVertexAttribP4ui;
    typedef void (*pfn_glVertexAttribP4uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
    pfn_glVertexAttribP4uiv glVertexAttribP4uiv;
    typedef void (*pfn_glVertexP2ui)(GLenum type, GLuint value);
    pfn_glVertexP2ui glVertexP2ui;
    typedef void (*pfn_glVertexP2uiv)(GLenum type, const GLuint *value);
    pfn_glVertexP2uiv glVertexP2uiv;
    typedef void (*pfn_glVertexP3ui)(GLenum type, GLuint value);
    pfn_glVertexP3ui glVertexP3ui;
    typedef void (*pfn_glVertexP3uiv)(GLenum type, const GLuint *value);
    pfn_glVertexP3uiv glVertexP3uiv;
    typedef void (*pfn_glVertexP4ui)(GLenum type, GLuint value);
    pfn_glVertexP4ui glVertexP4ui;
    typedef void (*pfn_glVertexP4uiv)(GLenum type, const GLuint *value);
    pfn_glVertexP4uiv glVertexP4uiv;
    typedef void (*pfn_glTexCoordP1ui)(GLenum type, GLuint coords);
    pfn_glTexCoordP1ui glTexCoordP1ui;
    typedef void (*pfn_glTexCoordP1uiv)(GLenum type, const GLuint *coords);
    pfn_glTexCoordP1uiv glTexCoordP1uiv;
    typedef void (*pfn_glTexCoordP2ui)(GLenum type, GLuint coords);
    pfn_glTexCoordP2ui glTexCoordP2ui;
    typedef void (*pfn_glTexCoordP2uiv)(GLenum type, const GLuint *coords);
    pfn_glTexCoordP2uiv glTexCoordP2uiv;
    typedef void (*pfn_glTexCoordP3ui)(GLenum type, GLuint coords);
    pfn_glTexCoordP3ui glTexCoordP3ui;
    typedef void (*pfn_glTexCoordP3uiv)(GLenum type, const GLuint *coords);
    pfn_glTexCoordP3uiv glTexCoordP3uiv;
    typedef void (*pfn_glTexCoordP4ui)(GLenum type, GLuint coords);
    pfn_glTexCoordP4ui glTexCoordP4ui;
    typedef void (*pfn_glTexCoordP4uiv)(GLenum type, const GLuint *coords);
    pfn_glTexCoordP4uiv glTexCoordP4uiv;
    typedef void (*pfn_glMultiTexCoordP1ui)(GLenum texture, GLenum type, GLuint coords);
    pfn_glMultiTexCoordP1ui glMultiTexCoordP1ui;
    typedef void (*pfn_glMultiTexCoordP1uiv)(GLenum texture, GLenum type, const GLuint *coords);
    pfn_glMultiTexCoordP1uiv glMultiTexCoordP1uiv;
    typedef void (*pfn_glMultiTexCoordP2ui)(GLenum texture, GLenum type, GLuint coords);
    pfn_glMultiTexCoordP2ui glMultiTexCoordP2ui;
    typedef void (*pfn_glMultiTexCoordP2uiv)(GLenum texture, GLenum type, const GLuint *coords);
    pfn_glMultiTexCoordP2uiv glMultiTexCoordP2uiv;
    typedef void (*pfn_glMultiTexCoordP3ui)(GLenum texture, GLenum type, GLuint coords);
    pfn_glMultiTexCoordP3ui glMultiTexCoordP3ui;
    typedef void (*pfn_glMultiTexCoordP3uiv)(GLenum texture, GLenum type, const GLuint *coords);
    pfn_glMultiTexCoordP3uiv glMultiTexCoordP3uiv;
    typedef void (*pfn_glMultiTexCoordP4ui)(GLenum texture, GLenum type, GLuint coords);
    pfn_glMultiTexCoordP4ui glMultiTexCoordP4ui;
    typedef void (*pfn_glMultiTexCoordP4uiv)(GLenum texture, GLenum type, const GLuint *coords);
    pfn_glMultiTexCoordP4uiv glMultiTexCoordP4uiv;
    typedef void (*pfn_glNormalP3ui)(GLenum type, GLuint coords);
    pfn_glNormalP3ui glNormalP3ui;
    typedef void (*pfn_glNormalP3uiv)(GLenum type, const GLuint *coords);
    pfn_glNormalP3uiv glNormalP3uiv;
    typedef void (*pfn_glColorP3ui)(GLenum type, GLuint color);
    pfn_glColorP3ui glColorP3ui;
    typedef void (*pfn_glColorP3uiv)(GLenum type, const GLuint *color);
    pfn_glColorP3uiv glColorP3uiv;
    typedef void (*pfn_glColorP4ui)(GLenum type, GLuint color);
    pfn_glColorP4ui glColorP4ui;
    typedef void (*pfn_glColorP4uiv)(GLenum type, const GLuint *color);
    pfn_glColorP4uiv glColorP4uiv;
    typedef void (*pfn_glSecondaryColorP3ui)(GLenum type, GLuint color);
    pfn_glSecondaryColorP3ui glSecondaryColorP3ui;
    typedef void (*pfn_glSecondaryColorP3uiv)(GLenum type, const GLuint *color);
    pfn_glSecondaryColorP3uiv glSecondaryColorP3uiv;

    // GL 4.0
    typedef void (*pfn_glMinSampleShading)(GLfloat value);
    pfn_glMinSampleShading glMinSampleShading;
    typedef void (*pfn_glBlendEquationi)(GLuint buf, GLenum mode);
    pfn_glBlendEquationi glBlendEquationi;
    typedef void (*pfn_glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
    pfn_glBlendEquationSeparatei glBlendEquationSeparatei;
    typedef void (*pfn_glBlendFunci)(GLuint buf, GLenum src, GLenum dst);
    pfn_glBlendFunci glBlendFunci;
    typedef void (*pfn_glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    pfn_glBlendFuncSeparatei glBlendFuncSeparatei;
    typedef void (*pfn_glDrawArraysIndirect)(GLenum mode, const void *indirect);
    pfn_glDrawArraysIndirect glDrawArraysIndirect;
    typedef void (*pfn_glDrawElementsIndirect)(GLenum mode, GLenum type, const void *indirect);
    pfn_glDrawElementsIndirect glDrawElementsIndirect;
    typedef void (*pfn_glUniform1d)(GLint location, GLdouble x);
    pfn_glUniform1d glUniform1d;
    typedef void (*pfn_glUniform2d)(GLint location, GLdouble x, GLdouble y);
    pfn_glUniform2d glUniform2d;
    typedef void (*pfn_glUniform3d)(GLint location, GLdouble x, GLdouble y, GLdouble z);
    pfn_glUniform3d glUniform3d;
    typedef void (*pfn_glUniform4d)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    pfn_glUniform4d glUniform4d;
    typedef void (*pfn_glUniform1dv)(GLint location, GLsizei count, const GLdouble *value);
    pfn_glUniform1dv glUniform1dv;
    typedef void (*pfn_glUniform2dv)(GLint location, GLsizei count, const GLdouble *value);
    pfn_glUniform2dv glUniform2dv;
    typedef void (*pfn_glUniform3dv)(GLint location, GLsizei count, const GLdouble *value);
    pfn_glUniform3dv glUniform3dv;
    typedef void (*pfn_glUniform4dv)(GLint location, GLsizei count, const GLdouble *value);
    pfn_glUniform4dv glUniform4dv;
    typedef void (*pfn_glUniformMatrix2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix2dv glUniformMatrix2dv;
    typedef void (*pfn_glUniformMatrix3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix3dv glUniformMatrix3dv;
    typedef void (*pfn_glUniformMatrix4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix4dv glUniformMatrix4dv;
    typedef void (*pfn_glUniformMatrix2x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix2x3dv glUniformMatrix2x3dv;
    typedef void (*pfn_glUniformMatrix2x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix2x4dv glUniformMatrix2x4dv;
    typedef void (*pfn_glUniformMatrix3x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix3x2dv glUniformMatrix3x2dv;
    typedef void (*pfn_glUniformMatrix3x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix3x4dv glUniformMatrix3x4dv;
    typedef void (*pfn_glUniformMatrix4x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix4x2dv glUniformMatrix4x2dv;
    typedef void (*pfn_glUniformMatrix4x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glUniformMatrix4x3dv glUniformMatrix4x3dv;
    typedef void (*pfn_glGetUniformdv)(GLuint program, GLint location, GLdouble *params);
    pfn_glGetUniformdv glGetUniformdv;
    typedef GLint (*pfn_glGetSubroutineUniformLocation)(GLuint program, GLenum shadertype, const GLchar *name);
    pfn_glGetSubroutineUniformLocation glGetSubroutineUniformLocation;
    typedef GLuint (*pfn_glGetSubroutineIndex)(GLuint program, GLenum shadertype, const GLchar *name);
    pfn_glGetSubroutineIndex glGetSubroutineIndex;
    typedef void (*pfn_glGetActiveSubroutineUniformiv)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
    pfn_glGetActiveSubroutineUniformiv glGetActiveSubroutineUniformiv;
    typedef void (*pfn_glGetActiveSubroutineUniformName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
    pfn_glGetActiveSubroutineUniformName glGetActiveSubroutineUniformName;
    typedef void (*pfn_glGetActiveSubroutineName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
    pfn_glGetActiveSubroutineName glGetActiveSubroutineName;
    typedef void (*pfn_glUniformSubroutinesuiv)(GLenum shadertype, GLsizei count, const GLuint *indices);
    pfn_glUniformSubroutinesuiv glUniformSubroutinesuiv;
    typedef void (*pfn_glGetUniformSubroutineuiv)(GLenum shadertype, GLint location, GLuint *params);
    pfn_glGetUniformSubroutineuiv glGetUniformSubroutineuiv;
    typedef void (*pfn_glGetProgramStageiv)(GLuint program, GLenum shadertype, GLenum pname, GLint *values);
    pfn_glGetProgramStageiv glGetProgramStageiv;
    typedef void (*pfn_glPatchParameteri)(GLenum pname, GLint value);
    pfn_glPatchParameteri glPatchParameteri;
    typedef void (*pfn_glPatchParameterfv)(GLenum pname, const GLfloat *values);
    pfn_glPatchParameterfv glPatchParameterfv;
    typedef void (*pfn_glBindTransformFeedback)(GLenum target, GLuint id);
    pfn_glBindTransformFeedback glBindTransformFeedback;
    typedef void (*pfn_glDeleteTransformFeedbacks)(GLsizei n, const GLuint *ids);
    pfn_glDeleteTransformFeedbacks glDeleteTransformFeedbacks;
    typedef void (*pfn_glGenTransformFeedbacks)(GLsizei n, GLuint *ids);
    pfn_glGenTransformFeedbacks glGenTransformFeedbacks;
    typedef GLboolean (*pfn_glIsTransformFeedback)(GLuint id);
    pfn_glIsTransformFeedback glIsTransformFeedback;
    typedef void (*pfn_glPauseTransformFeedback)(void);
    pfn_glPauseTransformFeedback glPauseTransformFeedback;
    typedef void (*pfn_glResumeTransformFeedback)(void);
    pfn_glResumeTransformFeedback glResumeTransformFeedback;
    typedef void (*pfn_glDrawTransformFeedback)(GLenum mode, GLuint id);
    pfn_glDrawTransformFeedback glDrawTransformFeedback;
    typedef void (*pfn_glDrawTransformFeedbackStream)(GLenum mode, GLuint id, GLuint stream);
    pfn_glDrawTransformFeedbackStream glDrawTransformFeedbackStream;
    typedef void (*pfn_glBeginQueryIndexed)(GLenum target, GLuint index, GLuint id);
    pfn_glBeginQueryIndexed glBeginQueryIndexed;
    typedef void (*pfn_glEndQueryIndexed)(GLenum target, GLuint index);
    pfn_glEndQueryIndexed glEndQueryIndexed;
    typedef void (*pfn_glGetQueryIndexediv)(GLenum target, GLuint index, GLenum pname, GLint *params);
    pfn_glGetQueryIndexediv glGetQueryIndexediv;

    // GL 4.1
    typedef void (*pfn_glReleaseShaderCompiler)(void);
    pfn_glReleaseShaderCompiler glReleaseShaderCompiler;
    typedef void (*pfn_glShaderBinary)(GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void *binary, GLsizei length);
    pfn_glShaderBinary glShaderBinary;
    typedef void (*pfn_glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
    pfn_glGetShaderPrecisionFormat glGetShaderPrecisionFormat;
    typedef void (*pfn_glDepthRangef)(GLfloat n, GLfloat f);
    pfn_glDepthRangef glDepthRangef;
    typedef void (*pfn_glClearDepthf)(GLfloat d);
    pfn_glClearDepthf glClearDepthf;
    typedef void (*pfn_glGetProgramBinary)(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
    pfn_glGetProgramBinary glGetProgramBinary;
    typedef void (*pfn_glProgramBinary)(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
    pfn_glProgramBinary glProgramBinary;
    typedef void (*pfn_glProgramParameteri)(GLuint program, GLenum pname, GLint value);
    pfn_glProgramParameteri glProgramParameteri;
    typedef void (*pfn_glUseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program);
    pfn_glUseProgramStages glUseProgramStages;
    typedef void (*pfn_glActiveShaderProgram)(GLuint pipeline, GLuint program);
    pfn_glActiveShaderProgram glActiveShaderProgram;
    typedef GLuint (*pfn_glCreateShaderProgramv)(GLenum type, GLsizei count, const GLchar *const*strings);
    pfn_glCreateShaderProgramv glCreateShaderProgramv;
    typedef void (*pfn_glBindProgramPipeline)(GLuint pipeline);
    pfn_glBindProgramPipeline glBindProgramPipeline;
    typedef void (*pfn_glDeleteProgramPipelines)(GLsizei n, const GLuint *pipelines);
    pfn_glDeleteProgramPipelines glDeleteProgramPipelines;
    typedef void (*pfn_glGenProgramPipelines)(GLsizei n, GLuint *pipelines);
    pfn_glGenProgramPipelines glGenProgramPipelines;
    typedef GLboolean (*pfn_glIsProgramPipeline)(GLuint pipeline);
    pfn_glIsProgramPipeline glIsProgramPipeline;
    typedef void (*pfn_glGetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint *params);
    pfn_glGetProgramPipelineiv glGetProgramPipelineiv;
    typedef void (*pfn_glProgramUniform1i)(GLuint program, GLint location, GLint v0);
    pfn_glProgramUniform1i glProgramUniform1i;
    typedef void (*pfn_glProgramUniform1iv)(GLuint program, GLint location, GLsizei count, const GLint *value);
    pfn_glProgramUniform1iv glProgramUniform1iv;
    typedef void (*pfn_glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
    pfn_glProgramUniform1f glProgramUniform1f;
    typedef void (*pfn_glProgramUniform1fv)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
    pfn_glProgramUniform1fv glProgramUniform1fv;
    typedef void (*pfn_glProgramUniform1d)(GLuint program, GLint location, GLdouble v0);
    pfn_glProgramUniform1d glProgramUniform1d;
    typedef void (*pfn_glProgramUniform1dv)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
    pfn_glProgramUniform1dv glProgramUniform1dv;
    typedef void (*pfn_glProgramUniform1ui)(GLuint program, GLint location, GLuint v0);
    pfn_glProgramUniform1ui glProgramUniform1ui;
    typedef void (*pfn_glProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, const GLuint *value);
    pfn_glProgramUniform1uiv glProgramUniform1uiv;
    typedef void (*pfn_glProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1);
    pfn_glProgramUniform2i glProgramUniform2i;
    typedef void (*pfn_glProgramUniform2iv)(GLuint program, GLint location, GLsizei count, const GLint *value);
    pfn_glProgramUniform2iv glProgramUniform2iv;
    typedef void (*pfn_glProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
    pfn_glProgramUniform2f glProgramUniform2f;
    typedef void (*pfn_glProgramUniform2fv)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
    pfn_glProgramUniform2fv glProgramUniform2fv;
    typedef void (*pfn_glProgramUniform2d)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
    pfn_glProgramUniform2d glProgramUniform2d;
    typedef void (*pfn_glProgramUniform2dv)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
    pfn_glProgramUniform2dv glProgramUniform2dv;
    typedef void (*pfn_glProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1);
    pfn_glProgramUniform2ui glProgramUniform2ui;
    typedef void (*pfn_glProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, const GLuint *value);
    pfn_glProgramUniform2uiv glProgramUniform2uiv;
    typedef void (*pfn_glProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
    pfn_glProgramUniform3i glProgramUniform3i;
    typedef void (*pfn_glProgramUniform3iv)(GLuint program, GLint location, GLsizei count, const GLint *value);
    pfn_glProgramUniform3iv glProgramUniform3iv;
    typedef void (*pfn_glProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    pfn_glProgramUniform3f glProgramUniform3f;
    typedef void (*pfn_glProgramUniform3fv)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
    pfn_glProgramUniform3fv glProgramUniform3fv;
    typedef void (*pfn_glProgramUniform3d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
    pfn_glProgramUniform3d glProgramUniform3d;
    typedef void (*pfn_glProgramUniform3dv)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
    pfn_glProgramUniform3dv glProgramUniform3dv;
    typedef void (*pfn_glProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
    pfn_glProgramUniform3ui glProgramUniform3ui;
    typedef void (*pfn_glProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, const GLuint *value);
    pfn_glProgramUniform3uiv glProgramUniform3uiv;
    typedef void (*pfn_glProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    pfn_glProgramUniform4i glProgramUniform4i;
    typedef void (*pfn_glProgramUniform4iv)(GLuint program, GLint location, GLsizei count, const GLint *value);
    pfn_glProgramUniform4iv glProgramUniform4iv;
    typedef void (*pfn_glProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    pfn_glProgramUniform4f glProgramUniform4f;
    typedef void (*pfn_glProgramUniform4fv)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
    pfn_glProgramUniform4fv glProgramUniform4fv;
    typedef void (*pfn_glProgramUniform4d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
    pfn_glProgramUniform4d glProgramUniform4d;
    typedef void (*pfn_glProgramUniform4dv)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
    pfn_glProgramUniform4dv glProgramUniform4dv;
    typedef void (*pfn_glProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
    pfn_glProgramUniform4ui glProgramUniform4ui;
    typedef void (*pfn_glProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, const GLuint *value);
    pfn_glProgramUniform4uiv glProgramUniform4uiv;
    typedef void (*pfn_glProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix2fv glProgramUniformMatrix2fv;
    typedef void (*pfn_glProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix3fv glProgramUniformMatrix3fv;
    typedef void (*pfn_glProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix4fv glProgramUniformMatrix4fv;
    typedef void (*pfn_glProgramUniformMatrix2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix2dv glProgramUniformMatrix2dv;
    typedef void (*pfn_glProgramUniformMatrix3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix3dv glProgramUniformMatrix3dv;
    typedef void (*pfn_glProgramUniformMatrix4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix4dv glProgramUniformMatrix4dv;
    typedef void (*pfn_glProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix2x3fv glProgramUniformMatrix2x3fv;
    typedef void (*pfn_glProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix3x2fv glProgramUniformMatrix3x2fv;
    typedef void (*pfn_glProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix2x4fv glProgramUniformMatrix2x4fv;
    typedef void (*pfn_glProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix4x2fv glProgramUniformMatrix4x2fv;
    typedef void (*pfn_glProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix3x4fv glProgramUniformMatrix3x4fv;
    typedef void (*pfn_glProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    pfn_glProgramUniformMatrix4x3fv glProgramUniformMatrix4x3fv;
    typedef void (*pfn_glProgramUniformMatrix2x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix2x3dv glProgramUniformMatrix2x3dv;
    typedef void (*pfn_glProgramUniformMatrix3x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix3x2dv glProgramUniformMatrix3x2dv;
    typedef void (*pfn_glProgramUniformMatrix2x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix2x4dv glProgramUniformMatrix2x4dv;
    typedef void (*pfn_glProgramUniformMatrix4x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix4x2dv glProgramUniformMatrix4x2dv;
    typedef void (*pfn_glProgramUniformMatrix3x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix3x4dv glProgramUniformMatrix3x4dv;
    typedef void (*pfn_glProgramUniformMatrix4x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
    pfn_glProgramUniformMatrix4x3dv glProgramUniformMatrix4x3dv;
    typedef void (*pfn_glValidateProgramPipeline)(GLuint pipeline);
    pfn_glValidateProgramPipeline glValidateProgramPipeline;
    typedef void (*pfn_glGetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    pfn_glGetProgramPipelineInfoLog glGetProgramPipelineInfoLog;
    typedef void (*pfn_glVertexAttribL1d)(GLuint index, GLdouble x);
    pfn_glVertexAttribL1d glVertexAttribL1d;
    typedef void (*pfn_glVertexAttribL2d)(GLuint index, GLdouble x, GLdouble y);
    pfn_glVertexAttribL2d glVertexAttribL2d;
    typedef void (*pfn_glVertexAttribL3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
    pfn_glVertexAttribL3d glVertexAttribL3d;
    typedef void (*pfn_glVertexAttribL4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    pfn_glVertexAttribL4d glVertexAttribL4d;
    typedef void (*pfn_glVertexAttribL1dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttribL1dv glVertexAttribL1dv;
    typedef void (*pfn_glVertexAttribL2dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttribL2dv glVertexAttribL2dv;
    typedef void (*pfn_glVertexAttribL3dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttribL3dv glVertexAttribL3dv;
    typedef void (*pfn_glVertexAttribL4dv)(GLuint index, const GLdouble *v);
    pfn_glVertexAttribL4dv glVertexAttribL4dv;
    typedef void (*pfn_glVertexAttribLPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
    pfn_glVertexAttribLPointer glVertexAttribLPointer;
    typedef void (*pfn_glGetVertexAttribLdv)(GLuint index, GLenum pname, GLdouble *params);
    pfn_glGetVertexAttribLdv glGetVertexAttribLdv;
    typedef void (*pfn_glViewportArrayv)(GLuint first, GLsizei count, const GLfloat *v);
    pfn_glViewportArrayv glViewportArrayv;
    typedef void (*pfn_glViewportIndexedf)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
    pfn_glViewportIndexedf glViewportIndexedf;
    typedef void (*pfn_glViewportIndexedfv)(GLuint index, const GLfloat *v);
    pfn_glViewportIndexedfv glViewportIndexedfv;
    typedef void (*pfn_glScissorArrayv)(GLuint first, GLsizei count, const GLint *v);
    pfn_glScissorArrayv glScissorArrayv;
    typedef void (*pfn_glScissorIndexed)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
    pfn_glScissorIndexed glScissorIndexed;
    typedef void (*pfn_glScissorIndexedv)(GLuint index, const GLint *v);
    pfn_glScissorIndexedv glScissorIndexedv;
    typedef void (*pfn_glDepthRangeArrayv)(GLuint first, GLsizei count, const GLdouble *v);
    pfn_glDepthRangeArrayv glDepthRangeArrayv;
    typedef void (*pfn_glDepthRangeIndexed)(GLuint index, GLdouble n, GLdouble f);
    pfn_glDepthRangeIndexed glDepthRangeIndexed;
    typedef void (*pfn_glGetFloati_v)(GLenum target, GLuint index, GLfloat *data);
    pfn_glGetFloati_v glGetFloati_v;
    typedef void (*pfn_glGetDoublei_v)(GLenum target, GLuint index, GLdouble *data);
    pfn_glGetDoublei_v glGetDoublei_v;

    // GL 4.2
    typedef void (*pfn_glDrawArraysInstancedBaseInstance)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
    pfn_glDrawArraysInstancedBaseInstance glDrawArraysInstancedBaseInstance;
    typedef void (*pfn_glDrawElementsInstancedBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance);
    pfn_glDrawElementsInstancedBaseInstance glDrawElementsInstancedBaseInstance;
    typedef void (*pfn_glDrawElementsInstancedBaseVertexBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
    pfn_glDrawElementsInstancedBaseVertexBaseInstance glDrawElementsInstancedBaseVertexBaseInstance;
    typedef void (*pfn_glGetInternalformativ)(GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params);
    pfn_glGetInternalformativ glGetInternalformativ;
    typedef void (*pfn_glGetActiveAtomicCounterBufferiv)(GLuint program, GLuint bufferIndex, GLenum pname, GLint *params);
    pfn_glGetActiveAtomicCounterBufferiv glGetActiveAtomicCounterBufferiv;
    typedef void (*pfn_glBindImageTexture)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
    pfn_glBindImageTexture glBindImageTexture;
    typedef void (*pfn_glMemoryBarrier)(GLbitfield barriers);
    pfn_glMemoryBarrier glMemoryBarrier;
    typedef void (*pfn_glTexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
    pfn_glTexStorage1D glTexStorage1D;
    typedef void (*pfn_glTexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
    pfn_glTexStorage2D glTexStorage2D;
    typedef void (*pfn_glTexStorage3D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
    pfn_glTexStorage3D glTexStorage3D;
    typedef void (*pfn_glDrawTransformFeedbackInstanced)(GLenum mode, GLuint id, GLsizei instancecount);
    pfn_glDrawTransformFeedbackInstanced glDrawTransformFeedbackInstanced;
    typedef void (*pfn_glDrawTransformFeedbackStreamInstanced)(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
    pfn_glDrawTransformFeedbackStreamInstanced glDrawTransformFeedbackStreamInstanced;

    // GL 4.3
    typedef void (*pfn_glClearBufferData)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
    pfn_glClearBufferData glClearBufferData;
    typedef void (*pfn_glClearBufferSubData)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
    pfn_glClearBufferSubData glClearBufferSubData;
    typedef void (*pfn_glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
    pfn_glDispatchCompute glDispatchCompute;
    typedef void (*pfn_glDispatchComputeIndirect)(GLintptr indirect);
    pfn_glDispatchComputeIndirect glDispatchComputeIndirect;
    typedef void (*pfn_glCopyImageSubData)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
    pfn_glCopyImageSubData glCopyImageSubData;
    typedef void (*pfn_glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);
    pfn_glFramebufferParameteri glFramebufferParameteri;
    typedef void (*pfn_glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint *params);
    pfn_glGetFramebufferParameteriv glGetFramebufferParameteriv;
    typedef void (*pfn_glGetInternalformati64v)(GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint64 *params);
    pfn_glGetInternalformati64v glGetInternalformati64v;
    typedef void (*pfn_glInvalidateTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
    pfn_glInvalidateTexSubImage glInvalidateTexSubImage;
    typedef void (*pfn_glInvalidateTexImage)(GLuint texture, GLint level);
    pfn_glInvalidateTexImage glInvalidateTexImage;
    typedef void (*pfn_glInvalidateBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr length);
    pfn_glInvalidateBufferSubData glInvalidateBufferSubData;
    typedef void (*pfn_glInvalidateBufferData)(GLuint buffer);
    pfn_glInvalidateBufferData glInvalidateBufferData;
    typedef void (*pfn_glInvalidateFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum *attachments);
    pfn_glInvalidateFramebuffer glInvalidateFramebuffer;
    typedef void (*pfn_glInvalidateSubFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glInvalidateSubFramebuffer glInvalidateSubFramebuffer;
    typedef void (*pfn_glMultiDrawArraysIndirect)(GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride);
    pfn_glMultiDrawArraysIndirect glMultiDrawArraysIndirect;
    typedef void (*pfn_glMultiDrawElementsIndirect)(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
    pfn_glMultiDrawElementsIndirect glMultiDrawElementsIndirect;
    typedef void (*pfn_glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint *params);
    pfn_glGetProgramInterfaceiv glGetProgramInterfaceiv;
    typedef GLuint (*pfn_glGetProgramResourceIndex)(GLuint program, GLenum programInterface, const GLchar *name);
    pfn_glGetProgramResourceIndex glGetProgramResourceIndex;
    typedef void (*pfn_glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
    pfn_glGetProgramResourceName glGetProgramResourceName;
    typedef void (*pfn_glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei count, GLsizei *length, GLint *params);
    pfn_glGetProgramResourceiv glGetProgramResourceiv;
    typedef GLint (*pfn_glGetProgramResourceLocation)(GLuint program, GLenum programInterface, const GLchar *name);
    pfn_glGetProgramResourceLocation glGetProgramResourceLocation;
    typedef GLint (*pfn_glGetProgramResourceLocationIndex)(GLuint program, GLenum programInterface, const GLchar *name);
    pfn_glGetProgramResourceLocationIndex glGetProgramResourceLocationIndex;
    typedef void (*pfn_glShaderStorageBlockBinding)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
    pfn_glShaderStorageBlockBinding glShaderStorageBlockBinding;
    typedef void (*pfn_glTexBufferRange)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
    pfn_glTexBufferRange glTexBufferRange;
    typedef void (*pfn_glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
    pfn_glTexStorage2DMultisample glTexStorage2DMultisample;
    typedef void (*pfn_glTexStorage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
    pfn_glTexStorage3DMultisample glTexStorage3DMultisample;
    typedef void (*pfn_glTextureView)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
    pfn_glTextureView glTextureView;
    typedef void (*pfn_glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
    pfn_glBindVertexBuffer glBindVertexBuffer;
    typedef void (*pfn_glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
    pfn_glVertexAttribFormat glVertexAttribFormat;
    typedef void (*pfn_glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    pfn_glVertexAttribIFormat glVertexAttribIFormat;
    typedef void (*pfn_glVertexAttribLFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    pfn_glVertexAttribLFormat glVertexAttribLFormat;
    typedef void (*pfn_glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex);
    pfn_glVertexAttribBinding glVertexAttribBinding;
    typedef void (*pfn_glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);
    pfn_glVertexBindingDivisor glVertexBindingDivisor;
    typedef void (*pfn_glDebugMessageControl)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
    pfn_glDebugMessageControl glDebugMessageControl;
    typedef void (*pfn_glDebugMessageInsert)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
    pfn_glDebugMessageInsert glDebugMessageInsert;
    typedef void (*pfn_glDebugMessageCallback)(GLDEBUGPROC callback, const void *userParam);
    pfn_glDebugMessageCallback glDebugMessageCallback;
    typedef GLuint (*pfn_glGetDebugMessageLog)(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
    pfn_glGetDebugMessageLog glGetDebugMessageLog;
    typedef void (*pfn_glPushDebugGroup)(GLenum source, GLuint id, GLsizei length, const GLchar *message);
    pfn_glPushDebugGroup glPushDebugGroup;
    typedef void (*pfn_glPopDebugGroup)(void);
    pfn_glPopDebugGroup glPopDebugGroup;
    typedef void (*pfn_glObjectLabel)(GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
    pfn_glObjectLabel glObjectLabel;
    typedef void (*pfn_glGetObjectLabel)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
    pfn_glGetObjectLabel glGetObjectLabel;
    typedef void (*pfn_glObjectPtrLabel)(const void *ptr, GLsizei length, const GLchar *label);
    pfn_glObjectPtrLabel glObjectPtrLabel;
    typedef void (*pfn_glGetObjectPtrLabel)(const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
    pfn_glGetObjectPtrLabel glGetObjectPtrLabel;

    // GL 4.4
    typedef void (*pfn_glBufferStorage)(GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
    pfn_glBufferStorage glBufferStorage;
    typedef void (*pfn_glClearTexImage)(GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
    pfn_glClearTexImage glClearTexImage;
    typedef void (*pfn_glClearTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
    pfn_glClearTexSubImage glClearTexSubImage;
    typedef void (*pfn_glBindBuffersBase)(GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
    pfn_glBindBuffersBase glBindBuffersBase;
    typedef void (*pfn_glBindBuffersRange)(GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
    pfn_glBindBuffersRange glBindBuffersRange;
    typedef void (*pfn_glBindTextures)(GLuint first, GLsizei count, const GLuint *textures);
    pfn_glBindTextures glBindTextures;
    typedef void (*pfn_glBindSamplers)(GLuint first, GLsizei count, const GLuint *samplers);
    pfn_glBindSamplers glBindSamplers;
    typedef void (*pfn_glBindImageTextures)(GLuint first, GLsizei count, const GLuint *textures);
    pfn_glBindImageTextures glBindImageTextures;
    typedef void (*pfn_glBindVertexBuffers)(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
    pfn_glBindVertexBuffers glBindVertexBuffers;

    // GL 4.5
    typedef void (*pfn_glClipControl)(GLenum origin, GLenum depth);
    pfn_glClipControl glClipControl;
    typedef void (*pfn_glCreateTransformFeedbacks)(GLsizei n, GLuint *ids);
    pfn_glCreateTransformFeedbacks glCreateTransformFeedbacks;
    typedef void (*pfn_glTransformFeedbackBufferBase)(GLuint xfb, GLuint index, GLuint buffer);
    pfn_glTransformFeedbackBufferBase glTransformFeedbackBufferBase;
    typedef void (*pfn_glTransformFeedbackBufferRange)(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
    pfn_glTransformFeedbackBufferRange glTransformFeedbackBufferRange;
    typedef void (*pfn_glGetTransformFeedbackiv)(GLuint xfb, GLenum pname, GLint *param);
    pfn_glGetTransformFeedbackiv glGetTransformFeedbackiv;
    typedef void (*pfn_glGetTransformFeedbacki_v)(GLuint xfb, GLenum pname, GLuint index, GLint *param);
    pfn_glGetTransformFeedbacki_v glGetTransformFeedbacki_v;
    typedef void (*pfn_glGetTransformFeedbacki64_v)(GLuint xfb, GLenum pname, GLuint index, GLint64 *param);
    pfn_glGetTransformFeedbacki64_v glGetTransformFeedbacki64_v;
    typedef void (*pfn_glCreateBuffers)(GLsizei n, GLuint *buffers);
    pfn_glCreateBuffers glCreateBuffers;
    typedef void (*pfn_glNamedBufferStorage)(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
    pfn_glNamedBufferStorage glNamedBufferStorage;
    typedef void (*pfn_glNamedBufferData)(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
    pfn_glNamedBufferData glNamedBufferData;
    typedef void (*pfn_glNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
    pfn_glNamedBufferSubData glNamedBufferSubData;
    typedef void (*pfn_glCopyNamedBufferSubData)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
    pfn_glCopyNamedBufferSubData glCopyNamedBufferSubData;
    typedef void (*pfn_glClearNamedBufferData)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
    pfn_glClearNamedBufferData glClearNamedBufferData;
    typedef void (*pfn_glClearNamedBufferSubData)(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
    pfn_glClearNamedBufferSubData glClearNamedBufferSubData;
    typedef void * (*pfn_glMapNamedBuffer)(GLuint buffer, GLenum access);
    pfn_glMapNamedBuffer glMapNamedBuffer;
    typedef void * (*pfn_glMapNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
    pfn_glMapNamedBufferRange glMapNamedBufferRange;
    typedef GLboolean (*pfn_glUnmapNamedBuffer)(GLuint buffer);
    pfn_glUnmapNamedBuffer glUnmapNamedBuffer;
    typedef void (*pfn_glFlushMappedNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length);
    pfn_glFlushMappedNamedBufferRange glFlushMappedNamedBufferRange;
    typedef void (*pfn_glGetNamedBufferParameteriv)(GLuint buffer, GLenum pname, GLint *params);
    pfn_glGetNamedBufferParameteriv glGetNamedBufferParameteriv;
    typedef void (*pfn_glGetNamedBufferParameteri64v)(GLuint buffer, GLenum pname, GLint64 *params);
    pfn_glGetNamedBufferParameteri64v glGetNamedBufferParameteri64v;
    typedef void (*pfn_glGetNamedBufferPointerv)(GLuint buffer, GLenum pname, void **params);
    pfn_glGetNamedBufferPointerv glGetNamedBufferPointerv;
    typedef void (*pfn_glGetNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
    pfn_glGetNamedBufferSubData glGetNamedBufferSubData;
    typedef void (*pfn_glCreateFramebuffers)(GLsizei n, GLuint *framebuffers);
    pfn_glCreateFramebuffers glCreateFramebuffers;
    typedef void (*pfn_glNamedFramebufferRenderbuffer)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    pfn_glNamedFramebufferRenderbuffer glNamedFramebufferRenderbuffer;
    typedef void (*pfn_glNamedFramebufferParameteri)(GLuint framebuffer, GLenum pname, GLint param);
    pfn_glNamedFramebufferParameteri glNamedFramebufferParameteri;
    typedef void (*pfn_glNamedFramebufferTexture)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
    pfn_glNamedFramebufferTexture glNamedFramebufferTexture;
    typedef void (*pfn_glNamedFramebufferTextureLayer)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
    pfn_glNamedFramebufferTextureLayer glNamedFramebufferTextureLayer;
    typedef void (*pfn_glNamedFramebufferDrawBuffer)(GLuint framebuffer, GLenum buf);
    pfn_glNamedFramebufferDrawBuffer glNamedFramebufferDrawBuffer;
    typedef void (*pfn_glNamedFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n, const GLenum *bufs);
    pfn_glNamedFramebufferDrawBuffers glNamedFramebufferDrawBuffers;
    typedef void (*pfn_glNamedFramebufferReadBuffer)(GLuint framebuffer, GLenum src);
    pfn_glNamedFramebufferReadBuffer glNamedFramebufferReadBuffer;
    typedef void (*pfn_glInvalidateNamedFramebufferData)(GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments);
    pfn_glInvalidateNamedFramebufferData glInvalidateNamedFramebufferData;
    typedef void (*pfn_glInvalidateNamedFramebufferSubData)(GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glInvalidateNamedFramebufferSubData glInvalidateNamedFramebufferSubData;
    typedef void (*pfn_glClearNamedFramebufferiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint *value);
    pfn_glClearNamedFramebufferiv glClearNamedFramebufferiv;
    typedef void (*pfn_glClearNamedFramebufferuiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint *value);
    pfn_glClearNamedFramebufferuiv glClearNamedFramebufferuiv;
    typedef void (*pfn_glClearNamedFramebufferfv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat *value);
    pfn_glClearNamedFramebufferfv glClearNamedFramebufferfv;
    typedef void (*pfn_glClearNamedFramebufferfi)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
    pfn_glClearNamedFramebufferfi glClearNamedFramebufferfi;
    typedef void (*pfn_glBlitNamedFramebuffer)(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    pfn_glBlitNamedFramebuffer glBlitNamedFramebuffer;
    typedef GLenum (*pfn_glCheckNamedFramebufferStatus)(GLuint framebuffer, GLenum target);
    pfn_glCheckNamedFramebufferStatus glCheckNamedFramebufferStatus;
    typedef void (*pfn_glGetNamedFramebufferParameteriv)(GLuint framebuffer, GLenum pname, GLint *param);
    pfn_glGetNamedFramebufferParameteriv glGetNamedFramebufferParameteriv;
    typedef void (*pfn_glGetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);
    pfn_glGetNamedFramebufferAttachmentParameteriv glGetNamedFramebufferAttachmentParameteriv;
    typedef void (*pfn_glCreateRenderbuffers)(GLsizei n, GLuint *renderbuffers);
    pfn_glCreateRenderbuffers glCreateRenderbuffers;
    typedef void (*pfn_glNamedRenderbufferStorage)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
    pfn_glNamedRenderbufferStorage glNamedRenderbufferStorage;
    typedef void (*pfn_glNamedRenderbufferStorageMultisample)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
    pfn_glNamedRenderbufferStorageMultisample glNamedRenderbufferStorageMultisample;
    typedef void (*pfn_glGetNamedRenderbufferParameteriv)(GLuint renderbuffer, GLenum pname, GLint *params);
    pfn_glGetNamedRenderbufferParameteriv glGetNamedRenderbufferParameteriv;
    typedef void (*pfn_glCreateTextures)(GLenum target, GLsizei n, GLuint *textures);
    pfn_glCreateTextures glCreateTextures;
    typedef void (*pfn_glTextureBuffer)(GLuint texture, GLenum internalformat, GLuint buffer);
    pfn_glTextureBuffer glTextureBuffer;
    typedef void (*pfn_glTextureBufferRange)(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
    pfn_glTextureBufferRange glTextureBufferRange;
    typedef void (*pfn_glTextureStorage1D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
    pfn_glTextureStorage1D glTextureStorage1D;
    typedef void (*pfn_glTextureStorage2D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
    pfn_glTextureStorage2D glTextureStorage2D;
    typedef void (*pfn_glTextureStorage3D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
    pfn_glTextureStorage3D glTextureStorage3D;
    typedef void (*pfn_glTextureStorage2DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
    pfn_glTextureStorage2DMultisample glTextureStorage2DMultisample;
    typedef void (*pfn_glTextureStorage3DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
    pfn_glTextureStorage3DMultisample glTextureStorage3DMultisample;
    typedef void (*pfn_glTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
    pfn_glTextureSubImage1D glTextureSubImage1D;
    typedef void (*pfn_glTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
    pfn_glTextureSubImage2D glTextureSubImage2D;
    typedef void (*pfn_glTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
    pfn_glTextureSubImage3D glTextureSubImage3D;
    typedef void (*pfn_glCompressedTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
    pfn_glCompressedTextureSubImage1D glCompressedTextureSubImage1D;
    typedef void (*pfn_glCompressedTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
    pfn_glCompressedTextureSubImage2D glCompressedTextureSubImage2D;
    typedef void (*pfn_glCompressedTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
    pfn_glCompressedTextureSubImage3D glCompressedTextureSubImage3D;
    typedef void (*pfn_glCopyTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
    pfn_glCopyTextureSubImage1D glCopyTextureSubImage1D;
    typedef void (*pfn_glCopyTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glCopyTextureSubImage2D glCopyTextureSubImage2D;
    typedef void (*pfn_glCopyTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    pfn_glCopyTextureSubImage3D glCopyTextureSubImage3D;
    typedef void (*pfn_glTextureParameterf)(GLuint texture, GLenum pname, GLfloat param);
    pfn_glTextureParameterf glTextureParameterf;
    typedef void (*pfn_glTextureParameterfv)(GLuint texture, GLenum pname, const GLfloat *param);
    pfn_glTextureParameterfv glTextureParameterfv;
    typedef void (*pfn_glTextureParameteri)(GLuint texture, GLenum pname, GLint param);
    pfn_glTextureParameteri glTextureParameteri;
    typedef void (*pfn_glTextureParameterIiv)(GLuint texture, GLenum pname, const GLint *params);
    pfn_glTextureParameterIiv glTextureParameterIiv;
    typedef void (*pfn_glTextureParameterIuiv)(GLuint texture, GLenum pname, const GLuint *params);
    pfn_glTextureParameterIuiv glTextureParameterIuiv;
    typedef void (*pfn_glTextureParameteriv)(GLuint texture, GLenum pname, const GLint *param);
    pfn_glTextureParameteriv glTextureParameteriv;
    typedef void (*pfn_glGenerateTextureMipmap)(GLuint texture);
    pfn_glGenerateTextureMipmap glGenerateTextureMipmap;
    typedef void (*pfn_glBindTextureUnit)(GLuint unit, GLuint texture);
    pfn_glBindTextureUnit glBindTextureUnit;
    typedef void (*pfn_glGetTextureImage)(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
    pfn_glGetTextureImage glGetTextureImage;
    typedef void (*pfn_glGetCompressedTextureImage)(GLuint texture, GLint level, GLsizei bufSize, void *pixels);
    pfn_glGetCompressedTextureImage glGetCompressedTextureImage;
    typedef void (*pfn_glGetTextureLevelParameterfv)(GLuint texture, GLint level, GLenum pname, GLfloat *params);
    pfn_glGetTextureLevelParameterfv glGetTextureLevelParameterfv;
    typedef void (*pfn_glGetTextureLevelParameteriv)(GLuint texture, GLint level, GLenum pname, GLint *params);
    pfn_glGetTextureLevelParameteriv glGetTextureLevelParameteriv;
    typedef void (*pfn_glGetTextureParameterfv)(GLuint texture, GLenum pname, GLfloat *params);
    pfn_glGetTextureParameterfv glGetTextureParameterfv;
    typedef void (*pfn_glGetTextureParameterIiv)(GLuint texture, GLenum pname, GLint *params);
    pfn_glGetTextureParameterIiv glGetTextureParameterIiv;
    typedef void (*pfn_glGetTextureParameterIuiv)(GLuint texture, GLenum pname, GLuint *params);
    pfn_glGetTextureParameterIuiv glGetTextureParameterIuiv;
    typedef void (*pfn_glGetTextureParameteriv)(GLuint texture, GLenum pname, GLint *params);
    pfn_glGetTextureParameteriv glGetTextureParameteriv;
    typedef void (*pfn_glCreateVertexArrays)(GLsizei n, GLuint *arrays);
    pfn_glCreateVertexArrays glCreateVertexArrays;
    typedef void (*pfn_glDisableVertexArrayAttrib)(GLuint vaobj, GLuint index);
    pfn_glDisableVertexArrayAttrib glDisableVertexArrayAttrib;
    typedef void (*pfn_glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
    pfn_glEnableVertexArrayAttrib glEnableVertexArrayAttrib;
    typedef void (*pfn_glVertexArrayElementBuffer)(GLuint vaobj, GLuint buffer);
    pfn_glVertexArrayElementBuffer glVertexArrayElementBuffer;
    typedef void (*pfn_glVertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
    pfn_glVertexArrayVertexBuffer glVertexArrayVertexBuffer;
    typedef void (*pfn_glVertexArrayVertexBuffers)(GLuint vaobj, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
    pfn_glVertexArrayVertexBuffers glVertexArrayVertexBuffers;
    typedef void (*pfn_glVertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
    pfn_glVertexArrayAttribBinding glVertexArrayAttribBinding;
    typedef void (*pfn_glVertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
    pfn_glVertexArrayAttribFormat glVertexArrayAttribFormat;
    typedef void (*pfn_glVertexArrayAttribIFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    pfn_glVertexArrayAttribIFormat glVertexArrayAttribIFormat;
    typedef void (*pfn_glVertexArrayAttribLFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    pfn_glVertexArrayAttribLFormat glVertexArrayAttribLFormat;
    typedef void (*pfn_glVertexArrayBindingDivisor)(GLuint vaobj, GLuint bindingindex, GLuint divisor);
    pfn_glVertexArrayBindingDivisor glVertexArrayBindingDivisor;
    typedef void (*pfn_glGetVertexArrayiv)(GLuint vaobj, GLenum pname, GLint *param);
    pfn_glGetVertexArrayiv glGetVertexArrayiv;
    typedef void (*pfn_glGetVertexArrayIndexediv)(GLuint vaobj, GLuint index, GLenum pname, GLint *param);
    pfn_glGetVertexArrayIndexediv glGetVertexArrayIndexediv;
    typedef void (*pfn_glGetVertexArrayIndexed64iv)(GLuint vaobj, GLuint index, GLenum pname, GLint64 *param);
    pfn_glGetVertexArrayIndexed64iv glGetVertexArrayIndexed64iv;
    typedef void (*pfn_glCreateSamplers)(GLsizei n, GLuint *samplers);
    pfn_glCreateSamplers glCreateSamplers;
    typedef void (*pfn_glCreateProgramPipelines)(GLsizei n, GLuint *pipelines);
    pfn_glCreateProgramPipelines glCreateProgramPipelines;
    typedef void (*pfn_glCreateQueries)(GLenum target, GLsizei n, GLuint *ids);
    pfn_glCreateQueries glCreateQueries;
    typedef void (*pfn_glGetQueryBufferObjecti64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
    pfn_glGetQueryBufferObjecti64v glGetQueryBufferObjecti64v;
    typedef void (*pfn_glGetQueryBufferObjectiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
    pfn_glGetQueryBufferObjectiv glGetQueryBufferObjectiv;
    typedef void (*pfn_glGetQueryBufferObjectui64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
    pfn_glGetQueryBufferObjectui64v glGetQueryBufferObjectui64v;
    typedef void (*pfn_glGetQueryBufferObjectuiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
    pfn_glGetQueryBufferObjectuiv glGetQueryBufferObjectuiv;
    typedef void (*pfn_glMemoryBarrierByRegion)(GLbitfield barriers);
    pfn_glMemoryBarrierByRegion glMemoryBarrierByRegion;
    typedef void (*pfn_glGetTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
    pfn_glGetTextureSubImage glGetTextureSubImage;
    typedef void (*pfn_glGetCompressedTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void *pixels);
    pfn_glGetCompressedTextureSubImage glGetCompressedTextureSubImage;
    typedef GLenum (*pfn_glGetGraphicsResetStatus)(void);
    pfn_glGetGraphicsResetStatus glGetGraphicsResetStatus;
    typedef void (*pfn_glGetnCompressedTexImage)(GLenum target, GLint lod, GLsizei bufSize, void *pixels);
    pfn_glGetnCompressedTexImage glGetnCompressedTexImage;
    typedef void (*pfn_glGetnTexImage)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
    pfn_glGetnTexImage glGetnTexImage;
    typedef void (*pfn_glGetnUniformdv)(GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
    pfn_glGetnUniformdv glGetnUniformdv;
    typedef void (*pfn_glGetnUniformfv)(GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
    pfn_glGetnUniformfv glGetnUniformfv;
    typedef void (*pfn_glGetnUniformiv)(GLuint program, GLint location, GLsizei bufSize, GLint *params);
    pfn_glGetnUniformiv glGetnUniformiv;
    typedef void (*pfn_glGetnUniformuiv)(GLuint program, GLint location, GLsizei bufSize, GLuint *params);
    pfn_glGetnUniformuiv glGetnUniformuiv;
    typedef void (*pfn_glReadnPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
    pfn_glReadnPixels glReadnPixels;
    typedef void (*pfn_glGetnMapdv)(GLenum target, GLenum query, GLsizei bufSize, GLdouble *v);
    pfn_glGetnMapdv glGetnMapdv;
    typedef void (*pfn_glGetnMapfv)(GLenum target, GLenum query, GLsizei bufSize, GLfloat *v);
    pfn_glGetnMapfv glGetnMapfv;
    typedef void (*pfn_glGetnMapiv)(GLenum target, GLenum query, GLsizei bufSize, GLint *v);
    pfn_glGetnMapiv glGetnMapiv;
    typedef void (*pfn_glGetnPixelMapfv)(GLenum map, GLsizei bufSize, GLfloat *values);
    pfn_glGetnPixelMapfv glGetnPixelMapfv;
    typedef void (*pfn_glGetnPixelMapuiv)(GLenum map, GLsizei bufSize, GLuint *values);
    pfn_glGetnPixelMapuiv glGetnPixelMapuiv;
    typedef void (*pfn_glGetnPixelMapusv)(GLenum map, GLsizei bufSize, GLushort *values);
    pfn_glGetnPixelMapusv glGetnPixelMapusv;
    typedef void (*pfn_glGetnPolygonStipple)(GLsizei bufSize, GLubyte *pattern);
    pfn_glGetnPolygonStipple glGetnPolygonStipple;
    typedef void (*pfn_glGetnColorTable)(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void *table);
    pfn_glGetnColorTable glGetnColorTable;
    typedef void (*pfn_glGetnConvolutionFilter)(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void *image);
    pfn_glGetnConvolutionFilter glGetnConvolutionFilter;
    typedef void (*pfn_glGetnSeparableFilter)(GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, void *row, GLsizei columnBufSize, void *column, void *span);
    pfn_glGetnSeparableFilter glGetnSeparableFilter;
    typedef void (*pfn_glGetnHistogram)(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void *values);
    pfn_glGetnHistogram glGetnHistogram;
    typedef void (*pfn_glGetnMinmax)(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void *values);
    pfn_glGetnMinmax glGetnMinmax;
    typedef void (*pfn_glTextureBarrier)(void);
    pfn_glTextureBarrier glTextureBarrier;

#ifdef __cplusplus
}
#endif
#define _P_L_H
#endif //_P_L_H
