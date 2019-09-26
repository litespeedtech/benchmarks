#!/usr/bin/env perl
#
# bench.pl: benchmark performance of various Huffman encoders by timing
# execution of the comp-enc driver.

use strict;
use warnings;

use Getopt::Long;
use Time::HiRes qw(time);

use constant SHORTEST => 5;
my $verbose;

sub time_run {
    my ($driver, $input, $count, $impl) = @_;
    my $begin = time;
    system($driver, $input, $count, $impl)
        and die "error running $driver $input $count $impl";
    my $end = time;
    $verbose and print STDERR "$driver $input $count $impl ran in ",
                                            $end - $begin, " seconds\n";
    return $end - $begin;
}

sub calibrate {
    my ($driver, $input, @impls) = @_;
    my $count = 10000;
    do {
        for my $impl (@impls) {
            my $time = time_run($driver, $input, $count, $impl);
            if ($time < SHORTEST)
            {
                goto incr;
            }
        }
        $verbose and print STDERR "$input calibrated to $count iterations\n";
        return $count;
  incr: $count *= 2;
    } while ($count < 100_000_000_000);
    die "count is too long";
}

sub median {
    my @times = sort { $a <=> $b } @_;
    my $nelem = @times;
    if ($nelem & 1) {
        return $times[$nelem/2];
    } else {
        return ($times[$nelem/2] + $times[$nelem/2+1]) / 2;
    }
}

my $num_runs = 5;

GetOptions(
    "impl=s" => \my @impls,
    "input=s" => \my @inputs,
    "runs=i" => \$num_runs,
    "verbose" => \$verbose,
    "help" => sub {
        print <<USAGE;
Usage: $0 -impl name -input input.txt >table.txt

    -impl name      Name of implementation to time.  This value is passed
                      as argument to the driver.  Can be specified more
                      than once.

    -input file     Input file.  Can be specified more than once.

    -runs number    Number of runs for each input/impl combo.  Defaults to $num_runs.

    -verbose        Print messages to stderr.
USAGE
        exit;
    },
);

my $driver = './comp-enc';
-x $driver or die "$driver is not executable";

print join "\t", "input", "# iter", @impls;
print "\n";

for my $input (@inputs) {
    $verbose and print STDERR "calibrating $input\n";
    my $count = calibrate($driver, $input, @impls);
    print "$input\t$count";
    for my $impl (@impls) {
        my @times;
        for (my $i = 0; $i < $num_runs; ++$i) {
            push @times, time_run($driver, $input, $count, $impl);
        }
        printf "\t%.2f", median(@times);
    }
    print "\n";
}
