set inFileName "c:\\tmp\\test.dat"
set fin [open $inFileName]
set res [dat_analysis::entropy $fin [file size $inFileName] 8 8]
close $fin
puts "$res"
