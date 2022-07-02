all:read write

read:6_fifo_read.c
	gcc $< -o $@

write:6_fifo_write.c
	gcc $< -o $@

.PHONY:clean
clean:
	rm -rf read write 
