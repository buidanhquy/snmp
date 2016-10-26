.PHONY: all clean

CC:=gcc

#APP:=snmp-demo-app-v1
#APP:=snmp-demo-app-v3
APP:=main
APP_DIR:=./src
APP_SRCS:=$(APP).c

SNMP_DIR:=./src
SNMP_SRCS:=net-snmp.c

CFLAGS:=-I. `net-snmp-config --cflags` 
CFLAGS+=-Iinclude
LIBS:=`net-snmp-config --libs`

all: $(APP)

$(APP): $(APP_SRCS:.c=.o) $(SNMP_SRCS:.c=.o)
	$(CC) -o $@ $^ $(LIBS)
$(APP_SRCS:.c=.o): %.o : $(APP_DIR)/%.c
	$(CC) -c -o $@ $^ $(CFLAGS)
$(SNMP_SRCS:.c=.o): %.o : $(SNMP_DIR)/%.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -fr *.o $(APP)

