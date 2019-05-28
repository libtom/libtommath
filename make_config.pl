#!/usr/bin/env perl

use strict;
use warnings;

use File::Glob 'bsd_glob';

#original
sub read_file {
  my $f = shift;
  open my $fh, "<", $f or die "FATAL: read_rawfile() cannot open file '$f': $!";
  binmode $fh;
  return do { local $/; <$fh> };
}
#original
sub write_file {
  my ($f, $data) = @_;
  die "FATAL: write_file() no data" unless defined $data;
  open my $fh, ">", $f or die "FATAL: write_file() cannot open file '$f': $!";
  binmode $fh;
  print $fh $data or die "FATAL: write_file() cannot write to '$f': $!";
  close $fh or die "FATAL: write_file() cannot close '$f': $!";
  return;
}
#original
sub patch_file {
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
#original
sub prepare_variable {
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

#changed from original
sub process_makefiles {
  my (@o) = @_;


  my $var_o = prepare_variable("OBJECTS", @o);
  (my $var_obj = $var_o) =~ s/\.o\b/.obj/sg;

  # TODO: update MSVC project files

  # update OBJECTS
  for my $m (qw/ makefile makefile.shared makefile_include.mk makefile.msvc makefile.unix makefile.mingw /) {
    my $old = read_file($m);
    my $new = $m eq 'makefile.msvc' ? patch_file($old, $var_obj)
                                    : patch_file($old, $var_o);
    if ($old ne $new) {
      write_file($m, $new);
      warn "changed: $m\n";
    }
  }
}

# changed from original but mainly just stripped down to what's needed
sub gather_dependencies
{
  my %depmap;
  foreach my $filename (glob 'bn*.c') {
    open(my $src, '<', $filename) or die "Can't open source file!\n";
    read $src, my $content, -s $src;
    close $src;

    $content =~ s{/\*.*?\*/}{}gs;
    foreach my $line (split /\n/, $content) {
      while ($line =~ /(fast_)?(s_)?mp\_[a-z_0-9]*(?=\()|(?<=\()mp\_[a-z_0-9]*(?=,)/g) {
        my $a = $&;
        next if $a eq "mp_err";
        $a = 'bn_' . $a . '.o';
        push @{$depmap{$filename}}, $a;
      }
    }
  }
  return %depmap;
}

#new
sub uniq {
    my %seen;
    grep !$seen{$_}++, @_;
}

#new
sub make_it_so{
  # TODO: get rid of hardcoded paths
  my $config = "make_config.conf";
  open(my $src, '<', $config) or die "Can't open config!\n";
  read $src, my $content, -s $src;
  close $src;

  my %depmap = gather_dependencies();
  my @all_deps;

  # read line by line
  foreach my $line (split /\n/, $content) {
    chomp $line;
    next if $line =~ /^#/;
    $line = 'bn_' . $line . '.c';
    #check %depmap for the dependencies of the entry in that line
    exists $depmap{$line} or die "\"$line\" does not exist.\n";
    #add those dependencies to the list of all dependecies
    push @all_deps, @{$depmap{$line}};
  }
  # unique the list of all dependencies
  my @uniq = uniq(@all_deps);
  # write the makefiles
  process_makefiles(@uniq);
}


make_it_so();





























