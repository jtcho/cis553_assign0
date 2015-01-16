all: 
	make -f demo.mk server_demo
	make -f demo.mk client_demo

clean:
	make -f demo.mk clean

tarball: clean
	rm -f server_demo
	rm -f client_demo
	tar cf ../tcp_demo.tar .

