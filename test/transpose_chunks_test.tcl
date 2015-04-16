set inFileName "c:\\tmp\\test.dat"
set outFileName "${inFileName}_8x4chunks.dat"
set cnt [dat_analysis::transpose $inFileName $outFileName 16 4 4]
puts "$cnt bytes processed"
