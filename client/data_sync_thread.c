#include <windows.h>
#include <winsock2.h>

#include "../common/debug.h"
#include "../common/object.h"
#include "data_sync_thread.h"

extern CommonObject background;
extern CommonObject self, enemy;
extern CommonObject target[ MAX_TARGET_NUM ];
extern CommonObject bullet[ MAX_BULLET_NUM ];
extern CommonObject number[8];
extern MouseState self_mouse, enemy_mouse;

extern BOOL end_program;
extern BOOL game_end;
extern BOOL target_generate;
extern BOOL bullet_enemy_generate;
extern BOOL bullet_self_generate;
extern BOOL start_program;
extern short point_self, point_enemy;
extern char remain_time;

DWORD WINAPI DataSyncThread( LPVOID arg )
{
    //各変数を宣言
	char host_name[256];
	char self_address[16];
	char pair_address[16];
	
	struct WSAData wsaData;
	SOCKET sock;
	struct sockaddr_in addr;
	struct in_addr local_addr;
	int len;
	BOOL yes = 1;
    char send_data[4];
    char receive_data[11];


    //クライアント側がマウスをクリックするまで待機
    while( enemy.isExist == FALSE )
    {
        Sleep( 10 );
    }
	
	//初期化
	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons( 2021 ); //ポート2021を使用 参照: https://ja.wikipedia.org/wiki/TCPやUDPにおけるポート番号の一覧
	
	
	//自分のローカルIPアドレスを取得
	gethostname( host_name, sizeof( host_name ) );  //実行しているパソコンの名前を取得
	local_addr.S_un.S_addr = *( u_long *)gethostbyname( host_name )->h_addr_list[0];
	
	strcpy( self_address, inet_ntoa( local_addr ) ); //パソコンの名前からローカルIPアドレスを取得
	
	
	//サーバーに自分のIPアドレスを送信する
	sock = socket( AF_INET, SOCK_DGRAM, 0 ); //UDP送信用ソケット作成
	addr.sin_addr.S_un.S_addr = inet_addr( "255.255.255.255" ); //ブロードキャストアドレスを指定
	
	setsockopt( sock, SOL_SOCKET, SO_BROADCAST, ( char *)&yes, sizeof( yes ) );
	sendto( sock, self_address, 16, 0, ( struct sockaddr *)&addr, sizeof( addr ) );
	
	closesocket( sock ); //UDPのソケットを閉じる


    //サーバーからの応答を待つ
	sock = socket( AF_INET, SOCK_DGRAM, 0 ); //UDP受信用ソケット作成
	addr.sin_addr.S_un.S_addr = INADDR_ANY; //ブロードキャストの受信用に設定
	
	bind( sock, ( struct sockaddr *)&addr, sizeof( addr ) );
	
	memset( pair_address, 0, sizeof( pair_address ) );
	recv( sock, pair_address, sizeof( pair_address ), 0 );
	
	closesocket( sock ); //UDPのソケットを閉じる
    WSACleanup();
	
	
	//TCPでデータを送信する
    WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	sock = socket( AF_INET, SOCK_STREAM, 0 ); //TCP送信用ソケット作成
    addr.sin_family = AF_INET;
    addr.sin_port = htons( 2020 );
	addr.sin_addr.S_un.S_addr = inet_addr( pair_address ); //送信先を指定

    Sleep( 100 );
	
	connect( sock, ( struct sockaddr *)&addr, sizeof( addr ) ); //サーバーに接続
    memset( receive_data, 0, sizeof( receive_data ) );


    self.isExist = TRUE; //メインスレッドに相手が見つかったことを報告
	
	while( end_program == FALSE )
	{
        //送信内容を書き込み
		send_data[ 0 ] = end_program;
        send_data[ 1 ] = enemy.x - 127;
        send_data[ 2 ] = enemy.y;
        send_data[ 3 ] = bullet_enemy_generate << 3;
        bullet_enemy_generate = FALSE;
        send_data[ 3 ] += enemy_mouse.click_wheel << 2;
        send_data[ 3 ] += enemy_mouse.click_right << 1;
        send_data[ 3 ] += self_mouse.click_left;

        //データを送受信
		send( sock, send_data, sizeof( send_data ), 0 );
		recv( sock, receive_data, sizeof( receive_data ), 0 );
		

        //受信内容をコピー
		game_end = receive_data[ 0 ];
        remain_time = receive_data[ 1 ];
        point_self = receive_data[ 2 ] * 100 + receive_data[ 3 ];
        point_enemy = receive_data[ 4 ] * 100 + receive_data[ 5 ];
        self.x = receive_data[ 6 ] + 127;
        self.y = receive_data[ 7 ];
		self_mouse.click_wheel_pass = self_mouse.click_wheel;
		self_mouse.click_left_pass = self_mouse.click_left;
        bullet_self_generate = ( receive_data[ 8 ] >> 3 ) & 1;
        self_mouse.click_wheel = ( receive_data[ 8 ] >> 2 ) & 1;
        self_mouse.click_right = ( receive_data[ 8 ] >> 1 ) & 1;
        self_mouse.click_left = receive_data[ 8 ] & 1;
        //的を生成
        if( receive_data[ 9 ] != 0 )
        {
            int i;
            for( i = 0; i < MAX_TARGET_NUM; i++ )
            {
                if( target[ i ].isExist == FALSE )
                {
                    target_generate = TRUE;
                    target[ i ].isExist = TRUE;
                    target[ i ].y = 0;
                    target[ i ].x = receive_data[ 9 ] + 64;
                    //的の種類をランダムに生成
                    switch( receive_data[ 10 ] )
                    {
                        case TYPE_TARGET1:
                            target[ i ].type = TYPE_TARGET1;
                            target[ i ].size_x = 10;
                            target[ i ].size_y = 5;
                            break;
                        case TYPE_TARGET2:
                            target[ i ].type = TYPE_TARGET2;
                            target[ i ].size_x = 10;
                            target[ i ].size_y = 5;
                            break;
                        case TYPE_TARGET3:
                            target[ i ].type = TYPE_TARGET3;
                            target[ i ].size_x = 6;
                            target[ i ].size_y = 3;
                            break;
                        case TYPE_TARGET4:
                            target[ i ].type = TYPE_TARGET4;
                            target[ i ].size_x = 4;
                            target[ i ].size_y = 2;
                            break;
                    }
                    break;
                }
            }
        }
        
		Sleep( 1 );
	}
	
	closesocket( sock );
	
	WSACleanup();

    ( void )arg;

    return 0;
}
