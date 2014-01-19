#! /usr/bin/perl -w
use strict;

my $password;

my $passfile = "/home/nate/mathrel2-private/pg-pwd";

{
    open(my $fh, "<", $passfile)
	or die("$passfile: $!");
    $password = chomp(<$fh>);
    close($fh);
}

print "$password\n";
