#! /usr/bin/perl -w
use strict;

my $password;

my $passfile = "/home/nate/mathrel2-private/pg-pwd";

{
    open(my $fh, "<", $passfile)
	or die("$passfile: $!");
    $password = <$fh>;
    chomp($password);
    close($fh);
}

print "$password\n";
