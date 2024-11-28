foreach ($treeType in 'RadixTree', 'CompressedRadixTree') {
    Write-Host "<<<<<<<< grade $treeType >>>>>>>>"
    
    foreach ($index in 'a', 'b', 'c') {
        .\main.exe .\input\test_${index}.txt .\output\test_${index}_${treeType}.txt $treeType
        $main_output = $LASTEXITCODE

        if (-not $main_output) {
            $diff_output = Compare-Object -ReferenceObject (Get-Content .\output\test_${index}_${treeType}.txt) -DifferenceObject (Get-Content .\expected\test_${index}_${treeType}.txt) -PassThru

            if (-not $diff_output) {
                Write-Host "test_${index}: OK"
            } else {
                Write-Host "grade: 0/100"
                Write-Host "<<<<<<<< grade test over >>>>>>>>"
                Exit
            }
        } else {
            Write-Host $main_output
            Write-Host "grade: 0/100"
            Write-Host "<<<<<<<< grade test over >>>>>>>>"
            Exit
        }
    }

    Write-Host "grade: 100/100"
    Write-Host "<<<<<<<< grade $treeType over >>>>>>>>"
}
