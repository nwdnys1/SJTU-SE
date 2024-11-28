for treeType in RadixTree CompressedRadixTree; do
    echo "<<<<<<<< grade ${treeType} >>>>>>>>"
    for index in a b c; do
        ./main ./input/test_${index}.txt ./output/test_${index}_${treeType}.txt ${treeType}
        if [ -z "$main_output" ]; then
            diff_output=$(diff -u ./output/test_${index}_${treeType}.txt ./expected/test_${index}_${treeType}.txt)
            if [ -z "$diff_output" ]; then
                echo "test_${index}: OK"
            else
                echo $diff_output
                echo "grade: 0/100"
                echo "<<<<<<<< grade test over >>>>>>>>"
                exit
            fi
        else
            echo "$main_output"
            echo "grade: 0/100"
            echo "<<<<<<<< grade test over >>>>>>>>"
            exit 
        fi
    done
    echo "grade: 100/100"
    echo "<<<<<<<< grade ${treeType} over >>>>>>>>"
done
