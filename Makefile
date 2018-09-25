# Makefile – makefile of our first driver

# if KERNELRELEASE is not defined, we've been called directly from the command line.
# Invoke the kernel build system.
ifeq (${KERNELRELEASE},)
    KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	${CC} -o cli_64 cli.c
	${CC} -m32 -o cli cli.c

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
	rm -f cli cli_64

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else
    obj-m := fransis.o
endif
