ds:
	g++ ./dumpswitch/*.cpp ./socket/*.cpp ./thread/*.cpp -pthread -o dumpswitch.out

n:
	g++ ./node/*.cpp ./socket/*.cpp ./thread/*.cpp -pthread -o node.out

ls:
	g++ ./listenserver/*.cpp -pthread -o listenserver.out
