CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CSpellBuild

clean:
	$(RM) -f $(BIN_DIR)/CSpellBuild

SRC = \
CSpellBuild.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I. \
-I../../CSpell/include \
-I../../CStrUtil/include \

LFLAGS = \
$(LEBUG) \
-L$(LIB_DIR) \
-L../../CSpell/lib \
-L../../CStrUtil/lib \
-lCSpell \
-lCStrUtil \

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(BIN_DIR)/CSpellBuild: $(OBJS)
	$(CC) -o $(BIN_DIR)/CSpellBuild $(OBJS) $(LFLAGS)
