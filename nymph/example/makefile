nymph: nymph_pp.c main.n box.n rect.n
	gcc -std=c11 -c helper_func.c
	gcc -std=c11 -c error_checking.c
	gcc -std=c11 -c nymph_pp.c
	gcc -std=c11 helper_func.o error_checking.o nymph_pp.o -o nymph
	./nymph main.n main
	gcc -std=c11 -c rect.c
	gcc -std=c11 -c box.c
	gcc -std=c11 -c main.c
	gcc -std=c11 main.o box.o rect.o -o out
