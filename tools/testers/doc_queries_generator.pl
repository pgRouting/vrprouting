#! /usr/bin/perl -w

=pod
File: aplgorithm testes

Copyright (c) 2013 pgRouting developers

Function contributors:
  	Celia Virginia Vergara Castillo
  	Stephen Woodbridge
  	Vadim Zhukov
	Nagase Ko

Mail:

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
=cut


eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
if 0; #$running_under_some_shell

use strict;
use lib './';
use File::Find ();
use File::Basename;
use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);
$Data::Dumper::Sortkeys = 1;

# for the convenience of &wanted calls, including -eval statements:
use vars qw/*name *dir *prune/;
*name   = *File::Find::name;
*dir    = *File::Find::dir;
*prune  = *File::Find::prune;

# TODO automatically get min requirement from CMakeLists.txt
my $POSGRESQL_MIN_VERSION = '12';
my $DOCUMENTATION = 0;
my $INTERNAL_TESTS = 0;
my $VERBOSE = 0;
my $DRYRUN = 0;
my $DEBUG = 0;
my $DEBUG1 = 0;
my $FORCE = 0;

my $DBNAME = "___vrp___test___";
my $DBUSER;
my $DBHOST;
my $DBPORT;

sub Usage {
    die "Usage: doc_queries_generator.pl -pgver vpg -pgisver vpgis -psql /path/to/psql\n" .
    " --pgver version       - postgresql version\n" .
    " [-d|--dbname] name    - database name (default '$DBNAME')\n" .
    " [-h|--host] host      - postgresql host or socket directory to use\n" .
    " [-p|--port] port      - postgresql port to use\n" .
    " [-U|--username] name  - postgresql user role to use\n" .
    " --postgis version     - postgis version (default: found)\n" .
    " --pgrouting version   - pgrouting version (default: found)\n" .
    " -psql /path/to/psql   - optional path to psql\n" .
    " -v                    - verbose messages for small debuging\n" .
    " -dbg                  - use when CMAKE_BUILD_TYPE = DEBUG\n" .
    " -debug                - verbose messages for debuging(enter twice for more)\n" .
    " -debug1               - DEBUG1 messages (for timing reports)\n" .
    " -ignorenotice         - ignore NOTICE statements when reporting failures\n" .
    " -alg 'dir'            - directory to select which algorithm subdirs to test\n" .
    " -documentation        - ONLY generate documentation examples\n" .
    " -force                - Force tests for unsupported versions >= 9.1 of postgreSQL \n" .
    " --help                - help\n";
}

print "RUNNING: doc_queries_generator.pl " . join(" ", @ARGV) . "\n";

my ($POSGRES_VER, $postgis_ver, $pgrouting_ver, $psql);
my $alg = '';
my @testpath = ("docqueries/");
my @test_directory = ();
my $clean;
my $ignore;


while (my $a = shift @ARGV) {
    if ( $a eq '--pgver') {
        $POSGRES_VER   = shift @ARGV || Usage();
    } elsif ( $a =~ '--dbname | -d') {
        $DBNAME   = shift @ARGV || Usage();
    } elsif ($a eq '--host | -h') {
        $DBHOST = shift @ARGV || Usage();
    } elsif ($a =~ '--port|-p') {
        $DBPORT = shift @ARGV || Usage();
    } elsif ($a =~ '--username|-U') {
        $DBUSER = shift @ARGV || Usage();
    } elsif ($a eq '--postgis') {
        $postgis_ver = shift @ARGV || Usage();
        $postgis_ver = " VERSION '$postgis_ver'";
    } elsif ($a eq '--pgrouting') {
        $pgrouting_ver = shift @ARGV || Usage();
        $pgrouting_ver = " VERSION '$pgrouting_ver'";
    } elsif ($a eq '-alg') {
        $alg = shift @ARGV || Usage();
        @testpath = ("docqueries/$alg");
    } elsif ($a eq '-psql') {
        $psql = shift @ARGV || Usage();
        die "'$psql' is not executable!\n" unless -x $psql;
    } elsif ($a eq '--help') {
        Usage();
    } elsif ($a eq '-ignorenotice') {
        $ignore = 1;;
    } elsif ($a =~ /^-debug1$/i) {
        $DEBUG1 = 1;
    } elsif ($a =~ /^-debug$/i) {
        $DEBUG++;
        $VERBOSE = 1;
    } elsif ($a =~ /^-v/i) {
        $VERBOSE = 1;
    } elsif ($a =~ /^-force/i) {
        $FORCE = 1;
    } elsif ($a =~ /^-doc(umentation)?/i) {
        $DOCUMENTATION = 1;
    } elsif ($a =~ /^-dbg/i) {
        $INTERNAL_TESTS = 1; #directory internalQueryTests is also tested
    } else {
        warn "Error: unknown option '$a'\n";
        Usage();
    }
}

my $connopts = "";
$connopts .= " -U $DBUSER" if defined $DBUSER;
$connopts .= " -h $DBHOST" if defined $DBHOST;
$connopts .= " -p $DBPORT" if defined $DBPORT;

$POSGRES_VER = '' if ! $POSGRES_VER;
$postgis_ver = '' if ! $postgis_ver;
$pgrouting_ver = '' if ! $pgrouting_ver;

mysystem("dropdb --if-exists $connopts $DBNAME");

%main::tests = ();

# SET of configuration file names
my @cfgs = ();
my %stats = (z_pass=>0, z_fail=>0, z_crash=>0);
my $TMP = "/tmp/pgr-test-runner-$$";
my $TMP2 = "/tmp/pgr-test-runner-$$-2";
my $TMP3 = "/tmp/pgr-test-runner-$$-3";

if (! $psql) {
    $psql = findPsql() || die "ERROR: can not find psql, specify it on the command line.\n";
}

my $OS = "$^O";
if (length($psql)) {
    if ($OS =~ /msys/
        || $OS =~ /MSWin/) {
        $psql = "\"$psql\"";
    }
}
print "Operative system found: $OS\n";


# Traverse desired filesystems
# Find the configuration files and put them in order
File::Find::find({wanted => \&want_tests}, @testpath);
@cfgs = sort @cfgs;



exit "Error: no test files found!\n" unless @cfgs;

createTestDB();


# c  one file in cfgs
# print join("\n",@cfgs),"\n";
for my $c (@cfgs) {
    my $found = 0;

    print "test.conf = $c\n" if $VERBOSE;

    # load the config file for the tests
    require $c;

    print Data::Dumper->Dump([\%main::tests],['test']) if $VERBOSE;

    if ($main::tests{any} && !$DOCUMENTATION) {
        push @{$stats{$c}}, run_test($c, $main::tests{any});
        $found++;
    }
    elsif ($main::tests{any}{documentation} && $DOCUMENTATION) {
        push @{$stats{$c}}, run_test($c, $main::tests{any});
        $found++;
    }

    if (! $found) {
        $stats{$c} = "No tests were found for '$POSGRES_VER-$postgis_ver'!";
    }
}


print Data::Dumper->Dump([\%stats], ['stats']);

unlink $TMP;
unlink $TMP2;
unlink $TMP3;

if ($stats{z_crash} > 0 || $stats{z_fail} > 0) {
#if ($stats{z_crash} > 0) {
    exit 1;  # signal we had failures
}

exit 0;      # signal we passed all the tests


# t  contents of array that has keys comment, data and test
sub run_test {
    # c  one configuration file
    my $c = shift;
    # t  array of configuration variables within the configuration file
    my $t = shift;

    my %res = ();

    my $dir = dirname($c);

    # capture the comment
    $res{comment} = $t->{comment} if $t->{comment};


    # refresh the database each time it enters a new directory
    mysystem("$psql $connopts -A -t -q -f tools/testers/sampledata.sql $DBNAME >> $TMP2 2>\&1 ");
    mysystem("$psql $connopts -A -t -q -f tools/testers/vroomdata.sql $DBNAME >> $TMP2 2>\&1 ");
    mysystem("$psql $connopts -A -t -q -f tools/testers/matrix_new_values.sql $DBNAME >> $TMP2 2>\&1 ");

    # process data for the tests (if any)
    for my $data_file (@{$t->{data}}) {
        mysystem("$psql $connopts -A -t -q -f '$dir/$data_file' $DBNAME >> $TMP2 2>\&1 ");
    }

    if ($DOCUMENTATION) {
        for my $test_file_name (@{$t->{documentation}}) {
            process_single_test($test_file_name, $dir, \%res);
            my $cmd = q(perl -pi -e 's/[ \t]+$//');
            $cmd .= " $dir/$test_file_name.result";
            mysystem( $cmd );
        }
    } else {
        for my $test_file_name (@{$t->{tests}}) {
            process_single_test($test_file_name, $dir, \%res)
        }
    }

    return \%res;
}

sub process_single_test{
    my $test_file_name = shift;
    my $dir = shift;
    my $res = shift;

    my $test_file = "$dir/$test_file_name.test.sql";
    my $result_file = "$dir/$test_file_name.result";
    print "Processing test: $test_file";
    my $t0 = [gettimeofday];


    my $level = "NOTICE";
    $level = "WARNING" if $ignore;
    $level = "DEBUG3" if $DEBUG1;


    if ($DOCUMENTATION) {
        open(PSQL, "|$psql $connopts --set='VERBOSITY terse' -e $DBNAME > $result_file 2>\&1 ") || do {
            $res->{"$test_file"} = "FAILED: could not open connection to db : $!";
            $stats{z_fail}++;
            return;
        };
    } else {
        open(PSQL, "|$psql $connopts  --set='VERBOSITY terse' -e $DBNAME > $TMP 2>\&1 ") || do {
            $res->{"$dir/$test_file_name.test.sql"} = "FAILED: could not open connection to db : $!";
            $stats{z_fail}++;
            return;
        };
    }

    # Read the test file
    open(TIN, "$test_file") || do {
        $res->{"$test_file"} = "FAILED: could not open '$test_file' : $!";
        $stats{z_fail}++;
        return;
    };
    my @d = ();
    @d = <TIN>;
    #closes the input file
    close(TIN);


    # Process the test file
    print PSQL "BEGIN;\n";
    print PSQL "SET client_min_messages TO $level;\n";
    print PSQL @d;
    print PSQL "\nROLLBACK;";

    # executes everything
    close(PSQL);

    if ($DOCUMENTATION) {
        print "\trun time: " . tv_interval($t0, [gettimeofday]) . "\n";
        return;
    }

    if (! -f "$result_file") {
        $res->{"$test_file"} = "\nFAILED: result file missing : $!";
        $stats{z_fail}++;
        return;
    }

    my $actual_results = $TMP2;
    my $expected_results = $TMP3;
    if ($ignore) {
        # ignoring NOTICE
        mysystem("grep -v NOTICE         '$TMP' | grep -v '^CONTEXT:' | grep -v '^PL/pgSQL function' | grep -v '^COPY' > $actual_results");
        mysystem("grep -v NOTICE '$result_file' | grep -v '^CONTEXT:' | grep -v '^PL/pgSQL function' | grep -v '^COPY' > $expected_results");
    } elsif ($DEBUG1) {
        # ignore CONTEXT lines
        mysystem("grep -v '^CONTEXT:'         '$TMP' | grep -v '^PL/pgSQL function' | grep -v '^COPY' > $actual_results");
        mysystem("grep -v '^CONTEXT:' '$result_file' | grep -v '^PL/pgSQL function' | grep -v '^COPY' > $expected_results");
    } else {
        mysystem("grep -v '^COPY'         '$TMP' | grep -v 'psql:tools' > $actual_results");
        mysystem("grep -v '^COPY' '$result_file' | grep -v 'psql:tools' > $expected_results");
    }


    # Use diff -w to ignore white space differences like \r vs \r\n
    my $diff = `diff -w '$expected_results' '$actual_results' `;

    #removing leading blanks and trailing blanks
    $diff =~ s/^\s*|\s*$//g;

    if ($diff =~ /connection to server was lost/) {
        $res->{"$test_file"} = "CRASHED SERVER: $diff";
        $stats{z_crash}++;
        # allow the server some time to recover from the crash
        warn "CRASHED SERVER: '$test_file', sleeping 20 sec ...\n";
        sleep 20;
    }

    print "\ttest run time: " . tv_interval($t0, [gettimeofday]);

    #if the diff has 0 length then everything was the same, so here we detect changes
    if (length($diff)) {
        $res->{"$test_file"} = "FAILED: $diff";
        $stats{z_fail}++ unless $DEBUG1;
        print "\tFAIL\n";
    } else {
        $res->{"$test_file"} = "PASS";
        $stats{z_pass}++;
        print "\tPASS\n";
    }
}

sub createTestDB {
    my $template;
    my $dbver = getServerVersion();
    my $dbshare = getSharePath($dbver);

    if ($DEBUG) {
        print "-- DBVERSION: $dbver\n";
        print "-- DBSHARE: $dbshare\n";
    }

    die "
    Unsupported postgreSQL version $dbver
    Minimum requierment is $POSGRESQL_MIN_VERSION version
    Use -force to force the tests\n"
    unless version_greater_eq($dbver, $POSGRESQL_MIN_VERSION) or ($FORCE);

    mysystem("createdb $connopts $DBNAME") if not dbExists();
    die "ERROR: could not create database'$DBNAME'!\n" unless dbExists();


    #TODO put as parameter
    my $encoding = "SET client_encoding TO 'UTF8';";
    
    mysystem("$psql $connopts -c \"$encoding CREATE EXTENSION IF NOT EXISTS plpython3u \" $DBNAME");
    mysystem("$psql $connopts -c \"$encoding CREATE EXTENSION IF NOT EXISTS postgis $postgis_ver \" $DBNAME");
    mysystem("$psql $connopts -c \"$encoding CREATE EXTENSION IF NOT EXISTS pgrouting $pgrouting_ver \" $DBNAME");
    mysystem("$psql $connopts -c \"$encoding DROP EXTENSION IF EXISTS vrprouting CASCADE\"  $DBNAME");
    mysystem("$psql $connopts -c \"$encoding CREATE EXTENSION vrprouting\"  $DBNAME");

    # print database information
    print `$psql $connopts -c "select version();" postgres `, "\n";
    print `$psql $connopts -c "select postgis_full_version();" $DBNAME `, "\n";
    print `$psql $connopts -c "select vrp_full_version();" $DBNAME `, "\n";
}

sub version_greater_eq {
    my ($a, $b) = @_;

    return 0 if !$a || !$b;

    my @a = split(/\./, $a);
    my @b = split(/\./, $b);

    my $va = 0;
    my $vb = 0;

    while (@a || @b) {
        $a = shift @a || 0;
        $b = shift @b || 0;
        $va = $va*1000+$a;
        $vb = $vb*1000+$b;
    }

    return 0 if $va < $vb;
    return 1;
}


sub getServerVersion {
    my $v = `$psql $connopts -q -t -c "select version()" postgres`;
    print "$psql $connopts -q -t -c \"select version()\" postgres\n    # RETURNED: $v\n" if $VERBOSE;
    if ($v =~ m/PostgreSQL (\d+(\.\d+)?)/) {
        my $version = $1 + 0;
        print "    Got: $version\n" if $VERBOSE;
        $version = int($version) if $version >= 12;
        print "    Got: $version\n" if $VERBOSE;
        return $version;
    }
    return undef;
}

sub dbExists {
    my $isdb = `$psql $connopts -l | grep ' $DBNAME '`;
    $isdb =~ s/^\s*|\s*$//g;
    return length($isdb);
}

sub findPsql {
    my $psql = `which psql`;
    $psql =~ s/^\s*|\s*$//g;
    print "which psql = $psql\n" if $VERBOSE;
    return length($psql)?$psql:undef;
}

# getSharePath is complicated by the fact that on Debian we can have multiple
# versions installed in a cluster. So we get the DB version by connectiong
# to the port for the server we want. Then we get the share path for the
# newest version od pg installed on the cluster. And finally we change the
# in the path to the version of the server.

sub getSharePath {
    my $pg = shift;

    my $share;
    my $isdebian = -e "/etc/debian_version";
    my $pg_config = `which pg_config`;
    $pg_config =~ s/^\s*|\s*$//g;
    print "which pg_config = $pg_config\n" if $VERBOSE;
    if (length($pg_config)) {
        $share = `"$pg_config" --sharedir`;
        $share =~ s/^\s*|\s*$//g;
        if ($isdebian) {
            $share =~ s/(\d+(\.\d+)?)$/$pg/;
            if (length($share) && -d $share) {
                return $share;
            }
        } else {
            return "$share"
        }
    }
    die "Could not determine the postgresql version" unless $pg;
    $pg =~ s/^(\d+(\.\d+)).*$/$1/;
    $share = "/usr/share/postgresql/$pg";
    return $share if -d $share;
    $share = "/usr/local/share/postgresql/$pg";
    return $share if -d $share;
    die "Could not determine the postgresql share dir for ($pg)!\n";
}

sub mysystem {
    my $cmd = shift;
    print "$cmd\n" if $VERBOSE || $DRYRUN;
    system($cmd) unless $DRYRUN;
}

sub want_tests {
    /^test\.conf\z/s && push @cfgs, $name;
}
