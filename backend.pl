#! /usr/bin/perl -w
use strict;
use DBI;

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

while (my @pair = $h->fetchrow_array()) {
    print "Advisor $pair[0], advisee $pair[1]\n";
}

$db-disconnect();

