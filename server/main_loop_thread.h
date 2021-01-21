#ifndef MAIN_LOOP_THREAD_H_
#define MAIN_LOOP_THREAD_H_

#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include "../common/object.h"
#include "../common/debug.h"

#define HTIMER DWORD
#define SET_TIMER(n) n=GetTickCount()
#define PASSED_TIME(n) (GetTickCount()-n)

DWORD WINAPI MainLoopThread( LPVOID arg ); //ゲーム処理用スレッド

void mainLoop( void );
void updateMouseState( void ); //マウスの状態を更新

#endif /* MAIN_LOOP_THREAD_H_ */
