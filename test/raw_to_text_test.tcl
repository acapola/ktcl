set inFileName "c:\\tmp\\test.dat"
set outFileName "${inFileName}.txt"
set fin [open $inFileName]
set fout [open $outFileName [list WRONLY CREAT TRUNC]]
set cnt [dat_analysis::raw_to_text $fin $fout -1]
#[expr 16*1024]
puts "$cnt bytes processed"
close $fout
close $fin	