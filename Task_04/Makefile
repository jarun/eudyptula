obj-m += coding_style.o

all:
	make -C $(KERNEL_SOURCE_ROOT) M=$(PWD) modules

clean:
	make -C $(KERNEL_SOURCE_ROOT) M=$(PWD) clean
