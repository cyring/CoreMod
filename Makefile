#
#  CoreMod by CyrIng
#
#  Copyright (C) 2012-2015 CYRIL INGENIERIE
#
#  Licenses: GPL2
#
#  Makefile
#
CC = gcc
CFLAGS = -Wall -Wno-unused-but-set-variable -O0 -mtune=generic -x c
APP_NAME=coremod
SRC_PATH=.
OBJ_PATH=./obj
BIN_PATH=./bin
INC_PATH=.
#
all :		$(APP_NAME)
		@echo 'Ready     : '$(BIN_PATH)/$(APP_NAME)
		@if [ `id -u` -ne 0 ]; then echo 'Remark    : You must be root to run CoreMod'; fi
#
help :
		@echo 'make [all]     : Compile, link and store executable file in' $(BIN_PATH)/$(APP_NAME)
		@echo 'make clean     : Remove object and binary files.'
		@echo 'make clean-all : Remove object and binary sub-directories.'
#
clean :
		@echo 'Cleaning ...'
		@if [ -e $(BIN_PATH)/$(APP_NAME) ];   then rm -v $(BIN_PATH)/$(APP_NAME);   else echo 'Nothing to do in '$(BIN_PATH); fi
		@if [ -e $(OBJ_PATH)/$(APP_NAME).o ]; then rm -v $(OBJ_PATH)/$(APP_NAME).o; else echo 'Nothing to do in '$(OBJ_PATH); fi
#
clean-all:
		@echo 'Cleaning all ...'
		@if [ -d $(BIN_PATH) ]; then rm -vr $(BIN_PATH); else echo 'Nothing to do in '$(BIN_PATH); fi
		@if [ -d $(OBJ_PATH) ]; then rm -vr $(OBJ_PATH); else echo 'Nothing to do in '$(OBJ_PATH); fi
#
$(APP_NAME)	: $(APP_NAME).o
		@echo -n 'Linking   : '
		@if [ ! -d $(BIN_PATH) ]; then mkdir $(BIN_PATH); fi
		@$(CC) $(OBJ_PATH)/$(APP_NAME).o -o $(BIN_PATH)/$(APP_NAME)
		@echo 'Done.'
#
$(APP_NAME).o	: $(APP_NAME).c
		@echo -n 'Compiling : '
		@if [ ! -d $(OBJ_PATH) ]; then mkdir $(OBJ_PATH); fi
		@$(CC) $(CFLAGS) -c $(APP_NAME).c -I $(INC_PATH) -o $(OBJ_PATH)/$(APP_NAME).o
		@echo 'Done.'
