#!/usr/bin/perl
#
#Generates a "single file" you can use to quickly add the whole source 
#without any makefile troubles
#

opendir(DIR,".");
@files = readdir(DIR);
closedir(DIR);

open(OUT,">mpi.c");
print OUT "/* File Generated Automatically by gen.pl */\n\n";
for (@files) {
   if ($_ =~ /\.c/ && !($_ =~ /mpi\.c/)) {
      $fname = $_;
      open(SRC,"<$fname");
      print OUT "/* Start: $fname */\n";
      while (<SRC>) {
         print OUT $_;
      }
      close(SRC);
      print OUT "\n/* End: $fname */\n\n";
   }
}
print OUT "\n/* EOF */\n";
close(OUT);
   