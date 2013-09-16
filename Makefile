BUILDDIR = ${CURDIR}/build

PROG=${BUILDDIR}/mkfatimg

CFLAGS = -Wall -O -Wno-deprecated -I${CURDIR}

SRCS =  src/main.c 

SRCS += fatlib-0.13/drive.c \
	fatlib-0.13/img.c \
	fatlib-0.13/buf.c \
	fatlib-0.13/fat.c \
	fatlib-0.13/file.c \
	fatlib-0.13/dir.c \
	fatlib-0.13/del.c \
	fatlib-0.13/stdio.c \
	fatlib-0.13/readdir.c \
	fatlib-0.13/seekdir.c \
	fatlib-0.13/opensubdir.c \
	fatlib-0.13/mkdir.c \
	fatlib-0.13/rmdir.c \
	fatlib-0.13/delete.c \
	fatlib-0.13/dstat.c \
	fatlib-0.13/nocp.c \
	fatlib-0.13/cp.c \
	fatlib-0.13/cp850.c \
	fatlib-0.13/cp852.c

OBJS=    $(SRCS:%.c=$(BUILDDIR)/%.o)
OBJS_EXTRA = $(SRCS_EXTRA:%.c=$(BUILDDIR)/%.so)
DEPS=    ${OBJS:%.o=%.d}

all: ${PROG}

${PROG}: $(OBJS) ${OBJS_EXTRA} Makefile
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) ${LDFLAGS_cfg}

${BUILDDIR}/%.o: %.c Makefile 
	@mkdir -p $(dir $@)
	$(CC) -MD -MP $(CFLAGS) -c -o $@ $(CURDIR)/$<

clean:
	rm -rf ${BUILDDIR} *~
