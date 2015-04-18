set inFileName "c:\\tmp\\test.dat"
#set fin [open $inFileName]
set res [dat_analysis::entropy $inFileName [file size $inFileName] 32 4]
#close $fin
puts "$res"
