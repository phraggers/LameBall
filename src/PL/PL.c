/*==============================
  Phraglib
  Platform implementations
  PL.c
===============================*/

/*
TODO:
- win32: xaudio2 buffer loading helpers, queue etc
- data compression/decompression
- vectors
- GL helpers/maths
- format loaders/decoders (bmp, png, jpg, wav, mp3, ogg, etc)
- threading

- linux
- macos
*/

#include "PL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

#define PL_OPENGL_MAJ 4
#define PL_OPENGL_MIN 5

/*==========================
  Platform non-specific
==========================*/

void PL_MemZero(ptr mem, u64 size)
{
    if(mem)
    {
        u8* byte = (u8*)(mem);
        for(u64 i=0; i<size; i++)
        {
            *byte++ = 0;
        }
    }
}

ptr PL_MemSet(ptr mem, u8 value, u64 size)
{
    if(!mem || !size)
    {
        return 0;
    }
    
    u8* byte = (u8*)(mem);
    for(u64 i=0; i<size; i++)
    {
        *byte++ = value;
    }
    
    return byte;
}

u64 PL_MemCpy(ptr src, ptr dst, u64 size)
{
    if(!src || !dst || !size)
    {
        return 0;
    }
    
    u64 result = 0;
    u64 byteCount = size;
    u8 *sb = (u8*)src;
    u8 *db = (u8*)dst;
    
    while(byteCount)
    {
        *db++ = *sb++;
        byteCount--;
        result++;
    }
    
    return result;
}

u64 PL_StrLen(cstr str)
{
    if(!str)
    {
        return 0;
    }
    
    u64 result = 0;
    
    for(cstr c = str;
        *c != 0;
        c++)
    {
        result++;
    }
    
    return result;
}

u64 PL_StrCpy(cstr src, cstr dst, u64 len)
{
    if(!src || !dst || !len)
    {
        return 0;
    }
    
    cstr sc = src;
    cstr dc = dst;
    u64 result = 0;
    
    for(result = 0;
        result < (len);
        result++)
    {
        if(*sc)
        {
            *dc++ = *sc++;
        }
        
        else
        {
            break;
        }
    }
    
    *dc = 0; // append \0 to end of string
    return result;
}

u64 PL_StrCat(cstr src, cstr dst, u64 len)
{
    if(!src || !dst || !len)
    {
        return 0;
    }
    
    u64 result = 0;
    cstr sc = src;
    cstr dc = 0;
    
    for(dc = dst;
        *dc != 0;
        dc++)
    {
        if(result == len)
        {
            return result;
        }
        
        else
        {
            result++;
        }
    }
    
    for(;
        result < (len);
        result++)
    {
        if(*sc)
        {
            *dc++ = *sc++;
        }
        
        else
        {
            break;
        }
    }
    
    *dc = 0; // append \0 to end of string
    return result;
}

void PL_StrVar(cstr dst, u64 maxlen,
               const cstr format, ...)
{
    if(!dst || !maxlen || !format)
    {
        return;
    }
    
    va_list args;
    va_start(args, format);
    vsnprintf(dst, maxlen, format, args);
    va_end(args);
}

void PL_Print(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

void PL_PrintErr(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);
}

// fwd decl for funcs to use this before it is defined
void PL_SetErrorString(const cstr format, ...);

/*============= Maths Functions ==================*/

i32 PL_sign(i32 v)
{
    return (v>=0)? 1 : -1;
}

r32 PL_abs(r32 v)
{
    return (r32)fabs(v);
}

i32 PL_round_i32(r32 v)
{
    return (i32)floorf(v);
}

u32 PL_round_u32(r32 v)
{
    return (u32)floorf(v);
}

i32 PL_trunc_i32(r32 v)
{
    return (i32)v;
}

i32 PL_floor_i32(r32 v)
{
    return (i32)floorf(v);
}

i32 PL_ceil_i32(r32 v)
{
    return (i32)ceilf(v);
}

r32 PL_div(r32 n, r32 d)
{
    if(d != 0.0f)
    {
        return n/d;
    }
    
    return 0.0f;
}

r64 PL_norm64(r64 v, r64 min, r64 max)
{
    return ((v-min) / (max-min));
}

r32 PL_norm32(r32 v, r32 min, r32 max)
{
    return ((v-min) / (max-min));
}

r32 PL_clamp(r32 v, r32 min, r32 max)
{
    return ((((v<min)?min:v)>max)?max:v);
}

r32 PL_clampMin(r32 v, r32 min)
{
    return ((v<min)?min:v);
}

r32 PL_clampMax(r32 v, r32 max)
{
    return ((v>max)?max:v);
}

r32 PL_clampNorm(r32 v)
{
    return ((((v<0.0f)?0.0f:v)>1.0f)?1.0f:v);
}

u32 PL_rotl(u32 v, i32 s)
{
    s &= 31;
    return ((v<<s) | (v>>(32-s)));
}

u32 PL_rotr(u32 v, i32 s)
{
    s &= 31;
    return ((v>>s) | (v<<(32-s)));
}

r32 PL_lerp(r32 a, r32 b, r32 t)
{
    return ((1.0f-t) *a) + (t*b);
}

r32 PL_qlerp(r32 a, r32 b, r32 t)
{
    return a + t * (b - a);
}

r32 PL_sqrt(r32 v)
{
    return sqrtf(v);
}

r32 PL_square(r32 v)
{
    return v*v;
}

r32 PL_pow32(r32 v, r32 s)
{
    return powf(v,s);
}

r64 PL_pi64(void)
{
    return (r64)3.141592653589793238462643383279502884L;
}

r32 PL_pi32(void)
{
    return (r32)3.141592653589793238463;
}

r64 PL_tau64(void)
{
    return (r64)6.283185307179586476925286766559L;
}

r32 PL_tau32(void)
{
    return (r32)6.2831853071795864769;
}

r32 PL_sin(r32 a)
{
    return sinf(a);
}

r32 PL_cos(r32 a)
{
    return cosf(a);
}

r32 PL_atan2(r32 y, r32 x)
{
    return atan2f(y,x);
}

/*=========== Vectors =============*/
// Thanks: Casey Muratori (Handmade Hero) for showing how to do vectors properly!

// ------ v2 ------ //

// constructors
v2 v2i(i32 a, i32 b)
{
    v2 result = {(r32)a, (r32)b};
    return result;
}
v2 v2u(u32 a, u32 b)
{
    v2 result = {(r32)a, (r32)b};
    return result;    
}
v2 v2r(r32 a, r32 b)
{
    v2 result = {a, b};
    return result;    
}

// maths operators
v2 v2add(v2 a, v2 b)
{
    v2 result = {a.i[0]+b.i[0], a.i[1]+b.i[1]};
    return result;
}
v2 v2sub(v2 a, v2 b)
{
    v2 result = {a.i[0]-b.i[0], a.i[1]-b.i[1]};
    return result;
}
v2 v2mul(v2 a, r32 v)
{
    v2 result = {a.i[0]*v, a.i[1]*v};
    return result;
}
v2 v2div(v2 a, r32 v)
{
    v2 result = {a.i[0]/v, a.i[1]/v};
    return result;    
}
v2 v2perp(v2 a)
{
    v2 result = {-a.y, a.x};
    return result;
}
v2 v2hadamard(v2 a, v2 b)
{
    v2 result = {a.i[0]*b.i[0], a.i[1]*b.i[1]};
    return result;
}
r32 v2dot(v2 a, v2 b)
{
    r32 result = (a.i[0]*b.i[0]) + (a.i[1]*b.i[1]);
    return result;
}
r32 v2lengthsq(v2 a)
{
    r32 result = v2dot(a,a);
    return result;
}
r32 v2length(v2 a)
{
    r32 result = PL_sqrt(v2lengthsq(a));
    return result;
}

// ------ v3 ------ //

// constructors
v3 v3i(i32 a, i32 b, i32 c)
{
    v3 result = {(r32)a, (r32)b, (r32)c};
    return result;
}
v3 v3u(u32 a, u32 b, u32 c)
{
    v3 result = {(r32)a, (r32)b, (r32)c};
    return result;    
}
v3 v3r(r32 a, r32 b, r32 c)
{
    v3 result = {a, b, c};
    return result;    
}

// maths operators
v3 v3add(v3 a, v3 b)
{
    v3 result = {a.i[0]+b.i[0], a.i[1]+b.i[1], a.i[2]+b.i[2]};
    return result;
}
v3 v3sub(v3 a, v3 b)
{
    v3 result = {a.i[0]-b.i[0], a.i[1]-b.i[1], a.i[2]-b.i[2]};
    return result;
}
v3 v3mul(v3 a, r32 v)
{
    v3 result = {a.i[0]*v, a.i[1]*v, a.i[2]*v};
    return result;
}
v3 v3div(v3 a, r32 v)
{
    v3 result = {a.i[0]/v, a.i[1]/v, a.i[2]/v};
    return result;    
}
v3 v3hadamard(v3 a, v3 b)
{
    v3 result = {a.i[0]*b.i[0], a.i[1]*b.i[1], a.i[2]*b.i[2]};
    return result;
}
r32 v3dot(v3 a, v3 b)
{
    r32 result = (a.i[0]*b.i[0]) + (a.i[1]*b.i[1]) + (a.i[2]*b.i[2]);
    return result;
}
r32 v3lengthsq(v3 a)
{
    r32 result = v3dot(a,a);
    return result;
}
r32 v3length(v3 a)
{
    r32 result = PL_sqrt(v3sq(a));
    return result;
}

// ------ v4 ------ //

// constructors
v4 v4i(i32 a, i32 b, i32 c, i32 d)
{
    v4 result = {(r32)a, (r32)b, (r32)c, (r32)d};
    return result;
}
v4 v4u(u32 a, u32 b, u32 c, u32 d)
{
    v4 result = {(r32)a, (r32)b, (r32)c, (r32)d};
    return result;    
}
v4 v4r(r32 a, r32 b, r32 c, r32 d)
{
    v4 result = {a, b, c, d};
    return result;    
}

// maths operators
v4 v4add(v4 a, v4 b)
{
    v4 result = {a.i[0]+b.i[0], a.i[1]+b.i[1], a.i[2]+b.i[2], a.i[3]+b.i[3]};
    return result;
}
v4 v4sub(v4 a, v4 b)
{
    v4 result = {a.i[0]-b.i[0], a.i[1]-b.i[1], a.i[2]-b.i[2], a.i[3]+b.i[3]};
    return result;
}
v4 v4mul(v4 a, r32 v)
{
    v4 result = {a.i[0]*v, a.i[1]*v, a.i[2]*v, a.i[3]*v};
    return result;
}
v4 v4div(v4 a, r32 v)
{
    v4 result = {a.i[0]/v, a.i[1]/v, a.i[2]/v, a.i[3]/v};
    return result;    
}
v4 v4hadamard(v4 a, v4 b)
{
    v4 result = {a.i[0]*b.i[0], a.i[1]*b.i[1], a.i[2]*b.i[2]};
    return result;
}
r32 v4dot(v4 a, v4 b)
{
    r32 result = (a.i[0]*b.i[0]) + (a.i[1]*b.i[1]) + (a.i[2]*b.i[2]);
    return result;
}
r32 v4lengthsq(v4 a)
{
    r32 result = v4dot(a,a);
    return result;
}
r32 v4length(v4 a)
{
    r32 result = PL_sqrt(v4sq(a));
    return result;
}

/*=========== HASH =============*/

// CREDIT: modified version from Paul Hsieh: www.azillionmonkeys.com/qed/hash.html
// I won't pretend to understand the magic constants used here
u32 PL_Hash32(ptr input, u64 inputSize)
{
    char placeholder[4] = {0};
    for(u8 i=0;i<(u8)inputSize;i++)
    {
        i8 *p=(i8*)input;
        p+=i;
        placeholder[i]=*p;
    }
    i8 *in = (i8*)input;
    if(inputSize<4) in = placeholder;
    u32 result = (u32)inputSize;
    u32 tmp;
    u64 len = inputSize;
    int rlen;
    
    rlen = len & 3;
    len >>= 2;
    
    while(len > 0)
    {
        result += PL_Reinterpret(u16, in);
        tmp = (PL_Reinterpret(u16, in + 2) << 11) ^ result;
        result = (result << 16) ^ tmp;
        in += 2 * sizeof(u16);
        result += result >> 11;
        len--;
    }
    
    switch (rlen)
    {
        case 1:
        {
            result += *in;
            result ^= result << 10;
            result += result >> 1;
        } break;
        case 2:
        {
            result += PL_Reinterpret(u16, in);
            result ^= result << 11;
            result += result >> 17;
        } break;
        case 3:
        {
            result += PL_Reinterpret(u16, in);
            result ^= result << 16;
            result ^= (in[sizeof(u16)]) << 18;
            result += result >> 11;
        } break;
    }
    
    result ^= result << 3;
    result += result >> 5;
    result ^= result << 4;
    result += result >> 17;
    result ^= result << 25;
    result += result >> 6;
    
    return result;
}

/*=========== RAND =================*/

static b8 PL_RAND_IS_INIT;
typedef union
{
    u8 rb[4];
    u32 ru32;
} PL_RAND_UNION;

r32 PL_Rand(void)
{
    if(!PL_RAND_IS_INIT)
    {
        time_t t = time(0);
        srand(PL_Hash32(&t, sizeof(time_t)));
        PL_RAND_IS_INIT = 1;
    }
    
    PL_RAND_UNION ru;
    for(int i=0;i<4;i++) {ru.rb[i] = (u8)(rand()%0xff);}
    r32 result = PL_norm32((r32)ru.ru32, 0.0f, (r32)UINT32_MAX);
    
    return result;
}

void PL_SeedRand(ptr value, u64 valueSize)
{
    srand(PL_Hash32(value, valueSize));
    PL_RAND_IS_INIT = 1;
}

/*======== OpenGL Helpers ==============*/

r32 PL_GetGLVersion(void)
{
    return ((r32)PL_OPENGL_MAJ + ((r32)PL_OPENGL_MIN/10.0f));
}

u32 PL_GLCreateProgram(cstr vertexShaderSrc, cstr fragmentShaderSrc)
{
    u32 vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    u32 fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    u32 result = 0;
    
    { // VertexShader compilation
        glShaderSource(vertexShaderID, 1, &vertexShaderSrc, 0);
        glCompileShader(vertexShaderID);
        i32 success;
        char infoLog[512] = {0};
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertexShaderID, 512, 0, infoLog);
            PL_SetErrorString("GLCreateProgram error: VertexShader failed to compile: %s", infoLog);
            return 0;
        }
    }
    
    { // FragmentShader compilation
        glShaderSource(fragmentShaderID, 1, &fragmentShaderSrc, 0);
        glCompileShader(fragmentShaderID);
        i32 success;
        char infoLog[512] = {0};
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragmentShaderID, 512, 0, infoLog);
            PL_SetErrorString("GLCreateProgram error: FragmentShader failed to compile: %s", infoLog);
            return 0;
        }
    }
    
    result = glCreateProgram();
    glAttachShader(result, vertexShaderID);
    glAttachShader(result, fragmentShaderID);
    glLinkProgram(result);
    { // program compilation
        i32 success;
        char infoLog[512] = {0};
        glGetProgramiv(result, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(result, 512, 0, infoLog);
            PL_SetErrorString("GLCreateProgram error: Program failed to compile: %s", infoLog);
        }
    }
    
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    return result;
}

/*==============================
      PHRAGLIB WIN32
      Windows Specific
===============================*/
#if defined(PL_WINDOWS)
#include <windows.h>
#include <xinput.h>
#include <xaudio2.h>

/* ======= WIN32 LIB FUNCTIONS ======== */

#define WIN32_LOAD_DLLFN(name) {name = (tfn_##name *)GetProcAddress(lib, #name ); if(!name) return 0;}

/*============ USER32 ================*/

typedef BOOL tfn_TranslateMessage(const MSG *lpMsg);
tfn_TranslateMessage *pfn_TranslateMessage;
#define TranslateMessage pfn_TranslateMessage

typedef LRESULT tfn_DispatchMessageA(const MSG *lpMsg);
tfn_DispatchMessageA *pfn_DispatchMessageA;
#define DispatchMessageA pfn_DispatchMessageA

typedef BOOL tfn_PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
tfn_PeekMessageA *pfn_PeekMessageA;
#define PeekMessageA pfn_PeekMessageA

typedef LRESULT tfn_DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
tfn_DefWindowProcA *pfn_DefWindowProcA;
#define DefWindowProcA pfn_DefWindowProcA

typedef ATOM tfn_RegisterClassA(const WNDCLASSA *lpWndClass);
tfn_RegisterClassA *pfn_RegisterClassA;
#define RegisterClassA pfn_RegisterClassA

typedef HWND tfn_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                                 int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
tfn_CreateWindowExA *pfn_CreateWindowExA;
#define CreateWindowExA pfn_CreateWindowExA

typedef BOOL tfn_DestroyWindow(HWND hWnd);
tfn_DestroyWindow *pfn_DestroyWindow;
#define DestroyWindow pfn_DestroyWindow

typedef BOOL tfn_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
tfn_MoveWindow *pfn_MoveWindow;
#define MoveWindow pfn_MoveWindow

typedef BOOL tfn_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
tfn_SetWindowPos *pfn_SetWindowPos;
#define SetWindowPos pfn_SetWindowPos

typedef BOOL tfn_GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
tfn_GetWindowPlacement *pfn_GetWindowPlacement;
#define GetWindowPlacement pfn_GetWindowPlacement

typedef BOOL tfn_SetWindowPlacement(HWND hWnd, const WINDOWPLACEMENT *lpwndpl);
tfn_SetWindowPlacement *pfn_SetWindowPlacement;
#define SetWindowPlacement pfn_SetWindowPlacement

typedef HDC tfn_GetDC(HWND hWnd);
tfn_GetDC *pfn_GetDC;
#define GetDC pfn_GetDC

typedef int tfn_ReleaseDC(HWND hWnd, HDC  hDC);
tfn_ReleaseDC *pfn_ReleaseDC;
#define ReleaseDC pfn_ReleaseDC

typedef BOOL tfn_SetWindowTextA(HWND hWnd, LPCSTR lpString);
tfn_SetWindowTextA *pfn_SetWindowTextA;
#define SetWindowTextA pfn_SetWindowTextA

typedef int tfn_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
tfn_MessageBoxA *pfn_MessageBoxA;
#define MessageBoxA pfn_MessageBoxA

typedef BOOL tfn_GetCursorPos(LPPOINT lpPoint);
tfn_GetCursorPos *pfn_GetCursorPos;
#define GetCursorPos pfn_GetCursorPos

typedef BOOL tfn_ScreenToClient(HWND hWnd, LPPOINT lpPoint);
tfn_ScreenToClient *pfn_ScreenToClient;
#define ScreenToClient pfn_ScreenToClient

typedef LONG tfn_GetWindowLongA(HWND hWnd, int nIndex);
tfn_GetWindowLongA *pfn_GetWindowLongA;
#define GetWindowLongA pfn_GetWindowLongA

typedef LONG tfn_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
tfn_SetWindowLongA *pfn_SetWindowLongA;
#define SetWindowLongA pfn_SetWindowLongA

typedef HMONITOR tfn_MonitorFromWindow(HWND hwnd, DWORD dwFlags);
tfn_MonitorFromWindow *pfn_MonitorFromWindow;
#define MonitorFromWindow pfn_MonitorFromWindow

typedef BOOL tfn_GetMonitorInfoA(HMONITOR hMonitor, LPMONITORINFO lpmi);
tfn_GetMonitorInfoA *pfn_GetMonitorInfoA;
#define GetMonitorInfoA pfn_GetMonitorInfoA

typedef BOOL tfn_GetClientRect(HWND hWnd, LPRECT lpRect);
tfn_GetClientRect *pfn_GetClientRect;
#define GetClientRect pfn_GetClientRect

typedef BOOL tfn_GetWindowRect(HWND hWnd, LPRECT lpRect);
tfn_GetWindowRect *pfn_GetWindowRect;
#define GetWindowRect pfn_GetWindowRect

static b32 Win32_LoadUserLib()
{
    HMODULE lib = LoadLibraryA("user32.dll");
    if(!lib) return 0;
    
    WIN32_LOAD_DLLFN(TranslateMessage);
    WIN32_LOAD_DLLFN(DispatchMessageA);
    WIN32_LOAD_DLLFN(PeekMessageA);
    WIN32_LOAD_DLLFN(DefWindowProcA);
    WIN32_LOAD_DLLFN(RegisterClassA);
    WIN32_LOAD_DLLFN(CreateWindowExA);
    WIN32_LOAD_DLLFN(DestroyWindow);
    WIN32_LOAD_DLLFN(MoveWindow);
    WIN32_LOAD_DLLFN(SetWindowPos);
    WIN32_LOAD_DLLFN(GetWindowPlacement);
    WIN32_LOAD_DLLFN(SetWindowPlacement);
    WIN32_LOAD_DLLFN(GetDC);
    WIN32_LOAD_DLLFN(ReleaseDC);
    WIN32_LOAD_DLLFN(SetWindowTextA);
    WIN32_LOAD_DLLFN(MessageBoxA);
    WIN32_LOAD_DLLFN(GetCursorPos);
    WIN32_LOAD_DLLFN(ScreenToClient);
    WIN32_LOAD_DLLFN(GetWindowLongA);
    WIN32_LOAD_DLLFN(SetWindowLongA);
    WIN32_LOAD_DLLFN(MonitorFromWindow);
    WIN32_LOAD_DLLFN(GetMonitorInfoA);
    WIN32_LOAD_DLLFN(GetClientRect);
    WIN32_LOAD_DLLFN(GetWindowRect);
    
    return 1;
}

/*========== GDI32 =============*/

typedef int tfn_ChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd);
tfn_ChoosePixelFormat *pfn_ChoosePixelFormat;
#define ChoosePixelFormat pfn_ChoosePixelFormat

typedef int tfn_DescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd);
tfn_DescribePixelFormat *pfn_DescribePixelFormat;
#define DescribePixelFormat pfn_DescribePixelFormat

typedef BOOL tfn_SetPixelFormat(HDC hdc, int format, const PIXELFORMATDESCRIPTOR *ppfd);
tfn_SetPixelFormat *pfn_SetPixelFormat;
#define SetPixelFormat pfn_SetPixelFormat

typedef BOOL tfn_SwapBuffers(HDC unnamedParam1);
tfn_SwapBuffers *pfn_SwapBuffers;
#define SwapBuffers pfn_SwapBuffers

static b32 Win32_LoadGDILib()
{
    HMODULE lib = LoadLibraryA("gdi32.dll");
    if(!lib) return 0;
    
    WIN32_LOAD_DLLFN(ChoosePixelFormat);
    WIN32_LOAD_DLLFN(DescribePixelFormat);
    WIN32_LOAD_DLLFN(SetPixelFormat);
    WIN32_LOAD_DLLFN(SwapBuffers);
    
    return 1;
}

/*=========== WGL ==============*/

typedef HGLRC tfn_wglCreateContext(HDC unnamedParam1);
tfn_wglCreateContext *pfn_wglCreateContext;
#define wglCreateContext pfn_wglCreateContext

typedef BOOL tfn_wglDeleteContext(HGLRC unnamedParam1);
tfn_wglDeleteContext *pfn_wglDeleteContext;
#define wglDeleteContext pfn_wglDeleteContext

typedef PROC tfn_wglGetProcAddress(LPCSTR unnamedParam1);
tfn_wglGetProcAddress *pfn_wglGetProcAddress;
#define wglGetProcAddress pfn_wglGetProcAddress

typedef BOOL tfn_wglMakeCurrent(HDC unnamedParam1, HGLRC unnamedParam2);
tfn_wglMakeCurrent *pfn_wglMakeCurrent;
#define wglMakeCurrent pfn_wglMakeCurrent

static b32 Win32_LoadWGLLib(void)
{
    HMODULE lib = LoadLibraryA("opengl32.dll");
    if(!lib) return 0;
    
    WIN32_LOAD_DLLFN(wglCreateContext);
    WIN32_LOAD_DLLFN(wglDeleteContext);
    WIN32_LOAD_DLLFN(wglGetProcAddress);
    WIN32_LOAD_DLLFN(wglMakeCurrent);
    
    return 1;
}

/*========= XINPUT =============*/

typedef DWORD WINAPI tfn_XInputGetState(DWORD dwUserIndex, XINPUT_STATE *pState);
typedef DWORD WINAPI tfn_XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
DWORD WINAPI sfn_XInputGetState(DWORD dwUserIndex, XINPUT_STATE *pState)
{return ERROR_DEVICE_NOT_CONNECTED;}
DWORD WINAPI sfn_XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
{return ERROR_DEVICE_NOT_CONNECTED;}

typedef enum
{
    WIN32_XINPUT_NOT_LOADED = 0,
    WIN32_XINPUT_1_4,
    WIN32_XINPUT_9_1_0,
    WIN32_XINPUT_1_3
} WIN32_XINPUT_LOADED_VERSION;

typedef struct
{
    WIN32_XINPUT_LOADED_VERSION version;
    tfn_XInputGetState *GetState;
    tfn_XInputSetState *SetState;
} Win32_XInput;

/*============== XAUDIO2 ================*/

typedef HRESULT tfn_XAudio2Create(IXAudio2 **ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);
HRESULT sfn_XAudio2Create(IXAudio2 **ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor)
{return XAUDIO2_E_XAPO_CREATION_FAILED;}

typedef enum
{
    WIN32_XAUDIO_NOT_LOADED = 0,
    WIN32_XAUDIO_2_9,
    WIN32_XAUDIO_2_9D,
    WIN32_XAUDIO_2_8,
    WIN32_XAUDIO_2_7
} WIN32_XAUDIO_LOADED_VERSION;

typedef struct
{
    WIN32_XAUDIO_LOADED_VERSION version;
    
    IXAudio2 *handle;
    IXAudio2MasteringVoice *masterVoice;
    IXAudio2SourceVoice *srcVoice;
    WAVEFORMATEX fmt;
    XAUDIO2_BUFFER bufferHeader;
    
    tfn_XAudio2Create *Create;
} Win32_XAudio;


/*====================
// Windows GL
====================*/
// WGL DEFS
#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_SWAP_EXCHANGE_ARB             0x2028
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_SWAP_COPY_ARB                 0x2029

typedef BOOL (WINAPI* pfn_wglChoosePixelFormatARB)(HDC hdc, const int *piAttribIList,
                                                   const FLOAT *pfAttribFList,
                                                   UINT nMaxFormats, int *piFormats,
                                                   UINT *nNunFormats);
typedef HGLRC (WINAPI* pfn_wglCreateContextAttribsARB)(HDC hDC, HGLRC hShareContext,
                                                       const int *attribList);
typedef BOOL (WINAPI* pfn_wglSwapIntervalEXT)(int interval);

pfn_wglChoosePixelFormatARB wglChoosePixelFormatARB;
pfn_wglCreateContextAttribsARB wglCreateContextAttribsARB;
pfn_wglSwapIntervalEXT wglSwapIntervalEXT;

//==============================
// Memory Allocation Functions
//==============================

ptr PL_Alloc(u64 size)
{
    ptr result = 0;
    result = (ptr)HeapAlloc(GetProcessHeap(), 0, size);
    return result;
}

ptr PL_Alloc0(u64 size)
{
    ptr result = 0;
    result = (ptr)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    return result;
}

ptr PL_ReAlloc(ptr oldMem, u64 newSize)
{
    ptr result = 0;
    result = (ptr)HeapReAlloc(GetProcessHeap(), 0, oldMem, newSize);
    return result;
}

ptr PL_ReAlloc0(ptr oldMem, u64 newSize)
{
    ptr result = 0;
    result = (ptr)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, oldMem, newSize);
    return result;
}

b32 PL_Free(ptr mem)
{
    BOOL win32Result = HeapFree(GetProcessHeap(), 0, mem);
    b32 result = (b32)((win32Result) ? 1 : 0);
    return result;
}

//===================
// Files
//===================

PL_File PL_FileOpen(const cstr path)
{
    PL_File result = {0};
    FILE *file = 0;
    
    if(fopen_s(&file, path, "a+"))
    {
        return result;
    }
    
    fseek(file, 0, SEEK_END);
    i64 size = _ftelli64(file);
    fseek(file, 0, SEEK_SET);
    
    result.handle = (ptr)file;
    result.size = size;
    
    return result;
}

void PL_FileClose(PL_File *file)
{
    if(file && file->handle)
    {
        fclose((FILE*)file->handle);
        file->handle = 0;
        file->size = 0;
    }
}

u64 PL_FileRead(PL_File *file, u64 offset, ptr dst, u64 size)
{
    u64 result = 0;
    
    if(file && file->handle && dst)
    {
        _fseeki64((FILE*)file->handle, (i64)offset, SEEK_SET);
        
        if(!size)
        {
            result = fread(dst, 1, (u64)file->size - offset, (FILE*)file->handle);
        }
        else
        {
            result = fread(dst, 1, size - offset, (FILE*)file->handle);
        }
        
        fseek(file->handle, 0, SEEK_SET);
    }
    else
    {
        return 0;
    }
    
    return result;
}

u64 PL_FileWrite(PL_File *file, u64 offset, ptr src, u64 size)
{
    u64 result = 0;
    
    if(file && file->handle && src && size)
    {
        _fseeki64((FILE*)file->handle, (i64)offset, SEEK_SET);
        result = fwrite(src, 1, size, (FILE*)file->handle);
        fseek((FILE*)file->handle, 0, SEEK_SET);
    }
    else
    {
        return 0;
    }
    
    return result;
}

u64 PL_FileWriteAppend(PL_File *file, ptr src, u64 size)
{
    u64 result = 0;
    
    if(file && file->handle && src && size)
    {
        fseek((FILE*)file->handle, 0, SEEK_END);
        result = fwrite(src, 1, size, (FILE*)file->handle);
        fseek((FILE*)file->handle, 0, SEEK_SET);
    }
    else
    {
        return 0;
    }
    
    return result;
}

void PL_PrintFile(PL_File *file, u64 offset, cstr format, ...)
{
    if(file && file->handle)
    {
        _fseeki64((FILE*)file->handle, (i64)offset, SEEK_SET);
        va_list args;
        va_start(args, format);
        vfprintf((FILE*)file->handle, format, args);
        va_end(args);
        fseek((FILE*)file->handle, 0, SEEK_SET);
    }
}

void PL_PrintFileAppend(PL_File *file, cstr format, ...)
{
    if(file && file->handle)
    {
        fseek((FILE*)file->handle, 0, SEEK_END);
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fseek((FILE*)file->handle, 0, SEEK_SET);
    }
}

void PL_MsgBox(const cstr title, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    char msg[1024] = {0};
    vsnprintf(msg, 1024, format, args);
    va_end(args);
    MessageBoxA(0, msg, title, MB_OK);
}

void  PL_MsgBoxInfo(const cstr title, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    char msg[1024] = {0};
    vsnprintf(msg, 1024, format, args);
    va_end(args);
    MessageBoxA(0, msg, title, MB_OK | MB_ICONINFORMATION);
}

void PL_MsgBoxError(const cstr title, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    char msg[1024] = {0};
    vsnprintf(msg, 1024, format, args);
    va_end(args);
    MessageBoxA(0, msg, title, MB_OK | MB_ICONSTOP);
}

/*====================
  Input Processing
====================*/

static PL_KEYCODE Win32_WinKeyToPLKeycode(u32 key, LPARAM lParam)
{
    switch(key)
    {
        case VK_ESCAPE: return K_ESC;
        case VK_PRINT: return K_PRTSC;
        case VK_SCROLL: return K_SCRLK;
        case VK_PAUSE: return K_PAUSE;
        case VK_NUMLOCK: return K_NUM;
        case VK_INSERT: return K_INS;
        case VK_DELETE: return K_DEL;
        case VK_HOME: return K_HOME;
        case VK_END: return K_END;
        case VK_PRIOR: return K_PGUP;
        case VK_NEXT: return K_PGDN;
        
        case VK_F1: return K_F1;
        case VK_F2: return K_F2;
        case VK_F3: return K_F3;
        case VK_F4: return K_F4;
        case VK_F5: return K_F5;
        case VK_F6: return K_F6;
        case VK_F7: return K_F7;
        case VK_F8: return K_F8;
        case VK_F9: return K_F9;
        case VK_F10: return K_F10;
        case VK_F11: return K_F11;
        case VK_F12: return K_F12;
        
        case VK_UP: return K_UP;
        case VK_DOWN: return K_DOWN;
        case VK_LEFT: return K_LEFT;
        case VK_RIGHT: return K_RIGHT;
        
        case VK_CAPITAL: return K_CAPS;
        case VK_BACK: return K_BACKSPACE;
        case VK_LSHIFT: return K_LSHIFT;
        case VK_RSHIFT: return K_RSHIFT;
        case VK_LCONTROL: return K_LCTRL;
        case VK_RCONTROL: return K_RCTRL;
        case VK_LWIN: return K_LWIN;
        case VK_RWIN: return K_RWIN;
        case VK_APPS: return K_MENU;
        
        case VK_TAB: return K_TAB;
        case VK_OEM_3: return K_TILDE;
        case VK_OEM_MINUS: return K_MINUS;
        case VK_OEM_PLUS: return K_EQUALS;
        
        case VK_RETURN:
        {
            if(!(lParam & (1<<24))) return K_RETURN;
            else return KP_RETURN;
        }
        
        case VK_SPACE: return K_SPACE;
        case VK_OEM_4: return K_LBRACKET;
        case VK_OEM_6: return K_RBRACKET;
        case VK_OEM_1: return K_SEMICOLON;
        case VK_OEM_7: return K_APOSTROPHE;
        case VK_OEM_5: return K_BACKSLASH;
        case VK_OEM_COMMA: return K_COMMA;
        case VK_OEM_PERIOD: return K_PERIOD;
        case VK_OEM_2: return K_SLASH;
        
        case 0x41: return K_A;
        case 0x42: return K_B;
        case 0x43: return K_C;
        case 0x44: return K_D;
        case 0x45: return K_E;
        case 0x46: return K_F;
        case 0x47: return K_G;
        case 0x48: return K_H;
        case 0x49: return K_I;
        case 0x4a: return K_J;
        case 0x4b: return K_K;
        case 0x4c: return K_L;
        case 0x4d: return K_M;
        case 0x4e: return K_N;
        case 0x4f: return K_O;
        case 0x50: return K_P;
        case 0x51: return K_Q;
        case 0x52: return K_R;
        case 0x53: return K_S;
        case 0x54: return K_T;
        case 0x55: return K_U;
        case 0x56: return K_V;
        case 0x57: return K_W;
        case 0x58: return K_X;
        case 0x59: return K_Y;
        case 0x5a: return K_Z;
        
        case 0x31: return K_1;
        case 0x32: return K_2;
        case 0x33: return K_3;
        case 0x34: return K_4;
        case 0x35: return K_5;
        case 0x36: return K_6;
        case 0x37: return K_7;
        case 0x38: return K_8;
        case 0x39: return K_9;
        case 0x30: return K_0;
        
        case VK_DIVIDE: return KP_SLASH;
        case VK_MULTIPLY: return KP_MULTIPLY;
        case VK_SUBTRACT: return KP_MINUS;
        case VK_ADD: return KP_PLUS;
        case VK_DECIMAL: return KP_PERIOD;
        
        case VK_NUMPAD0: return KP_0;
        case VK_NUMPAD1: return KP_1;
        case VK_NUMPAD2: return KP_2;
        case VK_NUMPAD3: return KP_3;
        case VK_NUMPAD4: return KP_4;
        case VK_NUMPAD5: return KP_5;
        case VK_NUMPAD6: return KP_6;
        case VK_NUMPAD7: return KP_7;
        case VK_NUMPAD8: return KP_8;
        case VK_NUMPAD9: return KP_9;
        
        case VK_VOLUME_MUTE: return KM_MUTE;
        case VK_VOLUME_DOWN: return KM_VOLDOWN;
        case VK_VOLUME_UP: return KM_VOLUP;
        case VK_MEDIA_PLAY_PAUSE: return KM_PLAYPAUSE;
        case VK_MEDIA_STOP: return KM_STOP;
        case VK_MEDIA_PREV_TRACK: return KM_PREV;
        case VK_MEDIA_NEXT_TRACK: return KM_NEXT;
        
        default: return K_NONE;
    }
}

/*======= State Components =========*/

typedef struct
{
    PL_Window window;
    
    b32 init;
    WNDCLASSA wndclass;
    DWORD style;
    WINDOWPLACEMENT wp;
    HWND handle;
    HDC dc;
    HGLRC rc;
} Win32_Window;

typedef struct
{
    PL_Timer timer;
    u64 perfFreq;
    u64 lastFramePerf;
} Win32_Timer;

typedef enum
{
    CPU_UNKNOWN = 0,
    CPU_AMD64, // amd or intel x64
    CPU_ARM,
    CPU_ARM64,
    CPU_IA64, // intel itanium 64
    CPU_I86 // intel x86
} CPU_TYPE;

typedef struct
{
    CPU_TYPE cpu;
    u32 cores;
    ptr minAppAddress;
    ptr maxAppAddress;
} Win32_SystemInfo;

/*========== WIN32 STATE =============*/

typedef struct
{
    char errorString[1024];
    b32 running;
    u64 userMemorySize;
    ptr userMemory;
    
    Win32_SystemInfo system;
    Win32_Timer timer;
    Win32_Window window;
    Win32_XInput xinput;
    Win32_XAudio xaudio;
    
    PL_Clock clock;
    PL_Input input;
    PL_Audio audio;
} Win32_State;
static Win32_State *win32_state;

void PL_SetErrorString(const cstr format, ...)
{
    PL_MemZero(win32_state->errorString, 1024);
    va_list args;
    va_start(args, format);
    vsnprintf(win32_state->errorString, 1024, format, args);
    va_end(args);
    
    PL_ErrorCallback();
}

void PL_Quit(void)
{
    win32_state->running = 0;
}

cstr PL_GetErrorString(void)
{
    return win32_state->errorString;
}

PL_Clock *PL_GetClock(void)
{
    return &win32_state->clock;
}

PL_Timer *PL_GetTimer(void)
{
    return &win32_state->timer.timer;
}

PL_Input* PL_GetInput(void)
{
    return &win32_state->input;
}

PL_Mouse* PL_GetMouse(void)
{
    return &win32_state->input.mouse;
}

PL_Gamepad* PL_GetGamepad(i32 id)
{
    if(id >= 0 && id <= GP_MAX_COUNT)
        return &win32_state->input.gamepads[id];
    
    return 0;
}

PL_ButtonState* PL_GetKeyState(PL_KEYCODE key)
{
    if(key > 0 && key < K_MAX)
        return &win32_state->input.keyboard[key];
    
    return 0;
}

/*========= OPENGL LOADERS ==========*/

#define WIN32_LOAD_WGLFN(name) {name = (pfn_##name)wglGetProcAddress( #name ); if(!name) return 0;}
static b32 Win32_LoadWGL(void)
{
    WIN32_LOAD_WGLFN(wglChoosePixelFormatARB);
    WIN32_LOAD_WGLFN(wglCreateContextAttribsARB);
    WIN32_LOAD_WGLFN(wglSwapIntervalEXT);
    
    return 1;
}

#define WIN32_LOAD_GLFN(name) name = (pfn_##name)wglGetProcAddress( #name );\
if(!name) {name = (pfn_##name)GetProcAddress(lib, #name );} if(!name) {return 0;}
static b32 Win32_LoadOpenGL(r32 version)
{
    HMODULE lib = LoadLibraryA("opengl32.dll");
    
    if(version >= 1.0)
    {
        WIN32_LOAD_GLFN(glCullFace);
        WIN32_LOAD_GLFN(glFrontFace);
        WIN32_LOAD_GLFN(glHint);
        WIN32_LOAD_GLFN(glLineWidth);
        WIN32_LOAD_GLFN(glPointSize);
        WIN32_LOAD_GLFN(glPolygonMode);
        WIN32_LOAD_GLFN(glScissor);
        WIN32_LOAD_GLFN(glTexParameterf);
        WIN32_LOAD_GLFN(glTexParameterfv);
        WIN32_LOAD_GLFN(glTexParameteri);
        WIN32_LOAD_GLFN(glTexParameteriv);
        WIN32_LOAD_GLFN(glTexImage1D);
        WIN32_LOAD_GLFN(glTexImage2D);
        WIN32_LOAD_GLFN(glDrawBuffer);
        WIN32_LOAD_GLFN(glClear);
        WIN32_LOAD_GLFN(glClearColor);
        WIN32_LOAD_GLFN(glClearStencil);
        WIN32_LOAD_GLFN(glClearDepth);
        WIN32_LOAD_GLFN(glStencilMask);
        WIN32_LOAD_GLFN(glColorMask);
        WIN32_LOAD_GLFN(glDepthMask);
        WIN32_LOAD_GLFN(glDisable);
        WIN32_LOAD_GLFN(glEnable);
        WIN32_LOAD_GLFN(glFinish);
        WIN32_LOAD_GLFN(glFlush);
        WIN32_LOAD_GLFN(glBlendFunc);
        WIN32_LOAD_GLFN(glLogicOp);
        WIN32_LOAD_GLFN(glStencilFunc);
        WIN32_LOAD_GLFN(glStencilOp);
        WIN32_LOAD_GLFN(glDepthFunc);
        WIN32_LOAD_GLFN(glPixelStoref);
        WIN32_LOAD_GLFN(glPixelStorei);
        WIN32_LOAD_GLFN(glReadBuffer);
        WIN32_LOAD_GLFN(glReadPixels);
        WIN32_LOAD_GLFN(glGetBooleanv);
        WIN32_LOAD_GLFN(glGetDoublev);
        WIN32_LOAD_GLFN(glGetError);
        WIN32_LOAD_GLFN(glGetFloatv);
        WIN32_LOAD_GLFN(glGetIntegerv);
        WIN32_LOAD_GLFN(glGetString);
        WIN32_LOAD_GLFN(glGetTexImage);
        WIN32_LOAD_GLFN(glGetTexParameterfv);
        WIN32_LOAD_GLFN(glGetTexParameteriv);
        WIN32_LOAD_GLFN(glGetTexLevelParameterfv);
        WIN32_LOAD_GLFN(glGetTexLevelParameteriv);
        WIN32_LOAD_GLFN(glIsEnabled);
        WIN32_LOAD_GLFN(glDepthRange);
        WIN32_LOAD_GLFN(glViewport);
    }
    
    if(version >= 1.1)
    {
        WIN32_LOAD_GLFN(glDrawArrays);
        WIN32_LOAD_GLFN(glDrawElements);
        WIN32_LOAD_GLFN(glPolygonOffset);
        WIN32_LOAD_GLFN(glCopyTexImage1D);
        WIN32_LOAD_GLFN(glCopyTexImage2D);
        WIN32_LOAD_GLFN(glCopyTexSubImage1D);
        WIN32_LOAD_GLFN(glCopyTexSubImage2D);
        WIN32_LOAD_GLFN(glTexSubImage1D);
        WIN32_LOAD_GLFN(glTexSubImage2D);
        WIN32_LOAD_GLFN(glBindTexture);
        WIN32_LOAD_GLFN(glDeleteTextures);
        WIN32_LOAD_GLFN(glGenTextures);
        WIN32_LOAD_GLFN(glIsTexture);
    }
    
    if(version >= 1.2)
    {
        WIN32_LOAD_GLFN(glDrawRangeElements);
        WIN32_LOAD_GLFN(glTexImage3D);
        WIN32_LOAD_GLFN(glTexSubImage3D);
        WIN32_LOAD_GLFN(glCopyTexSubImage3D);
    }
    
    if(version >= 1.3)
    {
        WIN32_LOAD_GLFN(glActiveTexture);
        WIN32_LOAD_GLFN(glSampleCoverage);
        WIN32_LOAD_GLFN(glCompressedTexImage3D);
        WIN32_LOAD_GLFN(glCompressedTexImage2D);
        WIN32_LOAD_GLFN(glCompressedTexImage1D);
        WIN32_LOAD_GLFN(glCompressedTexSubImage3D);
        WIN32_LOAD_GLFN(glCompressedTexSubImage2D);
        WIN32_LOAD_GLFN(glCompressedTexSubImage1D);
        WIN32_LOAD_GLFN(glGetCompressedTexImage);
    }
    
    if(version >= 1.4)
    {
        WIN32_LOAD_GLFN(glBlendFuncSeparate);
        WIN32_LOAD_GLFN(glMultiDrawArrays);
        WIN32_LOAD_GLFN(glMultiDrawElements);
        WIN32_LOAD_GLFN(glPointParameterf);
        WIN32_LOAD_GLFN(glPointParameterfv);
        WIN32_LOAD_GLFN(glPointParameteri);
        WIN32_LOAD_GLFN(glPointParameteriv);
        WIN32_LOAD_GLFN(glBlendColor);
        WIN32_LOAD_GLFN(glBlendEquation);
    }
    
    if(version >= 1.5)
    {
        WIN32_LOAD_GLFN(glGenQueries);
        WIN32_LOAD_GLFN(glDeleteQueries);
        WIN32_LOAD_GLFN(glIsQuery);
        WIN32_LOAD_GLFN(glBeginQuery);
        WIN32_LOAD_GLFN(glEndQuery);
        WIN32_LOAD_GLFN(glGetQueryiv);
        WIN32_LOAD_GLFN(glGetQueryObjectiv);
        WIN32_LOAD_GLFN(glGetQueryObjectuiv);
        WIN32_LOAD_GLFN(glBindBuffer);
        WIN32_LOAD_GLFN(glDeleteBuffers);
        WIN32_LOAD_GLFN(glGenBuffers);
        WIN32_LOAD_GLFN(glIsBuffer);
        WIN32_LOAD_GLFN(glBufferData);
        WIN32_LOAD_GLFN(glBufferSubData);
        WIN32_LOAD_GLFN(glGetBufferSubData);
        WIN32_LOAD_GLFN(glMapBuffer);
        WIN32_LOAD_GLFN(glUnmapBuffer);
        WIN32_LOAD_GLFN(glGetBufferParameteriv);
        WIN32_LOAD_GLFN(glGetBufferPointerv);
    }
    
    if(version >= 2.0)
    {
        WIN32_LOAD_GLFN(glBlendEquationSeparate);
        WIN32_LOAD_GLFN(glDrawBuffers);
        WIN32_LOAD_GLFN(glStencilOpSeparate);
        WIN32_LOAD_GLFN(glStencilFuncSeparate);
        WIN32_LOAD_GLFN(glStencilMaskSeparate);
        WIN32_LOAD_GLFN(glAttachShader);
        WIN32_LOAD_GLFN(glBindAttribLocation);
        WIN32_LOAD_GLFN(glCompileShader);
        WIN32_LOAD_GLFN(glCreateProgram);
        WIN32_LOAD_GLFN(glCreateShader);
        WIN32_LOAD_GLFN(glDeleteProgram);
        WIN32_LOAD_GLFN(glDeleteShader);
        WIN32_LOAD_GLFN(glDetachShader);
        WIN32_LOAD_GLFN(glDisableVertexAttribArray);
        WIN32_LOAD_GLFN(glEnableVertexAttribArray);
        WIN32_LOAD_GLFN(glGetActiveAttrib);
        WIN32_LOAD_GLFN(glGetActiveUniform);
        WIN32_LOAD_GLFN(glGetAttachedShaders);
        WIN32_LOAD_GLFN(glGetAttribLocation);
        WIN32_LOAD_GLFN(glGetProgramiv);
        WIN32_LOAD_GLFN(glGetProgramInfoLog);
        WIN32_LOAD_GLFN(glGetShaderiv);
        WIN32_LOAD_GLFN(glGetShaderInfoLog);
        WIN32_LOAD_GLFN(glGetShaderSource);
        WIN32_LOAD_GLFN(glGetUniformLocation);
        WIN32_LOAD_GLFN(glGetUniformfv);
        WIN32_LOAD_GLFN(glGetUniformiv);
        WIN32_LOAD_GLFN(glGetVertexAttribdv);
        WIN32_LOAD_GLFN(glGetVertexAttribfv);
        WIN32_LOAD_GLFN(glGetVertexAttribiv);
        WIN32_LOAD_GLFN(glGetVertexAttribPointerv);
        WIN32_LOAD_GLFN(glIsProgram);
        WIN32_LOAD_GLFN(glIsShader);
        WIN32_LOAD_GLFN(glLinkProgram);
        WIN32_LOAD_GLFN(glShaderSource);
        WIN32_LOAD_GLFN(glUseProgram);
        WIN32_LOAD_GLFN(glUniform1f);
        WIN32_LOAD_GLFN(glUniform2f);
        WIN32_LOAD_GLFN(glUniform3f);
        WIN32_LOAD_GLFN(glUniform4f);
        WIN32_LOAD_GLFN(glUniform1i);
        WIN32_LOAD_GLFN(glUniform2i);
        WIN32_LOAD_GLFN(glUniform3i);
        WIN32_LOAD_GLFN(glUniform4i);
        WIN32_LOAD_GLFN(glUniform1fv);
        WIN32_LOAD_GLFN(glUniform2fv);
        WIN32_LOAD_GLFN(glUniform3fv);
        WIN32_LOAD_GLFN(glUniform4fv);
        WIN32_LOAD_GLFN(glUniform1iv);
        WIN32_LOAD_GLFN(glUniform2iv);
        WIN32_LOAD_GLFN(glUniform3iv);
        WIN32_LOAD_GLFN(glUniform4iv);
        WIN32_LOAD_GLFN(glUniformMatrix2fv);
        WIN32_LOAD_GLFN(glUniformMatrix3fv);
        WIN32_LOAD_GLFN(glUniformMatrix4fv);
        WIN32_LOAD_GLFN(glValidateProgram);
        WIN32_LOAD_GLFN(glVertexAttrib1d);
        WIN32_LOAD_GLFN(glVertexAttrib1dv);
        WIN32_LOAD_GLFN(glVertexAttrib1f);
        WIN32_LOAD_GLFN(glVertexAttrib1s);
        WIN32_LOAD_GLFN(glVertexAttrib1sv);
        WIN32_LOAD_GLFN(glVertexAttrib2d);
        WIN32_LOAD_GLFN(glVertexAttrib2dv);
        WIN32_LOAD_GLFN(glVertexAttrib2f);
        WIN32_LOAD_GLFN(glVertexAttrib2fv);
        WIN32_LOAD_GLFN(glVertexAttrib2s);
        WIN32_LOAD_GLFN(glVertexAttrib2sv);
        WIN32_LOAD_GLFN(glVertexAttrib3d);
        WIN32_LOAD_GLFN(glVertexAttrib3dv);
        WIN32_LOAD_GLFN(glVertexAttrib3f);
        WIN32_LOAD_GLFN(glVertexAttrib3fv);
        WIN32_LOAD_GLFN(glVertexAttrib3s);
        WIN32_LOAD_GLFN(glVertexAttrib3sv);
        WIN32_LOAD_GLFN(glVertexAttrib4Nbv);
        WIN32_LOAD_GLFN(glVertexAttrib4Niv);
        WIN32_LOAD_GLFN(glVertexAttrib4Nsv);
        WIN32_LOAD_GLFN(glVertexAttrib4Nub);
        WIN32_LOAD_GLFN(glVertexAttrib4Nubv);
        WIN32_LOAD_GLFN(glVertexAttrib4Nuiv);
        WIN32_LOAD_GLFN(glVertexAttrib4Nusv);
        WIN32_LOAD_GLFN(glVertexAttrib4bv);
        WIN32_LOAD_GLFN(glVertexAttrib4d);
        WIN32_LOAD_GLFN(glVertexAttrib4dv);
        WIN32_LOAD_GLFN(glVertexAttrib4f);
        WIN32_LOAD_GLFN(glVertexAttrib4fv);
        WIN32_LOAD_GLFN(glVertexAttrib4iv);
        WIN32_LOAD_GLFN(glVertexAttrib4s);
        WIN32_LOAD_GLFN(glVertexAttrib4sv);
        WIN32_LOAD_GLFN(glVertexAttrib4ubv);
        WIN32_LOAD_GLFN(glVertexAttrib4uiv);
        WIN32_LOAD_GLFN(glVertexAttrib4usv);
        WIN32_LOAD_GLFN(glVertexAttribPointer);
    }
    
    if(version >= 2.1)
    {
        WIN32_LOAD_GLFN(glUniformMatrix2x3fv);
        WIN32_LOAD_GLFN(glUniformMatrix3x2fv);
        WIN32_LOAD_GLFN(glUniformMatrix2x4fv);
        WIN32_LOAD_GLFN(glUniformMatrix4x2fv);
        WIN32_LOAD_GLFN(glUniformMatrix3x4fv);
        WIN32_LOAD_GLFN(glUniformMatrix4x3fv);
    }
    
    if(version >= 3.0)
    {
        WIN32_LOAD_GLFN(glColorMaski);
        WIN32_LOAD_GLFN(glGetBooleani_v);
        WIN32_LOAD_GLFN(glGetIntegeri_v);
        WIN32_LOAD_GLFN(glEnablei);
        WIN32_LOAD_GLFN(glDisablei);
        WIN32_LOAD_GLFN(glIsEnabledi);
        WIN32_LOAD_GLFN(glBeginTransformFeedback);
        WIN32_LOAD_GLFN(glEndTransformFeedback);
        WIN32_LOAD_GLFN(glBindBufferRange);
        WIN32_LOAD_GLFN(glBindBufferBase);
        WIN32_LOAD_GLFN(glTransformFeedbackVaryings);
        WIN32_LOAD_GLFN(glGetTransformFeedbackVarying);
        WIN32_LOAD_GLFN(glClampColor);
        WIN32_LOAD_GLFN(glBeginConditionalRender);
        WIN32_LOAD_GLFN(glEndConditionalRender);
        WIN32_LOAD_GLFN(glVertexAttribIPointer);
        WIN32_LOAD_GLFN(glGetVertexAttribIiv);
        WIN32_LOAD_GLFN(glGetVertexAttribIuiv);
        WIN32_LOAD_GLFN(glVertexAttribI1i);
        WIN32_LOAD_GLFN(glVertexAttribI2i);
        WIN32_LOAD_GLFN(glVertexAttribI3i);
        WIN32_LOAD_GLFN(glVertexAttribI4i);
        WIN32_LOAD_GLFN(glVertexAttribI1ui);
        WIN32_LOAD_GLFN(glVertexAttribI2ui);
        WIN32_LOAD_GLFN(glVertexAttribI3ui);
        WIN32_LOAD_GLFN(glVertexAttribI4ui);
        WIN32_LOAD_GLFN(glVertexAttribI1iv);
        WIN32_LOAD_GLFN(glVertexAttribI2iv);
        WIN32_LOAD_GLFN(glVertexAttribI3iv);
        WIN32_LOAD_GLFN(glVertexAttribI4iv);
        WIN32_LOAD_GLFN(glVertexAttribI1uiv);
        WIN32_LOAD_GLFN(glVertexAttribI2uiv);
        WIN32_LOAD_GLFN(glVertexAttribI3uiv);
        WIN32_LOAD_GLFN(glVertexAttribI4uiv);
        WIN32_LOAD_GLFN(glVertexAttribI4bv);
        WIN32_LOAD_GLFN(glVertexAttribI4sv);
        WIN32_LOAD_GLFN(glVertexAttribI4ubv);
        WIN32_LOAD_GLFN(glVertexAttribI4usv);
        WIN32_LOAD_GLFN(glGetUniformuiv);
        WIN32_LOAD_GLFN(glBindFragDataLocation);
        WIN32_LOAD_GLFN(glGetFragDataLocation);
        WIN32_LOAD_GLFN(glUniform1ui);
        WIN32_LOAD_GLFN(glUniform2ui);
        WIN32_LOAD_GLFN(glUniform3ui);
        WIN32_LOAD_GLFN(glUniform4ui);
        WIN32_LOAD_GLFN(glUniform1uiv);
        WIN32_LOAD_GLFN(glUniform2uiv);
        WIN32_LOAD_GLFN(glUniform3uiv);
        WIN32_LOAD_GLFN(glUniform4uiv);
        WIN32_LOAD_GLFN(glTexParameterIiv);
        WIN32_LOAD_GLFN(glTexParameterIuiv);
        WIN32_LOAD_GLFN(glGetTexParameterIiv);
        WIN32_LOAD_GLFN(glGetTexParameterIuiv);
        WIN32_LOAD_GLFN(glClearBufferiv);
        WIN32_LOAD_GLFN(glClearBufferuiv);
        WIN32_LOAD_GLFN(glClearBufferfv);
        WIN32_LOAD_GLFN(glClearBufferfi);
        WIN32_LOAD_GLFN(glGetStringi);
        WIN32_LOAD_GLFN(glIsRenderbuffer);
        WIN32_LOAD_GLFN(glBindRenderbuffer);
        WIN32_LOAD_GLFN(glDeleteRenderbuffers);
        WIN32_LOAD_GLFN(glGenRenderbuffers);
        WIN32_LOAD_GLFN(glRenderbufferStorage);
        WIN32_LOAD_GLFN(glGetRenderbufferParameteriv);
        WIN32_LOAD_GLFN(glIsFramebuffer);
        WIN32_LOAD_GLFN(glBindFramebuffer);
        WIN32_LOAD_GLFN(glDeleteFramebuffers);
        WIN32_LOAD_GLFN(glGenFramebuffers);
        WIN32_LOAD_GLFN(glCheckFramebufferStatus);
        WIN32_LOAD_GLFN(glFramebufferTexture1D);
        WIN32_LOAD_GLFN(glFramebufferTexture2D);
        WIN32_LOAD_GLFN(glFramebufferTexture3D);
        WIN32_LOAD_GLFN(glFramebufferRenderbuffer);
        WIN32_LOAD_GLFN(glGetFramebufferAttachmentParameteriv);
        WIN32_LOAD_GLFN(glGenerateMipmap);
        WIN32_LOAD_GLFN(glBlitFramebuffer);
        WIN32_LOAD_GLFN(glRenderbufferStorageMultisample);
        WIN32_LOAD_GLFN(glFramebufferTextureLayer);
        WIN32_LOAD_GLFN(glMapBufferRange);
        WIN32_LOAD_GLFN(glFlushMappedBufferRange);
        WIN32_LOAD_GLFN(glBindVertexArray);
        WIN32_LOAD_GLFN(glDeleteVertexArrays);
        WIN32_LOAD_GLFN(glGenVertexArrays);
        WIN32_LOAD_GLFN(glIsVertexArray);
    }
    
    if(version >= 3.1)
    {
        WIN32_LOAD_GLFN(glDrawArraysInstanced);
        WIN32_LOAD_GLFN(glDrawElementsInstanced);
        WIN32_LOAD_GLFN(glTexBuffer);
        WIN32_LOAD_GLFN(glPrimitiveRestartIndex);
        WIN32_LOAD_GLFN(glCopyBufferSubData);
        WIN32_LOAD_GLFN(glGetUniformIndices);
        WIN32_LOAD_GLFN(glGetActiveUniformsiv);
        WIN32_LOAD_GLFN(glGetActiveUniformName);
        WIN32_LOAD_GLFN(glGetUniformBlockIndex);
        WIN32_LOAD_GLFN(glGetActiveUniformBlockiv);
        WIN32_LOAD_GLFN(glGetActiveUniformBlockName);
        WIN32_LOAD_GLFN(glUniformBlockBinding);
    }
    
    if(version >= 3.2)
    {
        WIN32_LOAD_GLFN(glDrawElementsBaseVertex);
        WIN32_LOAD_GLFN(glDrawRangeElementsBaseVertex);
        WIN32_LOAD_GLFN(glDrawElementsInstancedBaseVertex);
        WIN32_LOAD_GLFN(glMultiDrawElementsBaseVertex);
        WIN32_LOAD_GLFN(glProvokingVertex);
        WIN32_LOAD_GLFN(glFenceSync);
        WIN32_LOAD_GLFN(glIsSync);
        WIN32_LOAD_GLFN(glDeleteSync);
        WIN32_LOAD_GLFN(glClientWaitSync);
        WIN32_LOAD_GLFN(glWaitSync);
        WIN32_LOAD_GLFN(glGetInteger64v);
        WIN32_LOAD_GLFN(glGetSynciv);
        WIN32_LOAD_GLFN(glGetInteger64i_v);
        WIN32_LOAD_GLFN(glGetBufferParameteri64v);
        WIN32_LOAD_GLFN(glFramebufferTexture);
        WIN32_LOAD_GLFN(glTexImage2DMultisample);
        WIN32_LOAD_GLFN(glTexImage3DMultisample);
        WIN32_LOAD_GLFN(glGetMultisamplefv);
        WIN32_LOAD_GLFN(glSampleMaski);
    }
    
    if(version >= 3.3)
    {
        WIN32_LOAD_GLFN(glBindFragDataLocationIndexed);
        WIN32_LOAD_GLFN(glGetFragDataIndex);
        WIN32_LOAD_GLFN(glGenSamplers);
        WIN32_LOAD_GLFN(glDeleteSamplers);
        WIN32_LOAD_GLFN(glIsSampler);
        WIN32_LOAD_GLFN(glBindSampler);
        WIN32_LOAD_GLFN(glSamplerParameteri);
        WIN32_LOAD_GLFN(glSamplerParameteriv);
        WIN32_LOAD_GLFN(glSamplerParameterf);
        WIN32_LOAD_GLFN(glSamplerParameterfv);
        WIN32_LOAD_GLFN(glSamplerParameterIiv);
        WIN32_LOAD_GLFN(glSamplerParameterIuiv);
        WIN32_LOAD_GLFN(glGetSamplerParameteriv);
        WIN32_LOAD_GLFN(glGetSamplerParameterIiv);
        WIN32_LOAD_GLFN(glGetSamplerParameterfv);
        WIN32_LOAD_GLFN(glGetSamplerParameterIuiv);
        WIN32_LOAD_GLFN(glQueryCounter);
        WIN32_LOAD_GLFN(glGetQueryObjecti64v);
        WIN32_LOAD_GLFN(glGetQueryObjectui64v);
        WIN32_LOAD_GLFN(glVertexAttribDivisor);
        WIN32_LOAD_GLFN(glVertexAttribP1ui);
        WIN32_LOAD_GLFN(glVertexAttribP1uiv);
        WIN32_LOAD_GLFN(glVertexAttribP2ui);
        WIN32_LOAD_GLFN(glVertexAttribP2uiv);
        WIN32_LOAD_GLFN(glVertexAttribP3ui);
        WIN32_LOAD_GLFN(glVertexAttribP3uiv);
        WIN32_LOAD_GLFN(glVertexAttribP4ui);
        WIN32_LOAD_GLFN(glVertexAttribP4uiv);
        WIN32_LOAD_GLFN(glVertexP2ui);
        WIN32_LOAD_GLFN(glVertexP2uiv);
        WIN32_LOAD_GLFN(glVertexP3ui);
        WIN32_LOAD_GLFN(glVertexP3uiv);
        WIN32_LOAD_GLFN(glVertexP4ui);
        WIN32_LOAD_GLFN(glVertexP4uiv);
        WIN32_LOAD_GLFN(glTexCoordP1ui);
        WIN32_LOAD_GLFN(glTexCoordP1uiv);
        WIN32_LOAD_GLFN(glTexCoordP2ui);
        WIN32_LOAD_GLFN(glTexCoordP2uiv);
        WIN32_LOAD_GLFN(glTexCoordP3ui);
        WIN32_LOAD_GLFN(glTexCoordP3uiv);
        WIN32_LOAD_GLFN(glTexCoordP4ui);
        WIN32_LOAD_GLFN(glTexCoordP4uiv);
        WIN32_LOAD_GLFN(glMultiTexCoordP1ui);
        WIN32_LOAD_GLFN(glMultiTexCoordP1uiv);
        WIN32_LOAD_GLFN(glMultiTexCoordP2ui);
        WIN32_LOAD_GLFN(glMultiTexCoordP2uiv);
        WIN32_LOAD_GLFN(glMultiTexCoordP3ui);
        WIN32_LOAD_GLFN(glMultiTexCoordP3uiv);
        WIN32_LOAD_GLFN(glMultiTexCoordP4ui);
        WIN32_LOAD_GLFN(glMultiTexCoordP4uiv);
        WIN32_LOAD_GLFN(glNormalP3ui);
        WIN32_LOAD_GLFN(glNormalP3uiv);
        WIN32_LOAD_GLFN(glColorP3ui);
        WIN32_LOAD_GLFN(glColorP3uiv);
        WIN32_LOAD_GLFN(glColorP4ui);
        WIN32_LOAD_GLFN(glColorP4uiv);
        WIN32_LOAD_GLFN(glSecondaryColorP3ui);
        WIN32_LOAD_GLFN(glSecondaryColorP3uiv);
    }
    
    if(version >= 4.0)
    {
        WIN32_LOAD_GLFN(glMinSampleShading);
        WIN32_LOAD_GLFN(glBlendEquationi);
        WIN32_LOAD_GLFN(glBlendEquationSeparatei);
        WIN32_LOAD_GLFN(glBlendFunci);
        WIN32_LOAD_GLFN(glBlendFuncSeparatei);
        WIN32_LOAD_GLFN(glDrawArraysIndirect);
        WIN32_LOAD_GLFN(glDrawElementsIndirect);
        WIN32_LOAD_GLFN(glUniform1d);
        WIN32_LOAD_GLFN(glUniform2d);
        WIN32_LOAD_GLFN(glUniform3d);
        WIN32_LOAD_GLFN(glUniform4d);
        WIN32_LOAD_GLFN(glUniform1dv);
        WIN32_LOAD_GLFN(glUniform2dv);
        WIN32_LOAD_GLFN(glUniform3dv);
        WIN32_LOAD_GLFN(glUniform4dv);
        WIN32_LOAD_GLFN(glUniformMatrix2dv);
        WIN32_LOAD_GLFN(glUniformMatrix3dv);
        WIN32_LOAD_GLFN(glUniformMatrix4dv);
        WIN32_LOAD_GLFN(glUniformMatrix2x3dv);
        WIN32_LOAD_GLFN(glUniformMatrix2x4dv);
        WIN32_LOAD_GLFN(glUniformMatrix3x2dv);
        WIN32_LOAD_GLFN(glUniformMatrix3x4dv);
        WIN32_LOAD_GLFN(glUniformMatrix4x2dv);
        WIN32_LOAD_GLFN(glUniformMatrix4x3dv);
        WIN32_LOAD_GLFN(glGetUniformdv);
        WIN32_LOAD_GLFN(glGetSubroutineUniformLocation);
        WIN32_LOAD_GLFN(glGetSubroutineIndex);
        WIN32_LOAD_GLFN(glGetActiveSubroutineUniformiv);
        WIN32_LOAD_GLFN(glGetActiveSubroutineUniformName);
        WIN32_LOAD_GLFN(glGetActiveSubroutineName);
        WIN32_LOAD_GLFN(glUniformSubroutinesuiv);
        WIN32_LOAD_GLFN(glGetUniformSubroutineuiv);
        WIN32_LOAD_GLFN(glGetProgramStageiv);
        WIN32_LOAD_GLFN(glPatchParameteri);
        WIN32_LOAD_GLFN(glPatchParameterfv);
        WIN32_LOAD_GLFN(glBindTransformFeedback);
        WIN32_LOAD_GLFN(glDeleteTransformFeedbacks);
        WIN32_LOAD_GLFN(glGenTransformFeedbacks);
        WIN32_LOAD_GLFN(glIsTransformFeedback);
        WIN32_LOAD_GLFN(glPauseTransformFeedback);
        WIN32_LOAD_GLFN(glResumeTransformFeedback);
        WIN32_LOAD_GLFN(glDrawTransformFeedback);
        WIN32_LOAD_GLFN(glDrawTransformFeedbackStream);
        WIN32_LOAD_GLFN(glBeginQueryIndexed);
        WIN32_LOAD_GLFN(glEndQueryIndexed);
        WIN32_LOAD_GLFN(glGetQueryIndexediv);
    }
    
    if(version >= 4.1)
    {
        WIN32_LOAD_GLFN(glReleaseShaderCompiler);
        WIN32_LOAD_GLFN(glShaderBinary);
        WIN32_LOAD_GLFN(glGetShaderPrecisionFormat);
        WIN32_LOAD_GLFN(glDepthRangef);
        WIN32_LOAD_GLFN(glClearDepthf);
        WIN32_LOAD_GLFN(glGetProgramBinary);
        WIN32_LOAD_GLFN(glProgramBinary);
        WIN32_LOAD_GLFN(glProgramParameteri);
        WIN32_LOAD_GLFN(glUseProgramStages);
        WIN32_LOAD_GLFN(glActiveShaderProgram);
        WIN32_LOAD_GLFN(glCreateShaderProgramv);
        WIN32_LOAD_GLFN(glBindProgramPipeline);
        WIN32_LOAD_GLFN(glDeleteProgramPipelines);
        WIN32_LOAD_GLFN(glGenProgramPipelines);
        WIN32_LOAD_GLFN(glIsProgramPipeline);
        WIN32_LOAD_GLFN(glGetProgramPipelineiv);
        WIN32_LOAD_GLFN(glProgramUniform1i);
        WIN32_LOAD_GLFN(glProgramUniform1iv);
        WIN32_LOAD_GLFN(glProgramUniform1f);
        WIN32_LOAD_GLFN(glProgramUniform1fv);
        WIN32_LOAD_GLFN(glProgramUniform1d);
        WIN32_LOAD_GLFN(glProgramUniform1dv);
        WIN32_LOAD_GLFN(glProgramUniform1ui);
        WIN32_LOAD_GLFN(glProgramUniform1uiv);
        WIN32_LOAD_GLFN(glProgramUniform2i);
        WIN32_LOAD_GLFN(glProgramUniform2iv);
        WIN32_LOAD_GLFN(glProgramUniform2f);
        WIN32_LOAD_GLFN(glProgramUniform2fv);
        WIN32_LOAD_GLFN(glProgramUniform2d);
        WIN32_LOAD_GLFN(glProgramUniform2dv);
        WIN32_LOAD_GLFN(glProgramUniform2ui);
        WIN32_LOAD_GLFN(glProgramUniform2uiv);
        WIN32_LOAD_GLFN(glProgramUniform3i);
        WIN32_LOAD_GLFN(glProgramUniform3iv);
        WIN32_LOAD_GLFN(glProgramUniform3f);
        WIN32_LOAD_GLFN(glProgramUniform3fv);
        WIN32_LOAD_GLFN(glProgramUniform3d);
        WIN32_LOAD_GLFN(glProgramUniform3dv);
        WIN32_LOAD_GLFN(glProgramUniform3ui);
        WIN32_LOAD_GLFN(glProgramUniform3uiv);
        WIN32_LOAD_GLFN(glProgramUniform4i);
        WIN32_LOAD_GLFN(glProgramUniform4iv);
        WIN32_LOAD_GLFN(glProgramUniform4f);
        WIN32_LOAD_GLFN(glProgramUniform4fv);
        WIN32_LOAD_GLFN(glProgramUniform4d);
        WIN32_LOAD_GLFN(glProgramUniform4dv);
        WIN32_LOAD_GLFN(glProgramUniform4ui);
        WIN32_LOAD_GLFN(glProgramUniform4uiv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix2fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix3fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix4fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix2dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix3dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix4dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix2x3fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix3x2fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix2x4fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix4x2fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix3x4fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix4x3fv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix2x3dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix3x2dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix2x4dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix4x2dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix3x4dv);
        WIN32_LOAD_GLFN(glProgramUniformMatrix4x3dv);
        WIN32_LOAD_GLFN(glValidateProgramPipeline);
        WIN32_LOAD_GLFN(glGetProgramPipelineInfoLog);
        WIN32_LOAD_GLFN(glVertexAttribL1d);
        WIN32_LOAD_GLFN(glVertexAttribL2d);
        WIN32_LOAD_GLFN(glVertexAttribL3d);
        WIN32_LOAD_GLFN(glVertexAttribL4d);
        WIN32_LOAD_GLFN(glVertexAttribL1dv);
        WIN32_LOAD_GLFN(glVertexAttribL2dv);
        WIN32_LOAD_GLFN(glVertexAttribL3dv);
        WIN32_LOAD_GLFN(glVertexAttribL4dv);
        WIN32_LOAD_GLFN(glVertexAttribLPointer);
        WIN32_LOAD_GLFN(glGetVertexAttribLdv);
        WIN32_LOAD_GLFN(glViewportArrayv);
        WIN32_LOAD_GLFN(glViewportIndexedf);
        WIN32_LOAD_GLFN(glViewportIndexedfv);
        WIN32_LOAD_GLFN(glScissorArrayv);
        WIN32_LOAD_GLFN(glScissorIndexed);
        WIN32_LOAD_GLFN(glScissorIndexedv);
        WIN32_LOAD_GLFN(glDepthRangeArrayv);
        WIN32_LOAD_GLFN(glDepthRangeIndexed);
        WIN32_LOAD_GLFN(glGetFloati_v);
        WIN32_LOAD_GLFN(glGetDoublei_v);
    }
    
    if(version >= 4.2)
    {
        WIN32_LOAD_GLFN(glDrawArraysInstancedBaseInstance);
        WIN32_LOAD_GLFN(glDrawElementsInstancedBaseInstance);
        WIN32_LOAD_GLFN(glDrawElementsInstancedBaseVertexBaseInstance);
        WIN32_LOAD_GLFN(glGetInternalformativ);
        WIN32_LOAD_GLFN(glGetActiveAtomicCounterBufferiv);
        WIN32_LOAD_GLFN(glBindImageTexture);
        WIN32_LOAD_GLFN(glMemoryBarrier);
        WIN32_LOAD_GLFN(glTexStorage1D);
        WIN32_LOAD_GLFN(glTexStorage2D);
        WIN32_LOAD_GLFN(glTexStorage3D);
        WIN32_LOAD_GLFN(glDrawTransformFeedbackInstanced);
        WIN32_LOAD_GLFN(glDrawTransformFeedbackStreamInstanced);
    }
    
    if(version >= 4.3)
    {
        WIN32_LOAD_GLFN(glClearBufferData);
        WIN32_LOAD_GLFN(glClearBufferSubData);
        WIN32_LOAD_GLFN(glDispatchCompute);
        WIN32_LOAD_GLFN(glDispatchComputeIndirect);
        WIN32_LOAD_GLFN(glCopyImageSubData);
        WIN32_LOAD_GLFN(glFramebufferParameteri);
        WIN32_LOAD_GLFN(glGetFramebufferParameteriv);
        WIN32_LOAD_GLFN(glGetInternalformati64v);
        WIN32_LOAD_GLFN(glInvalidateTexSubImage);
        WIN32_LOAD_GLFN(glInvalidateTexImage);
        WIN32_LOAD_GLFN(glInvalidateBufferSubData);
        WIN32_LOAD_GLFN(glInvalidateBufferData);
        WIN32_LOAD_GLFN(glInvalidateFramebuffer);
        WIN32_LOAD_GLFN(glInvalidateSubFramebuffer);
        WIN32_LOAD_GLFN(glMultiDrawArraysIndirect);
        WIN32_LOAD_GLFN(glMultiDrawElementsIndirect);
        WIN32_LOAD_GLFN(glGetProgramInterfaceiv);
        WIN32_LOAD_GLFN(glGetProgramResourceIndex);
        WIN32_LOAD_GLFN(glGetProgramResourceName);
        WIN32_LOAD_GLFN(glGetProgramResourceiv);
        WIN32_LOAD_GLFN(glGetProgramResourceLocation);
        WIN32_LOAD_GLFN(glGetProgramResourceLocationIndex);
        WIN32_LOAD_GLFN(glShaderStorageBlockBinding);
        WIN32_LOAD_GLFN(glTexBufferRange);
        WIN32_LOAD_GLFN(glTexStorage2DMultisample);
        WIN32_LOAD_GLFN(glTexStorage3DMultisample);
        WIN32_LOAD_GLFN(glTextureView);
        WIN32_LOAD_GLFN(glBindVertexBuffer);
        WIN32_LOAD_GLFN(glVertexAttribFormat);
        WIN32_LOAD_GLFN(glVertexAttribIFormat);
        WIN32_LOAD_GLFN(glVertexAttribLFormat);
        WIN32_LOAD_GLFN(glVertexAttribBinding);
        WIN32_LOAD_GLFN(glVertexBindingDivisor);
        WIN32_LOAD_GLFN(glDebugMessageControl);
        WIN32_LOAD_GLFN(glDebugMessageInsert);
        WIN32_LOAD_GLFN(glDebugMessageCallback);
        WIN32_LOAD_GLFN(glGetDebugMessageLog);
        WIN32_LOAD_GLFN(glPushDebugGroup);
        WIN32_LOAD_GLFN(glPopDebugGroup);
        WIN32_LOAD_GLFN(glObjectLabel);
        WIN32_LOAD_GLFN(glGetObjectLabel);
        WIN32_LOAD_GLFN(glObjectPtrLabel);
        WIN32_LOAD_GLFN(glGetObjectPtrLabel);
    }
    
    if(version >= 4.4)
    {
        WIN32_LOAD_GLFN(glBufferStorage);
        WIN32_LOAD_GLFN(glClearTexImage);
        WIN32_LOAD_GLFN(glClearTexSubImage);
        WIN32_LOAD_GLFN(glBindBuffersBase);
        WIN32_LOAD_GLFN(glBindBuffersRange);
        WIN32_LOAD_GLFN(glBindTextures);
        WIN32_LOAD_GLFN(glBindSamplers);
        WIN32_LOAD_GLFN(glBindImageTextures);
        WIN32_LOAD_GLFN(glBindVertexBuffers);
    }
    
    if(version >= 4.5)
    {
        WIN32_LOAD_GLFN(glClipControl);
        WIN32_LOAD_GLFN(glCreateTransformFeedbacks);
        WIN32_LOAD_GLFN(glTransformFeedbackBufferBase);
        WIN32_LOAD_GLFN(glTransformFeedbackBufferRange);
        WIN32_LOAD_GLFN(glGetTransformFeedbackiv);
        WIN32_LOAD_GLFN(glGetTransformFeedbacki_v);
        WIN32_LOAD_GLFN(glGetTransformFeedbacki64_v);
        WIN32_LOAD_GLFN(glCreateBuffers);
        WIN32_LOAD_GLFN(glNamedBufferStorage);
        WIN32_LOAD_GLFN(glNamedBufferData);
        WIN32_LOAD_GLFN(glNamedBufferSubData);
        WIN32_LOAD_GLFN(glCopyNamedBufferSubData);
        WIN32_LOAD_GLFN(glClearNamedBufferData);
        WIN32_LOAD_GLFN(glClearNamedBufferSubData);
        WIN32_LOAD_GLFN(glMapNamedBuffer);
        WIN32_LOAD_GLFN(glMapNamedBufferRange);
        WIN32_LOAD_GLFN(glUnmapNamedBuffer);
        WIN32_LOAD_GLFN(glFlushMappedNamedBufferRange);
        WIN32_LOAD_GLFN(glGetNamedBufferParameteriv);
        WIN32_LOAD_GLFN(glGetNamedBufferParameteri64v);
        WIN32_LOAD_GLFN(glGetNamedBufferPointerv);
        WIN32_LOAD_GLFN(glGetNamedBufferSubData);
        WIN32_LOAD_GLFN(glCreateFramebuffers);
        WIN32_LOAD_GLFN(glNamedFramebufferRenderbuffer);
        WIN32_LOAD_GLFN(glNamedFramebufferParameteri);
        WIN32_LOAD_GLFN(glNamedFramebufferTexture);
        WIN32_LOAD_GLFN(glNamedFramebufferTextureLayer);
        WIN32_LOAD_GLFN(glNamedFramebufferDrawBuffer);
        WIN32_LOAD_GLFN(glNamedFramebufferDrawBuffers);
        WIN32_LOAD_GLFN(glNamedFramebufferReadBuffer);
        WIN32_LOAD_GLFN(glInvalidateNamedFramebufferData);
        WIN32_LOAD_GLFN(glInvalidateNamedFramebufferSubData);
        WIN32_LOAD_GLFN(glClearNamedFramebufferiv);
        WIN32_LOAD_GLFN(glClearNamedFramebufferuiv);
        WIN32_LOAD_GLFN(glClearNamedFramebufferfv);
        WIN32_LOAD_GLFN(glClearNamedFramebufferfi);
        WIN32_LOAD_GLFN(glBlitNamedFramebuffer);
        WIN32_LOAD_GLFN(glCheckNamedFramebufferStatus);
        WIN32_LOAD_GLFN(glGetNamedFramebufferParameteriv);
        WIN32_LOAD_GLFN(glGetNamedFramebufferAttachmentParameteriv);
        WIN32_LOAD_GLFN(glCreateRenderbuffers);
        WIN32_LOAD_GLFN(glNamedRenderbufferStorage);
        WIN32_LOAD_GLFN(glNamedRenderbufferStorageMultisample);
        WIN32_LOAD_GLFN(glGetNamedRenderbufferParameteriv);
        WIN32_LOAD_GLFN(glCreateTextures);
        WIN32_LOAD_GLFN(glTextureBuffer);
        WIN32_LOAD_GLFN(glTextureBufferRange);
        WIN32_LOAD_GLFN(glTextureStorage1D);
        WIN32_LOAD_GLFN(glTextureStorage2D);
        WIN32_LOAD_GLFN(glTextureStorage3D);
        WIN32_LOAD_GLFN(glTextureStorage2DMultisample);
        WIN32_LOAD_GLFN(glTextureStorage3DMultisample);
        WIN32_LOAD_GLFN(glTextureSubImage1D);
        WIN32_LOAD_GLFN(glTextureSubImage2D);
        WIN32_LOAD_GLFN(glTextureSubImage3D);
        WIN32_LOAD_GLFN(glCompressedTextureSubImage1D);
        WIN32_LOAD_GLFN(glCompressedTextureSubImage2D);
        WIN32_LOAD_GLFN(glCompressedTextureSubImage3D);
        WIN32_LOAD_GLFN(glCopyTextureSubImage1D);
        WIN32_LOAD_GLFN(glCopyTextureSubImage2D);
        WIN32_LOAD_GLFN(glCopyTextureSubImage3D);
        WIN32_LOAD_GLFN(glTextureParameterf);
        WIN32_LOAD_GLFN(glTextureParameterfv);
        WIN32_LOAD_GLFN(glTextureParameteri);
        WIN32_LOAD_GLFN(glTextureParameterIiv);
        WIN32_LOAD_GLFN(glTextureParameterIuiv);
        WIN32_LOAD_GLFN(glTextureParameteriv);
        WIN32_LOAD_GLFN(glGenerateTextureMipmap);
        WIN32_LOAD_GLFN(glBindTextureUnit);
        WIN32_LOAD_GLFN(glGetTextureImage);
        WIN32_LOAD_GLFN(glGetCompressedTextureImage);
        WIN32_LOAD_GLFN(glGetTextureLevelParameterfv);
        WIN32_LOAD_GLFN(glGetTextureLevelParameteriv);
        WIN32_LOAD_GLFN(glGetTextureParameterfv);
        WIN32_LOAD_GLFN(glGetTextureParameterIiv);
        WIN32_LOAD_GLFN(glGetTextureParameterIuiv);
        WIN32_LOAD_GLFN(glGetTextureParameteriv);
        WIN32_LOAD_GLFN(glCreateVertexArrays);
        WIN32_LOAD_GLFN(glDisableVertexArrayAttrib);
        WIN32_LOAD_GLFN(glEnableVertexArrayAttrib);
        WIN32_LOAD_GLFN(glVertexArrayElementBuffer);
        WIN32_LOAD_GLFN(glVertexArrayVertexBuffer);
        WIN32_LOAD_GLFN(glVertexArrayVertexBuffers);
        WIN32_LOAD_GLFN(glVertexArrayAttribBinding);
        WIN32_LOAD_GLFN(glVertexArrayAttribFormat);
        WIN32_LOAD_GLFN(glVertexArrayAttribIFormat);
        WIN32_LOAD_GLFN(glVertexArrayAttribLFormat);
        WIN32_LOAD_GLFN(glVertexArrayBindingDivisor);
        WIN32_LOAD_GLFN(glGetVertexArrayiv);
        WIN32_LOAD_GLFN(glGetVertexArrayIndexediv);
        WIN32_LOAD_GLFN(glGetVertexArrayIndexed64iv);
        WIN32_LOAD_GLFN(glCreateSamplers);
        WIN32_LOAD_GLFN(glCreateProgramPipelines);
        WIN32_LOAD_GLFN(glCreateQueries);
        WIN32_LOAD_GLFN(glGetQueryBufferObjecti64v);
        WIN32_LOAD_GLFN(glGetQueryBufferObjectiv);
        WIN32_LOAD_GLFN(glGetQueryBufferObjectui64v);
        WIN32_LOAD_GLFN(glGetQueryBufferObjectuiv);
        WIN32_LOAD_GLFN(glMemoryBarrierByRegion);
        WIN32_LOAD_GLFN(glGetTextureSubImage);
        WIN32_LOAD_GLFN(glGetCompressedTextureSubImage);
        WIN32_LOAD_GLFN(glGetGraphicsResetStatus);
        WIN32_LOAD_GLFN(glGetnCompressedTexImage);
        WIN32_LOAD_GLFN(glGetnTexImage);
        WIN32_LOAD_GLFN(glGetnUniformdv);
        WIN32_LOAD_GLFN(glGetnUniformfv);
        WIN32_LOAD_GLFN(glGetnUniformiv);
        WIN32_LOAD_GLFN(glGetnUniformuiv);
        WIN32_LOAD_GLFN(glReadnPixels);
        WIN32_LOAD_GLFN(glGetnMapdv);
        WIN32_LOAD_GLFN(glGetnMapfv);
        WIN32_LOAD_GLFN(glGetnMapiv);
        WIN32_LOAD_GLFN(glGetnPixelMapfv);
        WIN32_LOAD_GLFN(glGetnPixelMapuiv);
        WIN32_LOAD_GLFN(glGetnPixelMapusv);
        WIN32_LOAD_GLFN(glGetnPolygonStipple);
        WIN32_LOAD_GLFN(glGetnColorTable);
        WIN32_LOAD_GLFN(glGetnConvolutionFilter);
        WIN32_LOAD_GLFN(glGetnSeparableFilter);
        WIN32_LOAD_GLFN(glGetnHistogram);
        WIN32_LOAD_GLFN(glGetnMinmax);
        WIN32_LOAD_GLFN(glTextureBarrier);
    }
    
    return 1;
}

static void Win32_LoadXInput(void)
{
    WIN32_XINPUT_LOADED_VERSION version = WIN32_XINPUT_NOT_LOADED;
    HMODULE lib = 0;
    
    lib = LoadLibraryA("xinput1_4.dll");
    
    if(lib)
    {
        version = WIN32_XINPUT_1_4;
    }
    
    else
    {
        lib = LoadLibraryA("xinput9_1_0.dll");
        
        if(lib)
        {
            version = WIN32_XINPUT_9_1_0;
        }
        
        else
        {
            lib = LoadLibraryA("xinput1_3.dll");
            
            if(lib)
            {
                version = WIN32_XINPUT_1_3;
            }
            
            else
            {
                //NOTE: if xinput failed to load, controllers will just appear to be disconnected.
                PL_SetErrorString("Failed to load xinput DLL.");
                win32_state->xinput.version = WIN32_XINPUT_NOT_LOADED;
                win32_state->xinput.GetState = sfn_XInputGetState;
                win32_state->xinput.SetState = sfn_XInputSetState;
            }
        }
    }
    
    if(lib)
    {
        win32_state->xinput.version = version;
        win32_state->xinput.GetState = (tfn_XInputGetState *)GetProcAddress(lib, "XInputGetState");
        win32_state->xinput.SetState = (tfn_XInputSetState *)GetProcAddress(lib, "XInputSetState");
    }
}

static void Win32_LoadXAudio(void)
{
    WIN32_XAUDIO_LOADED_VERSION version = WIN32_XAUDIO_NOT_LOADED;
    HMODULE lib = 0;
    
    lib = LoadLibraryA("xaudio2_9.dll");
    
    if(lib)
    {
        version = WIN32_XAUDIO_2_9;
    }
    
    else
    {
        lib = LoadLibraryA("xaudio2_9d.dll");
        
        if(lib)
        {
            version = WIN32_XAUDIO_2_9D;
        }
        
        else
        {
            lib = LoadLibraryA("xaudio2_8.dll");
            
            if(lib)
            {
                version = WIN32_XAUDIO_2_8;
            }
            
            else
            {
                lib = LoadLibraryA("xaudio2_7.dll");
                
                if(lib)
                {
                    version = WIN32_XAUDIO_2_7;
                }
                
                else
                {
                    //NOTE: if xaudio doesnt load, audio just wont play
                    PL_SetErrorString("Failed to load xaudio DLL.");
                    win32_state->xaudio.version = WIN32_XAUDIO_NOT_LOADED;
                    win32_state->xaudio.Create = sfn_XAudio2Create;
                }
            }
        }
    }
    
    if(lib)
    {
        win32_state->xaudio.version = version;
        win32_state->xaudio.Create = (tfn_XAudio2Create *)GetProcAddress(lib, "XAudio2Create");
    }
}

static void Win32_AudioInit(void)
{
    if(win32_state->xaudio.version == WIN32_XAUDIO_NOT_LOADED)
    {
        return;
    }
    
    Win32_XAudio *xaudio = &win32_state->xaudio;
    IXAudio2 *handle = xaudio->handle;
    HRESULT ecode;
    
    ecode = XAudio2Create(&handle, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if(FAILED(ecode))
    {
        PL_SetErrorString("Failed to create XAudio2 Handle. Code(%d)", ecode);
        return;
    }
    
#if defined(__cplusplus)
    ecode = handle->CreateMasteringVoice(&xaudio->masterVoice, 2, AUDIO_SAMPLE_RATE, 0,0,0,0);
#else
    ecode = IXAudio2_CreateMasteringVoice(handle, &xaudio->masterVoice, 2, AUDIO_SAMPLE_RATE, 0,0,0,0);
#endif
    if(FAILED(ecode))
    {
        PL_SetErrorString("Failed to create XAudio2 Mastering Voice. Code(%d)", ecode);
        return;
    }
    
	WAVEFORMATEX *fmt = &xaudio->fmt;
    fmt->wFormatTag = WAVE_FORMAT_PCM;
    fmt->nChannels = 2;
    fmt->nSamplesPerSec = AUDIO_SAMPLE_RATE;
    fmt->wBitsPerSample = 16;
    fmt->nBlockAlign = (fmt->nChannels * fmt->wBitsPerSample)/8;
    fmt->nAvgBytesPerSec = fmt->nSamplesPerSec * fmt->nBlockAlign;
    fmt->cbSize = 0;
    
#if defined(__cplusplus)
    ecode = handle->CreateSourceVoice(&xaudio->srcVoice, (WAVEFORMATEX*)fmt, 0, XAUDIO2_MAX_FREQ_RATIO, 0,0,0);
#else
    ecode = IXAudio2_CreateSourceVoice(handle, &xaudio->srcVoice, (WAVEFORMATEX*)fmt, 0, XAUDIO2_MAX_FREQ_RATIO, 0, 0, 0);
#endif
    if(FAILED(ecode))
    {
        PL_SetErrorString("Failed to set XAudio2 audio format. Code(%d)", ecode);
        return;
    }
    
    XAUDIO2_BUFFER *buffer = &xaudio->bufferHeader;
    buffer->Flags = 0; //XAUDIO2_END_OF_STREAM if buffer is empty
    buffer->AudioBytes = AUDIO_BUFFER_SIZE;
    buffer->pAudioData = (BYTE*)win32_state->audio.buffer; // ptr to buffer
    buffer->PlayBegin = 0;
    buffer->PlayLength = 0; // 0 = whole buffer
    buffer->LoopBegin = 0;
    buffer->LoopLength = 0;
    buffer->LoopCount = XAUDIO2_LOOP_INFINITE;
    buffer->pContext = 0; // callback context
    
#if defined(__cplusplus)
    ecode = xaudio->srcVoice.SubmitSourceBuffer(buffer, 0);
#else
    ecode = IXAudio2SourceVoice_SubmitSourceBuffer(xaudio->srcVoice, buffer, 0);
#endif
    if(FAILED(ecode))
    {
        PL_SetErrorString("Failed to submit XAudio2 buffer. Code(%d)", ecode);
        return;
    }
    
#if defined(__cplusplus)
    ecode = xaudio->srcVoice.Start(0, XAUDIO2_COMMIT_NOW);
#else
    ecode = IXAudio2SourceVoice_Start(xaudio->srcVoice, 0, XAUDIO2_COMMIT_NOW);
#endif
    if(FAILED(ecode))
    {
        PL_SetErrorString("XAudio2 failed to play buffer. Code(%d)", ecode);
        return;
    }
}

PL_Audio* PL_GetAudio(void)
{
    return &win32_state->audio;
}

ptr PL_GetAudioBuffer(void)
{
    return (ptr)&win32_state->audio.buffer[0];
}

static void Win32_AudioFrame(void)
{
    XAUDIO2_VOICE_STATE state;
#if defined(__cplusplus)
    win32_state->xaudio.srcVoice.GetState(&state, 0);
#else
    IXAudio2SourceVoice_GetState(win32_state->xaudio.srcVoice, &state, 0);
#endif
    
    PL_GetAudio()->playCursor = state.SamplesPlayed % AUDIO_BUFFER_SIZE;
}

/*========= WIN32 TIMER =============*/

static void Win32_UpdateClock(void)
{
    PL_Clock *clock = &win32_state->clock;
    SYSTEMTIME st = {0};
    GetLocalTime(&st);
    clock->year = st.wYear;
    clock->month = st.wMonth;
    clock->day = st.wDay;
    clock->wday = ((st.wDayOfWeek) ? st.wDayOfWeek : 7);
    clock->hr = st.wHour;
    clock->min = st.wMinute;
    clock->sec = st.wSecond;
    clock->ms = st.wMilliseconds;
}

static void Win32_InitTimer(void)
{
    LARGE_INTEGER freqResult;
    QueryPerformanceFrequency(&freqResult);
    win32_state->timer.perfFreq = (u64)freqResult.QuadPart;
}

static u64 Win32_GetPerfCount(void)
{
    LARGE_INTEGER perf;
    QueryPerformanceCounter(&perf);
    return (u64)perf.QuadPart;
}

r64 Win32_GetPerfDiff(u64 start, u64 end)
{
    r64 result = (((r64)end - (r64)start) / (r64)win32_state->timer.perfFreq);
    return result;
}

r64 Win32_GetPerfElapsed(u64 prev)
{
    r64 result = Win32_GetPerfDiff(prev, Win32_GetPerfCount());
    return result;
}

static void Win32_UpdateTimer(void)
{
    Win32_UpdateClock();
    
    if(!win32_state->window.window.vsync)
    {
        u64 perf = Win32_GetPerfCount();
        r64 elapsedSec = Win32_GetPerfDiff(win32_state->timer.lastFramePerf, perf);
        r64 targetSecPerFrame = (1.0/(r64)win32_state->window.window.framerate);
        
        while(elapsedSec <= targetSecPerFrame)
        {
            perf = Win32_GetPerfCount();
            elapsedSec = Win32_GetPerfDiff(win32_state->timer.lastFramePerf, perf);
        }
    }
    
    if(win32_state->timer.timer.tLastFrame)
    {
        win32_state->timer.timer.tAvgFrame += win32_state->timer.timer.tLastFrame;
        win32_state->timer.timer.tAvgFrame /= 2.0f;
    }
    
    win32_state->timer.timer.tLastFrame = Win32_GetPerfElapsed(win32_state->timer.lastFramePerf);
    win32_state->timer.lastFramePerf = Win32_GetPerfCount();
    win32_state->timer.timer.frames++;
}

u64 PL_TimerStart(void)
{
    return Win32_GetPerfCount();
}

r64 PL_TimerElapsed(u64 timerperf)
{
    return Win32_GetPerfElapsed(timerperf);
}

/*========== SystemInfo ===============*/

static void Win32_SetSystemInfo(void)
{
    SYSTEM_INFO si = {0};
    GetNativeSystemInfo(&si);
    
    switch(si.wProcessorArchitecture)
    {
        case PROCESSOR_ARCHITECTURE_AMD64:
        {
            win32_state->system.cpu = CPU_AMD64;
        } break;
        
        case PROCESSOR_ARCHITECTURE_ARM:
        {
            win32_state->system.cpu = CPU_ARM;
        } break;
        
        case PROCESSOR_ARCHITECTURE_ARM64:
        {
            win32_state->system.cpu = CPU_ARM64;
        } break;
        
        case PROCESSOR_ARCHITECTURE_IA64:
        {
            win32_state->system.cpu = CPU_IA64;
        } break;
        
        case PROCESSOR_ARCHITECTURE_INTEL:
        {
            win32_state->system.cpu = CPU_I86;
        } break;
        
        default:
        {
            win32_state->system.cpu = CPU_UNKNOWN;
        }
    }
    
    win32_state->system.minAppAddress = (ptr)si.lpMinimumApplicationAddress;
    win32_state->system.maxAppAddress = (ptr)si.lpMaximumApplicationAddress;
    win32_state->system.cores = (u32)si.dwNumberOfProcessors;
}

/*============== WNDPROC ============*/

LRESULT CALLBACK Win32_WndProc(HWND window,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam)
{
    LRESULT result = 0;
    
    Win32_Window *win32_window = &win32_state->window;
    PL_Window *pl_window = &win32_window->window;
    PL_Input *input = &win32_state->input;
    if(!win32_window->init)
        return DefWindowProcA(window, msg, wParam, lParam);
    
    switch(msg)
    {
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
        {
            win32_state->running = 0;
        } break;
        
        case WM_ACTIVATEAPP:
        {
            if(wParam == TRUE) pl_window->focus = 1;
            if(wParam == FALSE) pl_window->focus = 0;
        } break;
        
        case WM_MOVE:
        {
            pl_window->dim.x = (int)(short)LOWORD(lParam);
            pl_window->dim.y = (int)(short)HIWORD(lParam);
        } break;
        
        case WM_SIZE:
        {
            pl_window->dim.w = (int)LOWORD(lParam);
            pl_window->dim.h = (int)HIWORD(lParam);
            glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        } break;
        
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            if((lParam & (1 << 29))) //an ALT key
            {
                if(lParam & (1 << 24)) //right ALT key
                {
                    win32_state->input.keyboard[K_RALT].isDown = 1;
                }
                
                else // left ALT key
                {
                    win32_state->input.keyboard[K_LALT].isDown = 1;
                }
                
            }
            
            PL_KEYCODE key = Win32_WinKeyToPLKeycode((u32)wParam, lParam);
            win32_state->input.keyboard[key].isDown = 1;
            
        } break;
        
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            //TODO: fix this. currently, if holding ALT, when another keyup
            // happens, the game will treat this as ALT key up followed by
            // another ALT keydown next frame, rather than ALT staying down.
            if(input->keyboard[K_RALT].isDown)
            {
                input->keyboard[K_RALT].isDown = 0;
            }
            
            if(input->keyboard[K_LALT].isDown)
            {
                input->keyboard[K_LALT].isDown = 0;
            }
            
            PL_KEYCODE key = Win32_WinKeyToPLKeycode((u32)wParam, lParam);
            input->keyboard[key].isDown = 0;
        } break;
        
        case WM_MOUSEMOVE:
        {
            POINT cursor = {0};
            GetCursorPos(&cursor);
            ScreenToClient(win32_window->handle, &cursor);
            input->mouse.px = cursor.x;
            input->mouse.py = cursor.y;
            input->mouse.rx = (PL_norm32((r32)cursor.x, 0.0f, (r32)pl_window->dim.w) * 2.0f) - 1.0f;
            input->mouse.ry = (PL_norm32((r32)cursor.y, 0.0f, (r32)pl_window->dim.h) * 2.0f) - 1.0f;
        } break;
        
        case WM_LBUTTONDOWN:
        {
            input->mouse.buttons[MB_LEFT].isDown = 1;
        } break;
        
        case WM_MBUTTONDOWN:
        {
            input->mouse.buttons[MB_MIDDLE].isDown = 1;
        } break;
        
        case WM_RBUTTONDOWN:
        {
            input->mouse.buttons[MB_RIGHT].isDown = 1;
        } break;
        
        case WM_XBUTTONDOWN:
        {
            if(wParam & MK_XBUTTON1)
            {
                input->mouse.buttons[MB_X1].isDown = 1;
            }
            
            if(wParam & MK_XBUTTON2)
            {
                input->mouse.buttons[MB_X2].isDown = 1;
            }
            
        } break;
        
        case WM_LBUTTONUP:
        {
            input->mouse.buttons[MB_LEFT].isDown = 0;
        } break;
        
        case WM_MBUTTONUP:
        {
            input->mouse.buttons[MB_MIDDLE].isDown = 0;
        } break;
        
        case WM_RBUTTONUP:
        {
            input->mouse.buttons[MB_RIGHT].isDown = 0;
        } break;
        
        case WM_XBUTTONUP:
        {
            if(wParam & MK_XBUTTON1)
            {
                input->mouse.buttons[MB_X1].isDown = 0;
            }
            
            if(wParam & MK_XBUTTON2)
            {
                input->mouse.buttons[MB_X2].isDown = 0;
            }
            
        } break;
        
        case WM_MOUSEWHEEL:
        {
            if(GET_WHEEL_DELTA_WPARAM(wParam) < 0)
            {
                input->mouse.wheel--;
            }
            
            if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                input->mouse.wheel++;
            }
            
        } break;
        
        default:
        {
            result = DefWindowProcA(window, msg, wParam, lParam);
        }
    }
    
    return result;
}

/*================================
   Window Functions
================================*/

// thanks Raymond Chen, the win32GOAT
static void Win32_FullscreenToggle(void)
{
    DWORD style = GetWindowLong(win32_state->window.handle, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitorInfo = {sizeof(monitorInfo)};
        if(GetWindowPlacement(win32_state->window.handle, &win32_state->window.wp) &&
           GetMonitorInfo(MonitorFromWindow(win32_state->window.handle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
        {
            SetWindowLong(win32_state->window.handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(win32_state->window.handle, HWND_TOP,
                         monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                         monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                         monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            win32_state->window.window.fullscreen = 1;
        }
    }
    else
    {
        SetWindowLong(win32_state->window.handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(win32_state->window.handle, &win32_state->window.wp);
        SetWindowPos(win32_state->window.handle, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        win32_state->window.window.fullscreen = 0;
    }
}

static void Win32_SetVSync(b32 vsync)
{
    if(vsync)
    {
        win32_state->window.window.vsync = 1;
        wglSwapIntervalEXT(1);
    }
    else
    {
        win32_state->window.window.vsync = 0;
        wglSwapIntervalEXT(0);
    }
}

static void Win32_ProcessXInputButton(XINPUT_GAMEPAD *xpad, PL_Gamepad *gamepad,
                                      WORD xbutton, PL_GAMEPADBTNCODE btnCode, r64 dT)
{
    if((xpad->wButtons & xbutton) != 0)
    {
        gamepad->buttons[btnCode].isDown = 1;
        gamepad->buttons[btnCode].tDown += dT;
    }
    else
    {
        gamepad->buttons[btnCode].isDown = 0;
    }
}

static void Win32_ProcessInput(r64 dT)
{
    PL_ButtonState *keyboard = win32_state->input.keyboard;
    for(int keyIndex = 0;
        keyIndex < K_MAX;
        keyIndex++)
    {
        if(keyboard[keyIndex].isDown)
        {
            if(!keyboard[keyIndex].wasDown)
            {
                keyboard[keyIndex].downTick = 1;
            }
            keyboard[keyIndex].tDown += dT;
        }
        else if(keyboard[keyIndex].wasDown)
        {
            keyboard[keyIndex].upTick = 1;
        }
    }
    
    PL_ButtonState *buttons = win32_state->input.mouse.buttons;
    for(int mouseBtnIndex = 0;
        mouseBtnIndex < MB_MAX;
        mouseBtnIndex++)
    {
        if(buttons[mouseBtnIndex].isDown)
        {
            if(!buttons[mouseBtnIndex].wasDown)
            {
                buttons[mouseBtnIndex].downTick = 1;
            }
            buttons[mouseBtnIndex].tDown += dT;
        }
        else if(buttons[mouseBtnIndex].wasDown)
        {
            buttons[mouseBtnIndex].upTick = 1;
        }
    }
    
    PL_Gamepad *gamepads = win32_state->input.gamepads;
    for(int gamepadIndex = 0;
        gamepadIndex < XUSER_MAX_COUNT;
        gamepadIndex++)
    {
        if(gamepads[gamepadIndex].isConnected)
        {
            for(int gamepadBtnIndex = 0;
                gamepadBtnIndex < GP_MAX;
                gamepadBtnIndex++)
            {
                if(gamepads[gamepadIndex].buttons[gamepadBtnIndex].isDown)
                {
                    if(!gamepads[gamepadIndex].buttons[gamepadBtnIndex].wasDown)
                    {
                        gamepads[gamepadIndex].buttons[gamepadBtnIndex].downTick = 1;
                    }
                    gamepads[gamepadIndex].buttons[gamepadBtnIndex].tDown += dT;
                }
                else if(gamepads[gamepadIndex].buttons[gamepadBtnIndex].wasDown)
                {
                    gamepads[gamepadIndex].buttons[gamepadBtnIndex].upTick = 1;
                }
            }
        }
    }
    
    for(int gamepadIndex = 0;
        gamepadIndex < XUSER_MAX_COUNT;
        gamepadIndex++)
    {
        XINPUT_STATE xpadState;
        
        if(win32_state->xinput.GetState((DWORD)gamepadIndex, &xpadState) == ERROR_SUCCESS)
        {
            XINPUT_GAMEPAD *xpad = &xpadState.Gamepad;
            PL_Gamepad *gamepad = &gamepads[gamepadIndex];
            
            gamepad->isConnected = 1;
            
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_A, GP_A, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_B, GP_B, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_X, GP_X, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_Y, GP_Y, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_START, GP_START, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_BACK, GP_BACK, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_LEFT_SHOULDER, GP_LEFTSHOULDER, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_RIGHT_SHOULDER, GP_RIGHTSHOULDER, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_DPAD_UP, GP_DPAD_UP, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_DPAD_DOWN, GP_DPAD_DOWN, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_DPAD_LEFT, GP_DPAD_LEFT, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_DPAD_RIGHT, GP_DPAD_RIGHT, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_LEFT_THUMB, GP_L3, dT);
            Win32_ProcessXInputButton(xpad, gamepad, XINPUT_GAMEPAD_RIGHT_THUMB, GP_R3, dT);
            
            gamepad->LT = ((r32)xpad->bLeftTrigger / 255.0f);
            gamepad->RT = ((r32)xpad->bRightTrigger / 255.0f);
            
            if(xpad->sThumbLX < 0) gamepad->LS.x = PL_norm32(xpad->sThumbLX, 0, 32768);
            else gamepad->LS.x = PL_norm32(xpad->sThumbLX, 0, 32767);
            if(xpad->sThumbLY < 0) gamepad->LS.y = PL_norm32(xpad->sThumbLY, 0, 32768);
            else gamepad->LS.y = PL_norm32(xpad->sThumbLY, 0, 32767);
            if(xpad->sThumbRX < 0) gamepad->RS.x = PL_norm32(xpad->sThumbRX, 0, 32768);
            else gamepad->RS.x = PL_norm32(xpad->sThumbRX, 0, 32767);
            if(xpad->sThumbRY < 0) gamepad->RS.y = PL_norm32(xpad->sThumbRY, 0, 32768);
            else gamepad->RS.y = PL_norm32(xpad->sThumbRY, 0, 32767);
            
            if(gamepad->LVib > 1.0f) gamepad->LVib = 1.0f;
            else if(gamepad->LVib < 0.0f) gamepad->LVib = 0.0f;
            if(gamepad->RVib > 1.0f) gamepad->RVib = 1.0f;
            else if(gamepad->RVib < 0.0f) gamepad->RVib = 0.0f;
            
            XINPUT_VIBRATION vib;
            vib.wLeftMotorSpeed = (WORD)(65535.0f * gamepad->LVib);
            vib.wRightMotorSpeed = (WORD)(65535.0f * gamepad->RVib);
            win32_state->xinput.SetState(gamepadIndex, &vib);
        }
        
        else
        {
            if(gamepads[gamepadIndex].isConnected)
            {
                PL_MemZero(&gamepads[gamepadIndex], sizeof(PL_Gamepad));
            }
        }
    }
}

static void Win32_UpdateInput(void)
{
    PL_ButtonState *keyboard = win32_state->input.keyboard;
    for(int keyIndex = 0;
        keyIndex < K_MAX;
        keyIndex++)
    {
        if(keyboard[keyIndex].isDown)
        {
            if(!keyboard[keyIndex].wasDown)
            {
                keyboard[keyIndex].wasDown = 1;
            }
            
            keyboard[keyIndex].downTick = 0;
        }
        else if(keyboard[keyIndex].wasDown)
        {
            keyboard[keyIndex].wasDown = 0;
        }
        else
        {
            keyboard[keyIndex].tDown = 0.0f;
        }
        
        keyboard[keyIndex].downTick = 0;
        keyboard[keyIndex].upTick = 0;
    }
    
    PL_ButtonState *buttons = win32_state->input.mouse.buttons;
    for(int mouseBtnIndex = 0;
        mouseBtnIndex < MB_MAX;
        mouseBtnIndex++)
    {
        if(buttons[mouseBtnIndex].isDown)
        {
            if(!buttons[mouseBtnIndex].wasDown)
            {
                buttons[mouseBtnIndex].wasDown = 1;
            }
            
            buttons[mouseBtnIndex].downTick = 0;
        }
        else if(buttons[mouseBtnIndex].wasDown)
        {
            buttons[mouseBtnIndex].wasDown = 0;
        }
        else
        {
            buttons[mouseBtnIndex].tDown = 0.0f;
        }
        
        buttons[mouseBtnIndex].downTick = 0;
        buttons[mouseBtnIndex].upTick = 0;
    }
    
    win32_state->input.mouse.wheel = 0;
    
    PL_Gamepad *gamepads = win32_state->input.gamepads;
    for(int gamepadIndex = 0;
        gamepadIndex < XUSER_MAX_COUNT;
        gamepadIndex++)
    {
        if(gamepads[gamepadIndex].isConnected)
        {
            for(int gamepadBtnIndex = 0;
                gamepadBtnIndex < GP_MAX;
                gamepadBtnIndex++)
            {
                if(gamepads[gamepadIndex].buttons[gamepadBtnIndex].isDown)
                {
                    if(!gamepads[gamepadIndex].buttons[gamepadBtnIndex].wasDown)
                    {
                        gamepads[gamepadIndex].buttons[gamepadBtnIndex].wasDown = 1;
                    }
                    
                    gamepads[gamepadIndex].buttons[gamepadBtnIndex].downTick = 0;
                }
                else if(gamepads[gamepadIndex].buttons[gamepadBtnIndex].wasDown)
                {
                    gamepads[gamepadIndex].buttons[gamepadBtnIndex].wasDown = 0;
                }
                else
                {
                    gamepads[gamepadIndex].buttons[gamepadBtnIndex].tDown = 0.0f;
                }
                
                gamepads[gamepadIndex].buttons[gamepadBtnIndex].downTick = 0;
                gamepads[gamepadIndex].buttons[gamepadBtnIndex].upTick = 0;
            }
        }
    }
}

static void Win32_UpdateWindow(void)
{
    RECT dim = {0};
    GetClientRect(win32_state->window.handle, &dim);
    win32_state->window.window.dim.w = dim.right;
    win32_state->window.window.dim.h = dim.bottom;
    GetWindowRect(win32_state->window.handle, &dim);
    win32_state->window.window.dim.x = dim.left;
    win32_state->window.window.dim.y = dim.top;
    glViewport(0, 0, win32_state->window.window.dim.w, win32_state->window.window.dim.h);
    SwapBuffers(win32_state->window.dc);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void Win32_MessageLoop(void)
{
    MSG msg;
    while(PeekMessageA(&msg, win32_state->window.handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    
    r64 dT = Win32_GetPerfElapsed(win32_state->timer.lastFramePerf);
    Win32_ProcessInput(dT);
}

static b32 Win32_CreateWindow(void)
{
    Win32_Window *win32_window = &win32_state->window;
    PL_Window *pl_window = &win32_window->window;
    WNDCLASSA *wndclass = &win32_window->wndclass;
    
    wndclass->style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass->lpfnWndProc = Win32_WndProc;
    wndclass->hInstance = GetModuleHandle(0);
    wndclass->lpszClassName = "PLWINDOWCLASS";
    
    if(!RegisterClassA(wndclass))
    {
        PL_SetErrorString("Error registering WNDCLASS. Code(%u)", GetLastError());
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    // temp window to get OpenGL context settings
    HWND tmpWindow = CreateWindowA(wndclass->lpszClassName,
                                   "tmpWindow", WS_OVERLAPPEDWINDOW,
                                   0,0,1,1,0,0, wndclass->hInstance, 0);
    
    if(!tmpWindow)
    {
        PL_SetErrorString("Failed to create Window. Code(%u)", GetLastError());
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    HDC tmpDC = GetDC(tmpWindow);
    HGLRC tmpRC = 0;
    int tmpPixelFormat = 0;
    
    { // tmpWindow Getting PixelFormat
        PIXELFORMATDESCRIPTOR pixelFormatDesc = {0};
        pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pixelFormatDesc.nVersion = 1;
        pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
        pixelFormatDesc.cColorBits = 32;
        pixelFormatDesc.cAlphaBits = 8;
        pixelFormatDesc.cDepthBits = 24;
        tmpPixelFormat = ChoosePixelFormat(tmpDC, &pixelFormatDesc);
        
        if(tmpPixelFormat)
        {
            if(SetPixelFormat(tmpDC, tmpPixelFormat, &pixelFormatDesc))
            {
                tmpRC = wglCreateContext(tmpDC);
            }
        }
        
        else
        {
            PL_SetErrorString("Failed to get pixel format. Code(%u)", GetLastError());
            PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
            return 0;
        }
        
        if(!tmpRC)
        {
            PL_SetErrorString("Failed to get WGL RenderContext. Code(%u)", GetLastError());
            PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
            return 0;
        }
    }
    
    win32_window->style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
    win32_window->handle = CreateWindowExA(0, wndclass->lpszClassName,
                                           "Window", win32_window->style,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           pl_window->dim.w, pl_window->dim.h, 0, 0,
                                           wndclass->hInstance, 0);
    
    if(!win32_window->handle)
    {
        PL_SetErrorString("Failed to create Window. Code(%u)", GetLastError());
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    win32_window->wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(win32_window->handle, &win32_window->wp);
    win32_window->dc = GetDC(win32_window->handle);
    
    if(!wglMakeCurrent(tmpDC, tmpRC))
    {
        PL_SetErrorString("Failed to set OpenGL RenderContext. Code(%u)", GetLastError());
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    if(!Win32_LoadWGL())
    {
        PL_SetErrorString("Failed to load WGL functions.");
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    int pixelFormat;
    BOOL status;
    UINT numFormats;
    
    const int pixelAttribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        0
    };
    
    int contextAttributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, PL_OPENGL_MAJ,
        WGL_CONTEXT_MINOR_VERSION_ARB, PL_OPENGL_MIN,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    
    status = wglChoosePixelFormatARB(win32_window->dc, pixelAttribs, 0, 1,
                                     &pixelFormat, &numFormats);
    
    if(!status || !numFormats)
    {
        PL_SetErrorString("Failed to get Windows OpenGL PixelFormat.");
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    PIXELFORMATDESCRIPTOR pixelFormatDesc = {0};
    DescribePixelFormat(win32_window->dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pixelFormatDesc);
    if(!SetPixelFormat(win32_window->dc, pixelFormat, &pixelFormatDesc))
    {
        PL_SetErrorString("Failed to set Windows OpenGL PixelFormat");
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    win32_window->rc = wglCreateContextAttribsARB(win32_window->dc, 0, contextAttributes);
    if(!win32_window->rc)
    {
        PL_SetErrorString("Failed to create Windows OpenGL RenderContext. Code(%u)", GetLastError());
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    // unset and clear up tmp
    wglMakeCurrent(tmpDC, 0);
    wglDeleteContext(tmpRC);
    ReleaseDC(tmpWindow, tmpDC);
    DestroyWindow(tmpWindow);
    
    if(!wglMakeCurrent(win32_window->dc, win32_window->rc))
    {
        PL_SetErrorString("Failed to set Windows OpenGL RenderContext. Code(%u)", GetLastError());
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    if(!Win32_LoadOpenGL((r32)PL_OPENGL_MAJ + ((r32)PL_OPENGL_MIN/10.0f)))
    {
        PL_SetErrorString("Failed to load OpenGL functions.");
        PL_MsgBoxError("Fatal", "%s", PL_GetErrorString());
        return 0;
    }
    
    win32_window->init = 1;
    Win32_SetVSync(pl_window->vsync);
    
    glViewport(0, 0, pl_window->dim.w, pl_window->dim.h);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(win32_window->dc);
    
    return 1;
}

int CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prevInstance,
                     LPSTR cmdLine,
                     int showCode)
{
    win32_state = (Win32_State*)PL_Alloc0(sizeof(Win32_State));
    
    if(win32_state)
    {
        win32_state->running = 1;
    }
    else
    {
        return -1;
    }
    
    win32_state->userMemorySize = MB(256);
    win32_state->window.window.framerate = 60;
    win32_state->window.window.dim.x = 0;
    win32_state->window.window.dim.y = 0;
    win32_state->window.window.dim.w = 960;
    win32_state->window.window.dim.h = 540;
    win32_state->window.window.vsync = 1;
    win32_state->userMemory = PL_Alloc0(win32_state->userMemorySize);
    
    if(!win32_state->userMemory)
    {
        return -1;
    }
    
    Win32_SetSystemInfo();
    if(!Win32_LoadUserLib()) return -1;
    if(!Win32_LoadGDILib()) return -1;
    if(!Win32_LoadWGLLib()) return -1;
    Win32_LoadXInput();
    Win32_LoadXAudio();
    
    if(!Win32_CreateWindow()) return -1;
    Win32_AudioInit();
    Win32_InitTimer();
    Win32_UpdateTimer();
    
    PL_Startup();
    
    while(win32_state->running)
    {
        Win32_MessageLoop();
        Win32_UpdateTimer();
        Win32_AudioFrame();
        PL_Frame();
        Win32_UpdateWindow();
        Win32_UpdateInput();
    }
    
    return 0;
}

// if subsystem:console is used
int main()
{
    return WinMain(GetModuleHandle(0), 0, GetCommandLineA(), SW_SHOWNORMAL);
}

u64 PL_GetUserMemorySize(void)
{
    return win32_state->userMemorySize;
}

ptr PL_GetUserMemory(void)
{
    return win32_state->userMemory;
}

b32 PL_SetUserMemorySize(u64 size)
{
    if(size > MB(256) &&
       size != win32_state->userMemorySize)
    {
        win32_state->userMemorySize = size;
        PL_ReAlloc0(win32_state->userMemory, win32_state->userMemorySize);
    }
    
    if(win32_state->userMemory)
        return 1;
    return 0;
}

PL_Window *PL_GetWindow(void)
{
    return &win32_state->window.window;
}

void PL_SetWindowTitle(const cstr format, ...)
{
    char title[256] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(title, 256, format, args);
    va_end(args);
    SetWindowTextA(win32_state->window.handle, title);
}

void PL_SetWindowPos(i32 x, i32 y, i32 w, i32 h)
{
    RECT dim = {0};
    GetClientRect(win32_state->window.handle, &dim);
    win32_state->window.window.dim.w = dim.right;
    win32_state->window.window.dim.h = dim.bottom;
    GetWindowRect(win32_state->window.handle, &dim);
    win32_state->window.window.dim.x = dim.left;
    win32_state->window.window.dim.y = dim.top;
    
    if(x == -1) x = dim.left;
    if(y == -1) y = dim.top;
    if(w == -1) w = dim.right;
    if(h == -1) h = dim.bottom;
    
    MoveWindow(win32_state->window.handle, x,y,w,h, 0);
}

void PL_SetWindowFramerate(i32 framerate)
{
    win32_state->window.window.framerate = framerate;
}

void PL_SetWindowVSync(b32 vsync)
{
    Win32_SetVSync(vsync);
}

void PL_SetWindowFullscreen(b32 fullscreen)
{
    if(win32_state->window.window.fullscreen)
    {
        if(!fullscreen) Win32_FullscreenToggle();
    }
    else
    {
        if(fullscreen) Win32_FullscreenToggle();
    }
}

b32 PL_ToggleWindowFullscreen(void)
{
    Win32_FullscreenToggle();
    return win32_state->window.window.fullscreen;
}

/*==============================
      PHRAGLIB MACOS
      Mac Specific
===============================*/
#elif defined(PL_MACOS)

//TODO

int main(int argc, char **argv)
{
    
    return 0;
}



/*==============================
      PHRAGLIB LINUX
      Linux Specific
===============================*/
#elif defined(PL_LINUX)

//TODO

int main(int argc, char **argv)
{
    
    return 0;
}






/*==============================
     END OF PLATFORM CODE
===============================*/
#endif // platform