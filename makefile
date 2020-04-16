ML	?= ../libml

CFLAGS	+= -Wall
CFLAGS	+= -I.
CFLAGS	+= -I$(ML)/include
CFLAGS	+= -g
#CFLAGS	+= -O

ARGS_OBJS = \
	args/print.o \
	args/someip.o \
	args/unix/dgram.o \
	args/unix/stream.o \

CL	= cli
CL_OUT	= $(CL)
CL_OBJS	= \
	$(ARGS_OBJS) \
	cli.o \
	$(ML)/file/select/read.o \
	$(ML)/file/select/write.o \
	$(ML)/net/inet/addr.o \
	$(ML)/net/inet/addr/hostent.o \
	$(ML)/net/inet/addr/name.o \
	$(ML)/net/recvn/timeout.o \
	$(ML)/net/sendn/timeout.o \
	$(ML)/net/udp/socket.o \
	$(ML)/net/unix/addr.o \
	$(ML)/net/unix/dgram/socket.o \
	print/data.o \
	print/hdr.o \
	print/msg.o \
	print/recv.o \
	recv.o \
	recvn.o \
	send.o \
	sendn.o \
	type/text.o \
	unix/addr.o \
	unix/open.o \
	unix/unlink.o  \

OUTS +=	$(CL_OUT)

SR	= srv
SR_OUT	= $(SR)
SR_OBJS	= \
	$(ARGS_OBJS) \
	srv.o \
	$(ML)/file/select/read.o \
	$(ML)/file/select/write.o \
	$(ML)/net/inet/addr/any.o \
	$(ML)/net/inet/addr/ip.o \
	$(ML)/net/recvn/timeout.o \
	$(ML)/net/sendn/timeout.o \
	$(ML)/net/udp/socket.o \
	$(ML)/net/unix/addr.o \
	$(ML)/net/unix/dgram/socket.o \
	$(ML)/net/unix/stream/socket.o \
	print/data.o \
	print/hdr.o \
	print/msg.o \
	print/recv.o \
	recv.o \
	recvn.o \
	send.o \
	sendn.o \
	type/text.o \
	unix/addr.o \
	unix/open.o \
	unix/unlink.o \

SRV_CFLAGS	+= -DSTREAM_TIMEOUT=10

CFLAGS	+= $(SRV_CFLAGS)

OUTS +=	$(SR_OUT)

DEBUGS	= $(addprefix debug-,$(OUTS))

include	someip.mk

si_opts = \
	--service $(si_service) \
	--method $(si_method) \
	--client $(si_client) \
	--session $(si_session) \

print_opts = \
	--print-recv \
	--print-hdr \
	--print-data \

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
	$(CC) $(CFLAGS) -o $@ -c $<

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

run-cli-unix-stream: $(CL_OUT)
	./$< $(si_opts) $(print_opts) \
		--timeout 15 \
		--unix-stream /tmp/hello-srv \
		--stdin \
		#< hello-cli.cmd
#	--unix-dgram /tmp/hello-srv \
#	--unix-dgram /tmp/vsomeip-0 \
#	--udp 127.0.0.1 30509 \


$(CL_OUT): $(CL_OBJS)

#--- server ------------------
debug-$(SR_OUT): $(SR_OUT)

run-$(SR_OUT): $(SR_OUT)
	./$< --udp $(si_port)

run-srv-unx-stream: $(SR_OUT)
	./$< $(si_opts) $(print_opts) \
		--unix-stream /tmp/hello-srv

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
