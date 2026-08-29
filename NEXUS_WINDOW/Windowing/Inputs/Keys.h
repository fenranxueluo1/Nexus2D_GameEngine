#pragma once
#include <SDL3/SDL.h>
/*
*  The key numbers are defined based on SDLK key code values
*/

// SDL 中"未知键"的值是 SDLK_UNKNOWN（即 0），不是 -1。
// 原先取值 -1 会导致"按键未知"的判断永远不成立。
constexpr int NEXUS_KEY_UNKNOWN = SDLK_UNKNOWN;

constexpr int NEXUS_KEY_BACKSPACE = SDLK_BACKSPACE;
constexpr int NEXUS_KEY_TAB = SDLK_TAB;
constexpr int NEXUS_KEY_CLEAR = SDLK_CLEAR;
constexpr int NEXUS_KEY_RETURN = SDLK_RETURN;
constexpr int NEXUS_KEY_PAUSE = SDLK_PAUSE;
constexpr int NEXUS_KEY_ESCAPE = SDLK_ESCAPE;
constexpr int NEXUS_KEY_SPACE = SDLK_SPACE;
constexpr int NEXUS_KEY_EXCLAIM = SDLK_EXCLAIM;
constexpr int NEXUS_KEY_QUOTEDBL = SDLK_DBLAPOSTROPHE;
constexpr int NEXUS_KEY_HASH = SDLK_HASH;
constexpr int NEXUS_KEY_DOLLAR = SDLK_DOLLAR;
constexpr int NEXUS_KEY_AMPERSAND = SDLK_AMPERSAND;
constexpr int NEXUS_KEY_QUOTE = SDLK_APOSTROPHE;
constexpr int NEXUS_KEY_LEFTPAREN = SDLK_LEFTPAREN;
constexpr int NEXUS_KEY_RIGHTPAREN = SDLK_RIGHTPAREN;
constexpr int NEXUS_KEY_ASTERISK = SDLK_ASTERISK;
constexpr int NEXUS_KEY_PLUS = SDLK_PLUS;
constexpr int NEXUS_KEY_COMMA = SDLK_COMMA;
constexpr int NEXUS_KEY_MINUS = SDLK_MINUS;
constexpr int NEXUS_KEY_PERIOD = SDLK_PERIOD;
constexpr int NEXUS_KEY_SLASH = SDLK_SLASH;
constexpr int NEXUS_KEY_0 = SDLK_0;
constexpr int NEXUS_KEY_1 = SDLK_1;
constexpr int NEXUS_KEY_2 = SDLK_2;
constexpr int NEXUS_KEY_3 = SDLK_3;
constexpr int NEXUS_KEY_4 = SDLK_4;
constexpr int NEXUS_KEY_5 = SDLK_5;
constexpr int NEXUS_KEY_6 = SDLK_6;
constexpr int NEXUS_KEY_7 = SDLK_7;
constexpr int NEXUS_KEY_8 = SDLK_8;
constexpr int NEXUS_KEY_9 = SDLK_9;
constexpr int NEXUS_KEY_COLON = SDLK_COLON;
constexpr int NEXUS_KEY_SEMICOLON = SDLK_SEMICOLON;
constexpr int NEXUS_KEY_LESS = SDLK_LESS;
constexpr int NEXUS_KEY_EQUALS = SDLK_EQUALS;
constexpr int NEXUS_KEY_GREATER = SDLK_GREATER;
constexpr int NEXUS_KEY_QUESTION = SDLK_QUESTION;
constexpr int NEXUS_KEY_AT = SDLK_AT;

constexpr int NEXUS_KEY_LEFTBRACKET = SDLK_LEFTBRACKET;
constexpr int NEXUS_KEY_BACKSLASH = SDLK_BACKSLASH;
constexpr int NEXUS_KEY_RIGHTBRACKET = SDLK_RIGHTBRACKET;
constexpr int NEXUS_KEY_CARET = SDLK_CARET;
constexpr int NEXUS_KEY_UNDERSCORE = SDLK_UNDERSCORE;
constexpr int NEXUS_KEY_BACKQUOTE = SDLK_GRAVE;
constexpr int NEXUS_KEY_A = SDLK_A;
constexpr int NEXUS_KEY_B = SDLK_B;
constexpr int NEXUS_KEY_C = SDLK_C;
constexpr int NEXUS_KEY_D = SDLK_D;
constexpr int NEXUS_KEY_E = SDLK_E;
constexpr int NEXUS_KEY_F = SDLK_F;
constexpr int NEXUS_KEY_G = SDLK_G;
constexpr int NEXUS_KEY_H = SDLK_H;
constexpr int NEXUS_KEY_I = SDLK_I;
constexpr int NEXUS_KEY_J = SDLK_J;
constexpr int NEXUS_KEY_K = SDLK_K;
constexpr int NEXUS_KEY_L = SDLK_L;
constexpr int NEXUS_KEY_M = SDLK_M;
constexpr int NEXUS_KEY_N = SDLK_N;
constexpr int NEXUS_KEY_O = SDLK_O;
constexpr int NEXUS_KEY_P = SDLK_P;
constexpr int NEXUS_KEY_Q = SDLK_Q;
constexpr int NEXUS_KEY_R = SDLK_R;
constexpr int NEXUS_KEY_S = SDLK_S;
constexpr int NEXUS_KEY_T = SDLK_T;
constexpr int NEXUS_KEY_U = SDLK_U;
constexpr int NEXUS_KEY_V = SDLK_V;
constexpr int NEXUS_KEY_W = SDLK_W;
constexpr int NEXUS_KEY_X = SDLK_X;
constexpr int NEXUS_KEY_Y = SDLK_Y;
constexpr int NEXUS_KEY_Z = SDLK_Z;
constexpr int NEXUS_KEY_DELETE = SDLK_DELETE;

constexpr int NEXUS_KEY_CAPSLOCK = SDLK_CAPSLOCK;

constexpr int NEXUS_KEY_F1 = SDLK_F1;
constexpr int NEXUS_KEY_F2 = SDLK_F2;
constexpr int NEXUS_KEY_F3 = SDLK_F3;
constexpr int NEXUS_KEY_F4 = SDLK_F4;
constexpr int NEXUS_KEY_F5 = SDLK_F5;
constexpr int NEXUS_KEY_F6 = SDLK_F6;
constexpr int NEXUS_KEY_F7 = SDLK_F7;
constexpr int NEXUS_KEY_F8 = SDLK_F8;
constexpr int NEXUS_KEY_F9 = SDLK_F9;
constexpr int NEXUS_KEY_F10 = SDLK_F10;
constexpr int NEXUS_KEY_F11 = SDLK_F11;
constexpr int NEXUS_KEY_F12 = SDLK_F12;


constexpr int NEXUS_KEY_SCROLLOCK = SDLK_SCROLLLOCK;

constexpr int NEXUS_KEY_INSERT = SDLK_INSERT;
constexpr int NEXUS_KEY_HOME = SDLK_HOME;
constexpr int NEXUS_KEY_PAGEUP = SDLK_PAGEUP;
constexpr int NEXUS_KEY_END = SDLK_END;
constexpr int NEXUS_KEY_PAGEDOWN = SDLK_PAGEDOWN;

constexpr int NEXUS_KEY_RIGHT = SDLK_RIGHT;
constexpr int NEXUS_KEY_LEFT = SDLK_LEFT;
constexpr int NEXUS_KEY_DOWN = SDLK_DOWN;
constexpr int NEXUS_KEY_UP = SDLK_UP;

constexpr int NEXUS_KEY_NUMLOCK = SDLK_NUMLOCKCLEAR;
constexpr int NEXUS_KEY_KP_DIVIDE = SDLK_KP_DIVIDE;
constexpr int NEXUS_KEY_KP_MULTIPLY = SDLK_KP_MULTIPLY;
constexpr int NEXUS_KEY_KP_MINUS = SDLK_KP_MINUS;
constexpr int NEXUS_KEY_KP_PLUS = SDLK_KP_PLUS;
constexpr int NEXUS_KEY_KP_ENTER = SDLK_KP_ENTER;
constexpr int NEXUS_KEY_KP1 = SDLK_KP_1;
constexpr int NEXUS_KEY_KP2 = SDLK_KP_2;
constexpr int NEXUS_KEY_KP3 = SDLK_KP_3;
constexpr int NEXUS_KEY_KP4 = SDLK_KP_4;
constexpr int NEXUS_KEY_KP5 = SDLK_KP_5;
constexpr int NEXUS_KEY_KP6 = SDLK_KP_6;
constexpr int NEXUS_KEY_KP7 = SDLK_KP_7;
constexpr int NEXUS_KEY_KP8 = SDLK_KP_8;
constexpr int NEXUS_KEY_KP9 = SDLK_KP_9;
constexpr int NEXUS_KEY_KP0 = SDLK_KP_0;
constexpr int NEXUS_KEY_KP_PERIOD = SDLK_KP_PERIOD;

constexpr int NEXUS_KEY_LCTRL = SDLK_LCTRL;
constexpr int NEXUS_KEY_LSHIFT = SDLK_LSHIFT;
constexpr int NEXUS_KEY_LALT = SDLK_LALT;
constexpr int NEXUS_KEY_RCTRL = SDLK_RCTRL;
constexpr int NEXUS_KEY_RSHIFT = SDLK_RSHIFT;
constexpr int NEXUS_KEY_RALT = SDLK_RALT;
