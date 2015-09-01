CFLAGS_miscdev.o := -DDEBUG

obj-m += miscdev.o

all:
	make -C $(KERNEL_SOURCE_ROOT) M=$(PWD) modules

clean:
	make -C $(KERNEL_SOURCE_ROOT) M=$(PWD) clean
