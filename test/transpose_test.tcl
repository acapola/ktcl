set inFileName "c:\\tmp\\test.dat"
set outFileName "${inFileName}_ortho.dat"
#set fin [open $inFileName]
#set fout [open $outFileName [list WRONLY CREAT TRUNC]]
set cnt [dat_analysis::transpose $inFileName $outFileName [file size $inFileName] [expr 16*1024]]
puts "$cnt bytes processed"
#close $fout
#close $fin	