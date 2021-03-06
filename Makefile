pocl_profiling:
	cd gaussian_blur && $(MAKE) pocl
	cd gaussian_blur/gaussian_blur_c && $(MAKE) c
	cd linear_trnsfrm && $(MAKE) pocl
	cd linear_trnsfrm/linear_trnsfrm_c && $(MAKE) c
	cd log_transform && $(MAKE) pocl
	cd log_transform/log_transform_c && $(MAKE) c
	cd matrixconv && $(MAKE) pocl
	cd matrixconv/matrixconv_c && $(MAKE) c
	cd matrixmul && $(MAKE) pocl
	cd matrixmul/matrixmul_c && $(MAKE) c
	cd negative && $(MAKE) pocl
	cd negative/negative_c && $(MAKE) c
	cd power_law_trnsfrm && $(MAKE) pocl
	cd power_law_trnsfrm/power_law_trnsfrm_c && $(MAKE) c
	cd transpose && $(MAKE) pocl
	cd transpose/transpose_c && $(MAKE) c
	cd reports/$(shell uname -p) && ./append.py

linux64_profiling:
	cd gaussian_blur && $(MAKE) linux64
	cd linear_trnsfrm && $(MAKE) linux64
	cd log_transform && $(MAKE) linux64
	cd matrixconv && $(MAKE) linux64
	cd matrixmul && $(MAKE) linux64
	cd negative && $(MAKE) linux64
	cd power_law_trnsfrm && $(MAKE) linux64
	cd transpose && $(MAKE) linux64

clean:
	cd devquery && $(MAKE) clean
	cd hello && $(MAKE) clean
	cd gaussian_blur && $(MAKE) clean
	cd gaussian_blur/gaussian_blur_c && $(MAKE) clean
	cd linear_trnsfrm && $(MAKE) clean
	cd linear_trnsfrm/linear_trnsfrm_c && $(MAKE) clean
	cd log_transform && $(MAKE) clean
	cd log_transform/log_transform_c && $(MAKE) clean
	cd matrixconv && $(MAKE) clean
	cd matrixconv/matrixconv_c && $(MAKE) clean
	cd matrixmul && $(MAKE) clean
	cd matrixmul/matrixmul_c && $(MAKE) clean
	cd negative && $(MAKE) clean
	cd negative/negative_c && $(MAKE) clean
	cd power_law_trnsfrm && $(MAKE) clean
	cd power_law_trnsfrm/power_law_trnsfrm_c && $(MAKE) clean
	cd transpose && $(MAKE) clean
	cd transpose/transpose_c && $(MAKE) clean
	rm -rf reports/*.csv
	rm -rf *~ reports/*~
	#rm -rf reports/*.xls

