# se chamou direto da linha de comando, chama o build do kernel
ifeq ($(KERNELRELEASE),)

	KERNEL_SOURCE := /dos/TheBest/Downloads/mc504-linux/linux-3.17.2

	PWD := $(shell pwd)
default:	 module hash_app

module:
	$(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
	${RM} hash_app

# se não, foi o kernel que chamou entao usamos a linguagem do kernel
else
	obj-m +=hash_ioctl.o
endif
