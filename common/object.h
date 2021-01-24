#ifndef OBJECT_H_
#define OBJECT_H_

#define MAX_TARGET_NUM 16
#define MAX_BULLET_NUM 16

typedef struct _COMMON_OBJECT_
{
    BOOL isExist;
    DWORD x;
    DWORD y;
    char type;
    size_t size_x;
    size_t size_y;
} CommonObject;

typedef struct _MOUSE_STATE_
{
    BOOL click_left;
    BOOL click_right;
    BOOL click_wheel;
    BOOL click_left_pass;
    BOOL click_wheel_pass;
} MouseState;

#endif /* OBJECT_H_ */
