obj-m += dbgmod.o

all:
	make -C $(KERNEL_SOURCE_ROOT) M=$(PWD) modules

clean:
	make -C $(KERNEL_SOURCE_ROOT) M=$(PWD) clean
