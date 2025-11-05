mapping:
	gcc -O0 -Wall -o mapping mapping.c

mapping_anon:
	gcc -O0 -Wall -o mapping mapping_anon.c

textfile.txt:
	echo "Hello, this is mmap test data!" > textfile.txt
	truncate -s 8096 $@

timing:
	gcc -O0 -Wall -o timing timing.c

timing_cached:
	gcc -O0 -Wall -o timing timing_cached.c

timing_uncached:
	gcc -O0 -Wall -o timing timing_uncached.c

clean:
	rm -f timing 2> /dev/null
	rm -f textfile.txt 2> /dev/null
	rm -f mapping 2> /dev/null