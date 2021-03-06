#include <windows.h>

#include "../common/debug.h"
#include "../common/object.h"
#include "../common/graphics_thread.h"
#include "main_loop_thread.h"
#include "data_sync_thread.h"

CommonObject background;
CommonObject self, enemy;
CommonObject target[ MAX_TARGET_NUM ];
CommonObject bullet[ MAX_BULLET_NUM ];
CommonObject number[8];
MouseState self_mouse, enemy_mouse;

BOOL end_program = FALSE; //プログラム終了もしくは強制終了
BOOL game_end = FALSE; //ゲーム正常終了
BOOL target_generate = FALSE;
BOOL bullet_self_generate = FALSE;
BOOL bullet_enemy_generate = FALSE;
int last_target_generate = 0;
short point_self = 0, point_enemy = 0;
char remain_time = 0;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    int i;
    //各スレッドのIDとハンドルを作成
    DWORD MainLoopThreadId, GraphicsThreadId, DataSyncThreadId;
    HANDLE GraphicsThreadHandle, DataSyncThreadHandle, MainLoopThreadHandle;

    //各変数の初期化
    background.isExist = TRUE;
    self.isExist = enemy.isExist = FALSE;
    self.y = enemy.y = DISPLAY_MAX_CHAR_Y / 2;
    self.x = DISPLAY_MAX_CHAR_X / 4;
    enemy.x = 200;
    self.size_x = enemy.size_x = 30;
    self.size_y = enemy.size_y = 15;
    self_mouse.click_left = self_mouse.click_right = self_mouse.click_wheel = FALSE;
    self_mouse.click_left_pass = self_mouse.click_wheel_pass = FALSE;
    enemy_mouse.click_left = enemy_mouse.click_right = enemy_mouse.click_wheel = FALSE;
    enemy_mouse.click_left_pass = enemy_mouse.click_wheel_pass = FALSE;

    for( i = 0; i < 8; i++ )
    {
        number[ i ].isExist = FALSE;
        number[ i ].y = 0;
        number[ i ].size_x = 7;
        number[ i ].size_y = 7;
    }
    //自分のポイント表示用
    number[0].x = DISPLAY_MAX_CHAR_X / 4;
    number[1].x = DISPLAY_MAX_CHAR_X / 4 + 7;
    number[2].x = DISPLAY_MAX_CHAR_X / 4 + 14;
    //残り時間表示用
    number[3].x = DISPLAY_MAX_CHAR_X / 2 - 7;
    number[4].x = DISPLAY_MAX_CHAR_X / 2;
    //敵のポイント表示用
    number[5].x = DISPLAY_MAX_CHAR_X * 3 / 4;
    number[6].x = DISPLAY_MAX_CHAR_X * 3 / 4 + 7;
    number[7].x = DISPLAY_MAX_CHAR_X * 3 / 4 + 14;

    for( i = 0; i < MAX_TARGET_NUM; i++ )
    {
        target[ i ].isExist = FALSE;
        target[ i ].type = 0;
        target[ i ].size_x = 0;
        target[ i ].size_y = 0;
    }
    for( i = 0; i < MAX_BULLET_NUM; i++ )
    {
        bullet[ i ].isExist = FALSE;
        bullet[ i ].size_x = 8;
        bullet[ i ].size_y = 2;
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
