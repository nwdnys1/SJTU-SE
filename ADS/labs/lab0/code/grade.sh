echo "<<<<<<<< grade test >>>>>>>>"
for index in a b c d; do
    main_output=$(./main ./input/test_${index}.txt)
    if [ -z "$main_output" ]; then
        diff_output=$(diff -u ./output/ ./answer/test_${index}/)
        rm -rf ./output/*
        if [ -z "$diff_output" ]; then
            echo "test_${index}: OK"
        else
            echo "$diff_output"
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
echo "<<<<<<<< grade test over >>>>>>>>"