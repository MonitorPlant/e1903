#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <windows.h>

#include "../common/debug.h"
#include "../common/object.h"

#define DISPLAY_MAX_CHAR_X 236 //横の解像度÷8
#define DISPLAY_MAX_CHAR_Y 67 //縦の解像度÷16くらい
#define DISPLAY_MAX_RESOLUTION_X 1920
#define DISPLAY_MAX_RESOLUTION_Y 1080

#define TYPE_LOADING0 1
#define TYPE_LOADING1 2
#define TYPE_LOADING2 3
#define TYPE_LOADING3 4
#define TYPE_TITLE 5
#define TYPE_RULES 6
#define TYPE_SELF_WIN 7
#define TYPE_ENEMY_WIN 8
#define TYPE_TARGET1 9
#define TYPE_TARGET2 10
#define TYPE_TARGET3 11
#define TYPE_TARGET4 12
#define TYPE_BULLET_SELF 13
#define TYPE_BULLET_ENEMY 14
#define TYPE_SELF 15
#define TYPE_ENEMY 16
#define TYPE_SELF_SHIELD 17
#define TYPE_ENEMY_SHIELD 18

#define CHAR_NUM(n) (20+n)

DWORD WINAPI GraphicsThread( LPVOID arg ); //グラフィック用スレッド

void displayUpdate( int map[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ] ); //画面出力関数
void writeObject( int map[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], CommonObject object );

#endif /* DISPLAY_H_ */
