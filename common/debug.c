#include <windows.h>
#include <string.h>

#include "debug.h"

void printErrorMessage( char* file_name, char* error_type )
{
    char message[1024] = "�v���O�����̎��s���ɃG���[���������܂���\n";

    //�G���[���b�Z�[�W�쐬
    strcat( message, "�t�@�C���� : ");
    strcat( message, file_name );
    strcat( message, "\n" );
    strcat( message, "�G���[�ӏ� : " );
    strcat( message, error_type );

    //�G���[���b�Z�[�W�E�B���h�E����
    MessageBox( NULL, TEXT( message ), TEXT( "���s���G���[" ), MB_ICONERROR );
}
