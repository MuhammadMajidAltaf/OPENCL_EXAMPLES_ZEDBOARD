c:linear_trnsfrm.c
	 gcc -I/usr/local/include linear_trnsfrm.c -o linear_trnsfrm -O3 /usr/local/lib/libpapi.a
	./linear_trnsfrm > test.txt
	@if [ "$(shell uname -p)" = "x86_64" ]; \
		then \
			echo 'c_Logic,Timing' > ../../reports/x86_64_c/linear_trnsfrm.csv; \
			sed -n '/PAPI/p' test.txt | awk '{print $$3,$$4}' OFS="," >> ../../reports/x86_64_c/linear_trnsfrm.csv; \
			rm -rf test.txt; \
	elif [ "$(shell uname -p)" = "armv7l" ]; \
		then \
			echo 'C_Logic,Timing' > ../../reports/armv7l_c/linear_trnsfrm.csv; \
			sed -n '/PAPI/p' test.txt | awk '{print $$3,$$4}' OFS="," >> ../../reports/armv7l_c/linear_trnsfrm.csv; \
			rm -rf test.txt; \
	else \
		echo "done"; \
	fi


clean:
	rm -rf output.pgm linear_trnsfrm *~
