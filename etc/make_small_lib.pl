#!/usr/bin/env perl

use strict;
use warnings;

use Getopt::Long;
use File::Basename 'basename';
use File::Glob 'bsd_glob';
use File::Spec;
use List::MoreUtils 'any';

# OS dependent path separator
my $sep = File::Spec->catfile('', '');

# A list of the functions that trade space for speed.
# They will be removed from the list if the option "-n" is given
my %fast_functions = (
       s_mp_balance_mul            => 1,
       s_mp_exptmod_fast           => 1,
       s_mp_invmod_fast            => 1,
       s_mp_karatsuba_mul          => 1,
       s_mp_karatsuba_sqr          => 1,
       s_mp_montgomery_reduce_fast => 1,
       s_mp_mul_digs_fast          => 1,
       s_mp_mul_high_digs_fast     => 1,
       s_mp_sqr_fast               => 1,
       s_mp_toom_mul               => 1,
       s_mp_toom_sqr               => 1
   );
# The global variable where gather_functions() puts all it's findings in.
my @dependency_list = ();

# Uniquefy an array.
sub uniq {
    my %seen;
    grep !$seen{$_}++, @_;
}

# reading a file while working around OS specific quirks.
# TODO: check if it can get replaced with functionailty from File::Slurper
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

# Change makefile to compile all the files needed but not more.
sub patch_makefile {
  my ($content, @variables) = @_;
  for my $v (@variables) {
    if ($v =~ /^([A-Z0-9_]+)\s*=.*$/si) {
      my $name = $1;
      $content =~ s/\n\Q$name\E\b.*?[^\\]\n/\n$v\n/s;
    }
    else {
      die "patch_file failed: " . substr($v, 0, 30) . "..";
    }
  }
  return $content;
}

# Pretty-print the list for the variable `OBJECTS` in the makfiles
sub prepare_makefile_variable {
  my ($varname, @list) = @_;
  my $output = "$varname=";
  my $len = length($output);
  foreach my $obj (sort @list) {
    $len = $len + length $obj;
    $obj =~ s/\*/\$/;
    if ($len > 100) {
      $output .= "\\\n";
      $len = length $obj;
    }
    $output .= $obj . ' ';
  }
  $output =~ s/ $//;
  return $output;
}

# Support for a certain IDE
sub prepare_msvc_files_xml {
  my ($all, $exclude_re, $targets) = @_;
  my $last = [];
  my $depth = 2;

  # sort files in the same order as visual studio (ugly, I know)
  my @parts = ();
  for my $orig (@$all) {
    my $p = basename($orig);
    $p =~ s|/|/~|g;
    $p =~ s|/~([^/]+)$|/$1|g;
    my @l = map { sprintf "% -99s", $_ } split /\//, $p;
    push @parts, [ basename($orig), join(':', @l) ];
  }

  my @sorted = map { $_->[0] } sort { $a->[1] cmp $b->[1] } @parts;

  my $files = "<Files>\r\n";
  for my $full (@sorted) {
    my @items = split /\//, $full; # split by '/'
    $full =~ s|/|\\|g;             # replace '/' bt '\'
    shift @items; # drop first one (src)
    pop @items;   # drop last one (filename.ext)
    my $current = \@items;
    if (join(':', @$current) ne join(':', @$last)) {
      my $common = 0;
      $common++ while ($last->[$common] && $current->[$common] && $last->[$common] eq $current->[$common]);
      my $back = @$last - $common;
      if ($back > 0) {
        $files .= ("\t" x --$depth) . "</Filter>\r\n" for (1..$back);
      }
      my $fwd = [ @$current ]; splice(@$fwd, 0, $common);
      for my $i (0..scalar(@$fwd) - 1) {
        $files .= ("\t" x $depth) . "<Filter\r\n";
        $files .= ("\t" x $depth) . "\tName=\"$fwd->[$i]\"\r\n";
        $files .= ("\t" x $depth) . "\t>\r\n";
        $depth++;
      }
      $last = $current;
    }
    $files .= ("\t" x $depth) . "<File\r\n";
    $files .= ("\t" x $depth) . "\tRelativePath=\"$full\"\r\n";
    $files .= ("\t" x $depth) . "\t>\r\n";
    if ($full =~ $exclude_re) {
      for (@$targets) {
        $files .= ("\t" x $depth) . "\t<FileConfiguration\r\n";
        $files .= ("\t" x $depth) . "\t\tName=\"$_\"\r\n";
        $files .= ("\t" x $depth) . "\t\tExcludedFromBuild=\"true\"\r\n";
        $files .= ("\t" x $depth) . "\t\t>\r\n";
        $files .= ("\t" x $depth) . "\t\t<Tool\r\n";
        $files .= ("\t" x $depth) . "\t\t\tName=\"VCCLCompilerTool\"\r\n";
        $files .= ("\t" x $depth) . "\t\t\tAdditionalIncludeDirectories=\"\"\r\n";
        $files .= ("\t" x $depth) . "\t\t\tPreprocessorDefinitions=\"\"\r\n";
        $files .= ("\t" x $depth) . "\t\t/>\r\n";
        $files .= ("\t" x $depth) . "\t</FileConfiguration>\r\n";
      }
    }
    $files .= ("\t" x $depth) . "</File>\r\n";
  }
  $files .= ("\t" x --$depth) . "</Filter>\r\n" for (@$last);
  $files .= "\t</Files>";
  return $files;
}

sub process_makefiles {
  my ($path, @o) = @_;
  my @all = map {$path.$sep . $_} @o;
  my $var_o = prepare_makefile_variable("OBJECTS", @o);
  (my $var_obj = $var_o) =~ s/\.o\b/.obj/sg;


  # update MSVC project files
  s/\.o/.c/ for @all;
  my $msvc_files = prepare_msvc_files_xml(\@all, qr/NOT_USED_HERE/, ['Debug|Win32', 'Release|Win32', 'Debug|x64', 'Release|x64']);
  for my $m (qw/libtommath_VS2008.vcproj/) {
    my $old = read_file($path.$sep.$m);
    my $new = $old;
    $new =~ s|<Files>.*</Files>|$msvc_files|s;
    if ($old ne $new) {
      write_file($path.$sep.$m, $new);
      warn "changed: $path$sep$m\n";
    }
  }

  # update OBJECTS
  for my $m (qw/ makefile makefile.shared makefile_include.mk makefile.msvc makefile.unix makefile.mingw /) {
    my $old = read_file($path.$sep.$m);
    my $new = $m eq 'makefile.msvc' ? patch_makefile($old, $var_obj)
                                    : patch_makefile($old, $var_o);
    if ($old ne $new) {
      write_file($path.$sep.$m, $new);
      warn "changed: $path$sep$m\n";
    }
  }
}

# TODO: that is not the best way to do it, but a simple one.
sub write_header
{
  my ($path, @entries) = @_;
  my $tcpath;
  my $content = "/* LibTomMath, multiple-precision integer library -- Tom St Denis */\n";
  $content = $content . "/* SPDX-License-Identifier: Unlicense */\n";
  foreach my $entry (@entries) {
    $entry =~ tr/[a-z]/[A-Z]/;
    $entry = '#define BN_' . $entry . "_C\n";
    $content = $content . $entry;
  }
  $tcpath = $path . $sep . 'tommath_class.h';
  write_file($tcpath, $content);
  warn "File $tcpath written\n";
  $tcpath = $path . $sep . 'tommath_superclass.h';
  write_file($tcpath, "");
  warn "File $tcpath emptied\n";
}

#  A stripped down version of "helper.pl"'s "update_dep()"
sub gather_functions
{
  my $path = shift;
  my %depmap;

  if(-d $path) {
    $path = $path . $sep . '*.c';
  }

  foreach my $filename (glob $path) {
    open(my $src, '<', $filename) or die "Can't open source file!\n";
    read $src, my $content, -s $src;
    close $src;

    $filename = basename($filename);
    $filename =~ s/^bn_|\.c$//g;

    $content =~ s{/\*.*?\*/}{}gs;
    my $list = "";
    foreach my $line (split /\n/, $content) {
      # TODO: change to read public functions only when scanning user source.
      while ($line =~ /(fast_)?(s_)?mp\_[a-z_0-9]*(?=\()|(?<=\()mp\_[a-z_0-9]*(?=,)/g) {
        my $a = $&;
        next if $a eq "mp_err";
        if($list eq "") {
          $list = $a;
        }
        else {
          $list = $list . "," . $a;
        }
      }
    }
    $depmap{$filename} = $list;
  }
  return %depmap;
}
# A stripped down version of "helper.pl"'s "draw_func()" changed to put its findings into
# a variable instead of printing them into "callgraph.txt"
sub gather_dependencies
{
   my ($deplist, $depmap, $funcslist) = @_;
   my @funcs = split ',', $funcslist;
   if ($deplist =~ /$funcs[0]/) {
      return $deplist;
   } else {
      $deplist = $deplist . $funcs[0];
   }
   push @dependency_list, $funcs[0];
   shift @funcs;
   my $olddeplist = $deplist;
   foreach my $i (@funcs) {
      $deplist = gather_dependencies($deplist, $depmap, ${$depmap}{$i}) if exists ${$depmap}{$i};
   }
   return $olddeplist;
}

sub start
{
  my ($sd, $td, $no, $cm, $ch) = @_;

  my %depmap;
  my %user_functions;
  my %ff_hash;
  my @functions;
  my @tmp;

  # TODO: checks&balances
   -e $td.$sep."tommath.h" or die "$td.$sep.tommath.h not found, please check path to LibTomMath sources\n";

  %depmap = gather_functions($td);
  %user_functions = gather_functions($sd);

  # we need them as an array, too, for simplicity.
  foreach (sort keys %user_functions) {
    push @functions, split /,/ , $user_functions{$_};
  }
  @functions = uniq(sort @functions);

  # No functions starting with "mp_" other than those in LibTomMath are allowed.
  # For now.
  foreach (@functions) {
    exists $depmap{$_} or die "Function \"$_\" does not exist in LibTomMath.\n";
  }

  # Compute dependencies for the functions the user uses.
  foreach (sort keys %user_functions) {
    gather_dependencies("", \%depmap, $user_functions{$_});
  }
  @dependency_list = uniq(sort @dependency_list);

  # make an even smaller lib by removing non-essential functions (e.g. Karatsuba for multiplication)
  if ($no == 1) {
    foreach (@dependency_list) {
      next if exists $fast_functions{$_};
      push @tmp, $_;
    }
    @dependency_list = @tmp;
    @tmp = ();
  }

  # If we use fast multiplication/division we need the cutoffs, too. They are in bn_cutoffs.c
  if (any {/kara|toom/} @dependency_list) {
    push @dependency_list, "cutoffs";
  }

  # Change makefiles (and MSVC's project file)
  if ($cm == 1) {
    # The makefiles need the names of the objectfiles.
    # It could be done in process_makefiles() itself, of course
    foreach my $entry (@dependency_list) {
      $entry = 'bn_' . $entry . '.o';
      push @tmp, $entry;
    }
    process_makefiles($td, @tmp);
  }
  # [Default] Change the headers "tommath_class.h" and "tommath_superclass.h"
  if ( ($ch == 1) || ( ($cm + $ch) == 0 )) {
    write_header($td, @dependency_list);
  }
  return 1;
}

sub die_usage {
  die <<"EOO";
usage: $0 -s   OR   $0 --source-dir         [./]
       $0 -t   OR   $0 --tommath-dir        [./libtommath]
       $0 -n   OR   $0 --no-optimization
       $0 -m   OR   $0 --change-makefiles
       $0 -c   OR   $0 --change-headers     [default]

The option --source-dir accepts a directory or a single file.

EOO
}

my $source_dir = "";
my $tommath_dir = "libtommath$sep";
my $config_file = "";
my $no_optimizations = 0;
my $change_makefiles = 0;
my $change_headers   = 0;

GetOptions( "s|source-dir=s"        => \$source_dir,
            "t|tommath-dir=s"       => \$tommath_dir,
            "n|no-optimizations"    => \$no_optimizations,
            "m|change-makefiles"    => \$change_makefiles,
            "c|change-headers"      => \$change_headers,
            "h|help"                => \my $help
          ) or die_usage;

my $exit_value = start($source_dir,
                       $tommath_dir,
                       $no_optimizations,
                       $change_makefiles,
                       $change_headers);
exit $exit_value;
