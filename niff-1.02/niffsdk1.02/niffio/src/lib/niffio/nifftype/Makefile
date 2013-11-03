all: storenif storeh

storenif: genstore.pl  niff.h loadobs.pl
	/usr/local/bin/perl genstore.pl niff.h > storenif

storeh: genstore.pl  niff.h loadobs.pl
	/usr/local/bin/perl genstore.pl -h  niff.h > storeh
