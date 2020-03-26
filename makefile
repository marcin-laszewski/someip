ML	?= ../libml

CFLAGS	+= -Wall
CFLAGS	+= -I.
CFLAGS	+= -I$(ML)/include
CFLAGS	+= -g
#CFLAGS	+= -O

CL	= hello-cli
CL_OUT	= hello/$(CL)
CL_OBJS	= \
	hello/cli.o \
	$(ML)/file/select/read.o \
	$(ML)/net/inet/addr.o \
	$(ML)/net/inet/addr/hostent.o \
	$(ML)/net/inet/addr/name.o \
	$(ML)/net/udp/socket.o \
	$(ML)/net/unix/addr.o \
	$(ML)/net/unix/dgram/socket.o \
	print/data.o \
	print/hdr.o \
	print/msg.o \
	print/recv.o \
	recv.o \
	send.o \
	type/text.o \

OUTS +=	$(CL_OUT)

SR	= hello-srv
SR_OUT	= hello/$(SR)
SR_OBJS	= \
	hello/srv.o \
	$(ML)/file/select/read.o \
	$(ML)/file/select/write.o \
	$(ML)/net/inet/addr/any.o \
	$(ML)/net/inet/addr/ip.o \
	$(ML)/net/udp/socket.o \
	$(ML)/net/unix/addr.o \
	$(ML)/net/unix/dgram/socket.o \
	print/data.o \
	print/hdr.o \
	print/msg.o \
	print/recv.o \
	recv.o \
	send.o \
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
	@echo 'make	doc'
	@echo 'make	distclean'
	@echo 'make		clean'

#--- general -----------------
all: $(OUTS)

$(DEBUGS):
	gdb -x $^.gdb $<

$(OUTS):
	@echo '[LD]	' $@
	@$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: %.c
	@echo '[CC]	' $@
	@$(CC) $(CFLAGS) -o $@ -c $<

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
		--print-data \
		--stdin \
	< $<.cmd

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

#--- doc ---------------------
doc::	index.html

%.html: %.txt
	@echo '[DOC]	' $< ' -> ' $@
	@asciidoc $^
