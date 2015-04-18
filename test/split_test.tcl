set inFileName "c:\\tmp\\test.dat"
set res [dat_analysis::split $inFileName 0 [file size $inFileName] 32 4 [list 0.dat 1.dat 2.dat 3.dat 4.dat 5.dat 6.dat 7.dat]]
puts "$res"
