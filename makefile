# ---------------------------------------------------------------------------
# Настройки компилятора и линкера
CC	  = sdcc
CFLAGS  = -I./$(INC_DIR) -c --stack-auto --disable-warning 154
LFLAGS  = --code-loc 0x2100 --xram-loc 0x6000 --stack-auto --stack-loc 0x80 

# Настройки M3P
M3P			= m3p
COMPORT		= com1
COMLOG		= $(COMPORT)_log.txt
BAUD		= 2400	
# ---------------------------------------------------------------------------
# Projects
PROJS = lab4 lab5 lab6

# Каталоги с исходными текстами и временными файлами
SRC_DIR = source
INC_DIR = headers
BIN_DIR = binaries

# Name patterns
PROJ_SRC = $(SRC_DIR)/$(1).c
PROJ_OBJ = $(SRC_DIR)/$(1).rel
PROJ_HEX = $(BIN_DIR)/$(1).hex
PROJ_BIN = $(BIN_DIR)/$(1).bin
# ---------------------------------------------------------------------------
# $(PROJS) is in the list because product is hex, but not exactly the $(proj)
.PHONY: all clean $(PROJS) $(foreach proj,$(PROJS),load-$(proj)) dist term

all: $(PROJS)

LIST_ALL_SRC = $(wildcard $(SRC_DIR)/*.c)
LIST_MAIN_SRC = $(foreach proj,$(PROJS),$(call PROJ_SRC,$(proj)))
LIST_SRC = $(filter-out $(LIST_MAIN_SRC),$(LIST_ALL_SRC))

LIST_ALL_OBJ = $(LIST_ALL_SRC:.c=.rel)
LIST_OBJ = $(LIST_SRC:.c=.rel)

BUILD_LIST = $(call PROJ_OBJ,$(1)) $(LIST_OBJ)

define PROJ_template
$(call PROJ_BIN,$(1)): $(call BUILD_LIST,$(1)) makefile | $(BIN_DIR)
	$(CC) $(LFLAGS) -o "$(call PROJ_HEX,$(1))" $$(patsubst %, "%", $(call BUILD_LIST,$(1)))
	$(M3P) hb166 "$(call PROJ_HEX,$(1))" "$(call PROJ_BIN,$(1))" bye
$(1): $(call PROJ_BIN,$(1))

load-$(1): $$(call PROJ_BIN,$(1))
	$(M3P) lfile load-$(1).m3p
endef

$(foreach proj, $(PROJS), $(eval $(call PROJ_template,$(proj))))

$(LIST_ALL_OBJ) : %.rel : %.c makefile
	$(CC) $(CFLAGS) -o "$@" $<

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean:
ifeq ($(OS),Windows_NT)
	del $(BIN_DIR)\*.hex
	del $(BIN_DIR)\*.bin
	del $(BIN_DIR)\*.map
	del $(BIN_DIR)\*.mem
	del $(BIN_DIR)\*.lnk
	del pm3p_*.txt
	del com?_log.txt
	del $(TARBALL).gz
	del $(SRC_DIR)\*.asm
	del $(SRC_DIR)\*.rel
	del $(SRC_DIR)\*.rst
	del $(SRC_DIR)\*.sym
	del $(SRC_DIR)\*.lst
else
	rm -f $(BIN_DIR)/*
	rm -f pm3p_*.txt
	rm -f com?_log.txt
	rm -f $(TARBALL).gz
	rm -f $(SRC_DIR)/*.asm
	rm -f $(SRC_DIR)/*.rel
	rm -f $(SRC_DIR)/*.rst
	rm -f $(SRC_DIR)/*.sym
	rm -f $(SRC_DIR)/*.lst
endif

term:
	$(M3P) echo $(COMLOG) $(BAUD) openchannel $(COMPORT) 0 term
