init: init.cpp
	g++ init.cpp -o init
client: client.cpp
	g++ client.cpp -o client
destroy: destroy.cpp
	g++ destroy.cpp -o destroy

compile: init client destroy

release: compile
	./init 10

debug:
	g++ init.cpp -o init -Wall -Werror -g -fsanitize=address,undefined
	g++ client.cpp -o client -Wall -Werror -g -fsanitize=address,undefined
	g++ destroy.cpp -o destroy -Wall -Werror -g -fsanitize=address,undefined

destroy_bank: destroy
	./destroy
