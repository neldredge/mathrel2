#! /usr/bin/perl -w
use strict;
use DBI;
use Data::Dumper;
use List::Util qw(min max);

my $password;

my $passfile = "/home/nate/mathrel2-private/pg-pwd";
my $database = "geneal";
my $host = "localhost";
my $dbuser = "mathrel";

{
    open(my $fh, "<", $passfile)
	or die("$passfile: $!");
    $password = <$fh>;
    chomp($password);
    close($fh);
}

my $db = DBI->connect("dbi:Pg:dbname=$database;host=localhost", $dbuser, $password)
    or die("DBI->connect: $!");

my $h = $db->prepare("SELECT advisor,advisee FROM advises")
    or die("prepare");

$h->execute() or die("execute");

my (%advisors, %advisees);
my %advise;

my $rows = 0;

my $biggest = 0;
my $smallest = 9999999;

while (my ($advisor, $advisee) = $h->fetchrow_array()) {
    $rows++;
    $biggest = max($biggest, $advisor, $advisee);
    $smallest = min($smallest, $advisor, $advisee);
    push(@{$advisors{$advisee}}, $advisor);
    push(@{$advisees{$advisor}}, $advisee);
}

print "$rows rows\nbiggest $biggest\nsmallest $smallest\n";

$db->disconnect();

