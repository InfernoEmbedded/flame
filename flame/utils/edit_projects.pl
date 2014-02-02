#!/usr/bin/perl

use strict;
use English;
use warnings;

use FileHandle;
use File::Find;
use feature 'switch';
use Cwd;

use 5.010;

sub regexOnFile {
	my $filename = $ARG;
	return unless $filename eq '.cproject';

	my $orig = $File::Find::name . '.orig';
#	rename($File::Find::name, $orig) or
#    	die "Coul not rename '$File::Find::name' '$orig': $OS_ERROR";

	my $old = new FileHandle($orig)
	or die "Could not open '$orig': $OS_ERROR";

	my $new = new FileHandle($File::Find::name, 'w')
	or die "Could not open '$File::Find::name': $OS_ERROR";

	while (<$old>) {
		given ($ARG) {
			when (/\/flame\/src\}\/\$\{ConfigName\}/) {
				$ARG =~ s/\/src//;
				warn "found $ARG";
				$new->print ($ARG);
			}
			default {
				$new->print ($ARG);
			}
		} ## end given
	} ## end while (<$old>)
} ## end sub regexOnFile

find(\&regexOnFile, (cwd()));
