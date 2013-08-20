#! /usr/bin/perl -w
#
# tme - a TeX macro expander
# @(#) tme 0.5.6 28/Oct/1996 jonathan
#

my $usage_msg = <<'EOF';
Usage:
	tme  [ -c ]  [ -D | -Dn ]  [ macros.tex ... ]  <input.tex  >output.tex
where:
	-c		expand macros in comments
			(default is to transparently copy them to the output)
	-D | -Dn	enable tme internal debugging printouts
			(very ugly, you don't want to try this)
	macros.tex ...	tme reads macro definitions from these files (only)
			(anything other than macro definitions is ignored)
	input.tex	tme reads TeX to macro-expand from standard input
	output.tex	tme writes the macro-expanded TeX to standard output
EOF

#
# tme is a program to expand user-written TeX macros, while leaving
# built-in macros alone.  Why would anyone want such a critter?  Well,
# many academic journals now accept papers for publication in TeX,
# generally LaTeX using the journal's house style files.  Unfortunately,
# the journals usually forbid user-defined macros :=(.  I'm not willing
# to write my papers without defining any macros for myself, so I need
# a way to automatically expand my macros, while leaving intact TeX's
# built-in macros and those defined in the journal's style files.  tme
# does exactly this.
#
# tme is written in perl, and requires perl version 5.002 or later
# to run.  (It could be brought down to 5.000 with minor kludges,
# but not to 4.* without major (and very unpleasant) surgery.)
#

#
# tme has only a limited understanding of TeX.  It handles macro
# definitions and uses such as
#
#	% cat macros.tex
#	% macro definitions
# 	\def\h{{\sf h}}
# 	\def\H{{\sf H}}
# 	\def\Jac[#1]{{\bf J} \Bigl[ #1 \Bigr]}
#	\newcommand{\papertitle}[1]{{\it #1\/}}
#	\renewcommand{\thesistitle}[1]{{\it #1\/}}
#
#	% cat input.tex
#	% text to expand
# 	$\Jac[\H(\h)]$
#	\bibitem{Choptuik-91}%%%
#	M.~W.~Choptuik,
#	% \papertitle{Consistency of Finite-Difference Solutions
#	%	      of Einstein's Equations},
#	Phys.~Rev.~D {\bf 44}(10), 3124--3135 (1991).
#
# Notice that:
# - Nested macros are ok, i.e. macros may reference other macros.
# - Both TeX's native \def and LaTex's \newcommand/\renewcommand are
#   supported.
# - For \def, "delimited" macro parameters (eg for \Jac above) are
#   supported, as per pp. 202-204 of the TeXbook.
# - Macro arguments can span multiple lines of text.  (This is hard
#   to do if you try to fake tme with editor scripts.)
#
# For the example just given, the output of tme is
#
#	% tme macros.tex <input.tex
#	% text to expand
#	${\bf J} \Bigl[ {\sf H}({\sf h}) \Bigr]$
#	\bibitem{Choptuik-91}%%%
#	M.~W.~Choptuik,
#	% \papertitle{Consistency of Finite-Difference Solutions
#	%	      of Einstein's Equations},
#	Phys.~Rev.~D {\bf 44}(10), 3124--3135 (1991).
#
# or if macros should be expanded in comments (see below),
#
#	% tme -c macros.tex <input.tex
#	% text to expand
#	${\bf J} \Bigl[ {\sf H}({\sf h}) \Bigr]$
#	\bibitem{Choptuik-91}%%%
#	M.~W.~Choptuik,
#	% {\it Consistency of Finite-Difference Solutions
#	%	      of Einstein's Equations\/},
#	Phys.~Rev.~D {\bf 44}(10), 3124--3135 (1991).
#
# Among (many) other things, tme does _not_ know about \catcode, any
# of TeX's variant macro-defining commands \edef, or \xdef, or the \def
# modifiers \long or \outer.  tme recognizes \gdef, but treats it as
# identical to \def.  More precisely, tme treats _all_ macros as global.
# tme makes no attempt to handle conditional macros.  tme allows only
# letter sequences or single non-letters as macro names.
#
# tme discards TeX comments (and anything else except macro definitions)
# in macro definition files.  When reading text from standard input to
# be macro-expanded, tme by default transparently copies TeX comments
# to standard output, but if the  -c  option is specified, tme instead
# expands macros in comments here as well as elsewhere in the input text.
#
# tme doesn't (yet) know about LaTex's \newenvironment/\renewenvironment.
# I might add this feature to tme in the future.
#
# Right now tme doesn't interpret \input in any way.  The "texexpand"
# program can be used to expand \input files if desired.  I might add
# this feature to tme in the future.
#
# tme's output may contain very long lines.  Right now tme doesn't do
# anything special about this, but I might add an output-line-splitting
# option sometime in the future.  (Or I might write a separate program
# to do it.)
#
# Comments, suggestions, improvements, bug fixes, etc, are always welcome.
#

#
# Bugs (user-level):
# - tme is painfully slow.  On my computer it takes $\sim 5$ times as
#   long to process a file as LaTeX does to _typeset_ it!  I wrote it
#   without any particular concern for efficiency: there are lots of
#   perl array and hash-table copies floating around, and a number of
#   functions use append-to-scratch-array quadratic algorithms.  But
#   it's still vastly faster than hacking the macro expansion in a
#   text editor...
# - Error checking?  If you have to ask, tme doesn't have enough for you!
#   tme is easily confused by TeX syntax errors.  Don't try running your
#   text through tme until TeX is reasonably happy with it (no fatal
#   errors at least).
# - tme's handling of blanks in the input is only an approximation to
#   TeX's.  In particular, there may be problems with macro definitions
#   where blanks separate \def, the macro name, the macro parameters,
#   and/or the macro expansion.  Basically, tme ignores blanks following
#   the \def and the macro name token, but not elsewhere.  The simple case
#   (with no macro arguments)
#	\def \foo {blah blah blah}
#   should thus be ok: tme ignores the blanks, which matches what TeX
#   seems to do, though not what a naive reading of the TeXbook would
#   suggest.  (I think TeX ignores the blanks when tokenizing the input;
#   the TeXbook's description of input tokenizing on pp. 46-47 doesn't
#   discuss the handling of macro definitions.)
#   The cases
#	\def \foo#1 {blah #1 blah}
#	\def \foo #1 {blah #1 blah}
#   are accepted, and I believe tme handles them correctly, but note that
#   "correctly" here involves quite different semantics from the previous
#   cases: here the blank following the #1 is _not_ ignored, rather it
#   changes the semantics of how the macro argument is delimited, as
#   discussed in the TeXbook (pp. 203-204).  For macros defined with
#   \newcommand or \renewcommand, tme ignores the blanks in any of
#	\newcommand \foo {blah blah}
#	\renewcommand \foo {blah blah}
#	\newcommand \foo [1] {blah #1 blah}
#	\renewcommand \foo [1] {blah #1 blah}
# - tme doesn't do anything special for macro definitions appearing in
#   text to be expanded.  This means that if a macro \foo has been defined,
#   the text-to-be-expanded
#	\def\foo{new definition}
#   won't work properly.  I could fix this if someone cares about it...
# - Please note the disclaimers of warranty in the (GNU public license)
#   copyright statement below.  tme works well enough to process my own
#   papers, but if it mangles your life's masterpiece and costs you the
#   Nobel prize, Big Contract, Thesis, and/or Job, don't sue me.
# - Seriously, before submitting tme output for publication I suggest
#   you check that it's semantically identical to the tme input.  The
#   easy way to do this is to just TeX/LaTeX both files and compare
#   the dvi files with 'cmp -l'.  (You could also dvips and compare
#   the postscript files.)  Except for the dvi timestamps, the files
#   should be byte-for-byte identical.  If they're not, please report
#   this to me as a tme bug.
#
# Bugs (implementation):
# - I did _try_ to use the Getopt library routines to parse the argument
#   list, but they don't seem to be able to handle switches which _don't_
#   allow a space between the switch and the (optional) argument.  Sigh...
# - There's a fair bit of duplicated code between  define_macro_via_def()
#   and  define_macro_via_newcommand() .
#

###############################################################################

#
# Copyright (C) 1996, Jonathan Thornburg <thornbur@theory.physics.ubc.ca>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.
#

###############################################################################

#
# <<<user documentation>>>
# <<<copyright>>>
# <<<this table of contents>>>
# <<<overall design notes>>>
# <<<design notes for data types>>>
# main - driver
#
# <<<global data for the macro engine>>>
# is_macro - is a given token defined as a macro?
# define_macro_via_def - define a macro via \def
# define_macro_via_newcommand - define a macro via \newcommand or \renewcommand
# get_macro_args - get a macro's arguments
# expand_macro - expand a macro
#
# match_tokens - match input stream against token list
# get_tokens_to_delimiter - get tokens until (and including) delimiter
# get_token_group - get a {}-balanced token group
# skip_blank_tokens - skip blank (actully any whitespace) tokens
#
# subarray_eq - are two subarrays element-by-element eq?
#
# <<<design notes on comment handling>>
# <<<global data for the input routines>>>
# tme_die - die() with input file/line message
# set_comment_handling - set the comment-handling policy for the input routines
# open_input_file - open an input file (also updates input-comment handling)
# close_input_file - close an input file
# push_tokens - push back a token or tokens into the input stream
# get_token - get the next (non-comment) token from the input stream
# get_bare_token - get the next bare-token from the input stream
#
# put_token - output a token
#

###############################################################################

#
# <<<overall design notes>>>
#

#
# TeX uses a 2-stage processing scheme, where input characters are
# first tokenized in TeX's "mouth", then macros are expanded on the
# token stream in TeX's "stomach".  tme follows a similar design,
# since otherwise we'd have prefix problems, eg macros \e and \end
# would be confused.
#

#
# The tme macro engine operates by reading the input token stream,
# and copying it to the output, except that when a previously defined
# macro is seen, the macro is expanded and the expansion is "pushed back"
# FIFO-fashion into the input stream, since it might itself contain
# further macros.
#
# Some examples may clarify this:
#
#
# First, consider a simple case:
#
# macro defn:	\def\foo{123}
# input:		\a\foo\b
#
# unread input		read tokens	output tokens	action
# ------------		-----------	-------------	------
# \a\foo\b
# \foo\b			\a			read token
# 					\a		copy token to output
# \b			\foo				read token
# 123\b							push macro expansion
# 							   back into input
# 			123\b				read tokens
# 					123\b		copy tokens to output
#
#
# Next, consider a nested macro:
#
# macro defns:	\def\h{{\sf h}}
# 		\def\H{{\sf H}}
# 		\def\Jac[#1]{{\sf J} \Bigl[ #1 \Bigr]}
# input:		$\Jac[\H(\h)]$
#
# unread input		read tokens	output tokens	action
# ------------		-----------	-------------	------
# $\Jac[\H(\h)]$
# \Jac[\H(\h)]$		$				read token
# 					$		copy token to output
# [\H(\h)]$		\Jac				read token
# \H(\h)]$		[				read macro delimiter
# 							   token list
# ]$			\H(\h)				read macro arg tokens
# $			]				read macro delim token
# {\sf J} \Bigl[ \H(\h) \Bigr]$				push macro expansion
# 							   back into input
# \H(\h) \Bigr]$		{\sf J} \Bigl[ 			read tokens
# 					{\sf J} \Bigl[	copy tokens to output
# (\h) \Bigr]$		\H				read token
# {\sf H}(\h) \Bigr]$					push macro expansion
# 							   back into input
# \h) \Bigr]$		{\sf H}(			read tokens
# 					{\sf H}(	copy tokens to output
# ) \Bigr]$		\h				read token
# {\sf h}) \Bigr]$					push macro expansion
# 							   back into input
# 			{\sf h}) \Bigr]$		read tokens
# 					{\sf h}) \Bigr]$ copy tokens to output
#
# Notice how pushed-back input is pushed back so as to be read *before*
# the unread input.
#
#
# Finally, consider a simple macro, invoked with blanks (denoted here by _)
# separating the parameters:
#
# macro defn:	\def\foo#1{#1#1}
# input:		\a\foo_\x\b
# unread input		read tokens	output tokens	action
# ------------		-----------	-------------	------
# \a\foo_\x\b
# \foo_\x\b		\a				read token
# \foo_\x\b				\a		copy token to output
# _\x\b			\foo				read token
# \x\b			_				skip over blank token
# \b			\x				read macro arg token
# \x\x\b						push macro expansion
# 							   back into input
# 			\x\x\b				read tokens
# 					\x\x\b		copy tokens to output
#
# Notice how the blank between \foo and \x was skipped over (discarded)
# in parsing the macro arguments.
#
# Actually, this last example doesn't quite work the way we've just
# described it.  We implement "skipping over blank tokens" by reading
# until a non-blank token is found, then pushing that token back into
# the input, so the example really works like this:
#
# macro defn:	\def\foo#1{#1#1}
# input:		\a\foo_\x\b
# unread input		read tokens	output tokens	action
# ------------		-----------	-------------	------
# \a\foo_\x\b
# \foo_\x\b		\a				read token
# \foo_\x\b				\a		copy token to output
# _\x\b			\foo				read token
# \x\b			_				read token,
# 							   see that it's blank,
# 							   discard it
# \b			\x				read token
# \x\b							push token
# 							   back into input
# \b			\x				read macro arg token
# \x\x\b						push macro expansion
# 							   back into input
# 			\x\x\b				read tokens
# 					\x\x\b		copy tokens to output
#

###############################################################################

#
# <<<design notes for data types>>>
#

#
# tme uses the following main data types:
# - A "bare-token" is represented as a (scalar) nonempty perl character
#   string, the empty string representing EOF.  We don't bother with
#   TeX's separate character and category codes; we compare tokens as
#   character strings only.  A bare-token may be a TeX comment.
# - A "token" (with no qualifiers) is a bare-token, except that comments
#   may have been screened out.  See the input-routines comments for
#   details on how comments are handled.
# - A "token list" is represented as a (possibly empty) perl array of
#   tokens.
# - A "token group" is a {}-balanced token list.
#

###############################################################################

#
# This is the main perl program.
#

require 5.002;				# needed for FileHandle module

use strict;
use integer;				# slight efficiency gain
use English;
use FileHandle;

my $file;
my $token;

#
# parse args
# ... alas, the Getopt library won't do the job, sigh
#
my $debug = 0;
my $expand_macros_in_comments = 0;
	while ((scalar(@ARGV) > 0) && ($ARGV[0] =~ /^-/))
	{
	if ($ARGV[0] =~ /^-D([0-9]*)$/)
		{ $debug = ($1 eq '') ? 1 : $1; }
	elsif ($ARGV[0] eq '-c')
		{ $expand_macros_in_comments = 1; }
	else	{
		print($usage_msg);
		exit(1);
		}

	shift(@ARGV);
	}

#
# read the macro file(s), noting any macro definitions
# (and silently ignoring anything else)
#
if (! scalar(@ARGV))
	{
	print STDERR (
"tme: warning: no macro files, we'll just (slowly) copy stdin to stdout\n"
		     );
	}

set_comment_handling('define');
	foreach $file (@ARGV)
	{
	open_input_file($file);
		while (($token = get_token()) ne '')
		{
		if (($token eq '\def') || ($token eq '\gdef'))
			{ define_macro_via_def(); }
		elsif ( ($token eq '\newcommand')
			|| ($token eq '\renewcommand') )
			{ define_macro_via_newcommand(); }
		}
	close_input_file();
	}

#
# copy stdin to stdout, expanding macros as we go
#
open_input_file('-');
set_comment_handling('expand');
	while (($token = get_token()) ne '')
	{
	if (is_macro($token))
		{
		my %args = get_macro_args($token);
		expand_macro($token, {%args});
		}
	   else { put_token($token); }
	}

###############################################################################
###############################################################################
###############################################################################

#
# <<<global data for the macro engine>>>
# (uses effectively "static" storage allocation)
#

#
# A macro definition is taken to have one of the (equivalent) forms
# (TeXbook p.203-204)
#	\def _ <name token> _ <parameter token list> { <expansion token list> }
#	\gdef _ <name token> _ <parameter token list> { <expansion token list> }
#	\newcommand _ { <name token> } _ { <expansion token list> }
#	\newcommand _ { <name token> } _ [N] _ { <expansion token list> }
#	\renewcommand _ { <name token> } _ { <expansion token list> }
#	\renewcommand _ { <name token> } _ [N] _ { <expansion token list> }
# where in each case _ denotes optional whitespace (which will be ignored).
#
# Regardless of which form of definition is used, a macro is stored as
# the following data structures:
#	$macro_N_pars{$macro_name} = number of parameters
#	$macro_par_delim{$macro_name} = { <parameter delimiter hash> }
#	$macro_defn{$macro_name} = [ <expansion token list> ]
# where { <parameter delimiter hash> } is a hash:
# - keys are '#0', '#1', '#2', ...
# - values are references to (possibly empty) token lists of the
#   "parameter delimiter" tokens following the keys, with the '#0'
#   case being the (possibly empty) list of nonblank tokens (blank
#   tokens being ignored) immediately following the macro name and
#   preceding the #1 token (if any) or the { token.
#
# For example, the macro definition
#	\def\Jac[#1]{{\bf J} \Bigl[ #1 \Bigr]}
# would be stored as the data structures
#	$macro_N_pars{'\Jac'} = 1
#	$macro_par_delim{'\Jac'} = {
#				   '#0' => [ '[' ],
#				   '#1' => [ ']' ],
#				   }
#	$macro_defn{'\Jac'} = [
#			      '{', '\bf', ' ', 'J', '}', ' ',
#			      '\Bigl', '[', ' ', '#1', ' ', '\Bigr', ']'
#			      ]
#
sub BEGIN
{
my %macro_N_pars = {};
my %macro_par_delim = {};
my %macro_defn = {};

###############################################################################

#
# This function tests whether or not a given token has been defined as
# a macro.
#
# Arguments:
# $token = (in) The token to be tested.
#
# Results:
# The function returns 1 if the token has been defined, 0 otherwise.
#
sub is_macro
{
my ($token) = @_;

return(
    exists($macro_defn{$token}) ? 1 : 0
      );
}

###############################################################################

#
# This subroutine defines a macro via \def (or \gdef), i.e. it gets a
# \def macro definition from the input stream and records it for later
# use.  We assume that the \def token has already been seen.
#
sub define_macro_via_def()
{
my $token;

#
# get macro name
#
skip_blank_tokens();
my $name = get_token();
if ($debug > 0)
	{ print('defining macro ',$name," via \def...\n"); }

#
# get macro parameters
#
my $N_pars = 0;
my %par_delim = {};		# will build up <parameter delimiter hash>
my $par = '#0';			# pseudo-parameter for tokens preceding #1
skip_blank_tokens();
	# outer loop: loops over parameters #0, #1, #2, ...
	while (1)
	{
	my @delim = ();			# delimiter token list for $par

		while (($token = get_token()) ne '')
		{
		if ($token eq '{') { last; }	# } (text match)# loop control
		if ($token =~ /^#[1-9]/) { last; }		# loop control

		push(@delim, ($token));
		}

	$par_delim{$par} = [@delim];
	if ($token eq '{') { last; }	# } (text match)	# loop control

	++$N_pars;
	$par = $token;
	}

#
# get macro definition
#
my @defn = get_token_group();

if ($debug > 0)
	{
	print('macro ',$name," definition:\n");
	print('   N_pars = ',$N_pars,"\n");
	print("   par_delim = {\n");
	my ($key, $ref_to_aval);
		while (($key,$ref_to_aval) = each(%par_delim))
		{
		print('               ',$key,
		      ' => <',join(':',@$ref_to_aval),">\n");
		}
	print("               }\n");
	print('   defn = <', join(':', @defn), ">\n");
	print("\n");
	}

# store macro in global data structures for later use
$macro_N_pars{$name} = $N_pars;
$macro_par_delim{$name} = { %par_delim };
$macro_defn{$name} = [@defn];
}

###############################################################################

#
# This subroutine defines a macro via \newcommand or \renewcommand, i.e.
# it gets a \newcommand/renewcommand macro definition from the input
# stream and records it for later use.  We assume that the \newcommand
# or \renewcommand token has already been seen.
#
sub define_macro_via_newcommand()
{
my $token;

my $tme_die_msg = '\newcommand or \renewcommand syntax error';

#
# get macro name
#
skip_blank_tokens();
(get_token() eq '{')
	|| tme_die($tme_die_msg);
my $name = get_token();
(get_token() eq '}')
	|| tme_die($tme_die_msg);
if ($debug > 0)
	{ print('defining macro ',$name,
		' via \newcommand or \renewcommand...',"\n"); }

#
# get macro number of parameters
#
my $N_pars = 0;				# default value
skip_blank_tokens();
if (($token = get_token()) eq '[')
	{
	( ($token = get_token()) =~ /^[1-9]$/ )
		|| tme_die($tme_die_msg);
	$N_pars = 0 + $token;		# convert to integer (we hope) ...
	(get_token() eq ']')
		|| tme_die($tme_die_msg);
	}
   else { push_tokens($token); }

#
# build macro parameter hash
#
my %par_delim = {};		# will build up <parameter delimiter hash>
my $i;
	for ($i = 0 ; $i <= $N_pars ; ++$i)
	{
	my $par = '#' . $i;
	$par_delim{$par} = [];	# empty delimiter token list
	}

#
# get macro definition
#
skip_blank_tokens();
(get_token() eq '{')
	|| tme_die($tme_die_msg);
my @defn = get_token_group();

if ($debug > 0)
	{
	print('macro ',$name," definition:\n");
	print('   N_pars = ',$N_pars,"\n");
	print("   par_delim = {\n");
	my ($key, $ref_to_aval);
		while (($key,$ref_to_aval) = each(%par_delim))
		{
		print('               ',$key,
		      ' => <',join(':',@$ref_to_aval),">\n");
		}
	print("               }\n");
	print('   defn = <', join(':', @defn), ">\n");
	print("\n");
	}

# store macro in global data structures for later use
$macro_N_pars{$name} = $N_pars;
$macro_par_delim{$name} = { %par_delim };
$macro_defn{$name} = [@defn];
}

###############################################################################

#
# This function gets a macro's arguments from the input stream.
# (The macro must already be defined.)
#
# Arguments:
# $name = (in) The name of the macro (a token).
#
# Results:
# The function returns a hash:
# - keys are '#1', '#2', ...
# - values are references to token lists of the arguments
#
sub get_macro_args
{
my ($name) = @_;
my %args = {};

if ($debug > 0)
	{ print("getting macro args for ",$name," ...\n"); }

my $N_pars = $macro_N_pars{$name};
my $ref_to_par_delim = $macro_par_delim{$name};
my %par_delim = %$ref_to_par_delim;

# special-case the pseudo-parameter #0
my $par = '#0';
my $ref_to_delim = $par_delim{$par};
my @delim = @$ref_to_delim;
if (scalar(@delim) > 0)
	{
	match_tokens(@delim)
		|| tme_die('usage of ',$name,' doesn\'t match its definition');
	}

# get arguments for "normal" parameters
my $i;
	for ($i = 1 ; $i <= $N_pars ; ++$i)
	{
	$par = '#' . $i;
	$ref_to_delim = $par_delim{$par};
	@delim = @$ref_to_delim;

	if ($debug >= 8)
		{ print('getting arg for parameter ',$par,
			', delim = <',join(':',@delim),">\n"); }

	my @arg;

	# here we actually get the argument,
	# following the rules in the TeXbook (pp. 203-204)
	if (scalar(@delim) == 0)
		{
		# undelimited parameter
		skip_blank_tokens();

		my $token = get_token();
		@arg = ($token eq '{')	# } (text match)
		       ? get_token_group()	# this function returns the
						# _interior_ of the group,
						# i.e. it implicitly strips
						# off the enclosing {}-braces
		       : ($token);

		if ($debug >= 8)
			{ print('   got undelimited @arg = ',
				'<', join(':',@arg),">\n"); }
		}
	   else {
		# delimited parameter
		@arg = get_tokens_to_delimiter(@delim);

		# if the argument is a token group, we must
		# explicitly strip off the enclosing {}-braces
		if ((@arg[0] eq '{') && (@arg[$#arg] eq '}'))
			{
			if ($debug >= 8)
				{ print('   stripping enclosing {}-braces',
					' from delimited @arg',"\n"); }
			shift(@arg);
			pop(@arg);
			}

		if ($debug >= 8)
			{ print('   got delimited @arg = ',
				'<', join(':',@arg),">\n"); }
		}

	$args{$par} = [@arg];
	}

if ($debug > 0)
	{
	print("got args = {\n");
	my ($key, $ref_to_aval);
		while (($key,$ref_to_aval) = each(%args))
		{
		print('           ',$key,' => ',
		      '<', join(':',@$ref_to_aval),">\n");
		}
	print("           }\n");
	}

return(%args);
}

###############################################################################

#
# This function expands a macro, pushing the expansion back into the
# input token stream.
#
# Arguments:
# $name = (in) The name of the macro (a token).
# $ref_to_args = (in) A reference to the arguments of the macro (a hash
#		      as returned by  get_macro_args() ).
#
sub expand_macro
{
my ($name, $ref_to_args) = @_;
my %args = %$ref_to_args;

if ($debug > 0)
	{ print('expanding macro ',$name," ...\n"); }

my $ref_to_defn = $macro_defn{$name};
my @defn = @$ref_to_defn;

my $token;
my @expansion = ();
	foreach $token (@defn)
	{
	if ($token =~ /^#[1-9]/)
		{
		my $ref_to_arg = $args{$token};
		my @arg = @$ref_to_arg;
		if ($debug >= 8)
			{ print('   token ',$token,' => ',
				'<', join(':',@arg),">\n"); }
		push(@expansion, @arg);
		}
	   else {
		if ($debug >= 8)
			{ print('   token ',$token,"\n"); }
		push(@expansion, ($token));
		}
	}

if ($debug > 0)
	{ print('expansion = <',join(':',@expansion),">\n"); }

push_tokens(@expansion);
}

###############################################################################

#
# end of scope for macro-engine global data structures
#
}

###############################################################################
###############################################################################
###############################################################################

#
# This function attempts to match a specified token list against tokens
# from the input stream.
#
# Arguments:
# @token_list = (in) The token list to match against the input stream.
#
# Results:
# The function returns 1 iff the tokens match, 0 iff it finds a mismatch.
# In the former case, all the matched tokens have been read from the
# input stream; in the latter, only those tokens up to and including
# the first mismatch have been read.
#
sub match_tokens
{
my @token_list = @_;

my $token;

	foreach $token (@token_list)
	{
	if (get_token() ne $token)
		{ return(0) };					# flow control
	}

return(1);
}

###############################################################################

#
# This function gets tokens from the input stream until (and including)
# the first appearance of a specified "delimiter" token list which isn't
# nested inside {}-groups.
#
# Arguments:
# @delimiter = (in) The delimiter token list.
#
# Results:
# The function returns the (possibly empty) list of tokens encountered
# _before_ the beginning of the delimiter, discarding the delimiter
# itself.
#
sub get_tokens_to_delimiter
{
my @delimiter = @_;
my $N_delimiter = scalar(@delimiter);

my $token;
my @token_list = ();
my $N_token_list = 0;

my $nesting = 0;
TOKEN_LOOP:
	while (($token = get_token()) ne '')
	{
	push(@token_list, ($token));
	++$N_token_list;

	if ($token eq '{') { ++$nesting };
	if ($token eq '}') { --$nesting };

	# starting index in @token_list of "tail" (= last N_delimiter elements)
	my $tail_index = $N_token_list - $N_delimiter;

	if ( ($nesting == 0)			# only check possible delims
						# when they're not hidden
						# inside {}-groups
	     && subarray_eq($N_delimiter,
			    [@token_list], $tail_index,
			    [@delimiter], 0) )
		{
		# tail matches @delimiter
		# ==> chop off delimiter, return preceding tokens
		$#token_list = $tail_index - 1;
		return(@token_list);				# flow control
		}
	}

# trouble...
tme_die('EOF encountered while looking for delimiter ',
	'<',join(':',@delimiter),'>');
}

###############################################################################

#
# This function gets a {}-balanced token group from the input stream,
# assuming that the opening '{' has already been seen.
#
# Results:
# The function returns the _interior_ of the token group, discarding
# the closing '}'.
#
sub get_token_group
{
my @token_group = ();
my $token;

my $nesting = 0;
	while (($token = get_token()) ne '')
	{
	if (($nesting == 0) && ($token eq '}'))	# { (text match)
		{ last; }					# flow control

	if ($token eq '{') { ++$nesting };
	if ($token eq '}') { --$nesting };

	push(@token_group, $token);
	}

return(@token_group);
}

###############################################################################

#
# This function skips blank (actually any whitespace) tokens from the
# input stream.  When a non-blank (actually non-white-space) token is
# found, it's pushed back into the input.
#
sub skip_blank_tokens
{
my $token;

	while (($token = get_token()) =~ '^\s+$')
	{
	}
push_tokens($token);
}

###############################################################################
###############################################################################
###############################################################################

#
# This function compares two subarrays @x[$xposn .. $xposn+$len-1]
# and @y[$yposn .. $yposn+$len-1], returning 1 iff they're
# element-by-element eq, 0 iff they're not.
#
# Arguments:
# $len = (in) The number of array elements to be compared.
# $ref_to_x = (in) A reference to the @x array.
# $xposn = (in) The starting index in the @x array for the comparison.
# $ref_to_y = (in) A reference to the @y array.
# $yposn = (in) The starting indey in the @y array for the comparison.
#
sub subarray_eq
{
my ($len, $ref_to_x, $xposn, $ref_to_y, $yposn) = @_;
my @x = @$ref_to_x;
my @y = @$ref_to_y;

my $i;
	for ($i = 0 ; $i < $len ; ++$i)
	{
	if ($x[$xposn+$i] ne $y[$yposn+$i])
		{ return(0); }					# flow control
	}

return(1);
}

###############################################################################
###############################################################################
###############################################################################

#
# <<<design notes on comment handling>>
#

#
# The handling of comments is a tricky point in tme:
# - When we're reading macro-definition files, we want to ignore comments.
#   In particular, TeX ignores comments in it's "mouth", i.e. when tokenizing
#   the input, so comments never make it into macro definitions.  We want
#   to follow this same behavior.
# - When we're reading text from standard input to be macro-expanded, there
#   are two cases:
#   - By default, we want to transparently copy comments to standard output.
#     output.
#   - If the -c command line flag is specified, we want to treat comments
#     just like any other input text, i.e. we want to process their bodies
#     through our macro engine.
#
# We implement this as follows:
# - When we're reading macro-definition files,  get_bare_token()  returns
#   comment tokens containing the entire input-stream text text from '%'
#   through "\n" inclusive;  get_token()  then discards these tokens.
# - When we're reading TeX in which to expand macros:
#   - By default,  get_bare_token()  continues to return comment tokens
#     containing the entire input-stream text from '%' through "\n"
#     inclusive;  get_token()  then transparently copies these to the
#     output stream.
#   - If the -c command line flag is specified,  get_bare_token()  and
#      get_token()  give no special treatment to '%', it's just treated
#     as a normal single-character token and passed up into the macro
#     engine.
#

###############################################################################

#
# <<<global data for the input routines>>>
# (uses effectively "static" storage allocation)
#

sub BEGIN
{

#
# stuff for the input file itself
#
my $input_file;			# current input file name for error msgs
my $input_fh = new FileHandle;

#
# The input routines keep two distinct types of buffers:
# - macro expansions and things that we read and decide we don't want
#   to handle just yet, must be "pushed back" into the input token stream
# - untokenized input characters need to be buffered
#
my @token_buffer;		# pushed-back "input" tokens
my $char_buffer;		# untokenized input characters

#
# stuff for handling comments, cf design notes above
#
my $generate_comment_tokens;	# should  get_bare_token()  generate special
				# (multicharacter) comment tokens containing
				# the entire input-stream text from '%' through
				# "\n" inclusive?
my $echo_comment_tokens;	# should  get_token()  echo comment tokens?
my $process_comment_tokens;	# should  get_token()  return comment tokens
				# for processing by the macro engine?

###############################################################################

#
# This function does a die() using the concatenation of a caller-specified
# message and the current input file/line.
#
# Arguments:
# @msg = (in) The caller-specified message.  If this is a list, its
#	      elements are concatenated together, just like die() does.
#
sub tme_die
{
my @msg = @_;

die(@msg,"\n",
    'in input file ',$input_file,', line ',$INPUT_LINE_NUMBER,"\n");
}

###############################################################################

#
# This function sets the comment-handling policy for the input routines,
# by storing appropriate values into the global variables described in
# the design notes and comments above.
#
# Arguments:
# $mode = (in) 'define' ==> Set policy for reading macro definitions.
#	       'expand' ==> Set policy for expanding macros (details
#			    also depend on $expand_macros_in_comments
#			    global variable).
#
sub set_comment_handling
{
my ($mode) = @_;

if    ($mode eq 'define')
	{
	if ($debug > 0)
		{ print("setting macro-definition comment handling...\n"); }
	$generate_comment_tokens = 1;
	$echo_comment_tokens = 0;
	$process_comment_tokens = 0;
	}
elsif ($mode eq 'expand')
	{
	if ($debug > 0)
		{ print("setting macro-expansion comment handling...\n"); }
	if ($expand_macros_in_comments)
		{
		# -c command line flag specified
		if ($debug > 0)
			{ print("   (including within comments)\n"); }
		$generate_comment_tokens = 0;
		$echo_comment_tokens = 0;
		$process_comment_tokens = 1;
		}
	   else {
		# default (no -c command line flag)
		if ($debug > 0)
			{ print("   (but not within comments)\n"); }
		$generate_comment_tokens = 1;
		$echo_comment_tokens = 1;
		$process_comment_tokens = 0;
		}
	}
else	{
	tme_die(
'tme internal error: set_comment_handling() got bad $mode=\'', $mode,"'!\n"
	       );
	}
}

###############################################################################

#
# This function opens an input file, from which get_token() and friends
# will then read.  Any buffered input tokens or characters are discarded.
# The input file name is saved in the global variable $input_file for
# error messages, except that '-' is mapped into '(stdin)' for readability.  
#
# Arguments:
# $file = (in) The file name to open.
#
sub open_input_file
{
my ($file) = @_;

if ($debug > 0)
	{ print('opening input file \'',$file,"'...\n"); }

$input_file = $file;
open($input_fh, $input_file)
	|| die('couldn\'t open input file ',$file,"!\n");

@token_buffer = ();
$char_buffer = '';

if ($input_file eq '-')
	{ $input_file = '(stdin)'; }	# more readable name for error msgs
}

###############################################################################

#
# This function closes an input file opened by  open_input_file() .
#
sub close_input_file
{
close($input_fh);
}

###############################################################################

#
# This function pushes a token or tokens back into the buffered token
# stream, as in the stdio function ungetc() or the common extension
# ungets().
#
# Arguments:
# @tokens = (in) The token or token list to push back.  These tokens
#		 will be returned by future  get_token()  calls in
#		 left-to-right order, eg
#			push_tokens('a', 'b', 'c');
#		 is equivalent to
#			push_tokens('c');
#			push_tokens('b');
#			push_tokens('a');
#		 in that either will cause the next 3  get_token()
#		 calls to return 'a', 'b', and 'c' respectively.
#
sub push_tokens
{
my @tokens = @_;

if ($debug >= 5 )
	{ print('push_list(',join(':',@tokens),') '); }

unshift(@token_buffer, @tokens);
}

###############################################################################

#
# This function reads a bare-token or bare-tokens from the input stream,
# and returns a token.  Comment tokens are handled as specified by the
#  $echo_comment_tokens  and  $process_comment_tokens  global flags.
#
# Results:
# The function returns the next token, or '' for EOF.
#
sub get_token
{
my $token;

	while (($token = get_bare_token()) ne '')
	{
	if ($token =~ /^%/)
		{
		if ($echo_comment_tokens) { put_token($token); }
		if ($process_comment_tokens) { last; }		# flow control
		next;						# flow control
		}
	   else { last; }					# flow control
	}

if ($debug >= 5 )
	{ print('get(',$token,') '); }
return($token);
}

###############################################################################

#
# This function gets the next bare-token from the pushed-back tokens
# (if there are any), or failing that from the input stream.
#
# We use a simple approximation to TeX's actual tokenizing algorithm
# (described in the TeXbook pp. 46-47):  We test the untokenized input
# against the following regular expressions, taking the first match
# found as the next token:
#	^\\[a-zA-Z]+		% eg \foo
#	^\\[^a-zA-Z]		% eg \$ or \,
#	^#[1-9#]		% eg #1 or ##
#	^%.*\n			% eg TeX comments (optional)
#	^(.|\n)			% anything else			
#
# Comment tokens are handled as specified by the  $generate_comment_tokens
# global flag.
#
# Results:
# The function returns the next bare-token, or '' for EOF.
#
sub get_bare_token
{
my $bare_token;

# return pushed-back token if any are available
if (scalar(@token_buffer) > 0)
	{ return( shift(@token_buffer ) ) }			# flow control

# refill input character buffer if needed
if ($char_buffer eq '')
	{
	$char_buffer = <$input_fh>;
	if (! defined($char_buffer))
		{ return('') };					# flow control
	}

# tokenize the input characters
if (    ( $char_buffer =~ /^(\\[a-zA-Z]+)/ )	# eg \foo
     || ( $char_buffer =~ /^(\\[^a-zA-Z])/ )	# eg \$ or \,
     || ( $char_buffer =~ /^(#[1-9#])/ )	# eg #1 or ##
     || ( $generate_comment_tokens
	  && ( $char_buffer =~ /^(%.*\n)/ ) )	# eg % TeX comment
     || ( $char_buffer =~ /^(.|\n)/ )    )	# anything else
	{
	# got a token
	$bare_token = $1;
	}
   else {
	tme_die(
'tme internal error: can\'t match',"\n",
'$char_buffer=\'',$char_buffer,'\''
	       );
	}

# delete the token from the input character buffer
substr($char_buffer, 0, length($bare_token)) = '';

return($bare_token);
}

###############################################################################

#
# end of scope for input-routines global data structures
#
}

###############################################################################
###############################################################################
###############################################################################

#
# This subroutine outputs a token.
#
# Arguments:
# $token = (in) The token to output.
#
sub put_token
{
my ($token) = @_;

if ($debug > 0)
	{ print('<',$token,'> '); }
   else { print($token); }
}
