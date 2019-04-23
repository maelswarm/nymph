default: nymph.c nymph.h hmap.c hmap.h
	gcc -std=c11 -g nymph.c hmap.c -o nymph
	cp nymph /usr/local/bin