#!/usr/bin/perl

open(CMD, "cat commands.h *-cmd.h|");
open(CMDP, ">commands-perl.pl");
while(<CMD>){
  if(/#define (\S+) (.+)/){
     print CMDP "\$${1} = $2;\n";
     print CMDP "\$PUKE_NAME2NUM\{'$1'\} = $2;\n";
     print CMDP "\$PUKE_NUM2NAME\{'$2'\} = '$1';\n";
   }
}

print CMDP "\n1;\n";
