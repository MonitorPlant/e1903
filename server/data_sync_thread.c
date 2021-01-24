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
    char send_data[11];
    char receive_data[4];

	//Sleep( 5000 );
	//enemy.isExist = TRUE;
	
	//初期化
	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons( 2021 ); //ポート2021を使用 参照: https://ja.wikipedia.org/wiki/TCPやUDPにおけるポート番号の一覧
	
	
	//自分のローカルIPアドレスを取得
	gethostname( host_name, sizeof( host_name ) );  //実行しているパソコンの名前を取得
	local_addr.S_un.S_addr = *( u_long *)gethostbyname( host_name )->h_addr_list[0];
	
	strcpy( self_address, inet_ntoa( local_addr ) ); //パソコンの名前からローカルIPアドレスを取得
	
	
	//クライアントからの接続を待つ
	sock = socket( AF_INET, SOCK_DGRAM, 0 ); //UDP受信用ソケット作成
	addr.sin_addr.S_un.S_addr = INADDR_ANY; //ブロードキャストの受信用に設定
	
	bind( sock, ( struct sockaddr *)&addr, sizeof( addr ) );
	
	memset( pair_address, 0, sizeof( pair_address ) );
	recv( sock, pair_address, sizeof( pair_address ), 0 );
	
	closesocket( sock ); //UDPのソケットを閉じる

	Sleep( 500 );
	
	
	//クライアントに自分のIPアドレスを送信する
	sock = socket( AF_INET, SOCK_DGRAM, 0 ); //UDP送信用ソケット作成
	addr.sin_addr.S_un.S_addr = inet_addr( "255.255.255.255" ); //ブロードキャストアドレスを指定
	
	setsockopt( sock, SOL_SOCKET, SO_BROADCAST, ( char *)&yes, sizeof( yes ) );
	sendto( sock, self_address, 16, 0, ( struct sockaddr *)&addr, sizeof( addr ) );
	
	closesocket( sock ); //UDPのソケットを閉じる
	WSACleanup();

	
	//TCPで接続を待つ
	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	sock = socket( AF_INET, SOCK_STREAM, 0 ); //TCP送信用ソケット作成
	addr.sin_family = AF_INET;
	addr.sin_port = htons( 2020 );
	addr.sin_addr.S_un.S_addr = INADDR_ANY; //送信先を指定
	
	bind( sock, ( struct sockaddr *)&addr, sizeof( addr ) );
	listen( sock, 5 ); //TCPクライントからの接続要求を待つ状態にする
	len = sizeof( addr );
	sock = accept( sock, ( struct sockaddr *)&addr, &len );
	if( sock == INVALID_SOCKET )
	{
		char str[2] = { '0', '\0' };
		str[0] = '0' + WSAGetLastError();
		printErrorMessage( "data_sync_thread.c", str );
		end_program = TRUE;
		return -1;
	}
	memset( receive_data, 0, sizeof( receive_data ) );

    enemy.isExist = TRUE; //メインスレッドに相手が見つかったことを報告

	while( self.isExist == FALSE )
	{
		Sleep( 10 );
	}
	
	while( game_end == FALSE )
	{
        //送信内容を書き込み
		send_data[ 0 ] = end_program;
        send_data[ 1 ] = remain_time;
		send_data[ 2 ] = point_self / 100;
		send_data[ 3 ] = point_self % 100;
		send_data[ 4 ] = point_enemy / 100;
		send_data[ 5 ] = point_enemy % 100;
        send_data[ 6 ] = self.x - 127;
        send_data[ 7 ] = self.y;
        send_data[ 8 ] = ( self_mouse.click_wheel << 2 ) + ( self_mouse.click_right << 1 ) + ( self_mouse.click_left );
		if( target_generate )
		{
			int i;
			target_generate = FALSE;
			for( i = 0; i < MAX_TARGET_NUM; i++ )
			{
				if( target[ i ].y < 10 )
				{
					send_data[ 9 ] = target[ i ].x;
					send_data[ 10 ] = target[ i ].type;
					break;
				}
			}
		}
		else
		{
			send_data[ 9 ] = 0;
			send_data[ 10 ] = 0;
		}

        //データを送受信
		send( sock, send_data, sizeof( send_data ), 0 );
		recv( sock, receive_data, sizeof( receive_data ), 0 );
		

        //受信内容をコピー
		end_program = receive_data[ 0 ];
        enemy.x = receive_data[ 1 ] + 127;
        enemy.y = receive_data[ 2 ];
		enemy_mouse.click_wheel_pass = enemy_mouse.click_wheel;
		enemy_mouse.click_left_pass = enemy_mouse.click_left;
        enemy_mouse.click_wheel = ( receive_data[ 3 ] >> 2 ) & 1;
        enemy_mouse.click_right = ( receive_data[ 3 ] >> 1 ) & 1;
        enemy_mouse.click_left = receive_data[ 3 ] & 1;

		Sleep( 1 );
	}
	
	closesocket( sock );
	
	WSACleanup();

    ( void )arg;

    return 0;
}
