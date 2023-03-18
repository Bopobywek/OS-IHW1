all: io_manipulator transformer

transformer: transformer.c constants.h
	gcc transformer.c -o transformer -lpthread
io_manipulator: io_manipulator.c constants.h
	gcc io_manipulator.c -o io_manipulator -lpthread


clean:
	rm io_manipulator transformer
