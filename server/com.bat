@echo off
bcc32 -tW main.c ../common/debug.c ../common/loading.c ../common/parts.c ../common/rules.c ../common/result.c ../common/title.c main_loop_thread.c graphics_thread.c data_sync_thread.c
