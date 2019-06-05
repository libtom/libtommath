#!/usr/bin/env perl
#
# Generates a "single file" you can use to quickly
# add the whole source without any makefile troubles
#
use strict;
use warnings;

use Getopt::Long;
use File::Basename 'basename';
use File::Glob 'bsd_glob';
use File::Spec;
use Cwd 'getcwd';
use List::MoreUtils 'uniq';

# OS dependent path separator
my $sep = File::Spec->catfile('', '');


# reading a file while working around OS specific quirks.
# TODO: check if it can get replaced with functionality from File::Slurper
sub read_file {
  my $f = shift;
  open my $fh, "<", $f or die "FATAL: read_rawfile() cannot open file '$f': $!";
  binmode $fh;
  return do { local $/; <$fh> };
}

# Writing to a file while working around OS specific quirks.
sub write_file {
  my ($f, $data) = @_;
  die "FATAL: write_file() no data" unless defined $data;
  open my $fh, ">", $f or die "FATAL: write_file() cannot open file '$f': $!";
  binmode $fh;
  print $fh $data or die "FATAL: write_file() cannot write to '$f': $!";
  close $fh or die "FATAL: write_file() cannot close '$f': $!";
  return;
}

# without recursion for now.
sub read_directory
{
  my $dir = shift;
  my @filenames = ();
  $dir = $dir.$sep . 'bn*.c';

  foreach my $file (glob $dir) {
    $file = basename($file);
    $file =~ tr/[a-z]/[A-Z]/;
    $file =~ s/\.C/_C/;
    push @filenames, $file;
  }
  @filenames = sort @filenames;
  return @filenames;
}

sub read_config
{
  my $config = shift;
  my $content = read_file($config);
  my @functions = ();

  for my $line (split /\n/, $content){
    if($line =~ /^.*(BN_[a-zA-Z0-9_]+_C).*$/){
      push @functions, $1;
    }
  }
  # It is a mess because we just swallowed everything.
  return uniq(sort @functions);
}

sub make_static
{
  my $input = shift;
  my $content = "";
    foreach my $line (split /\n/, $input) {
      if ($line =~ /^\s*(mp_err|int|void)\s+(fast_)?(s_)?mp\_[a-z_0-9]+\(.*$/ ) {
        $line = "static " . $line;
      }
      $content = $content . $line . "\n";
    }
  return $content;
}

sub generate_bigfile
{
  my($of, $td, $cf, $mf) = @_;
  my @functions_wanted = ();
  my $all_functions = "";
  my $single_function = "";

  # TODO: checks&balances
   -e $td.$sep."tommath.h"
       or die $td.$sep."tommath.h not found, please check path to LibTomMath sources\n";

  if ($cf ne "") {
     @functions_wanted = read_config($cf);
  }
  else {
     @functions_wanted = read_directory($td);
  }

  foreach my $function (@functions_wanted) {
    $function =~ tr/[A-Z]/[a-z]/;
    $function =~ s/_c$/.c/;

    $single_function = "/* Start: $function */\n";
    my $content = read_file($td.$sep.$function);
    # Change content here.
    if ($mf == 1) {
       $content = make_static($content);
    }
    $single_function = $single_function . $content;
    $single_function = $single_function . "\n/* End: $function */\n\n";
    $all_functions = $all_functions . $single_function;
  }

  write_file($of, $all_functions);
  return 1;
}



sub die_usage {
  die <<"EOO";
usage: $0 -o   OR   $0 --output        [./mpi.c]
       $0 -c   OR   $0 --config-file   [none, use all files in LTM dir]
       $0 -t   OR   $0 --tommath-dir   [./]
       $0 -s   OR   $0 --make-static   [no]

The configuration file must be formatted like the entries in tommath_class.h.
All entries of the form

#define BN_FUNCTIONNAME_C

or simply

BN_FUNCTIONNAME_C

where FUNCTIONNAME is the name of the function, will get included.

If you have run the script etc/make_small_lib.pl you need to give
tommath_class.h as the argument to --config-file.

EOO
}

my $current_dir = getcwd;

my $output = $current_dir . $sep . "mpi.c";
my $tommath_dir = $current_dir;
my $config_file = "";
my $make_static = 0;

GetOptions( "o|output=s"          => \$output,
            "t|tommath-dir=s"     => \$tommath_dir,
            "c|config-file=s"     => \$config_file,
            "s|make-static"       => \$make_static,
            "h|help"              => \my $help
          ) or die_usage;

my $exit_value = generate_bigfile ($output, $tommath_dir, $config_file, $make_static);
exit $exit_value;
