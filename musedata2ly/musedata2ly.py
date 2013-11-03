#!@PYTHON@

# musedata = musedata.stanford.edu
# musedata = COBOL for musicians.


# TODO
#
# * clefs,
# * keys,
# * staffs,
# * multiple voices (they use `Backspace' (shudder)
# * tuplets
# *** partial
#

#
# I completely forgot how this was supposed to work --hwn 5/2002
#
#

import re
import sys
import string
import getopt
import os
program_name = 'musedata2ly'
version = '@TOPLEVEL_VERSION@'
if version == '@' + 'TOPLEVEL_VERSION' + '@':
	version = '2.0.0'	# '(unknown version)'	   # uGUHGUHGHGUGH

partcombine = 0

ref_header_dict = {
	'COM': 'composer',
	'OPR': 'collection',
	'OTL': 'title',
	'OMV': 'subtitle',
	'YOR': 'source',
	'AGN': 'instrument',
	'END': 'encodingdate',
	'CDT': 'date',
	'OCY': 'composedin',
	'AST': 'genre',
	'YEC': 'copyright',
	'YEM': 'license',
	'YEN': 'encodingcountry',
	'EED': 'editor',
	'SCA': 'opus',
	'ONM': 'onm',
	'ENC': 'musedataencoder',
	'KEY': 'musedatakey',
	'AFT': 'musedatastage'
	}

tuplet_break = {
	1:	1,
	2:	1,
	4:	2,
	8:	4,
	16:	8,
	32:	8,
	64:	8,
	128:	8
	}

class Ref_parser:
	def __init__ (self, fn):
		self.dict = {}

		ls = open (fn).readlines ()
		self.parse (ls)
	def parse (self,ls):
		for l in ls:
			m = re.match('!!!([A-Z]+):[ \t]+(.*)$',l)
			if m:
				key = m.group(1)
				val = m.group (2)
				val = re.sub ('[ \t]+', ' ', val)
				try:

					key =ref_header_dict [key]
				except KeyError:
					sys.stderr.write ('\nUnknown ref key \`%s\'' % key)
				s = ''
				try:
					s = self.dict[key]
				except KeyError:
					pass

				s = s + val
				self.dict[key] = s
	def dump( self):
		str = ''
		for (k,v) in self.dict.items ():
			str = str +'  %s = "%s"\n' % (k,v)
		str = '\\header {\n%s}' % str
		return str


verbose = 0


actab = {-2: 'eses', -1: 'es', 0 : '', 1: 'is', 2:'isis'}

def pitch_to_lily_string (tup):
	(o,n,a) = tup

	nm = chr((n + 2) % 7 + ord ('a'))
	nm = nm + actab[a]
	if o > 0:
		nm = nm + "'" * o
	elif o < 0:
		nm = nm + "," * -o
	return nm

def get_key (s):
	i = string.atoi (s)
	return ''

def get_timesig (s):
	s = s.strip(" ");
	sys.stderr.write("Time signature '%s'\n" % s)
	if s == '0/0':
		return '\\defaultTimeSignature \\time 2/2\n'
	elif s == '1/1':
		return '\\defaultTimeSignature \\time 4/4\n'
	else:
		return '\\numericTimeSignature \\time %s\n' % s


divisions = 4
def get_divisions_per_quarter (s):
	divisions = string.atoi (s)
	return ''

def get_directive (s):
	return '%% %s\n' % s

def get_transposing (s):
	return ''

def get_num_instruments (s):
	return ''

def get_lilypond_notename (p, ac):
	if p > 5:
		p = p - 7
	s = chr (p + ord ('c'))
	infix = 'i'
	if ac < 0:
		infix = 'e'
		ac = -ac

	while ac:
		s = s + infix + 's'
		ac = ac - 1
	return s
def get_clef (s):
	return ''

SPACES = ' '
DIGITS = "0123456789"


clef_dict = {
04: 'treble',
05: 'french',
11: 'baritone',
12: 'tenor',
13: 'alto',
14: 'mezzosoprano',
15: 'soprano',
21: 'subbass',
22: 'bass',
23: 'varbaritone',
}
attr_dict = {
	'C' : get_clef,
	'K' : get_key ,
	'T' : get_timesig,
	'Q' : get_divisions_per_quarter,
	'D' : get_directive,
	'X' : get_transposing,
	'I' : get_num_instruments,
	}

class Attribute_set:
	def __init__ (self, dict):
		self.dict = dict
	def dump (self):
		s = ''
		if self. dict.has_key ('T'):
			s = s+ get_timesig  (self.dict['T'])
		if self.dict.has_key('K'):
			k = string.atoi(self.dict['K'])
			pos_keys = ( 'c', 'g', 'd', 'a', 'e', 'b', 'fis', 'cis', 'gis', 'dis', 'ais', 'eis')
			neg_keys = ('c', 'f', 'bes', 'es', 'as', 'des', 'ges', 'ces', 'fes', 'beses', 'eses', 'ases' )
			if k >= 0:
				pitch = pos_keys[k % 12]
			else:
				pitch = neg_keys[(-k) % 12]
			s += '  \\key ' + pitch + ' \\major\n'
			sys.stderr.write('Key pitch %s k %d\n' % (pitch, k))
		if self.dict.has_key('C'):
			clef = string.atoi(self.dict['C'])
			sys.stderr.write("clef dict %s as-int %d\n" % (self.dict['C'], clef))
			s += ' \\clef ' + clef_dict[clef] + '\n'
			# sys.stderr.write('Clef %s\n' % self.dict['C'])

		return s


script_table = {
'v': '\\upbow',
'n': '\\downbow',
'o': '\\harmonic',
'0': '"openstring',
'Q': '\\thumb',
'>': '^',
'V': '^',
'.': '.',
'_': '-',
'=': '"det leg"',
'i': '|',
's': '"\\\\textsharp"',
'n': '"\\\\textnatural"',
'b': '"\\\\textflat"',
'F': '\\fermata',
'E': '\\fermata',
}


class Chord:
	def __init__ (self):
		self.rest_type = 0
		self.pitches = []
		self.grace = 0
		self.cue = 0
		self.acciaccatura = 0
		self.slurstart = []
		self.slurstop  = []
		self.scripts = []
		self.syllables = []
		self.dynamics = []
		self.dots = 0
		self.basic_duration = 4
		self.tied = 0
		self.divs = 0;

		self.note_suffix = self.note_prefix = ''
		self.chord_suffix = self.chord_prefix = ''

	def add_script (self,s):
		self.scripts.append (s)
	def set_duration (self, d):
		self.basic_duration = d
	def add_syllable (self, s):
		self.syllables.append (s)
	def add_pitch (self,t):
		self.pitches.append (t)
	def add_dynamics (self, d):
		self.dynamics.append(d)
	def prepend (self, s):
		self.note_prefix += s
	def append (self, s):
		self.chord_prefix += s
	def duration(self):
		sd = ''
		if self.basic_duration == 0.5:
			sd = '\\breve'
		else:
			sd = '%d' % self.basic_duration
		sd += '.' * self.dots
		return sd

	def dump (self):
		str = ''

		if self.grace:
			if self.acciaccatura:
				str += ' \\once \\override Stem #\'stroke-style = #"grace"'
			str += ' \\grace {'

		sd = self.duration()

		for p in self.pitches:
			if str:
				str = str + ' '
			str = str + pitch_to_lily_string (p)

		if len (self.pitches) > 1:
			str = '<%s>' % str
		elif len (self.pitches) == 0:
			if self.rest_type == 1:
				str = 'r'
			elif self.rest_type == 2:
				str = 's'
			elif self.rest_type == 3:
				str = 'R'

		str = str + sd + '(' * len (self.slurstart) + ')' * len (self.slurstop)
		if self.dynamics != []:
			str += '\\'
			# sys.stderr.write("Dynamics '%s'\n" % self.dynamics)
			if self.dynamics == [ 'Z', 'p' ]:
				str += 'sfp'
			else:
				for d in self.dynamics:
					if d == 'p' or d == 'm' or d == 'f':
						str += d
					elif d == 'Z':
						str += 'sfz'
					elif d == 'R':
						str += 'rfz'
			# sys.stderr.write("Append dynamics '%s' for '%s'\n" % ( str, self.dynamics))

		for s in self.scripts:
			str = str + '-' + s

		str = self.note_prefix +str  + self.note_suffix
		str = self.chord_prefix + str + self.chord_suffix
		if self.tied:
			str += " ~"
		return str


class Measure_start:
	def __init__ (self, ln, parser):
		sys.stderr.write("measure %s\n" % ln)
		self.barnum = ln
		type = self.barnum[1:7]
		if parser.pending_repeats != 0 and type == 'heavy2':
			self.barnum = 'm       ' + ln[8:]
			sys.stderr.write("Suppress heavy2 because pending_repeats != 0; becomes '%s'\n" % self.barnum)
		if parser.pending_alternative != 0 and type == 'heavy2':
			self.barnum = 'm       ' + ln[8:]
			sys.stderr.write("Suppress heavy2 because pending_alternative %d; becomes '%s'\n" % (parser.pending_alternative, self.barnum))
		has_stop_end = 0
		flags = re.split (' +', ln[16:])
		for f in flags:
			sys.stderr.write("check flag '%s' ('%s' '%s')\n" % (f, f[0:9], f[0:8]))
			if f == ':|':
				if has_stop_end:
					# done repeat state at stop-end already
					continue
				elif parser.pending_repeats == 0:
					sys.stderr.write("Do explicit \\repeat at beginning and repeat close for %s pending %d\n" % (f, parser.pending_repeats))
					parser.entries.insert(0, RepeatToken(" \\repeat volta 2 {\n"))
					parser.append_entry(RepeatToken(" }"))
				else:
					barline = ' \\bar ":|"'
					sys.stderr.write("write explicit bar/close pending %d\n" % parser.pending_repeats)
			elif f == '|:':
				parser.append_entry(RepeatToken("\n  \\repeat volta 2 {"))
				parser.pending_repeats += 1
				sys.stderr.write("manual insert a repeat here pending %d\n" % parser.pending_repeats)
				if type == 'heavy3':
					self.barnum = 'm       ' + ln[8:]
					sys.stderr.write("Suppress heavy3 because explicit repeat; becomes '%s'\n" % self.barnum)
			elif f[0:9] == 'start-end':
				num = re.split ('\.', f[9:])
				# sys.stderr.write("Have %s, number %s\n" % (f, num[0]))
				sys.stderr.write("No support yet for non-full-bar repeats\n")
				n = string.atoi(num[0])
				if n == 1:
					if parser.pending_repeats == 0:
						parser.entries.insert(0, RepeatToken(" \\repeat volta 2 {"))
						parser.pending_repeats += 1
						sys.stderr.write("auto-insert a repeat at beginning of piece\n")
					parser.append_entry(RepeatToken(" }\n  \\alternative { { "))
				else:
					parser.append_entry(RepeatToken("\n  {"))
				parser.pending_alternative += 1
				sys.stderr.write("handle start-alternative %s[%d] pending repeats %d alternative %d\n" % (f, n, parser.pending_repeats, parser.pending_alternative))
			elif f[0:8] == 'stop-end' or f[0:8] == 'disc-end':
				has_stop_end = 1
				num = re.split ('\.', f[8:])
				sys.stderr.write("Have %s, number %s\n" % (f, num[0]))
				sys.stderr.write("No support yet for non-full-bar repeats\n")
				n = string.atoi(num[0])
				parser.append_entry(RepeatToken(" }"))
				parser.pending_alternative -= 1
				if n > 1:
					parser.append_entry(RepeatToken("\n  }"))
					parser.pending_repeats -= 1
				sys.stderr.write("handle end-alternative %s[%d] pending repeats %d alternative %d\n" % (f, n, parser.pending_repeats, parser.pending_alternative))

	def dump (self):
		barnum = self.barnum[8:11]
		barline = ''
		type = self.barnum[1:7]
		if type == 'dotted':
			barline = ' \\bar ":"'
		elif type == 'double':
			barline = ' \\bar "||"'
		elif type == 'heavy1':
			sys.stderr.write("Fixme: no support for single heavy bar line\n")
		elif type == 'heavy2':
			barline = ' \\bar "|."'
		elif type == 'heavy3':
			barline = ' \\bar ".|"'
		elif type == 'heavy4':
			barline = ' \\bar ".|."'

		return barline + ' | % ' + barnum + '\n'

class VoiceSplitter:
	def dump (self):
		if partcombine:
			return ' \\partcombine {'
		else:
			return ' << {'

class VoiceJoiner:
	def dump (self):
		if partcombine:
			return ' }'
		else:
			return ' } >>'

class VoiceSwitcher:
	def dump (self):
		if partcombine:
			return ' } {'
		else:
			return ' } \\\\ {'

class RepeatToken:
	def __init__ (self, str):
		self.str = str
	def dump (self):
		return self.str


class MarkupToken:
	def __init__ (self, str):
		self.str = str
	def dump(self):
		return self.str

class Tuplet:
	def __init__ (self, str):
		self.str = str
	def dump (self):
		return self.str


class Partial:
	def __init__ (self, divs, divs_per_q):
		self.divs = divs;
		self.divs_per_q = divs_per_q;
	def dump(self):
		dur = 4 * self.divs_per_q / self.divs
		sys.stderr.write("Dump partial: dur %d divs/Q %d divs %d\n" % (dur, self.divs_per_q, self.divs))
		if dur == 0.5:
			sd = '\\breve'
		else:
			sd = '%d' % dur
		dots = 0
		while (int(dur) / 3) * 3 == int(dur):
			dur /= 3
			dots += 1
		sd += '.' * dots
		return '\\partial ' + sd;


class Parser:
	def append_entry (self, e):
		self.measure.append (e)
	def append_chord (self,c ):
		self.chords.append (c)
		self.measure.append (c)
	def last_chord (self):
		return self.chords[-1]
	def __init__ (self, fn):
		self.current_div = 0
		self.divs_per_q = 1
		self.divs_per_q_remainder = 1
		self.enum = 4
		self.denom = 4
		self.header_dict = {
			'tagline' :'automatically converted from Musedata',
			'copyright' : 'all rights reserved -- free for noncommercial use'
			#  musedata license (argh)
			}
		self.entries = []
		self.chords = []
		self.measure = []
		self.current_tuplet = 1
		self.voices = 1
		self.second_voice_divs = 0
		self.pending_repeats = 0
		self.pending_alternative = 0
		self.pending_grace = 0
		self.piece = None
		self.append_pre_time = ''
		self.prepend_pre_time = ''

		lines = open (fn).readlines ()
		lines = map (lambda x: re.sub ("\r$", '', x), lines)
		lines = self.parse_header (lines)
		lines = self.append_lines (lines)
		str = string.join (lines, '\n')
		lines = re.split ('[\n\r]+', str)
		self.parse_body (lines)

	def parse_header (self, lines):
		enter = string.split (lines[3], ' ')
		self.header_dict['enteredby']  = string.join (enter[1:])
		self.header_dict['enteredon'] = enter[0]
		self.header_dict['opus'] = lines[4]
		self.header_dict['source'] = lines[5]
		(self.header_dict['composer'], self.header_dict['title']) = string.split(lines[6], ' ', 1)
		# self.header_dict['title'] = lines[6]
		self.header_dict['subtitle'] = lines[7]
		self.header_dict['instrument']= lines[8]
		self.header_dict['musedatamisc'] =lines[9]
		self.header_dict['musedatagroups'] =lines[10]
		self.header_dict['musedatagroupnumber']=lines[11]
		lines =  lines[12:]
		comment = 0
		while lines:
			if lines[0][0]  == '$':
				sys.stderr.write("found a '$' header line %s\n" % lines[0])
				break
			lines = lines[1:]
		return lines

	def set_time_sig (self, sig):
		(s_enum, s_denom) = re.split('/', sig)
		self.enum = string.atoi(s_enum)
		self.denom = string.atoi(s_denom)
		if self.denom == 0 and self.enum == 0:
			self.enum = 2
			self.denom = 2
		elif self.denom == 1 and self.enum == 1:
			self.enum = 4
			self.denom = 4

	def parse_musical_attributes (self,l):
		atts = re.split('([A-Z][0-9]?):', l)
		atts = atts[1:]
		found = {}
		while len (atts):
			id = atts[0]
			val = atts[1]
			atts = atts[2:]
			found[id] = val
			# sys.stderr.write ("attribute %s value %s\n" % (id, val))
			if id == 'T':
				self.set_time_sig(val)

		try:
			self.divs_per_q = string.atoi (found['Q'])
			self.divs_per_bar = 4 * self.enum * self.divs_per_q / self.denom
			self.divs_per_q_remainder = self.divs_per_q
			while 2 * (self.divs_per_q_remainder / 2) == self.divs_per_q_remainder:
				self.divs_per_q_remainder /= 2;
			# sys.stderr.write("divs/q %d divs/bar %d divs_per_q_remainder %d\n" % (self.divs_per_q, self.divs_per_bar, self.divs_per_q_remainder) )
		except KeyError:
			pass

		attr = Attribute_set (found)
		self.append_entry (attr)
		if attr.dict.has_key('D'):
			self.header_dict['piece'] = attr.dict['D']

	# Duplicate definition? --RFHH
	# def append_entry (self, e):
	#	self.entries.append (e)

	def parse_line_comment (self,l):
		pass

	def parse_note_line (self,l):
		ch = None
		tail = []
		if verbose:
			print DIGITS+DIGITS+DIGITS
			print l
		sys.stderr.write ("Chord %s\n" % l)
		pi = l[0:5]
		di = l[5:8]
		# if pi[0] == 'r':
			# sys.stderr.write ("Rest duration string %s\n" % di);

		tied = l[8:9] == '-'
		# if tied:
			# sys.stderr.write("line '%s': tied\n" % l)

		cue = grace = 0
		if (pi[0] == 'g'):
			grace = 1
			pi = pi[1:]
		elif (pi[0] == 'c'):
			cue = 1
			pi = pi[1:]

		is_chord_continuation = pi[0] == ' '
		if is_chord_continuation:
			# sys.stderr.write("chord continuation... ")
			ch = self.last_chord ()
			pi = pi[1:]
		else:
			# sys.stderr.write("new chord... ")
			ch = Chord ()
			ch.chord_suffix += self.append_pre_time
			ch.chord_prefix += self.prepend_pre_time
			self.append_pre_time = ''
			self.prepend_pre_time = ''


		ch.cue = ch.cue or cue
		ch.grace = ch.grace or grace

		while pi[0] in SPACES:
			pi = pi[1:]

		if pi[0] == 'r':
			ch.rest_type = 1
		elif pi[0] == 'i':
			ch.rest_type = 2
		else:
			ch.rest_type = 0
			name =  ((ord (pi[0]) -ord('A')) + 5) % 7
			alter = 0
			pi = pi[1:]
			while pi and pi[0] in '#f':
				if pi[0] == '#':
					alter = alter + 1
				else:
					alter = alter - 1
				pi = pi[1:]

			oct = string.atoi (pi) - 3

			pittup = (oct, name ,alter)
			ch.add_pitch (pittup)

		# ch.dots = 0

		if pi[0] == 'r' and string.atoi(di) == self.divs_per_bar:
			# sys.stderr.write("full-bar rest: add dots\n")
			dur = self.enum
			ch.dots = 0
			ch.rest_type = 3
			while (dur / 3) * 3 == dur:
				dur /= 3
				ch.dots += 1
		else:
			dot_str = l[17:18]
			if dot_str  == '.':
				ch.dots = 1
			elif dot_str == ':':
				ch.dots = 2

		base_dur = None
		if ch.cue or ch.grace:
			c = di[2]
			if self.pending_grace:
				ch.grace = 0
			else:
				sys.stderr.write("grace note c '%c'\n" % c)
				self.pending_grace = 1
			if c == '0':
				ch.acciaccatura = 1
				base_dur = 8
			elif c == 'A':
				base_dur = 0.5
			else:
				base_dur = 1 << (9 - (ord (c) - ord ('0')))
			ch.set_duration (base_dur)

		elif not is_chord_continuation:
			if self.pending_grace:
				self.pending_grace = 0
				ch.prepend(" } ")
			fact  = (1,1)
			if ch.dots == 1:
				fact = (2,3)
			elif ch.dots == 2:
				fact = (4, 7)

			# Start tuplet?
			#						--RFHH
			divs = string.atoi(di)
			if self.divs_per_q_remainder * (divs / self.divs_per_q_remainder) != divs:
				sys.stderr.write("We have a tuplet\n")
				tuplet = 8 * self.divs_per_q / divs
				if tuplet < 1:
					sys.stderr.write("Oops tuplet %d divs_per_q %d di %d\n" % ( tuplet, self.divs_per_q, divs ))
				while tuplet > 1 and 2 * (tuplet / 2) == tuplet:
					tuplet = tuplet / 2
			else:
				tuplet = 1
			if self.current_tuplet != tuplet:
				if self.current_tuplet != 1:
					t = Tuplet("} ")
					self.append_chord (t)
					sys.stderr.write("Regular tuplet close: %s\n" % t.dump())
				if tuplet != 1:
					t = Tuplet("\\times 2/%d { " % tuplet)
					self.append_chord (t)
					sys.stderr.write("Regular tuplet open: %s\n" % t.dump())
				self.current_tuplet = tuplet
			if tuplet != 1:
				fact = (fact[0] * tuplet, fact[1] * 2)
			# sys.stderr.write("divs_per_q %d dots %d fact %d/%d di %d\n" % (self.divs_per_q, ch.dots, fact[0], fact[1], divs))
			base_dur =  (4 * self.divs_per_q* fact[1]) / (divs* fact[0])
			ch.divs = divs
			ch.set_duration (base_dur)

			if self.voices > 1:
				sys.stderr.write("Second voice: pending divs %d current divs %d\n" % (self.second_voice_divs, divs))
				self.second_voice_divs -= divs
				if self.second_voice_divs <= 0:
					sys.stderr.write("Second voice: close\n")
					tail.append(VoiceJoiner())
					self.second_voice_divs = 0
					self.voices -= 1
			self.current_div += divs

			if tuplet != 1 and tuplet_break[base_dur] != 0:
				end_div = self.divs_per_q * 4 / tuplet_break[base_dur]
				sys.stderr.write("divs_per_q %d end_div %d current_div %d\n" % (self.divs_per_q, end_div, self.current_div))
				if (self.current_div / end_div) * end_div == self.current_div:
					t = Tuplet("} ")
					sys.stderr.write("Break tuplet close: %s\n" % t.dump())
					tail.append (t)
					self.current_tuplet = 1
		# else:
			# chord, cannot change duration

		ch.tied = ch.tied or tied

		if l[26:27] == '[':
			ch.start_beam = 1
		elif l[26:27] == ']':
			ch.end_beam = 1

		comment = 0;
		additional = l[31:43]
		for c in additional:
			if c == '&' :
				comment = ~comment;
				continue
			if comment:
				continue

			if c in '([{z':
				ch.slurstart.append( 0)
				# sys.stderr.write("Start slur for %s\n" % ch.dump())
				continue
			elif c in ')]}x':
				ch.slurstop.append( 0)
				# sys.stderr.write("Stop slur for %s\n" % ch.dump())
				continue

			if c == '*':
				ch.start_tuplet = 1
				continue
			elif c == '!':
				if self.current_tuplet != 1:
					t = Tuplet("} ")
					sys.stderr.write("Forced tuplet close: %s\n" % t.dump())
					tail.append (t)
					self.current_tuplet = 1
				continue

			if c in DIGITS:
				ch.add_script (c)
				continue

			if c == '-' :
				ch.tied = 1
				continue

			if c == ' ' :
				continue

			if c == 'p' or c == 'm' or c == 'f' or c == 'Z' or c == 'R':
				sys.stderr.write("Add dynamics '%s'\n" % c)
				ch.add_dynamics(c)
				continue

			if c == 't':
				ch.add_script('\\trill')
				continue

			if c == 'r':
				ch.add_script('\\turn')
				continue

			try:
				scr = script_table[c]
				ch.add_script (scr)
				c = None
			except KeyError:
				sys.stderr.write ("\nFixme: script `%s' not done\n" % c)

		text = l[40:81]
		sylls = string.split (text,'|')

		for syl in sylls:
			ch.add_syllable (syl)

		if not is_chord_continuation:
			self.append_chord (ch)
		for elt in tail:
			self.append_entry(elt)

		# sys.stderr.write("Add (existing?) chord duration %d\n" % ch.divs)


	def parse_measure_line (self,l):
		self.append_entry (Measure_start(l, self))
		sys.stderr.write("now coalesce measure '%s' to global note list\n" % l)
		self.entries.extend(self.measure)
		self.measure = []
		self.current_div = 0
		while self.voices > 1:
			sys.stderr.write("Close voice at bar end\n")
			self.append_entry (VoiceJoiner())
			self.voices -= 1


	def parse_backup_line (self,l):
		# Algorithm:
		# Insert a VoiceSplitter record ' << {' (or ' } \\ { ' for
		# third and higher voices) at the appropriate time in
		# the measure queue. Append a VoiceSwitcher record
		# ' } \\ {' to the current measure.
		# Fill the new voice with skips until the backup time.
		# At the end of the measure, close any pending split voices
		# " ' } >>'.
		# sys.stderr.write("\nFixme: handle time backup `%s'\n" % l)
		divs = string.atoi(l[6:8])
		self.second_voice_divs = divs
		i = len(self.measure)
		reversed = list(self.measure)
		reversed.reverse()
		for c in reversed:
			if isinstance(c, Chord):
				# sys.stderr.write("Chord %s\n" % c.dump())
				divs -= c.divs
				i -= 1
				if divs <= 0:
					break

		if divs < 0:
			sys.stderr.write("\nFixme: backup doesn't fit chords\n")

		self.current_div -= divs

		if self.current_tuplet != 1:
			ch = self.measure[-1]
			ch.chord_suffix += " }"
			# sys.stderr.write("Close tuplet at backup: %s\n" % ch.dump())
			self.current_tuplet = 1

		self.measure.insert(i, VoiceSplitter())
		self.append_entry(VoiceSwitcher())
		self.voices += 1
		# sys.stderr.write("Create second voice: pending divs %d\n" % self.second_voice_divs)


	def parse_duration (self, l):
		s = ''
		while l[0] in '0123456789':
			s = s + l[0]
			l= l[1:]
		print l
		num = string.atoi (s)
		den = 4 * divisions

		current_dots = 0
		try_dots = [3, 2, 1]
		for d in try_dots:
			f = 1 << d
			multiplier = (2*f-1)
			if num % multiplier == 0 and den % f == 0:
				num = num / multiplier
				den = den / f
				current_dots = current_dots + d

		if num <> 1:
			sys.stderr.write ('huh. Durations left')
		return '%s%s' % (den, '.' * current_dots)

	def append_lines (self,ls):
		nls = []
		for l in ls:
			if l[0] == 'a':
				nls[-1] = nls[-1]+l[1:]
			else:
				nls.append(l)
		return nls

	def dump (self):
		s = ''
		ln = ''

		if partcombine:
			s += ' \\set Score.soloText = #"1."\n'
			s += ' \\set Score.soloIIText = #"2."\n'
			s += ' \\set Score.aDueText = #"a 2"\n'

		pending_multi_rest = 0
		multi_rest = None
		ln_nl = -1
		for e in self.entries:

			next = ''
			# if isinstance(e, Measure_start):
				# sys.stderr.write("Dump measure start %s\n" % e.dump())
			if isinstance(e, Chord):
				if e.rest_type == 3:
					pending_multi_rest += 1
					multi_rest = 'R' + e.duration()
					# sys.stderr.write("Have a full-bar rest, now %d\n" % pending_multi_rest)
					continue

			if not isinstance(e, Measure_start) and pending_multi_rest > 0:
				# sys.stderr.write("Now a %s\n" % e.dump())
				next = ' ' + multi_rest + ("*%d" % pending_multi_rest)
				# sys.stderr.write("Dump %d full-bar rests\n" % pending_multi_rest)
				pending_multi_rest = 0

			next += ' ' + e.dump()
			if ln_nl == -1:
				ln_nl = 0
			next_nl = string.find(next, '\n')
			if next_nl == -1:
				next_nl = len(next)
			next_nl += len(ln) - ln_nl
			if next_nl > 80:
				s = s +ln +  '\n'
				ln = ''
			ln = ln + next
			ln_nl = string.rfind(ln, '\n')

		s = s + ln

		s = '\\notes {\n %s \n}\n' % s
		if self.current_tuplet != 1:
			s = s + "}\n"
		return s

	def parse_musical_directions (self,l):
		type = l[16:18]
		sys.stderr.write("Musical direction '%s'\n" % l)
		if len(l) > 18:
			s_location = l[18]
			s_spread = string.strip(l[20:23])
			word = l[24:]
		else:
			s_location = " "
			s_spread   = "0"
			word       = ""

		sys.stderr.write("Fixme: handle direction: type '%s' location '%s' spread '%s' word '%s'\n" % (type, s_location, s_spread, word))

		if s_location[0] == ' ':
			location = '_'
		else:
			location = '^'
		spread = 0
		if s_spread != '':
			spread = string.atoi(s_spread)

		dyn_font = 0
		s = ''
		for t in type:
			if t == 'A':
				sys.stderr.write("Fixme: insert 'segno'\n")
			elif t == 'B':
				s = word
			elif t == 'C':
				s = word
			elif t == 'D':
				s = word
			elif t == 'E':
				if spread == 0:
					self.append_pre_time += " \\<"
				else:
					self.append_pre_time += " \\>"
			elif t == 'F':
				self.append_pre_time += " \\!"
			elif t == 'G':
				dyn_font = 1
			elif t == 'H':
				sys.stderr.write("Fixme: insert dashes begin\n")
			elif t == 'J':
				sys.stderr.write("Fixme: insert dashes end\n")
			elif t == 'P':
				sys.stderr.write("Fixme: insert pedal begin")
			elif t == 'Q':
				sys.stderr.write("Fixme: insert pedal end")
			else:
				continue
		if s != '':
			if dyn_font == 1:
				self.append_pre_time += "%s\\markup { \\dynamic { %s } }" % (location, word)
			else:
				self.append_pre_time += "%s\\markup { %s }" % (location, word)


	def parse_body (self,lines):
		comment_switch = 0
		for l in lines:
			if not l:
				continue

			c = l[0]
			if c == '&':
				comment_switch = not comment_switch
				continue

			if comment_switch:
				continue

			if 0:
				pass
			elif c == '$':
				self.parse_musical_attributes (l)
			elif c == '@':
				self.parse_line_comment (l)
			elif c == '*':
				self.parse_musical_directions (l)
			elif c in 'ABCDEFGgri ':
				self.parse_note_line (l)
			elif c == 'm':
				if l[0:7] == 'measure':
					barnum = string.atoi(l[8:11])
					if barnum == 1 and self.current_div > 0 and self.current_div != 4 * self.enum * self.divs_per_q / self.denom:
						self.entries.append(Partial(self.current_div, self.divs_per_q))
				self.parse_measure_line (l)
			elif c == 'b':
				self.parse_backup_line(l)
			elif c == '/':
				break
			elif c in 'PS':
				pass			# ignore sound & print
			else:
				sys.stderr.write ("\nUnrecognized record `%s'\n"%  l)
		sys.stderr.write("\nEnd of lines: voices %d\n" % self.voices)
		while self.voices > 1:
			self.append_entry (VoiceJoiner())
			self.voices -= 1




def help ():
	sys.stdout.write (
"""Usage: musedata2ly [OPTIONS]... FILE1 [FILE2 ...]

Convert musedata to LilyPond.

Options:
  -h,--help          print this help
  -o,--output=FILE   set output filename to FILE
  -v,--version       show version information
  -r,--ref=REF       read background information from ref-file REF
  -p,--part          generate a file with only notes, no score info

Musedata (http://www.ccarh.org/musedata/) is an electronic library of
classical music scores, currently comprising XXX scores.  The music is
encoded in so-called Musedata format
(http://www.ccarh.org/publications/books/beyondmidi/online/musedata).
musedata2ly converts a set of musedata files to one .ly file, and will
include a \header field if a .ref file is supplied.

This converter is not complete -- this is left to the user as an excercise.

Report bugs to bug-lilypond@gnu.org.

Written by Han-Wen Nienhuys <hanwen@cs.uu.nl>.

""")


def print_version ():
	sys.stdout.write ("""musedata2ly (GNU LilyPond) %s

This is free software.  It is covered by the GNU General Public License,
and you are welcome to change it and/or distribute copies of it under
certain conditions.  Invoke as `midi2ly --warranty' for more information.

Copyright (c) 2000--2004 by Han-Wen Nienhuys <hanwen@cs.uu.nl>.
""" % version)
def identify():
	sys.stderr.write ("%s from LilyPond %s\n" % (program_name, version))


part_only = 0


(options, files) = getopt.getopt (sys.argv[1:], 'r:vo:hpt:', ['ref=', 'verbose', 'output=', 'help', 'part=', 'version'])
out_filename = None
ref_file = None
for opt in options:
	o = opt[0]
	a = opt[1]
	sys.stderr.write("Now consider option ['%s','%s']\n" % (o, a))
	if o== '--help' or o == '-h':
		help ()
		sys.exit (0)
	elif o == '--version' or o == '-v':
		print_version ()
		sys.exit(0)
	elif o == '--ref' or o == '-r':
		ref_file = a
	elif o == '--output' or o == '-o':
		out_filename = a
	elif o == '--verbose' :
		verbose = 1
	elif o == '-p' or o == '--part':
		part_only = 1
	elif o == '-t' or o == '--tuplet':
		(b, v) = split(a, '.')
		tuplet_break[string.atoi(v)] = string.atoi(b)
	else:
		print o
		raise getopt.error

identify()



ly = ''


found_ids = ''
staves = ''

for f in files:
	if f == '-':
		f = ''

	sys.stderr.write ('Processing `%s\'\n' % f)

	e = Parser(f)

	id = os.path.basename (f)
	id = re.sub ('[^a-zA-Z0-9]', 'x', id)

	def num2let (match):
		return chr (ord (match.group ()) - ord('0') + ord('A'))

	id = re.sub ('[0-9]', num2let, id)

	id = 'voice%s' % id
	ly =ly + '\n\n%s = %s\n\n' % (id, e.dump ())

	instr = ''
	if e.header_dict['instrument']:
		instr += ' \\set Staff.instrumentName = "'
		instr += string.strip(e.header_dict['instrument'])
		instr += '"\n'

	if not part_only:
		found_ids += '\\new Staff { %s \\set Score.skipBars = ##t \\%s }\n' % (instr, id)
	else:
		staves += '\\new Staff { %s \\set Score.skipBars = ##t \\%s }\n' % (instr, id)

sys.stderr.write("Header of (last) parse: '%s'\n" % e.header_dict)

ly_head = ''
if ref_file:
	head = Ref_parser (ref_file)
	if not out_filename:
		t = ''
		st = ''
		try:
			t = head.dict['title']
			st= head.dict['subtitle']
		except KeyError:
			pass

		t = t + '-' +st

		t = re.sub ("^ +(.*) +$", r"\1", t)
		t = re.sub ("\\.", '', t)
		out_filename = re.sub ('[^a-zA-Z0-9-]', '-', t)
		out_filename = out_filename+ '.ly'
	ly_head = head.dump ()

else:
	ly_head = '\\header {\n'
	if dict.has_key(e.header_dict, 'piece'):
		ly_head += '    piece = "' + string.strip(e.header_dict['piece']) + '"\n'
	if dict.has_key(e.header_dict, 'composer'):
		ly_head += '    composer = "' + string.strip(e.header_dict['composer']) + '"\n'
	if dict.has_key(e.header_dict, 'title'):
		ly_head += '    title = "' + string.replace(string.strip(e.header_dict['title']), '"', '\\"') + '"\n'
	if dict.has_key(e.header_dict, 'subtitle'):
		ly_head += '    subtitle = "' + string.strip(e.header_dict['subtitle']) + '"\n'
	ly_head += '}\n'

if not out_filename:
	out_filename = 'musedata.ly'

sys.stderr.write ('Writing `%s\'\n' % out_filename)

ly_version = '\n\\version "2.0.0"\n'

fo = open (out_filename, 'w')
fo.write ('%% lily was here -- automatically converted by musedata.ly\n')
if part_only:
	fo.write(ly_version + ly + found_ids)
else:
	found_ids = '\n\n\n\\score { << %s >> }\n' % found_ids
	fo.write(ly_version + ly_head + ly + found_ids)
fo.close ()

if part_only:
	part_filename = string.rstrip(out_filename, '.ly') + '_part.ly'
	fo = open (part_filename, 'w')
	fo.write ('%% lily was here -- automatically converted by musedata.ly\n')
	found_ids = '\n\\include "%s"\n\n\\score { << %s >> }\n' % (out_filename, staves)
	fo.write(ly_version + ly_head + found_ids)
	fo.close ()
