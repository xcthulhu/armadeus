#!/usr/bin/perl -w
#* ****************************************************************************
#*                                                                            *
#* add_device_config_entry.pl                                                 *
#* Description                                                                *
#*   During the creation of the Armadeus buildroot variant, registers Armadeus*
#* packages to the original Buildroot build system                            *
#* Function                                                                   *
#*   Patches packages/Config.in file, registers packages in file              *
#*****************************************************************************/
#* ****************************************************************************
#*                                                                            *
#* Started on: Mon, 09 Jul 2007 18:27:16 +0200                                *
#* bistromath 2.6.20-16-generic #2 SMP Thu Jun 7 20:19:32 UTC 2007            *
#* Revisions:                                                                 *
#*                                                                            *
#*****************************************************************************/
#******************************************************************************
#*   This program is free software; you can redistribute it and/or modify     *
#*   it under the terms of the GNU General Public License as published by     *
#*   the Free Software Foundation; either version 2 of the License, or        *
#*   (at your option) any later version.                                      *
#******************************************************************************
#
#******************************************************************************
#*  Contributors:                                                             *
#* (c) 2007,2009 Jean-Baptiste Mayer (jibee@jibee.com) (initial work)         *
#*  Name/Pseudo <email>                                                       *
#******************************************************************************

use strict;
use warnings;

use Carp;
use IO::File;


my $buildroot_dir;
my $log_file;

parse_commandline(@ARGV);

if(not defined($buildroot_dir))
{
	showhelp();
	croak "I don't know about buildroot_dir";
}

my $source_file="$buildroot_dir/package/Config.in";

# Parse Config.in for package declarations. Resulting packages are stored in 
# existing_packages hash
my %existing_packages;
# gnuconfig package is declared at root level - we hence include this file in 
# the search 
read_config_in($buildroot_dir."/Config.in", \%existing_packages);
read_config_in($source_file, \%existing_packages);

# Search source tree for packages
my %packages=detect_packages($buildroot_dir);

# Bogus packages are packages that are declared in the Config.in files but do 
# not exist in the source tree (because we deleted them probably)
my %bogus_packages = map { $_ => $_ } grep { not defined $packages{$_} } keys %existing_packages;
if(%bogus_packages)
{
	my_log("The following packages could not be recognized:");
	my_log("Bogus: ".$_) foreach keys %bogus_packages;
}

# New packages are packages found in the source tree but not declared in 
# Config.in
my %new_packages = map { $_ => $packages{$_} } grep { not defined $existing_packages{$_} } keys %packages;
if(%new_packages)
{
	my_log("The following packages will be recognized:");
	my_log("Adding: ".$_) foreach keys %new_packages;
}

update_config_in();


sub detect_packages
{
	my ($buildroot_dir)=@_;
	my %packages;
	my @packages=`find $buildroot_dir/package -name Config.in`;
	foreach my $line (@packages)
	{
		if($line =~ /^$buildroot_dir\/package\/([^\/]+)\/Config\.in$/x)
		{
			my $name=$1;
			$packages{$name}=$line;
			my_log("Found package ".$name." ".$line);
		}
		else
		{
			my_log("Could not parse ".$line);
		}
	}
	return %packages;
}

sub update_config_in
{
	my $backup_file="$buildroot_dir/package/Config.in.bak";
	system "mv", $source_file, $backup_file;
	my $source=new IO::File($backup_file, "r");
	my $destination=new IO::File($source_file, "w");

	my $has_matched=0;

	while(my $line=<$source>)
	{
		if($line  =~ /^comment\ "Other\ stuff"$/x)
		{
			printPackages($destination, \%new_packages);
			$has_matched=1;
		}
		if($line =~ /source\ "?(([^\/]+\/)*)package\/([^\/]+)\/Config\.in"?/x)
		{
			my $name=$3;
			if($bogus_packages{$name})
			{
				my_log("removing bogus package ".$name);
			}
			else
			{
				print $destination $line;
			}
		}
		else 
		{
			print $destination $line;
		}
	}

	printPackages($destination, \%new_packages) unless $has_matched;
	return;
}
sub printPackages
{
	my ($destination, $new_packages)=@_;
	print $destination "comment \"Armadeus specific packages\"\n";
	foreach my $key (keys %$new_packages)
	{
		print $destination "source \"package/$key/Config.in\"\n";
	}
	return;
}

sub showhelp
{
	print("usage:\n");
	print("add_device_config_entry buildroot=BUILDROOT_DIR\n");
	return;
}

sub parse_commandline
{
	my @argv=@_;
	my $want_log;
	foreach my $l (@argv)
	{
		if($l=~/([^=]*)=(.*)/x)
		{
			$buildroot_dir=$2 if $1 eq "buildroot";
			$want_log=$2 if $1 eq "wantlog"
		}
	}

	if($want_log)
	{
		$log_file=new IO::File($want_log, "w");
	}
	return;
}

sub my_log
{
	my (@elements)=@_;
	if(defined($log_file))
	{
		foreach my $line (@elements)
		{
			print $log_file $line."\n";
		}
	}
	return;
}

sub read_config_in
{
	my ($source_file, $packages)=@_;
	my_log("Parsing source file $source_file");
	my $file=new IO::File($source_file, "r");

	while(my $line=<$file>)
	{
		if($line =~ /source\ "?(([^\/]+\/)*)package\/([^\/]+)\/Config\.in"?/x)
		{
			my $name=$3;
			my_log("Found package declaration '".$name."'");
			$packages->{$name}=$line;
		}
	}
	close $file;
	return;
}

