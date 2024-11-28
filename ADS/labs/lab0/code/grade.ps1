Write-Host "<<<<<<<< grade test >>>>>>>>"
$suffix="a", "b", "c", "d"
foreach($index in $suffix)
{
    $main_output=$(.\main.exe .\input\test_$index.txt)
    if("$main_output" -eq "")
    {
        $files1=Get-ChildItem .\output
        $files2=Get-ChildItem .\answer\test_$index
        if($files1.Count -ne $files2.Count){
            Write-Host "Directories .\output and .\answer\test_$($index) do not have the same number of files"
            Write-Host "grade: 0/100"
            Write-Host "<<<<<<<< grade test over >>>>>>>>"
            exit
        }else{
            for($i = 0; $i -lt $files1.Count; $i++){
                $file1Content = Get-Content $files1[$i].FullName
                $file2Content = Get-Content $files2[$i].FullName
                $diff = Compare-Object $file1Content $file2Content -PassThru
                if($diff){
                    if([System.IO.Path]::GetExtension($files1[$i].FullName) -ne ".huffzip"){
                        foreach($line in $diff){
                            switch($line.SideIndicator){
                                "<=" { Write-Host "<=$($line)" }
                                "=>" { Write-Host "=>$($line)" }
                            }
                        }
                    }else{
                        Write-Host "Binary files $($files1[$i].FullName) and $($files2[$i].FullName) are different"
                    }
                    rm -Force .\output\*
                    Write-Host "grade: 0/100"
                    Write-Host "<<<<<<<< grade test over >>>>>>>>"
                    exit
                }
            }
            Write-Host "test_$($index): OK"
            rm -Force .\output\*
        }
    }else{
        Write-Host $main_output
        Write-Host "grade: 0/100"
        Write-Host "<<<<<<<< grade test over >>>>>>>>"
        exit 
    }
}
Write-Host "grade: 100/100"
Write-Host "<<<<<<<< grade test over >>>>>>>>"