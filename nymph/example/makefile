nymph: nymph_compiler.c main.n box.n rect.n
	gcc -std=c11 nymph_compiler.c -o nymph
	./nymph main.n main
	gcc -std=c11 -c rect.c rect.h
	gcc -std=c11 -c box.c box.h
	gcc -std=c11 -c main.c main.h
	gcc -std=c11 main.o box.o rect.o -o out
