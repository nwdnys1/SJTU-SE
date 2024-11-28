#! /bin/bash

seed=${1:-0}
ele_cnt_array=(50 100 200 500 1000)
p_array_print=(1/2 1/e 1/4 1/8)
p_array=(0.5 0.36787944117144233 0.25 0.125)

echo -n '#ele'
printf ',%s' "${p_array_print[@]}"
echo
for ele_cnt in "${ele_cnt_array[@]}"; do
	echo -n $ele_cnt
	for p in "${p_array[@]}"; do
		echo -n ','
		./test-main $ele_cnt $seed $p
	done
	echo
done
