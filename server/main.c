#include <windows.h>

#include "../common/debug.h"
#include "main_loop_thread.h"
#include "graphics_thread.h"
#include "data_sync_thread.h"
#include "../common/object.h"

CommonObject map_source;
CommonObject self, enemy;
CommonObject target[ MAX_TARGET_NUM ];
CommonObject bullet[ MAX_BULLET_NUM ];
MouseState self_mouse, enemy_mouse;

BOOL end_program = FALSE; //プログラム終了もしくは強制終了
BOOL game_end = FALSE; //ゲーム正常終了
char point_self = 0, point_enemy = 0;
char remain_time = 0;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    int i;
    //各スレッドのIDとハンドルを作成
    DWORD MainLoopThreadId, GraphicsThreadId, DataSyncThreadId;
    HANDLE GraphicsThreadHandle, DataSyncThreadHandle, MainLoopThreadHandle;

    //各変数の初期化
    map_source.isExist = TRUE;
    map_source.isChanging = FALSE;
    self.isExist = enemy.isExist = FALSE;
    self.isChanging = enemy.isChanging = FALSE;
    self.x = enemy.x = DISPLAY_MAX_CHAR_Y / 2;
    self.y = DISPLAY_MAX_CHAR_X / 4;
    enemy.y = DISPLAY_MAX_CHAR_X / 2;
    self_mouse.click_left = self_mouse.click_right = self_mouse.click_wheel = FALSE;
    enemy_mouse.click_left = enemy_mouse.click_right = enemy_mouse.click_wheel = FALSE;

    for( i = 0; i < MAX_TARGET_NUM; i++ )
    {
        target[ i ].isExist = FALSE;
        target[ i ].isChanging = FALSE;
    }
    for( i = 0; i < MAX_BULLET_NUM; i++ )
    {
        bullet[ i ].isExist = FALSE;
        bullet[ i ].isChanging = FALSE;
    }

    //グラフィックススレッド起動
    GraphicsThreadHandle = CreateThread(
        NULL, //セキュリティ属性
        0, //スタックサイズ
        GraphicsThread, //スレッド関数本体
        NULL, //スレッド関数に渡す引数
        0, //作成オプション(0またはCREATE_SUSPENDED)
        &GraphicsThreadId //スレッドID
    );
    //通信スレッド起動
    DataSyncThreadHandle = CreateThread(
        NULL, //セキュリティ属性
        0, //スタックサイズ
        DataSyncThread, //スレッド関数本体
        NULL, //スレッド関数に渡す引数
        0, //作成オプション(0またはCREATE_SUSPENDED)
        &DataSyncThreadId //スレッドID
    );
    //メインスレッド起動
    MainLoopThreadHandle = CreateThread(
        NULL, //セキュリティ属性
        0, //スタックサイズ
        MainLoopThread, //スレッド関数本体
        NULL, //スレッド関数に渡す引数
        0, //作成オプション(0またはCREATE_SUSPENDED)
        &MainLoopThreadId //スレッドID
    );

    //各スレッド起動確認
    if( GraphicsThreadHandle == NULL )
    {
        printErrorMessage( "main.c", "CreateThread()" );
        end_program = TRUE;
    }
    if( DataSyncThreadHandle == NULL )
    {
        printErrorMessage( "main.c", "CreateThread()" );
        end_program = TRUE;
    }
    if( MainLoopThreadHandle == NULL )
    {
        printErrorMessage( "main.c", "CreateThread()" );
        end_program = TRUE;
    }

    //プログラム終了まで待機 もしくはEscapeが押されると強制終了
    while( end_program == FALSE )
    {
        Sleep( 10 );
        if( GetKeyState( VK_ESCAPE ) & 0x8000 )
        {
            break;
        }
    }

    //各スレッド終了
    SuspendThread( MainLoopThreadHandle );
    SuspendThread( GraphicsThreadHandle );
    SuspendThread( DataSyncThreadHandle );
    CloseHandle( MainLoopThreadHandle );
    CloseHandle( GraphicsThreadHandle );
    CloseHandle( DataSyncThreadHandle );

    ( void )hInstance;
    ( void )hPrevInstance;
    ( void )lpCmdLine;
    ( void )nCmdShow;

    return 0;
}
