#!/usr/bin/perl
#
# usage:  just start it.  Then copy the files to the local database.
#
# I have a extra cddb category (x-converted) for them, becauce I'm to
# lazy to sort them all into the right category...
#
# make sure the output directory does *not* exist, the script refuses
# to work if it does (just to make sure that it does not overwrite
# something important)
#
# (c) 1998 Gerd Knorr <kraxel@cs.tu-berlin.de>  /  Copying: GNU GPL
#

# config: output directory
$dir="/tmp/cddb";

#############################################################################

$out=0;
open(IN,"$ENV{'HOME'}/.workmandb") ||\
        die "open $ENV{'HOME'}/.workmandb: $!";
mkdir("$dir",0777) || die "mkdir $dir: $!";

while (<IN>) {
    if (/^tracks/) {
        if ($out) {
            print OUT "EXTD=\n";
            for ($i = 2, $n = 0; $i < $t[1]+2; $i++) {
                printf OUT "EXTT%d=\n",$i-2;
            }
            print OUT "PLAYORDER=\n";
            close OUT;
        }
        @t = split;
        for ($i = 2, $n = 0; $i < $t[1]+2; $i++) {
            $len = $t[$i]/75;
            for ($quer = 0; $len > 0; $quer += $len % 10, $len /= 10) {};
            $n += $quer + ($len % 60);
        }
        $m = ($t[$t[1]+2] - int($t[2]/75));
        $id = (($n % 0xff) << 24)  |  ($m << 8)  |  $t[1];
        $magic = sprintf("%08x",$id);
        open(OUT,">$dir/$magic") || die "open $dir/$magic: $!";
        $out=1;
    }

    $cdname = $1 if (/^cdname (.*)/);
    if (/^artist (.*)/) {
        $artist = $1;

        print OUT "# xmcd 2.0 CD database file\n";
        print OUT "#\n";
        print OUT "# Track frame offsets:\n";
        for ($i = 2, $n = 0; $i < $t[1]+2; $i++) {
            print OUT "#\t$t[$i]\n";
        }
        print OUT "#\n";
        print OUT "# Disc length: $t[$t[1]+2] seconds\n";
        print OUT "#\n";
        print OUT "# Revision: 1\n";
        print OUT "# Submitted via: wdb2cddb 1.0\n";
        print OUT "#\n";
        
        print OUT "DISCID=$magic\n";
        print OUT "DTITLE=$artist / $cdname\n";
        print "$magic - $artist / $cdname\n";
        $track=0;
    }
    if (/^track (.*)/) {
        print OUT "TTITLE$track=$1\n";
        $track++;
    }
}

close IN;

print OUT "EXTD=\n";
for ($i = 2, $n = 0; $i < $t[1]+2; $i++) {
    printf OUT "EXTT%d=\n",$i-2;
}
print OUT "PLAYORDER=\n";
close OUT;

print "\n*** The CDDB files are in $dir ***\n\n";

