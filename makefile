ML	?= ../libml

CFLAGS	+= -Wall
CFLAGS	+= -I.
CFLAGS	+= -I$(ML)/include
CFLAGS	+= -g
#CFLAGS	+= -O

CL	= cli-hello
CL_OUT	= $(CL)
CL_OBJS	= \
	client.o \
	$(ML)/file/select/read.o \
	$(ML)/net/addr.o \
	$(ML)/net/addr/hostent.o \
	$(ML)/net/addr/name.o \
	$(ML)/net/udp/socket.o \
	print/data.o \
	print/hdr.o \
	print/msg.o \
	print/recv.o \
	recv.o \
	send.o \
	type/text.o \

OUTS +=	$(CL_OUT)

SR	= srv-hello
SR_OUT	= $(SR)
SR_OBJS	= \
	$(ML)/file/select/read.o \
	$(ML)/file/select/write.o \
	$(ML)/net/inet/bind/any.o \
	$(ML)/net/udp/socket.o \
	print/data.o \
	print/hdr.o \
	print/msg.o \
	print/recv.o \
	recv.o \
	send.o \
	server.o \
	type/text.o \

OUTS +=	$(SR_OUT)

DEBUGS	= $(addprefix debug-,$(OUTS))

include	someip.mk

help::
	@echo 'make	all'
	@echo 'make		$(CL_OUT)'
	@echo 'make		$(SR_OUT)'
	@echo 'make	run-$(CL_OUT)'
	@echo 'make	run-$(SR_OUT)'
	@echo 'make	distclean'
	@echo 'make		clean'

#--- general -----------------
all: $(OUTS)

$(DEBUGS):
	gdb -x $^.gdb $<

$(OUTS):
	@echo '[LD]	' $@
	@$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	@echo '[CC]	' $@
	@$(CC) $(CFLAGS) -o $@ -c $< $(LIBS)

#--- client ------------------
debug-$(CL_OUT): $(CL_OUT)

run-$(CL_OUT): $(CL_OUT)
	./$< \
		--udp $(si_host) $(si_port) \
		--service $(si_service) \
		--method $(si_method) \
		--client $(si_client) \
		--session $(si_session) \
		--print-recv \
		--print-hdr \
		--print-data

$(CL_OUT): $(CL_OBJS)

#--- server ------------------
debug-$(SR_OUT): $(SR_OUT)

run-$(SR_OUT): $(SR_OUT)
	./$< --udp $(si_port)

$(SR_OUT): $(SR_OBJS)

#--- clean -------------------
distclean: clean clean-baks clean-outs

clean::
	@echo '[CLEAN]	objs'
	@$(RM) $(CL_OBJS) $(SR_OBJS)

clean-baks::
	@echo '[CLEAN]	baks'
	@find . -name '*~' -exec $(RM) '{}' ';'

clean-outs::
	@echo '[CLEAN]	outs'
	@$(RM) $(OUTS)
