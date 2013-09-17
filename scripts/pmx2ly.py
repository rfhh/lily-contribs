#!@PYTHON@
# encoding=utf8

# PMX is a Musixtex preprocessor written by Don Simons, see
# http://www.gmd.de/Misc/Music/musixtex/software/pmx/

# TODO:
#  * block openings aren't parsed.

import os
import string
import sys
import re
import getopt
import argparse
from functools import partial

program_name = 'pmx2ly'
version = '@TOPLEVEL_VERSION@'
if version == '@' + 'TOPLEVEL_VERSION' + '@':
	version = '(unknown version)'	   # uGUHGUHGHGUGH


MAX_OCTAVE	= 10
OCTAVE		= 7

DIRECTION_NEUTRAL	= 0
DIRECTION_UP		= 1
DIRECTION_DOWN		= 2

FILL_SOLID		= 0
FILL_DOTTED		= 1
FILL_DASHED		= 2


warnings = []

def warn(string):
	if not string in warnings:
		sys.stderr.write(string)
		warnings.append(string)


def encodeint(i):
	return chr( i  + ord('A'))


actab = {
	-2: 'eses',
	-1: 'es',
	0 : '',
	1: 'is',
	2: 'isis'
}


def pitch_to_lily_string(tup):
	(o, n, a, f) = tup

	if n == 's':
		return n

	nm = chr((n + 2) % OCTAVE + ord('a'))
	nm = nm + actab[a]
	if o > 0:
		nm = nm + "'" * o
	elif o < 0:
		nm = nm + "," * -o
	if f & FLAG_CAUTIONARY:
		nm = nm + "?"
	if f & FLAG_FORCED:
		nm = nm + "!"
	if f & FLAG_SUPPRESS:
		warn("\n**** Refuse to really suppress the cautionary")
	return nm


def gcd(a, b):
	if b == 0:
		return a
	c = a
	while c:
		c = a % b
		a = b
		b = c
	return a


def rat_simplify(r):
	(n, d) = r
	if d < 0:
		d = -d
		n = -n
	if n == 0:
		return (0, 1)
	else:
		g = gcd(n, d)
		return (n/g, d/g)


def rat_multiply(a, b):
	(x, y) = a
	(p, q) = b

	return rat_simplify((x*p, y*q))


def rat_divide(a, b):
	(p, q) = b
	return rat_multiply(a, (q, p))


tuplet_table = {
	2: 3,
	3: 2,
	5: 4
}


def rat_add(a, b):
	(x, y) = a
	(p, q) = b

	return rat_simplify((x*q + p*y, y*q))


def rat_neg(a):
	(p, q) = a
	return (-p, q)


def rat_larger(a, b):
	return rat_subtract(a, b )[0] > 0


def rat_subtract(a, b ):
	return rat_add(a, rat_neg(b))


class Bar:
	def __init__(self, text):
		self.text = text

	def dump(self):
		return ' \\bar "' + self.text + '"\n   '


class Repeat:
	def __init__(self):
		self.total_volte = 0
		self.closed_volte = 0

	def begin_volta(self):
		self.total_volte = self.total_volte + 1

	def end_volta(self):
		self.closed_volte = self.closed_volte + 1

	def dump(self):
		if self.total_volte == 0:
			volta = 2
		else:
			volta = self.total_volte
		return " \\repeat volta " + str(volta) + " {\n   "


class RepeatClose:
	def __init__(self, repeat):
		self.repeat = repeat

	def dump(self):
		close = " }\n    "
		if self.repeat.total_volte > 0:
			close = close + "\\alternative {\n   "
		return close


class Volta:
	def __init__(self, repeat, text):
		repeat.begin_volta()
		self.text = text

	def dump(self):
		return "{ % open volta\n   "


class VoltaClose:
	def __init__(self, repeat):
		self.repeat = repeat
		self.count = repeat.total_volte
		repeat.end_volta()

	def dump(self):
		close = " } % close volta\n    "
		if self.count == self.repeat.total_volte:
			close = close + "} % close alternative\n   "
		return close


class Break:
	def __init__(self, type):
		self.type = type

	def dump(self):
		return ' ' + self.type + '\n   '


class Barcheck :
	def __init__(self):
		pass

	def dump(self):
		return ' |'


class Barnumber :
	def __init__(self, number, meter):
		self.number = number
		self.meter = meter

	def dump(self):
		return ' | % ' + str(self.number + 1) + '\n   '


class Meter :
	def __init__(self, num, denom):
		if denom == 3:
			raise Exception('weird denom', basic_duration)
		self.num = num
		self.denom = denom

	def to_rat(self):
		return rat_simplify((self.num, self.denom))

	def dump(self):
		return ' \\time ' + str(self.num) + "/" + str(self.denom) + '\n   '


class Beam:
	def __init__(self, ch):
		self.char = ch

	def dump(self):
		return self.char


class Tie:
	def __init__(self, id):
		self.id = id
		self.start_chord = None
		self.end_chord = None

	def calculate(self):
		s = self.start_chord
		e = self.end_chord

		if e and s:
			s.chord_suffix = s.chord_suffix + '~'
			# check if the first note is altered. If so, copy the
			# alteration to the second note.
			for p in s.pitches:
				(op, np, ap, fp) = p
				for i in range(len(e.pitches)):
					(oq, nq, aq, fq) = e.pitches[i]
					if op == oq and np == nq and ap != aq:
						e.pitches[i] = (oq, nq, ap, fq)
		else:
			if s:
				s.chord_suffix = s.chord_suffix + '~'
			warn("\nOrphaned tie")


class Slur:
	def __init__(self, id):
		self.id = id
		self.start_chord = None
		self.end_chord = None

	def calculate(self):
		s = self.start_chord
		e = self.end_chord

		if e and s:
			s.note_suffix = s.note_suffix + '('
			e.note_suffix = e.note_suffix + ')'
		else:
			warn("\nOrphaned slur")


class Grace:
	def __init__(self, items, item_duration, slashed, slurred, after, direction):
		self.items = items
		self.item_duration = item_duration
		self.pending = items
		self.slashed = slashed
		self.slurred = slurred
		self.after = after
		self.direction = direction
		self.start_chord = None
		self.end_chord = None

	def calculate(self):
		s = self.start_chord
		e = self.end_chord

		if e and s:
			if self.after:
				warn('\nFIXME: for \\afterGrace, invert the grace and the graced note');
			elif self.slashed:
				if self.slurred:
					s.note_prefix = s.note_prefix + '\\acciaccatura { '
				else:
					s.note_prefix = s.note_prefix + '\\slashedGrace { '
			elif self.slurred:
				s.note_prefix = s.note_prefix + '\\appoggiatura { '
			else:
				s.note_prefix = s.note_prefix + '\\grace { '
			if self.items > 1:
				s.note_suffix = s.note_suffix + '['
				e.note_suffix = e.note_suffix + ']'
			e.note_suffix = e.note_suffix + ' }'
		else:
			warn("\nOrphaned grace")


class Melisma:
	def __init__(self, is_start):
		self.is_start = is_start

	def dump(self):
		if self.is_start:
			return " \\melisma"
		else:
			return " \\melismaEnd \\autoBeamOff"


class Voice:
	def __init__(self):
		self.entries = []
		self.chords = []
		self.staff = None
		self.meter = None
		self.pickup = None

		self.time = (0, 1)
		self.bar = 0
		self.preset_id = None
		self.nonempty = False
		self.repeats = []
		self.last_name = 0
		self.last_oct = 0
		self.lyrics = ''
		self.lyrics_label = None

		self.current_slurs = []
		self.slurs = []
		self.pending_slur = None

		self.current_ties = []
		self.ties = []
		self.pending_tie = None

		self.current_grace = None
		self.pending_grace = None
		self.graces = []

		self.volta = []
		self.pending_volta = None

		self.pending_beam = False
		self.pending_melisma = None


	def set_preset_id(self, preset):
		self.preset_id = preset

	def set_meter(self, meter, pickup):
		self.meter = meter
		self.pickup = pickup
		(pd, pn) = pickup
		if pd != 0:
			self.time = rat_subtract(meter.to_rat(), pickup)
			self.bar = -1
		(d, n) = self.time
		# sys.stderr.write("set_meter(): time %d / %d\n" % (d, n))

	def start_tie(self, id):
		if self.pending_tie != None:
			raise Exception("Already have a pending tie", "")
		self.pending_tie = Tie(id)

	def replace_tie(self, slur):
		tie = Tie(slur.id)
		tie.start_chord = slur.start_chord
		tie.end_chord = self.chords[-1]
		self.ties.append(tie)

	def end_tie(self, id):
		for tie in self.current_ties:
			if tie.id == id:
				self.current_ties.remove(tie)
				tie.end_chord = self.chords[-1]
				return True
		return False

	def toggle_tie(self, id):
		if not self.end_tie(id):
			tie = Tie(id)
			tie.start_chord = self.chords[-1]
			self.current_ties.append(tie)
			self.ties.append(tie)

	def start_slur(self, id):
		self.pending_slur = Slur(id)

	def remove_slur(self, id):
		for s in self.current_slurs:
			if s.id == id:
				self.current_slurs.remove(s)
				self.slurs.remove(s)
				return s
		return None

	def end_slur(self, id):
		for s in self.current_slurs:
			if s.id == id:
				self.current_slurs.remove(s)
				s.end_chord = self.chords[-1]
				return True
		return False

	def toggle_slur(self, id):
		if not self.end_slur(id):
			s = Slur(id)
			s.start_chord = self.chords[-1]
			self.current_slurs.append(s)
			self.slurs.append(s)

	def handle_pending(self):
		if self.pending_slur:
			s = self.pending_slur
			s.start_chord = self.chords[-1]
			self.current_slurs.append(s)
			self.slurs.append(s)
			self.pending_slur = None
		if self.pending_tie:
			tie = self.pending_tie
			tie.start_chord = self.chords[-1]
			self.current_ties.append(tie)
			self.ties.append(tie)
			self.pending_tie = None
		if self.pending_grace:
			s = self.pending_grace
			# sys.stderr.write("Handle pending grace, items %d pending %d\n" % (s.items, s.pending))
			s.pending = s.pending - 1
			if s.pending == 0:
				s.end_chord = self.chords[-1]
				self.current_grace = s
				self.pending_grace = None
			if s.pending == s.items - 1:
				s.start_chord = self.chords[-1]
				self.graces.append(s)
		if self.pending_beam:
			ch = self.chords[-1]
			ch.chord_suffix = ch.chord_suffix + "["
			self.pending_beam = False
		if self.pending_melisma:
			ch = self.chords[-1]
			ch.chord_prefix = ch.chord_prefix + "\\autoBeamOn "
			self.add_nonchord(self.pending_melisma)
			self.pending_melisma = None

	def last_chord(self):
		return self.chords[-1]

	def add_chord(self, ch):
		self.chords.append(ch)
		self.entries.append(ch)

	def add_nonchord(self, nch):
		self.entries.append(nch)

	def add_skip_bar(self, count, basic_duration):
		ch = Chord()
		self.add_chord(ch)
		ch.multibar = 1
		ch.skip = True
		ch.count = count
		ch.basic_duration = basic_duration
		ch.time = (0, 1)

	def idstring(self):
		if self.preset_id:
			return self.preset_id
		else:
			return 'staff%svoice%s ' % (encodeint(self.staff.number) , encodeint(self.number))


	def add_mark(self, elevation, text):
		ch = Chord()
		self.add_chord(ch)
		ch.basic_duration = self.meter.denom
		ch.count = 0
		ch.skip = True
		ch.time = self.time
		ch.bar  = self.bar
		if int(elevation) >= 0:
			direction = ''
		else:
			direction = '\\once \\override Score.RehearsalMark #\'direction = #DOWN '
		text = re.sub('~', '\char ##x00A0 ', text)
		text = re.sub(r'\\', r'\\\\', text)
		ch.chord_suffix = ch.chord_suffix + ' ' + direction + "\\mark \\markup{\"" + text + "\"}"


	def dump(self):
		out = ''
		ln = ''
		for e in self.entries:
			next = e.dump()
			if next != '' and next[-1] == '\n':
				out = out + ln + next
				ln = ''
				continue

			if 0 and len(ln) + len(next) > 72:
				out = out + ln + '\n'
				ln = ''
			ln = ln + next


		out = out  + ln
		id = self.idstring()

		lyrics = ''
		if self.lyrics != '':
			out = ' \\autoBeamOff\n\n   ' + out
			lyrics = '\n\\addlyrics {\n    %s\n}' % self.lyrics
		if self.preset_id:
			if self.pickup:
				out = (' \\partial %d*%d\n   ' % (self.pickup[1], self.pickup[0])) + out
			out = '%s = {\n   %s}%s\n' % (self.preset_id, out, lyrics)
		else:
			out = '%s = <<\n{\n   %s\n}\n%s\n\\timeLine\n>>\n'% (id, out, lyrics)
		return out


	def calculate_multibar(self):
		first = True
		for c in self.chords:
			if first:
				first = False
			elif c.multibar > 0 and p.multibar > 0 and c.skip == p.skip and c.basic_duration == p.basic_duration and c.count == p.count:
				p.suppressed = True
				c.multibar = c.multibar + p.multibar
			p = c


	def calculate_graces(self):
		lastgr = 0
		lastc = None
		for c in self.chords:
			if c.grace and  not lastgr:
				sys.stderr.write("Handle grace prefix...\n");
				c.chord_prefix = c.chord_prefix + '\\grace { '
			elif not c.grace and lastgr:
				sys.stderr.write("Handle grace suffix...\n");
				lastc.chord_suffix = lastc.chord_suffix + ' } '
			lastgr = c.grace
			lastc = c


	def calculate(self):
		# sys.stderr.write("Skip calculating graces\n")
		# self.calculate_graces()
		self.calculate_multibar()
		for s in self.slurs:
			s.calculate()
		for t in self.ties:
			t.calculate()
		for g in self.graces:
			g.calculate()
		for s in self.chords:
			if not s.skip:
				self.nonempty = True
				break


	def compact_multibar(self):
		compacted = []
		multi = 0
		last_bar = None
		last_chord = None
		def new_multi():
			if multi > 0:
				ch = Chord()
				ch.multibar = multi
				ch.skip = last_chord.skip
				ch.basic_duration = last_chord.basic_duration
				ch.count = last_chord.count
				ch.bar = last_chord.bar
				ch.time = last_chord.time
				compacted.append(ch)
				if last_bar:
					compacted.append(last_bar)

		for b in self.entries:
			if isinstance(b, Chord) and b.count > 0 and last_chord != None and b.count == last_chord.count and b.skip == last_chord.skip:
				multi = multi + 1
				last_chord = b
			elif isinstance(b, Barnumber):
				last_bar = b
			# elif isinstance(b, Meter):
			else:
				new_multi()
				multi = 0
				compacted.append(b)

		new_multi()

		# sys.stderr.write("\nReplace timeline.entries with compacted[%d]" % len(compacted))
		self.entries = compacted


class Clef:
	def __init__(self, cl):
		self.type = cl

	def dump(self):
		return ' \\clef ' + self.type + '\n   '

key_table = {
	'+0':'c \major',
	'+1':'g \major',
	'+2':'d \major',
	'+3':'a \major',
	'+4':'e \major',
	'+5':'b \major',
	'+6':'fis \major',
	'-1':'f \major',
	'-2':'bes \major',
	'-3':'ees \major',
	'-4':'aes \major',
	'-5':'des \major',
	'-6':'ges \major'
	}


class Key:
	def __init__(self, key, bar, time):
		self.key = key
		self.bar  = bar
		self.time = time

	def dump(self):
		if self.key >= 0:
			key = '+' + str(self.key)
		else:
			key = '-' + str(-self.key)
		return ' \\key ' + key_table[key] + '\n   '


class Alteration:
	def __init__(self, note, octave, alteration, flags, bar, time, mode):
		# if alteration == 'ff' or alteration == 'ss':
		#	sys.stderr.write("\nSee alteration %s" % alteration)
		if note == None:
			raise Exception("Cannot have an Alteration without a note name", note)
		if octave == None:
			raise Exception("Cannot have an Alteration without an octave", note)
		self.note = note
		self.octave = octave
		self.alteration = alteration
		self.flags = flags
		self.bar = bar
		self.time = time
		self.accidental_mode = mode

	def dump(self):
		(n, d) = self.time
		return 'bar=%d t=%d/%d note=%d alt=%s flags=%d' % (self.bar, n, d, self.note, self.alteration, self.flags)

class AlterationReset:
	def __init__(self, bar, time):
		self.bar = bar
		self.time = time

	def dump(self):
		return '<AlterationReset> bar=%d t=%d/%d' % (self.bar, self.time[0], self.time[1])


clef_table = {
	'b':'bass'  ,
	'r':'baritone',
	'n':'tenor',
	'a':'alto',
	'm':'mezzosoprano',
	's':'soprano',
	't':'treble',
	'f':'frenchviolin',
	'0': 'treble',
	'1': 'soprano',
	'2': 'mezzosoprano',
	'3': 'alto',
	'4': 'tenor',
	'5': 'baritone',
	'6': 'bass',
	'7': 'frenchviolin'
	}


class Staff:
	def __init__(self):
		self.voices = (Voice(), Voice())
		self.clef = None
		self.voice_idx = 0
		self.number = None
		self.key = 0
		self.alterations = []
		self.alteration_spans_octaves = False
		self.accidental_mode = ACCIDENTAL_ABSOLUTE
		self.lyrics_voice = 0
		self.lyrics = ''
		self.instrument_name = None

		i = 0
		for v  in self.voices:
			v.staff = self
			v.number = i
			i = i+1

	def set_accidental_mode(self, mode):
		self.accidental_mode = mode

	def set_clef(self, letter):
		if clef_table.has_key(letter):
			clstr = clef_table[letter]
			self.voices[0].add_nonchord(Clef(clstr))
		else:
			sys.stderr.write("Clef type `%c' unknown\n" % letter)

	def set_key(self, keysig):
		self.key = keysig
		self.alterations.append(Key(keysig, 0, (0, 1)))
		self.voices[0].add_nonchord(Key(keysig, 0, (0, 1)))

	def set_alteration(self, note, octave, alteration, flags, time):
		self.alterations.append(Alteration(note, octave, alteration, flags,
						   self.current_voice().bar, time,
						   self.accidental_mode))

	def reset_alterations(self):
		self.alterations.append(AlterationReset(self.current_voice().bar,
							self.current_voice().time))

	def current_voice(self):
		return self.voices[self.voice_idx]

	def next_voice(self):
		self.voice_idx = (self.voice_idx + 1) % len(self.voices)


	def calculate_alteration(self, voice):
		# sys.stderr.write("traverse the voice to calculate the alterations\n")
		alteration = [[0] * MAX_OCTAVE for i in range(OCTAVE)]
		default_alteration = [[0] * MAX_OCTAVE for i in range(OCTAVE)]
		alterations = sorted(self.alterations, key = lambda alt: (alt.bar, alt.time[0] / (1.0 * alt.time[1])))
		alt_index = 0
		for ch in voice.chords:
			# sys.stderr.write("\nConsider chord %s at bar=%d time=%d/%d" % (ch.dump(), ch.bar, ch.time[0], ch.time[1]))
			while alt_index < len(alterations):
				a = alterations[alt_index]
				if a.bar > ch.bar or \
					(a.bar == ch.bar and rat_larger(a.time, ch.time)):
					break
				# sys.stderr.write("\nConsider alteration[%d] %d:(%d/%d) %s" % (alt_index, a.bar, a.time[0], a.time[1], a.dump()))
				if False:
					pass
				elif isinstance(a, Key):
					for alt in range(OCTAVE):
						for o in range(MAX_OCTAVE):
							default_alteration[alt][o] = 0
					alt = 6
					for k in range(a.key):
						alt = alt + 4
						for o in range(MAX_OCTAVE):
							default_alteration[alt % OCTAVE][o] = (k + OCTAVE) / OCTAVE
					alt = 3
					for k in range(-a.key):
						alt = alt + 3
						for o in range(MAX_OCTAVE):
							default_alteration[alt % OCTAVE][o] = -((k + OCTAVE) / OCTAVE)
					for k in range(OCTAVE):
						for o in range(MAX_OCTAVE):
							alteration[k][o] = default_alteration[k][o]
				elif isinstance(a, AlterationReset):
					for k in range(OCTAVE):
						for o in range(MAX_OCTAVE):
							alteration[k][o] = default_alteration[k][o]
				elif isinstance(a, Alteration):
					c = a.alteration
					if False:
						pass
					elif c[0] == 'f':
						alt = -1
						if c == 'ff':
							alt = -2
					elif c == 'n':
						alt = 0
					elif c[0] == 's':
						alt = 1
						if c == 'ss':
							alt = 2
					else:
						raise Exception('unknown accidental', c)
					if a.accidental_mode == ACCIDENTAL_RELATIVE:
						if self.alteration_spans_octaves:
							for o in range(MAX_OCTAVE):
								alteration[a.note][o] = default_alteration[a.note][o] + alt
						else:
							alteration[a.note][a.octave] = default_alteration[a.note][o] + alt
					else:
						if self.alteration_spans_octaves:
							for o in range(MAX_OCTAVE):
								if alt != alteration[a.note][o]:
									alteration[a.note][o] = alt
						else:
							if alt != alteration[a.note][a.octave]:
								alteration[a.note][a.octave] = alt
				alt_index = alt_index + 1

			for p in range(len(ch.pitches)):
				if not ch.skip:
					(o, n, a, f) = ch.pitches[p]
					a = alteration[n][o]
					ch.pitches[p] = (o, n, a, f)


	def calculate(self):
		for v in self.voices:
			self.calculate_alteration(v)
		for v in self.voices:
			v.calculate()


	def idstring(self):
		return 'staff%s' % encodeint(self.number)


	def dump(self):
		nonempty = 0
		for v in self.voices:
			if v.nonempty:
				nonempty = nonempty + 1

		if nonempty == 1:
			out = '\n\n' + self.voices[0].dump()
			refs = '\n    \\' + self.voices[0].idstring()
		else:
			out = ''
			refs = ''
			for v in self.voices:
				out = out + "\n\n" + v.dump()
			for v in self.voices:
				voice = '\n    \\' + v.idstring()
				if v != self.voices[0]:
					refs = voice + ' \\\\' + refs
				else:
					refs = voice + refs

		instr = ''
		if self.instrument_name:
			instr = '\n    \set Staff.instrumentName = \markup{' + self.instrument_name + '}'
		out = out + '\n\n%s = \\new Staff = %s <<%s%s\n>>\n\n' % (self.idstring(), self.idstring(), instr, refs)
		return out


class Tuplet:
	def __init__(self, number, base, dots):
		self.chords = []
		self.number = number
		if dots == 1:
			self.replaces = 3
		elif dots == 2:
			self.replaces = 7
		else:
			self.replaces = 1
			while 2 * self.replaces < number:
				self.replaces = 2 * self.replaces
		self.base = base
		self.dots = dots

		# <n>x<T>
		# algorithm: iterate over 2-powers <p> while just holds <T> / <b> >= 1 <n>
		# rewrite to: while just holds <b> <= <n> <T>
		nT = (base * number, 1)
		while dots > 0:
			nT = rat_multiply(nT, (2, 3))
			dots = dots - 1
		note_base = 1
		while not rat_larger((note_base, 1), nT):
			note_base = note_base * 2
		note_base = note_base / 2
		self.note_base = note_base

	def add_chord(self, ch):
		ch.dots = 0
		ch.basic_duration = self.note_base
		self.chords.append(ch)

		if len(self.chords) == 1:
			ch.chord_prefix = '\\times %d/%d { ' % (self.replaces, self.number)
		# elif len(self.chords) == self.number:
		# 	ch.chord_suffix = ' }'

	def dump(self):
		return ' \\times %d/%d { ' % (self.replaces, self.number)


class TupletEnd:
	def dump(self):
		return '} '


FLAG_CAUTIONARY = 0x1 << 0
FLAG_SUPPRESS   = 0x1 << 1
FLAG_FORCED     = 0x1 << 2

ACCIDENTAL_ABSOLUTE = 0
ACCIDENTAL_RELATIVE = 1

PAPER_A4     = 0
PAPER_LETTER = 1

class Chord:
	def __init__(self):
		self.pitches = []
		self.dots = 0
		self.count = 1
		self.basic_duration = 0
		self.scripts = []
		self.grace = 0
		self.chord_prefix = ''
		self.chord_suffix = ''
		self.note_prefix = ''
		self.note_suffix = ''
		self.multibar = 0
		self.skip = False
		self.bar = -1
		self.time = []
		self.suppressed = False

	def dump(self):
		if self.suppressed:
			return ''

		if self.multibar != 0:
			if self.grace:
				sys.stderr.write('Ooppsss... multibar grace?????\n')
			if self.skip:
				rest = 's'
			else:
				rest = 'R'
			if self.count != 1:
				multi = str(self.count) + "*" + str(self.multibar)
			else:
				multi = str(self.multibar)
			v = self.chord_prefix + ' ' + rest + str(self.basic_duration) + "*" + multi + self.chord_suffix
			return v

		v = ''
		sd = ''
		if self.basic_duration == 0.5:
			sd = '\\breve'
		else:
			sd = '%d' % self.basic_duration
		if self.count != 1:
			sd = sd + "*" + str(self.count)
		sd = sd + '.' * self.dots
		for p in self.pitches:
			if v:
				v = v + ' '
			v = v + pitch_to_lily_string(p)

		if self.skip:
			v = v + 's'
		elif len(self.pitches) > 1:
			v = '<%s>' % v
		elif len(self.pitches) == 0:
			v = 'r'

		v = v + sd
		for s in self.scripts:
			v = v + s

		v = self.note_prefix + v + self.note_suffix
		if self.grace:
			sys.stderr.write('chord is grace: %s %s %s\n' % (self.chord_prefix, v, self.chord_suffix))
		v = ' ' + self.chord_prefix + v + self.chord_suffix

		return v


SPACE=' \t\n'
DIGITS ='0123456789'
basicdur_table = {
	9: 0.5,
	7: 1 ,
	2: 2 ,
	4: 4 ,
	8: 8 ,
	1: 16,
	3: 32,
	6: 64
	}

halftime_table = {
	0.5:	7,
	1:	2,
	2:	4,
	4:	8,
	8:	1,
	16:	3,
	32:	6,
	}


ornament_table = {
	't': '\\prall',
	'm': '\\mordent',
	'x': '"x"',
	'+': '+',
	'u': '"pizz"',
	'p': '|',
	'(': '"paren"',
	')': '"paren"',
	'g': '"segno"',
	'.': '.',
	'fd': '\\fermata',
	'f': '\\fermata',
	'_': '-',
	'T': '\\trill',
	'>': '>',
	'^': '^',
	}


figure_table = [
	'zero',
	'one',
	'two',
	'three',
	'four',
	'five',
	'six',
	'seven',
	'eight',
	'nine',
	]


class M_TX:
	def __init__(self):
		poet = None
		composer = None
		title = None


class Parser:
	def __init__(self):
		self.staffs = []
		self.timeline = Voice()
		self.timeline.set_preset_id('timeLine')
		self.forced_duration = None
		self.tuplets_expected = 0
		self.tuplets = []
		self.last_basic_duration = 4
		self.last_tuplet_duration = None
		self.meter = None
		self.pickup = (0, 1)
		self.keysig = 0
		self.title = ''
		self.composer = ''
		self.instrument = ''
		self.accidental_mode = ACCIDENTAL_ABSOLUTE
		self.paper_size = PAPER_A4
		self.current_repeat = None
		self.last_repeat = None
		self.volta = None
		self.instruments = []
		self.staff_groups = []
		self.lyrics = {}
		self.defined_fonts = []
		self.mtx = M_TX()

		self.musicsize = 20

		self.tex_dispatch_table()


	def set_staffs(self, number):
		self.staffs = map(lambda x: Staff(), range(0, number))

		self.staff_idx = 0

		# self.timeline.set_meter(self.meter, self.pickup)
		# self.timeline.add_nonchord(meter)
		i = 0
		for s in self.staffs:
			s.number = i
			s.set_key(self.keysig)
			s.set_accidental_mode(self.accidental_mode)
			for v in s.voices:
				v.set_meter(self.meter, self.pickup)
			i = i+1


	def current_staff(self):
		return self.staffs[self.staff_idx]


	def current_voice(self):
		return self.current_staff().current_voice()


	def next_staff(self):
		self.staff_idx = (self.staff_idx + 1)% len(self.staffs)
		self.current_staff().voice_idx = 0


	def add_skip_bars(self, voice, target_bar):
		bars = target_bar - voice.bar
		for b in range(bars):
			if voice.bar + b == -1:
				# special case: we are in pickup time
				voice.add_skip_bar(self.pickup[0], self.pickup[1])
			else:
				voice.add_skip_bar(self.meter.num, self.meter.denom)
			voice.add_nonchord(Barnumber(voice.bar + b, self.meter))
		voice.time = (0, 1)
		voice.bar += bars
		# (d, s) = voice.time
		# sys.stderr.write("%s add_skip_bars(%d): bar %d target %d time %d/%d\n" % (voice.idstring(), bars, voice.bar, target_bar, d, s))


	def catch_up(self, voice, target_bar):
		# sys.stderr.write("%s: catch up from %d to bar %d\n" % (voice.idstring(), voice.bar, target_bar))
		meter = self.meter
		size = len(self.timeline.entries)
		for b in reversed(self.timeline.entries):
			if isinstance(b, Barnumber):
				# sys.stderr.write("  -- bar number %d (voice.bar %d)\n" % (b.number, voice.bar))
				if b.number < voice.bar:
					break
			elif isinstance(b, Meter):
				pass
				# sys.stderr.write("  -- meter\n")
				# meter = b
			elif isinstance(b, Chord):
				pass
				# sys.stderr.write("  -- chord, multibar %d\n" % b.multibar)
				# bar += b.multibar
				# if voice.bar < target_bar:
					# voice.add_skip_bar(self.meter.num, self.meter.denum)
					# voice.bar = voice.bar + b.multibar
			elif isinstance(b, Repeat):
				pass
			elif isinstance(b, RepeatClose):
				pass
			elif isinstance(b, Volta):
				pass
			elif isinstance(b, VoltaClose):
				pass
			elif isinstance(b, Bar):
				pass
			elif isinstance(b, Break):
				pass
			else:
				sys.stderr.write("What is this: %s\n" % b.dump())
			size = size - 1

		for b in self.timeline.entries[size:]:
			if isinstance(b, Barnumber):
				barnumber = Barnumber(b.number, b.meter)
				# sys.stderr.write("  -- add empty bar %d length %d/%d\n" % (barnumber.number, b.meter.num, b.meter.denum))
				voice.add_skip_bar(self.meter.num, self.meter.denom)
				voice.add_nonchord(barnumber)
		voice.bar = target_bar


	def parse_grace(self, left):
		left = left[1:]
		slashed = 0
		slurred = 0
		items = -1
		after = 0
		direction = 0
		item_duration = 8
		# process the grace note options
		while left[0] in '0123456789msxluAWX':
			c = left[0]
			left = left[1:]
			if c == 'm':
				if not left[0] in '0123456789':
					warn("""
Huh? expected number of grace note beams, found %s Left was `%s'""" % (left[0], left[:20]))
				else:
					item_duration = 4 << (ord(left[0]) - ord('0'))
					left = left[1:]
			elif c == 's':
				slurred = 1
			elif c == 'x':
				slashed = 1
			elif c == 'l':
				direction = -1
			elif c == 'u':
				direction = 1
			elif c in 'AW':
				after = 1
			elif c == 'X':
				warn("\nIgnore: no horizontal shift in grace");
				while left[0] in ".0123456789":
					left = left[1:]
			else:
				if not c in '0123456789':
					warn("""
Huh? expected number of grace notes, found %s Left was `%s'""" % (c, left[:20]))
				elif items == -1:
					items = ord(c) - ord('0')
				else:
					items = 10 * items + ord(c) - ord('0')
		if items == -1:
			items = 1
		# sys.stderr.write("Detect grace items %d\n" % items)
		grace = Grace(items, item_duration, slashed, slurred, after, direction)

		return (left, grace)


	def parse_rest(self, left):
		multibar = 0
		left = left[1:]
		while left[0] in 'mp':
			c = left[0]
			left = left[1:]
			if False:
				pass
			elif c == 'm':
				multibar = 1
			elif c == 'p':
				multibar = 1
				if left[0] == 'o':
					# ignore off-center attribute
					left = left[1:]

		return (left, multibar)


	def parse_alteration(self, c, left):
		alteration = c
		if c == left[0]:
			alteration = c + c
			left = left[1:]
		alteration_flags = 0
		while left[0] in '+-<>ciA':
			c = left[0]
			left = left[1:]
			if False:
				pass
			elif c in '+-':
				warn("\nIgnore: no alteration shift")
				m = re.match('\A([0-9]+)([+-][0-9]+)', left)
				if not m:
					left = c + left
					break	# the +- is not an alteration, it's an octave
				left = left[len(m.group()):]
			elif c in '<>':
				warn("\nIgnore: no alteration shift")
				if not left[0] in '-.0123456789':
					raise Exception("alteration shift malformed", c + left[0])
				while left[0] in '-.0123456789]':
					left = left[1:]
			elif c == 'c':
				alteration_flags = alteration_flags | FLAG_CAUTIONARY;
			elif c == 'i':
				alteration_flags = alteration_flags | FLAG_SUPPRESS;
			elif c == 'A':
				warn("\nFIXME: accidental post order")
				if left[0] == 'o':
					left = left[1:]

		return (left, alteration, alteration_flags)


	def parse_tuplet(self, left, basic_duration, dots):
		first_dots = 0
		m = re.match(r'\A\d\d?', left)
		tupnumber = string.atoi(m.group())
		left = left[len(m.group()):]
		subs = None
		while left[0] in 'dn':
			c = left[0]
			left = left[1:]
			if False:
				pass
			elif c == 'd':
				first_dots = 1
				warn("\nFIXME: handle dotted first note under tuplet")
			elif c == 'n':
				if left[0] in SPACE:
					warn("\nFIXME: handle tuplet without number")
					subs = 0
				else:
					while left[0] in '+-0123456789fs':
						if left[0] == 'f':
							warn("\nIgnore: tuplet number flip")
							left = left[1:]
						elif left[0] == 's':
							m = re.match(r'\A[.0-9]+', left)
							if m:
								warn("\nIgnore: tuplet slope")
								left = left[len(m.group()):]
							else:
								raise Exception("Tuplet slope should carry a number", left[:20])
						else:
							m = re.match(r'\A\d+', left)
							if m:
								warn("\nIgnore: tuplet number subst")
								subs = atoi(m.group())
								left = left[len(m.group()):]
							else:
								m = re.match(r'\A([+-][.0-9]+)(([+-][.0-9]+))', left)
								if m:
									warn("\nIgnore: no tuplet shape shifts")
									left = left[len(m.group()):]

		tup = Tuplet(tupnumber, basic_duration, dots)
		self.tuplets_expected = tupnumber
		self.tuplets.append(tup)
		# self.current_voice().add_nonchord(tup)

		return (left, tup, dots)


	def parse_note(self, left):
		name = None
		ch = None

		grace = None
		if left[0] == 'G':
			(left, grace) = self.parse_grace(left)

		v = self.current_voice()
		if left[0] == 'z':
			chord_continuation = True
			ch = v.last_chord()
			left = left[1:]
		else:
			chord_continuation = False
			ch = Chord()
			ch.time = v.time
			ch.bar = v.bar
			v.add_chord(ch)

		if grace:
			v.pending_grace = grace
		pending_grace = v.pending_grace
		if pending_grace:
			basic_duration = pending_grace.item_duration

		v.handle_pending()

		durdigit = None
		count = 1
		forced_duration  = 0
		dots = 0
		octave = None
		alteration = None
		alteration_flags = 0
		extra_oct = 0
		flats = 0
		sharps = 0

		# what about 's'?
		if left[0] == 'r':
			# sys.stderr.write("Process note: rest from '%s'" % left[:20])
			(left, multibar) = self.parse_rest(left)
		else:
			multibar = 0
			if left[0] in '.,':
				left = left[1:]
				try:
					durdigit = halftime_table[self.last_basic_duration]
				except KeyError:
					sys.stderr.write("""
Huh? expected duration; last_basic_duration is `%d'""" % self.last_basic_duration)
			name = (ord(left[0]) - ord('a') + 5) % 7
			# sys.stderr.write("Process note '%s' name '%d'\n" % (left[0], name))
			left = left[1:]

		while left[0] in '0123456789dsfnuleraber+-.,SDF':
			c = left[0]
			left = left[1:]
			if False:
				pass
			elif c in DIGITS and durdigit == None and \
					self.tuplets_expected == 0:
				if multibar != 0:
					bars = c
					while left[0] in DIGITS:
						bars = bars + left[0]
						left = left[1:]
					multibar = string.atoi(bars)
				else:
					durdigit = string.atoi(c)
					if durdigit == 0:
						durdigit = 7
			elif c in DIGITS:
				octave = string.atoi(c) - 3
			elif c == 'd':
				dots = dots + 1
				m = re.match(r'\A([+-][.\d]+)([+-][.\d]+)?', left)
				if m:
					warn("\nIgnore: no dot horizontal/vertical shift")
					left = left[len(m.group()):]
			elif c in 'sfn':
				(left, alteration, alteration_flags) = self.parse_alteration(c, left)
			elif c in 'ul':
				warn("\nFIXME: stem direction")
			elif c == 'a':
				ch.chord_suffix = ch.chord_suffix + '\\noBeam'
			elif c == 'b':
				ch.skip = True
			elif c in 'er':
				warn("\nIgnore: no note horizontal shift")
			elif c == '+':
				extra_oct = extra_oct + 1
			elif c == '-':
				extra_oct = extra_oct - 1
			elif c == '.':
				dots = dots + 1
				left = c + left
				forced_duration = 3
				break
			elif c == ',':
				left = c + left
				forced_duration = 2
				break
			elif c == 'S':
				warn("\nFIXME: stem length")
				while left[0] in DIGITS:
					left = left[1:]
				if left[0] == ':':
					left = left[1:]
			elif c == 'D':
				warn("\nFIXME: double duration within tuplet")
			elif c == 'F':
				warn("\nFIXME: dotted double duration within tuplet")

		if multibar != 0:
			count = self.meter.num
			basic_duration = self.meter.denom
		elif durdigit:
			try:
				basic_duration =  basicdur_table[durdigit]
			except KeyError:
				sys.stderr.write("""
Huh? expected duration, found %d Left was `%s'""" % (durdigit, left[:20]))

				basic_duration = 4
		elif not pending_grace:
			basic_duration = self.last_basic_duration

		if not pending_grace:
			self.last_basic_duration = basic_duration

		if left[0] == 'x':
			left = left[1:]
			(left, tup, first_dots) = self.parse_tuplet(left, basic_duration, dots)
			dots = 0
		if self.tuplets_expected > 0:
			self.last_tuplet_duration = basic_duration
			basic_duration = self.tuplets[-1].note_base

		if name <> None and octave == None:
			e = 0
			if v.last_name < name and name - v.last_name > 3:
				e = -1
			elif v.last_name > name and v.last_name - name > 3:
				e = 1

			octave = v.last_oct  + e + extra_oct

		if name <> None:
			v.last_oct = octave
			v.last_name = name

		if name <> None:
			ch.pitches.append((octave, name, 0, 0))

		if alteration:
			if name == None:
				raise Exception("Cannot have an alteration without a note name", str(alteration))
			self.current_staff().set_alteration(name, octave, alteration, alteration_flags, ch.time)

		ch.multibar = multibar
		ch.count = count
		ch.basic_duration = basic_duration

		if multibar == 0:
			ch.dots = dots
			if forced_duration:
				self.forced_duration = ch.basic_duration / forced_duration

		if not pending_grace and not chord_continuation:
			# if tup != None
			#	sys.stderr.write("Would add tuplet basic_duration %f tupnumber %d\n" % (basic_duration, tup.number))
			# calculate the current time in the bar. Correct for tuplets.
			# If current time fills the bar, start a new bar, and reset
			# incidental alterations.
			if multibar != 0:
				v.time = self.meter.to_rat()
			else:
				if basic_duration == 0.5:
					t = (count * 2, 1)
				elif basic_duration == 0:
					t = (count, 1)
				else:
					t = (count, int(basic_duration))
				for d in range(dots):
					t = rat_multiply(t, (3, 2))
				if self.tuplets_expected > 0:
					t = rat_multiply(t, (self.tuplets[-1].replaces, self.tuplets[-1].number))
				v.time = rat_add(v.time, t)
				# sys.stderr.write("%s: add time %d/%d\n" % (v.idstring(), t[0], t[1]))

			# sys.stderr.write("%s: check current bar %d time %d/%d against bar time %d/%d\n" % (v.idstring(), v.bar, v.time[0], v.time[1], self.meter.to_rat()[0], self.meter.to_rat()[1]))
			if v.time == self.meter.to_rat():
				v.time = (0, 1)
				if multibar > 0:
					v.bar += multibar
				else:
					v.bar += 1
				if v == self.current_staff().voices[0]:
					self.current_staff().reset_alterations()
				barnumber = Barnumber(v.bar - 1, self.meter)
				v.add_nonchord(barnumber)
				# sys.stderr.write("\n%s: increase bar count to %d" % (v.idstring(), v.bar))
				if v == self.staffs[0].voices[0]:
					self.add_skip_bars(self.timeline, v.bar)
					# sys.stderr.write("Added bar %d to timeline\n" % barnumber.number)

		if multibar == 0:
			if self.tuplets_expected > 0:
				self.tuplets[-1].add_chord(ch)
				self.tuplets_expected = self.tuplets_expected - 1
				if self.tuplets_expected == 0:
					v.add_nonchord(TupletEnd())
					self.last_basic_duration = self.last_tuplet_duration

		return left


	def parse_basso_continuo(self, left):
		while left[0] in DIGITS +'#n-':
			c = left[0]
			left = left[1:]

			scr = ''
			if c in '#n-':
				scr = '\\raise #1.0 '
				if c == '#':
					scr = scr + '\\sharp'
				elif c == 'n':
					scr = scr + '\\natural'
				else:
					scr = scr + '\\flat'
				if len(left) > 0 and left[0] in DIGITS:
					scr = '\\concat{{' + scr + '}{' + '\\musicglyph #"' + figure_table[int(left[0])] + '"' +'}}'
					left = left[1:]
			else:
				scr = '\\musicglyph #"' + figure_table[int(c)] + '"'

			scr = '_\\markup\\smaller{%s}' % scr

			self.current_voice().last_chord().scripts.append(scr)
		return left


	def parse_beams(self, left):
		c = left[0]
		if left[0] == '[':
			left = left[1:]
			while left[0] in 'ulfhm+-0123456789':
				left=left[1:]
			self.current_voice().pending_beam = True
		else:
			ch = self.current_voice().chords[-1]
			ch.chord_suffix = ch.chord_suffix + "]"
			left = left[1:]

		return left


	def parse_key(self, left):
		key = ""
		#The key is changed by a string of the form K[+-]<num>[+-]<num>
		#where the first number is the transposition and the second number is the
		#new key signature.  For now, we won't bother with the transposition.
		if left[2] != '0':
			sys.stderr.write("Transposition not implemented yet: ")
			left = left[1:]
			while left[0] in '+-0123456789':
				left = left[1:]
		else:
			left=left[3:]
			key = ''
			while left[0] in '+-0123456789':
				key=key + left[0]
				left=left[1:]
			key_obj = Key(int(key), self.current_voice().bar, self.current_voice().time)
			self.current_voice().add_nonchord(key_obj)
			self.current_staff().alterations.append(key_obj)
		return left


	#------------------------------------------------------------------------------------
	#
	# Handle TeX
	#
	#------------------------------------------------------------------------------------

	def tex_dispatch_table(self):
		# parameter types:
		#  a - alphabetic [A-Za-z]
		#  t - text [A-Za-z0-9]
		#  d - dimen
		#  p - any parameter type
		#  = - assignment of TeX variable
		#  * - unlimited number of parameters
		#  S - special
		self.tex_functions = {
			# M-Tx
			'\\mtxInstrfont':	('', self.tex_require),
			'\\mtxeightsf':		('', self.tex_require),
			'\\mtxEightsf':		('', self.tex_require),
			'\\mtxtensf':		('', self.tex_require),
			'\\mtxTensf':		('', self.tex_require),
			'\\mtxelevensf':	('', self.tex_require),
			'\\mtxElevensf':	('', self.tex_require),
			'\\mtxtwelvesf':	('', self.tex_require),
			'\\mtxTwelvesf':	('', self.tex_require),
			'\\mtxBigsf':		('', self.tex_require),
			'\\mtxBIGsf':		('', self.tex_require),
			'\\mtxAllsf':		('', self.tex_require),
			'\\mtxTinySize':	('', self.tex_require),
			'\\mtxSmallSize':	('', self.tex_require),
			'\\mtxNormalSize':	('', self.tex_require),
			'\\mtxLargeSize':	('', self.tex_require),
			'\\mtxHugeSize':	('', self.tex_require),
			'\\mtxTitle':		('', self.tex_get_title),
			'\\mtxPoetComposer':	('', self.tex_get_composer),

			# M-Tx:musixlyr interface
			'\\mtxSetLyrics':	('pp', self.tex_set_lyrics),
			'\\mtxCopyLyrics':	('tp', self.tex_require),
			'\\mtxAssignLyrics':	('tp', self.tex_assign_lyrics),
			'\\mtxAuxLyr':		('p', self.tex_require),
			'\\mtxLyrlink':		('', self.tex_lyr_link),
			'\\mtxLowLyrlink':	('', self.tex_lyr_link),
			'\\mtxLyricsAdjust':	('td', self.tex_ignore),
			'\\mtxAuxLyricsAdjust':	('td', self.tex_ignore),
			'\\mtxLyrModeAlter':	('t', self.tex_require),
			'\\mtxLyrModeNormal':	('t', self.tex_require),
			'\\mtxBM':		('', self.tex_melisma_begin),
			'\\mtxEM':		('', self.tex_melisma_end),
			'\\mtxAuxBM':		('', self.tex_require),
			'\\mtxAuxEM':		('', self.tex_require),
			'\\mtxTenorClef':	('', self.tex_require),
			'\\mtxVerseNumber':	('t', self.tex_require),
			'\\mtxInterInstrument':	('td', self.tex_ignore),
			'\\mtxStaffBottom':	('d', self.tex_ignore),
			'\\mtxGroup':		('ttt', self.tex_require),
			'\\mtxTwoInstruments':	('tt', self.tex_require),
			'\\mtxTitleLine':	('t', self.tex_add_title),
			'\\mtxComposerLine':	('pp', self.tex_add_composer),
			'\\mtxInstrName':	('t', self.tex_add_instrument),
			'\\mtxSetSize':		('td', self.tex_ignore),
			'\\mtxDotted':		('', self.tex_require),
			# do not follow: 't':			('', self.tex_require),
			# do not follow: 'rp':			('', self.tex_require),
			'\\mtxSharp':		('', self.tex_require),
			'\\mtxFlat':		('', self.tex_require),
			'\\mtxIcresc':		('', self.tex_require),
			'\\mtxTcresc':		('', self.tex_require),
			'\\mtxCresc':		('', self.tex_require),
			'\\mtxIdecresc':	('', self.tex_require),
			'\\mtxTdecresc':	('', self.tex_require),
			'\\mtxDecresc':		('', self.tex_require),
			'\\mtxPF':		('', self.tex_require),
			'\\mtxLchar':		('tp', self.tex_require),
			'\\mtxCchar':		('tp', self.tex_require),
			'\\mtxZchar':		('tp', self.tex_add_markup),
			'\\mtxVerseNumberOffset':('', self.tex_ignore),
			'\\mtxVerse':		('', self.tex_require),

			# MusiXTeX
			'\\setlyrics':		('pp', self.tex_set_lyrics),
			'\\assignlyrics':	('tp', self.tex_assign_lyrics),
			'\\lyrlink':		('', self.tex_lyr_link),
			'\\lowlyrlink':		('', self.tex_lyr_link),
			'\\setsongraise':	('tp', self.tex_ignore),
			'\\beginmel':		('', self.tex_melisma_begin),
			'\\endmel':		('', self.tex_melisma_end),
			'\\backturn':		('p', self.tex_require),
			'\\Shakesw':		('p', self.tex_require),
			'\\songbottom':		('p', self.tex_require),
			'\\songtop':		('p', self.tex_require),
			'\\headline':		('=', self.tex_require),
			'\\hsize':		('=', self.tex_ignore),
			'\\vsize':		('=', self.tex_ignore),
			'\\zcharnote':		('ap', self.tex_add_markup),
			'\\lcharnote':		('ap', self.tex_require),
			'\\ccharnote':		('ap', self.tex_require),
			'\\zchar':		('ap', self.tex_require),
			'\\lchar':		('ap', self.tex_require),
			'\\cchar':		('ap', self.tex_require),
			'\\zql':		('p', self.tex_require),
			'\\sepbarrules':	('', self.tex_ignore),
			'\\indivbarrules':	('', self.tex_ignore),
			'\\sepbarrule':		('t', self.tex_ignore),
			'\\groupbottom':	('tt', self.tex_require),
			'\\grouptop':		('tt', self.tex_require),
			'\\twolines':		('bb', self.tex_vbox),
			'\\startbarno':		('=', self.tex_set_barno),
			'\\barno':		('=', self.tex_set_barno),
			'\\footline':		('=', self.tex_ignore),
			'\\pageno':		('=', self.tex_ignore),
			'\\Figu':		('pp', self.tex_require),
			'\\figdrop':		('p', self.tex_require),
			'\\nbbbbl':		('p', self.tex_ignore),
			'\\zq':			('', self.tex_require),
			'\\zh':			('', self.tex_require),
			'\\rw':			('', self.tex_require),
			'\\lw':			('', self.tex_require),
			'\\rh':			('', self.tex_require),
			'\\lh':			('', self.tex_require),
			'\\rq':			('', self.tex_require),
			'\\lq':			('', self.tex_require),
			'\\zhu':		('', self.tex_require),
			'\\zhl':		('', self.tex_require),
			'\\zqu':		('', self.tex_require),
			'\\zcu':		('', self.tex_require),
			'\\zcl':		('', self.tex_require),
			'\\zccu':		('', self.tex_require),
			'\\zccl':		('', self.tex_require),
			'\\zcccu':		('', self.tex_require),
			'\\zcccl':		('', self.tex_require),
			'\\zccccu':		('', self.tex_require),
			'\\zccccl':		('', self.tex_require),

			'\\smalltype':		('', self.tex_text_size),
			'\\Smalltype':		('', self.tex_text_size),
			'\\normtype':		('', self.tex_text_size),
			'\\medtype':		('', self.tex_text_size),
			'\\bigtype':		('', self.tex_text_size),
			'\\Bigtype':		('', self.tex_text_size),
			'\\BIgtype':		('', self.tex_text_size),
			'\\BIGtype':		('', self.tex_text_size),

			# Plain TeX
			'\\ref':		('', self.tex_ignore),
			'\\sixrm':		('', self.tex_require),
			'\\sevenrm':		('', self.tex_require),
			'\\eightrm':		('', self.tex_require),
			'\\ninerm':		('', self.tex_require),
			'\\tenrm':		('', self.tex_require),
			'\\elevenrm':		('', self.tex_require),
			'\\twelverm':		('', self.tex_require),
			'\\fourteenrm':		('', self.tex_require),
			'\\vbox':		('*', self.tex_vbox),
			'\\centerline':		('p', self.tex_centerline),
			'\\global':		('', self.tex_require),
			'\\hfill':		('', self.tex_require),
			'\\hoffset':		('=', self.tex_ignore),
			'\\voffset':		('=', self.tex_ignore),
			'\\vsize':		('=', self.tex_ignore),
			'\\kern':		('d', self.tex_ignore),
			'\\write':		('pp', self.tex_ignore),
			'\\setsize':		('pp', self.tex_ignore),
		}


	def tex_get_title(self, name, params):
		return (self.mtx.title, '')


	def tex_get_composer(self, name, params):
		self.composer = self.mtx.composer
		self.poet = self.mtx.poet

		return ('\\markup{\\fill-line{{%s} {%s}}}' % (self.mtx.poet, self.mtx.composer), '')


	def tex_lyr_link(self, name, params):
		return ('@MTX_LYRLINK@', '')


	def tex_ignore(self, name, params):
		warn("\nWarning: ignore TeX function '%s'" % name)
		return ('', '')


	def tex_require(self, name, params):
		warn("\nFIXME: implement TeX function '%s'" % name)
		return ('', '')
	

	def tex_interpret(self, name, params, replace):
		# sys.stderr.write("\nFIXME: implement TeX interpreter for '%s': replace with %s" % (name, replace))
		for i in range(len(params)):
			(out, result) = self.parse_tex(params[i])
			replace = re.sub('#%d' % (i + 1), "{" + result + "}", replace)
		(out, replace) = self.parse_tex(replace)
		return (replace, '')


	def tex_set_lyrics(self, name, params):
		(label, lyrics) = params
		if label in self.lyrics.keys():
			self.lyrics[label] = self.lyrics[label] + "\n   " + self.tex_expand_lyrics(lyrics)
		else:
			self.lyrics[label] = self.tex_expand_lyrics(lyrics)
		# sys.stderr.write("\nSet lyrics{%s} to '%s'" % (label, self.lyrics[label]))
		return ('', '')


	def tex_copy_lyrics(self, name, params):
		sys.stderr.write("\nFIXME: implement TeX function '%s'" % name)
		return ('', '')


	def tex_assign_lyrics(self, name, params):
		(staff, label) = params
		warn("\nAssign lyrics{%s} to staff[%s]" % (staff, label))
		s = self.staffs[int(staff) - 1]
		s.voices[s.lyrics_voice].lyrics_label = label
		return ('', '')


	def tex_melisma_begin(self, name, params):
		self.current_voice().pending_melisma = Melisma(True)
		return ('', '')


	def tex_melisma_end(self, name, params):
		self.current_voice().pending_melisma = Melisma(False)
		return ('', '')


	def tex_add_title(self, name, params):
		self.mtx.title = '\\markup{ ' + ' '.join(params) + ' }'
		return ('', '')


	def tex_add_composer(self, name, params):
		self.mtx.poet = params[0]
		self.mtx.composer = params[1]
		return ('', '')


	def tex_add_instrument(self, name, params):
		return (params[0], '')


	def tex_rewrite_lyrics(self, name, params):
		warn("\nFIXME: implement TeX rewrite_lyrics")
		return ('', '')


	def tex_add_markup(self, name, params):
		(elevation, text) = params
		ch = Chord()
		self.current_voice().add_chord(ch)
		ch.basic_duration = self.meter.denom
		ch.count = 0
		ch.skip = True
		ch.time = self.current_voice().time
		ch.bar  = self.current_voice().bar
		if int(elevation) >= 0:
			direction = '^'
		else:
			direction = '-'
		text = re.sub('~', '\char ##x00A0 ', text)
		ch.chord_suffix = ch.chord_suffix + direction + "\\markup{\"" + text + "\"}"

		return ('', '')


	def tex_set_barno(self, name, params):
		warn("\nFIXME: set barno to %s" % params[0])
		return ('', '')


	def tex_vbox(self, name, params):
		out = '\\center-column{ '
		for p in params:
			out = out + '\\line{ ' + p + '}'
		out = out + '}'

		# sys.stderr.write("\nvbox returns '%s'" % out)

		return (out, '')


	def tex_centerline(self, name, params):
		# out = '\\center-align { ' + ''.join(params) + ' }'
		sys.stderr.write("\nDon't know how to center within a \column thingy")
		out = ''.join(params)

		sys.stderr.write("\ncenterline returns '%s'" % out)

		return (out, '')


	def tex_text_size(self, name, params):
		if False:
			pass
		elif name == '\\smalltype':
			out = '\\fontsize #-2'
		elif name == '\\Smalltype':
			out = '\\fontsize #-1'
		elif name == '\\normtype':
			out = '\\fontsize #0'
		elif name == '\\medtype':
			out = '\\fontsize #+1'
		elif name == '\\bigtype':
			out = '\\fontsize #+2'
		elif name == '\\Bigtype':
			out = '\\fontsize #+3'
		elif name == '\\BIgtype':
			out = '\\fontsize #+4'
		elif name == '\\BIGtype':
			out = '\\fontsize #+5'
		else:
			raise Exception("Unknown text size", name)

		sys.stderr.write("\ntex_text_size returns '%s'" % out)

		return (out + ' {', '}')


	TEX_CONTROLS = '~`!@#$%^&*()_-+=|\\{}][:;"\'<>,.?/'

	accented = {
		'\\`a': 'à',
		'\\\'a': 'á',
		'\\"a': 'ä',
		'\\Ha': 'ä',
		'\\\~a': 'ã',

		'\\`e': 'è',
		'\\\'e': 'é',
		'\\"e': 'ë',
		'\\He': 'ë',
		'\\\~e': 'e',

		'\\`i': 'ì',
		'\\\'i': 'í',
		'\\"i': 'ï',

		'\\`o': 'ò',
		'\\\'o': 'ó',
		'\\"o': 'ö',
		'\\Ho': 'ő',

		'\\`u': 'ù',
		'\\\'u': 'ú',
		'\\"u': 'ü',
		'\\Hu': 'ű',

		'\\\"y': 'ÿ',

		'\\`A': 'À',
		'\\\'A': 'Á',
		'\\"A': 'Ä',
		'\\hA': 'Ä',
		'\\\~A': 'Ã',

		'\\`E': 'È',
		'\\\'E': 'É',
		'\\"E': 'Ë',
		'\\hE': 'Ë',
		'\\\~E': 'E',

		'\\`I': 'Ì',
		'\\\'I': 'Í',
		'\\"I': 'Ï',

		'\\`O': 'Ò',
		'\\\'O': 'Ó',
		'\\"O': 'Ö',
		'\\hO': 'Ő',

		'\\`U': 'Ù',
		'\\\'U': 'Ú',
		'\\"U': 'Ü',
		'\\hU': 'Ű',

		'\\\"Y': 'Ÿ',

		'\\l': 'ł',
		'\\L': 'Ł',
		'\\\~n': 'ñ',
		'\\\~N': 'Ñ',

		'~': ' ',
	}


	def untex(self, t):
		for k in self.accented.keys():
			if k in t:
				pass
			t = t.replace(k, self.accented[k])
			if len(k) > 2:
				t = t.replace(k[:2] + '{' + k[2] + '}', self.accented[k])
		return t


	def tex_expand_lyrics(self, lyrics):
		self.untex(lyrics)
		lyrics = re.sub(r'\s*@MTX_LYRLINK@\s+', '~', lyrics)
		lyrics = re.sub('_', '~', lyrics)
		lyrics = re.sub('-', ' -- ', lyrics)

		return lyrics


	def tex_expand_assign(self, left):
		m = re.match(r'\A([^=]+)=([a-zA-Z0-9]+)', left)
		if not m:
			raise Exception("Expect 'name=value' but have %s", left[:20])
		name = m.groups()[0]
		value = m.groups()[1]
		left = left[len(m.group()):]

		return (left, name, value)


	def tex_def(self, left):
		if left[0] != '\\':
			raise Exception("Expect backslash-name, but have %s" % left[:20])
		m = re.match(r'\A(\\[A-Za-z]+)((#\d)*)', left)
		name = m.groups()[0]
		left = left[len(m.group()):]
		if left[0] == '{':
			left = left[1:]
			open = 1
			replace = ''
			while open > 0:
				c = left[0]
				left = left[1:]
				if c == '{':
					open = open + 1
				elif c == '}':
					open = open - 1
				replace = replace + c
		else:
			replace = left[0]
			left = left[1:]
		param_descr = ['p'] * m.groups()[1].count('#')
		if name == '\\mtxversion':
			pass
		elif name == '\\mtxdate':
			pass
		else:
			# warn("\nFIXME: add \\def\\%s=%s" % (name, params[0]))
			self.tex_functions[name] = (param_descr, partial(self.tex_interpret, replace=replace))

		return left


	def tex_input(self, left):
		nl = left.find('\n')
		file = left[:nl].strip(SPACE)
		left = left[nl + 1:]
		warn("\nFIXME: need to include file '%s'" % file)

		return left


	def tex_font(self, left):
		if left[0] != '\\':
			raise Exception("Expect backslash-name, but have %s" % left[:20])
		(left, font, value) = self.tex_expand_assign(left)
		self.defined_fonts.append((font, value))
		m = re.match(r'\A\s+(at|scaled)\s+', left)
		if not m:
			return left
		left = left[len(m.group()):]
		if m.groups()[0] == 'at':
			m = re.match(r'\A\S+', len)
			left = left[len(m.group()):]
		elif m.groups()[0] == 'scaled':
			if left.startswith('\\magstep'):
				left = left[len('\\magstep'):]
				m = re.match(r'\A([1-5]|half)', left)
				if not m:
					raise Exception("magstep must be [1-5]|half", left[:20])
				left = left[len(m.group()):]
			else:
				m = re.match(r'\A\d+')
				if not m:
					raise Exception("scaled must be number", left[:20])
				left = left[len(m.group()):]

		return left


	def tex_params(self, left, params):
		# @return a list of parameter strings; each may be the result of a function call

		# TeX parameters are of a number of types:
		#  - numbers/dimensions
		#  - { .... }
		#  - ' ' <string>
		#  - \<function>...

		dimension = r'(pt|\\noteskip|\\internote|cm|truemm|mm|in)'
		out = []
		i = 0
		finished = len(params) == 0
		nesting = 0
		while not finished:
			if params[i] == '=' and left[0] == '=':
				left = left[1:]
			if left[0] in SPACE:
				while left[0] in SPACE:
					left = left[1:]
				if left[0] in '\\{':
					continue
				else:
					if False:
						pass
					elif params[i] == 'a':
						m = re.match(r'\A[A-Za-z]+', left)
					elif params[i] == 't':
						m = re.match(r'\A[A-Za-z_0-9]+', left)
					elif params[i] == 'l':
						m = re.match(r'\A[^\n]+', left)
					elif params[i] == 'd':
						m = re.match(r'\A-?[0-9.]+' + dimension, left)
					elif params[i] == 'p':
						m = re.match(r'\A\S+', left)
					else:
						raise Exception("Unknown TeX parameter type", params[i])
					out.append(m.group())
					left = left[len(m.group()):]
			elif left[0] == '\\':
				(left, p, post) = self.parse_tex_function(left)
				if post != '':
					warn("\nFIXME: tex_params has post '%s'" % post)
				out.append(p)
			elif left[0] == '{':
				if params[i] != '*':
					(left, p) = self.parse_tex(left[1:])
					if left[0] != '}':
						raise Exception("parse_tex does not leave '}'", left[:20])
					out.append(p)
					left = left[1:]
				else:
					nesting = nesting + 1
					left = left[1:]
					continue
			elif left[0] in '-0123456789.':
				# warn("\nFIXME: what if this is a dimension, not just a number?")
				m = re.match(r'\A-?[0-9.]+' + dimension + '?', left)
				out.append(m.group())
				left = left[len(m.group()):]
			elif params[i] == '*' and left[0] == '}':
				nesting = nesting - 1
				left = left[1:]
				finished = nesting == 0
			elif left[0] in self.TEX_CONTROLS:
				warn("\nFIXME: handle TeX control sequences")
			if params[i] != '*':
				i = i + 1
				if i == len(params):
					break

		return (left, out)


	def parse_tex_function(self, left):
		# @return (left, result) where result is a string that is the expansion of a TeX function
		result = ''
		post = ''
		if len(left) > 0 and left[0] == '\\' and not left[1] in SPACE:
			type = 1
			while left[1] == '\\':
				type = type + 1
				left = left[1:]

			m = re.match(r'\A\\[A-Za-z]+', left)
			if not m:
				return (left, '', '')

			left = left[len(m.group()):]
			if m.group() == '\\def' or m.group() == '\\gdef':
				left = self.tex_def(left)
			elif m.group() == '\\font':
				left = self.tex_font(left)
			elif m.group() == '\\input':
				left = self.tex_input(left)
			elif m.group() in self.tex_functions.keys():
				(params, func) = self.tex_functions[m.group()]
				(left, params) = self.tex_params(left, params)
				(result, post) = func(m.group(), params)
				if post != '':
					warn("\nFIXME: handle post property at the end of my scope (func = %s)" % m.group())
			else:
				warn("\nFIXME: unsupported TeX function '%s', hope it does not take parameters" % m.group())

		# strip pending \ that marks end-of-TeX for pmx
		if len(left) > 0 and left[0] == '\\' and left[1] in SPACE:
			left = left[1:]

		return (left, result, post)


	def parse_tex(self, left):
		# @return (left, words) where words is a string that is the expansion of input text

		# How is this a duplicate? This is a top-level wrapper..

		post = ''
		words = ''
		sep = ' '
		while len(left) > 0 and left[0] != '}':
			if left[0] in SPACE:
				left = left[1:]
			elif left[0] == '\\':
				while left[1] == '\\':
					left = left[1:]
				if left[0] == ' ':
					sep = ' '
				else:
					m = re.match(r'\A\\([^A-Za-z]({.}|.))', left)
					if m:
						words = words + self.untex(m.group())
						left = left[len(m.group()):]
					else:
						(left, w, p) = self.parse_tex_function(left)
						if p != '':
							warn("\nFIXME: parse_tex has post '%s'" % p)
							post = post + ' ' + p
						if words:
							words = words + " " + w
						else:
							words = w
					sep = ''
			elif left[0] == '{':
				(left, w) = self.parse_tex(left[1:])
				if left[0] != '}':
					raise Exception("parse_tex does not leave '}'", left[:20])
				if words:
					words = words + " " + w
				else:
					words = w
				left = left[1:]
				sep = ''
			else:
				m = re.match(r'\A[^\\\s}]+', left)
				w = self.untex(m.group())
				if words:
					words = words + sep + w
				else:
					words = w
				sep = ' '
				left = left[len(m.group()):]

		if post != '':
			words = words + post

		return (left, words)


	def expand_tex(self, left, name, nparam):
		# Old special-case function that handles \ref and \kern<dimen>
		# TODO: replace with the generic stuff
		pattern = re.compile(r'\A(' + name + r')[^A-Za-z]')
		m = re.match(pattern, left)
		if not m:
			return (left, None)

		pattern = re.compile(r'\A' + name)
		left = re.sub(pattern, '', left)
		params = []
		for i in range(len(nparam)):
			if left[0] in DIGITS:
				params.append(left[0])
				left = left[1:]
			elif left[0] == '{':
				left = left[1:]
				(left, subparams) = expand_tex(left)
				b = left.find('}')
				if b == -1:
					raise Exception('TeX parameter close not found')
				p = left[1:b]
				left = left[b+1:]
				if p.startswith('\\ref'):
					p = p[len('\\ref'):]
				p = re.sub('\A\\\\kern-?\d*\.?\d+pt', r'', p)
				p = re.sub('\A\\\\kern-?\.?\d+\\\\noteskip', r'', p)
				params.append(p)
			else:
				raise Exception('TeX parameter must start with \'{\'', left[0])
		return (left, params)

	# -------------------------------------------------------------------------------------------


	def parse_preamble(self, ls):
		def atonum(a):
			if re.search('\\.', a):
				return string.atof(a)
			else:
				return string.atoi(a)

		number_count = 12
		numbers = []

		while ls[0] == '\n':
			ls = ls[1:]

		if ls[0] == '---\n':
			# warn("\nFIXME: parse TeX pre-header")
			i = 1
			tex_defs = ''
			while ls[i] != '---\n':
				tex_defs = tex_defs + ls[i]
				i = i + 1
			ls = ls[i + 1:]
			# sys.stderr.write('\nFIXME: parse TeX pre-header \'%s\'' % tex_defs)
			while not tex_defs in SPACE:
				tex_defs = re.sub('\A\s*', '', tex_defs)
				(tex_defs, result) = self.parse_tex(tex_defs)
				# sys.stderr.write('\nFIXME: parse rest of TeX pre-header \'%s\'' % tex_defs)

		while len(numbers) < number_count:
			opening = ls[0]
			ls = ls[1:]

			opening = re.sub('[ \t\n]+', ' ', opening)
			opening = re.sub('^ ', '', opening)
			opening = re.sub(' $', '', opening)
			if opening == '':
				continue
			opening = string.split(opening, ' ')

			numbers = numbers + map(atonum, opening)

		off = 0
		(no_staffs, no_instruments) = tuple(numbers[off:2])
		off = off + 2

		staves = [0] * no_instruments
		if no_instruments < 0:
			no_instruments = -no_instruments
			for i in range(no_instruments):
				staves[i] = numbers[off]
				off = off + 1
		elif no_instruments < no_staffs:
			staves[0] = no_staffs - no_instruments + 1
			for i in range(no_instruments - 1):
				staves[i + 1] = 1
		else:
			for i in range(no_instruments):
				staves[i] = 1

		(timesig_num, timesig_den, ptimesig_num, ptimesig_den, pickup_beats, keysig_number) = tuple(numbers[off:off + 6])
		(no_pages, no_systems, musicsize, fracindent) = tuple(numbers[off + 6:])

		self.meter = Meter(timesig_num, timesig_den)
		self.pickup = (pickup_beats, timesig_den)
		self.timeline.add_nonchord(self.meter)
		self.timeline.set_meter(self.meter, self.pickup)
		self.keysig = keysig_number
		self.musicsize = musicsize

		# ignore this.
		# opening = map (string.atoi, re.split ('[\t ]+', opening))

		self.set_staffs(no_staffs)

		staff = 0
		for s in staves:
			if ls[0][0] == '\\':
				ls[0] = self.parse_tex(ls[0])[1]
			line = ls[0].strip(SPACE)
			ls = ls[1:]
			for i in range(s):
				self.staffs[staff].instrument_name = line
				staff = staff + 1

		line = ls[0]
		ls = ls[1:]

		for s in self.staffs:
			s.set_clef(line[0])
			line = line[1:]

		# dump path
		ls = ls[1:]

		# dump more ?
		return ls

	def parse_ornament(self, left):
		left = left[1:]
		e = self.current_voice().last_chord()

		id = left[0]
		left = left[1:]
		if left[0] == 'd':
			id = id +'d'
			left = left [1:]

		orn = '"orn"'
		if id == 'e':
			if left[0] in 'sfn':
				(octave, name, a, f) = e.pitches[-1]
				alteration = left[0]
				self.current_staff().alterations.insert(len(self.current_staff().alterations) - 1,
									Alteration(name, octave, alteration, 0,
										   e.bar, e.time, ACCIDENTAL_ABSOLUTE))
				e.chord_prefix = e.chord_prefix + '\\once \\set suggestAccidentals = ##t '
				left = left[1:]
		else:
			try:
				orn = ornament_table[id]
			except KeyError:
				warn("\nFIXME: unknown ornament `%s'\n" % id)

			if id == 'T':
				if left[0] == 't':
					warn("\nFIXME: trill wave without \\trill")
					left = left[1:]
				duration = 0.0
				dtxt = ''
				while left[0] in '0123456789.':
					dtxt = dtxt + left[0]
					left = left[1:]
				if duration == 0.0:
					# OK, trill without spanner
					left = left[1:]
				else:
					warn("\nFIXME: trill spanner")
			e.scripts.append('-' + orn)
			if left[0] in '+-':
				warn('\nIgnore: no ornament positioning')
				left = left[1:]
				while left[0] in '-.0123456789':
					left = left[1:]
		return left


	def parse_barcheck(self, left):
		if not isinstance(self.current_voice().entries[-1], Barnumber):
			self.current_voice().add_nonchord(Barcheck())

		return left [1:]


	def parse_id(self, left):
		id = None

		if re.match('\A[A-Z0-9]', left[0]):
			id = left[0]
			left = left[1:]
		while left[0] in 'uld0123456789+-.':
			left= left[1:]

		return (id, left)


	def parse_slur(self, left):
		c = left[0]
		left = left[1:]

		(id, left) = self.parse_id(left)

		v = self.current_voice()
		slur_direction = DIRECTION_NEUTRAL
		slur_fill = FILL_SOLID
		while len(left) > 0 and left[0] in 'udltb+-fnhHps':
			p = left[0]
			left = left[1:]
			if False:
				pass
			elif p == 'u':
				slur_direction = DIRECTION_UP
			elif p in 'dl':
				slur_direction = DIRECTION_DOWN
			elif p == 't':
				if c == ')':
					c = 'T'
				elif c == '(':
					# c = '{'
					pass
				elif c == 's':
					if v.current_slurs:
						c = 'T'
					else:
						c = 't'
				else:
					warn('\nOopps... what is this directive: %s' % (c + p))
			elif p == 'b':
				slur_fill = FILL_DOTTED
			elif p in '+-':
				m = re.match(r'\A-?[0-9]+', left)
				left = left[len(m.group()):]
				warn("\nIgnore: raise slur by %s" % m.group())
				p = left[0]
				if p in '+-':
					left = left[1:]
					m = re.match(r'\A-?[0-9]+', left)
					left = left[len(m.group()):]
					warn("\nIgnore: shift slur by %s" % m.group())
					p = left[0]
					if p in '+-':
						left = left[1:]
						m = re.match(r'\A-?[0-9]+', left)
						left = left[len(m.group()):]
						warn("\nIgnore: alter slur by %s" % m.group())
						p = left[0]
						if p == ':':
							left = left[1:]
							m = re.match(r'\A([1-7])([1-7])', left)
							left = left[len(m.group()):]
							warn("\nIgnore: alter slur slope by %s" % m.group())
			elif p in 'fnhH':
				warn("\nIgnore: alter slur curvature by %s" % p)
			elif p == 'p':
				m = re.match(r'\A([+-])([st])')
				warn("\nIgnore: alter slur height %s" % m.group())
			elif p == 's':
				m = re.match(r'\A(-?[0-9])(-?[0-9])', left)
				warn("\nIgnore: alter broken slur by %s" % m.group())
				left = left[len(m.group()):]
				p = left[0]
				if p == 's':
					left = left[1:]
					m = re.match(r'\A(-?[0-9])(-?[0-9])', left)
					warn("\nIgnore: alter broken slur by %s" % m.group())

		if c == 's':
			v.toggle_slur(id)
		elif c == '(':
			v.start_slur(id)
		elif c == ')':
			v.end_slur(id)
		elif c == 't':
			v.toggle_tie(id)
		elif c == '{':
			v.start_tie(id)
		elif c == '}':
			v.end_tie(id)
		elif c == 'T':
			s = v.remove_slur(id);
			v.replace_tie(s)

		return left


	def parse_mumbo_jumbo(self, left):
		left = left[1:]
		while left and  left[0] <> '\\':
			left = left[1:]

		left  = left[1:]
		return left


	def parsex(self, left):
		warn("\nFIXME: handle 'x' command")
		left = left[1:]
		while left[0] in DIGITS:
			left = left[1:]

		return left


	def parse_global(self, left):
		while left[0] in 'abcdeIiKNprRSsTv':
			c = left[0]
			left = left[1:]
			if 0:
				pass
			elif c == 'a':
				# ignore, eat the number (float?)
				while left[0] in '-.0123456789':
					left = left[1:]
			elif c in 'bs':
				# ignore accidental size
				pass
			elif c == 'c':
				c = left[0]
				left = left[1:]
				if c == 'l':
					self.paper_size = PAPER_LETTER
				elif c == '4':
					self.paper_size = PAPER_A4
				else:
					raise Exception('unknown paper size', c)
			elif c == 'd':
				# ignore dot placement directive
				pass
			elif c == 'e':
				# ignore interstaff spacing directive
				pass
			elif c in 'Ii':
				# ignore, eat the number (float?)
				while left[0] in '-.0123456789':
					left = left[1:]
			elif c == 'K':
				# ignore multi-voice rest placement directive
				pass
			elif c == 'N':
				f = left.find(SPACE)
				part_base = left[0:f]
				left = left[f:]
				warn('\nFIXME: set scor2prt file names to \'%s\'' % part_base)
			elif c == 'p':
				# ignore the curvature options
				left = re.sub('\A(([-+][stch])|l)+', '', left)
			elif c == 'r':
				self.accidental_mode = ACCIDENTAL_RELATIVE
				for s in self.staffs:
					s.set_accidental_mode(self.accidental_mode)
			elif c == 'R':
				f = left.find(SPACE)
				filename = left[0:f]
				left = left[f:]
				warn('\nFIXME: include file \'%s\'' % filename)
			elif c == 'S':
				while left[0] in '-0':
					left = left[1:]
			elif c == 'T':
				# ignore tuplet appearance directive
				pass
			elif c == 'v':
				# ignore ragged-bottom directive
				pass
			else:
				raise Exception('unknown global parameter', c)
		return left


	def parse_meter(self, left):
		left = left[1:]
		patt = re.compile(r'([o0-9]/[o0-9]/[o0-9]/[o0-9])')
		m = re.match(patt, left)
		if m:
			comps = re.split('/', m.group())
			left = left[len(m.group()):]
			comps = map(string.atoi , comps)
		else:
			ix = 0
			comps = [0] * 4
			while left[0] in 'o0123456789':
				if left[0] == 'o':
					comps[ix] = 1
					left = left[1:]
				elif left[0] == '1':
					comps[ix] = string.atoi(left[:2])
					left = left[2:]
				else:
					comps[ix] = string.atoi(left[0])
					left = left[1:]
				ix = ix + 1

		meter = Meter(comps[0], comps[1])

		# sys.stderr.write("\nAdd meter %d/%d" % (meter.num, meter.denom))
		self.timeline.add_nonchord(meter)
		self.meter = meter

		return left


	def open_repeat(self):
		# sys.stderr.write("\nFIXME: start a repeat in the timeline")
		self.current_repeat = Repeat()
		self.last_repeat = self.current_repeat
		self.timeline.add_nonchord(self.current_repeat)
		self.timeline.repeats.append(self.current_repeat)
		self.volta = None


	def close_repeat(self):
		# sys.stderr.write("\nFIXME: end a repeat in the timeline")
		if self.current_repeat:
			self.timeline.add_nonchord(RepeatClose(self.current_repeat))
			if self.volta:
				self.timeline.add_nonchord(VoltaClose(self.current_repeat))
			self.current_repeat = None


	def restart_repeat(self):
		# sys.stderr.write("\nFIXME: restart a repeat in the timeline")
		if self.volta:
			warn("\nWarning: assume the repeat restart is handled by the volta")
		else:
			self.close_repeat()
			self.open_repeat()


	def open_volta(self, text):
		# sys.stderr.write("\nFIXME: open a volta alternative in the timeline")
		if self.current_repeat:
			self.close_repeat()
		volta = Volta(self.last_repeat, text)
		self.timeline.add_nonchord(volta)
		self.volta = volta


	def close_volta(self):
		# sys.stderr.write("\nFIXME: close a volta alternative in the timeline")
		if self.volta:
			self.timeline.add_nonchord(VoltaClose(self.last_repeat))
		else:
			warn("\nFIXME: volta close outside repeat")


	def parse_body(self, left):
		while left:
			c = left[0]
			# Here, don't remove the first char; keep it for some arguments
			if c == '%':
				f = string.find(left, '\n')
				if f < 0:
					left = ''
				left = left[f+1:]
			elif c == 'm':
				left = self.parse_meter(left)
			elif c in 'lhw':
				left = left[1:]
				m = re.match(r'\A[.0123456789]+', left)
				if m:
					if not c in 'hw':
						raise Exception("mark text must be followed by blank or [+-]", left[0])
					# page size directive, ignore
					while len(left) > 0 and left[0] in '0123456789.imp':
						left = left[1:]
				elif left[0] in '+-' + SPACE:
					if c == 'l':
						direction = -1
					else:
						direction = 1
					f = string.find(left, '\n')
					if f < 0:
						left = ''
					else:
						left = left[f+1:]

					f = string.find(left, '\n')
					self.timeline.add_mark(direction, left[:f])
					left = left[f+1:]
				else:
					raise Exception("page width must be followed by number", left[0])
			elif c in 'Gzabcdefgr.,':
				left = self.parse_note(left)
			elif c in DIGITS + 'n#-':
				left = self.parse_basso_continuo(left)
			elif c in SPACE:
				left = left[1:]
			elif c in 's()t{}':
				left = self.parse_slur(left)
			elif c == '|':
				left = self.parse_barcheck(left)
			elif c == 'o':
				left = self.parse_ornament(left)
			elif c == 'x':
				left = self.parsex(left)
			elif c == 'C':
				self.current_staff().set_clef(str(left[1]))
				left = left[2:]
			elif c == 'K':
				left = self.parse_key(left)
			elif c == 'P':
				# ignore page numbering directive
				left = left[1:]
				left = re.sub(r'\A[lr\d]*', '', left)
			elif c == 'I':
				warn('\nFIXME: support MIDI options')
				left = left[1:]
				left = re.sub(r'\A\S*', '', left)
			elif c in '[]':
				left = self.parse_beams(left)
			elif left[:2] == "//":
				self.current_staff().next_voice()
				left = left[2:]
			elif c == '/':
				bar = self.current_voice().bar
				for v in self.current_staff().voices[self.current_staff().voice_idx + 1:]:
					v = self.current_staff().next_voice()
					self.catch_up(self.current_voice(), bar)
				self.next_staff()
				left = left[1:]
				while left[0] != '\n':
					left = left[1:]
				left = self.parse_barcheck(left)
			# elif c == '\\':
			# 	left = self.parse_mumbo_jumbo(left)
			elif c == '\r':
				left = left[1:]
			elif c == 'T':
				if not left[1] in 'ict':
					warn("""
Huh? Unknown T parameter `%s', before `%s'""" % (left[1], left[:20] ))
					left = left[1:]
					continue

				s = string.find(left, '\n');
				f = s + 1 + string.find(left[s+1:], '\n');
				sys.stderr.write("\nSee title block '%s' '%s'" % (left[0:s], left[s+1:f]))
				if left[1] == 'i':
					self.instrument = self.parse_tex(left[s+1:f])[1]
				elif left[1] == 'c':
					self.composer = self.parse_tex(left[s+1:f])[1]
				elif left[1] == 't':
					self.title = self.parse_tex(left[s+1:f])[1]
				left = left[f:]
			elif c == 'A':
				left = self.parse_global(left[1:])
			elif c == 'B':
				# ignore bass stem direction directive
				left = left[1:]
			elif c == 'R':
				left = left[1:]
				if left[0] == 'b':
					self.timeline.add_nonchord(Bar('|'))
				elif left[0] == 'd':
					self.timeline.add_nonchord(Bar('||'))
				elif left[0] == 'D':
					self.timeline.add_nonchord(Bar('|.'))
				elif left[0] == 'z':
					self.timeline.add_nonchord(Bar(''))
				elif left[0:2] == 'lr':
					self.restart_repeat()
					left = left[1:]
				elif left[0] == 'l':
					self.open_repeat()
				elif left[0] == 'r':
					self.close_repeat()
				else:
					raise Exception("Unknown barline modifier", left[0])
				left = left[1:]
			elif c == 'V':
				left = left[1:]
				if not left[0] in 'bx \n\r':
					text = ''
					while not left[0] in SPACE:
						text = text + left[0]
						left = left[1:]
					self.open_volta(text)
				elif left[0] in 'bx':
					self.close_volta()
					if left[0] == 'b':
						left = left[1:]
						text = ''
						while not left[0] in SPACE:
							text = text + left[0]
							left = left[1:]
						self.open_volta(text)
					else:
						left = left[1:]
				else:
					pass
			elif c in 'L':
				# sys.stderr.write('\nFIXME: handle line/page/movement breaks')
				left = left[1:]
				while left[0] in DIGITS:
					left = left[1:]
				if left[0] == 'P':
					left = left[1:]
					while left[0] in DIGITS:
						left = left[1:]
					if option_page_breaks:
						self.timeline.add_nonchord(Break('\\pageBreak'))
				if left[0] == 'M':
					warn('\nFIXME: handle movement breaks')
					left = left[1:]
					while not left[0] in SPACE:
						left = left[1:]
				elif option_line_breaks:
					self.timeline.add_nonchord(Break('\\break'))
			elif c == 'P':
				warn('\FIXME: set page number')
				left = left[1:]
			elif c == 'X':
				left = left[1:]
				warn("\nIgnore: no horizontal shift");
				while left[0] in '-.0123456789p:SPB':
					left = left[1:]
			elif c == '\\':
				(left, result, post) = self.parse_tex_function(left)
				if post != '':
					warn("\nFIXME: parse_body has post '%s'" % post)
			else:
				warn("""
Huh? Unknown directive `%s', before `%s'""" % (c, left[:20] ))
				left = left[1:]


	def quote_string(self, string):
		m = re.match(r'\s*\\markup', string)
		if m:
			return string
		else:
			return '"' + string + '"'

		
	def dump(self):
		out = "\\header {\n"
		if self.title:
			out = out + "    title = " + self.quote_string(self.title) + "\n"
		if self.composer:
			out = out + "    composer = " + self.quote_string(self.composer) + "\n"
		if self.instrument:
			out = out + "    instrument = " + self.quote_string(self.instrument) + "\n"
		out = out + "}\n\n"
		out = out + '#(set-global-staff-size %d)\n\n' % self.musicsize

		defaults = '\n\
    \\compressFullBarRests\n\
    \\accidentalStyle modern-cautionary\n\
'
		if not isinstance(self.timeline.entries[-1], Bar):
			self.timeline.add_nonchord(Bar('|.'))

		out = out + self.timeline.dump()
		refs = ''
		i = 0
		for s in self.staffs:
			out = out + s.dump()
			instr = ''
			refs = '{\n        \\' + s.idstring() + '\n    }\n    ' + refs
			i = i + 1

		out = out + "\n\n\\score { <<\n    %s%s\n >> }" % (refs , defaults)
		return out


	def parse(self, fn):
		ls = open(fn).readlines()
		def subst(s):
			return re.sub('%.*$', '', s)

		ls = map(subst, ls)

#		print left
		if fn.endswith('.tex'):
			left = string.join(ls, ' ')
			left = self.parse_tex(left)
		else:
			def newline(s):
				return re.sub('\r\n', '\n', s)
			ls = map(newline, ls)
			# ls = filter(lambda x: x != '\r', ls)
			ls = self.parse_preamble(ls)

			left = string.join(ls, ' ')

			if not option_strip_header:
				self.parse_body(left)
				for c in self.staffs:
					c.calculate()
					for v in c.voices:
						if v.lyrics_label:
							v.lyrics = self.lyrics[v.lyrics_label]
				self.timeline.compact_multibar()
				self.timeline.calculate()

		return left





argDescription = """Convert PMX to LilyPond.
"""
argEpilog = """
PMX is a Musixtex preprocessor written by Don Simons, see
http://www.gmd.de/Misc/Music/musixtex/software/pmx/.

Report bugs to bug-lilypond@gnu.org.

Written by Han-Wen Nienhuys <hanwen@cs.uu.nl>.
Updated by Rutger Hofman <lily@rutgerhofman.nl>.
"""


versionText = """%s (GNU LilyPond) %s

This is free software.  It is covered by the GNU General Public License,
and you are welcome to change it and/or distribute copies of it under
certain conditions.  Invoke as `midi2ly --warranty' for more information.

Copyright (c) 2000--2004 by Han-Wen Nienhuys <hanwen@cs.uu.nl>
Copyright (c) 2013 by Rutger Hofman <lily@rutgerhofman.nl>
""" % (program_name, version)


def identify():
	sys.stderr.write("%s from LilyPond %s\n" % (program_name, version))


argParser = argparse.ArgumentParser(description=argDescription, epilog=argEpilog);
# argParser.add_argument('--help', '-h', action="help")
argParser.add_argument('--version', '-v', action="version", version='%(prog) ' + versionText)
argParser.add_argument('--output', '-o', nargs=1)
argParser.add_argument('--line-breaks', '-l', action='store_true', help='retain pmx line breaks')
argParser.add_argument('--page-breaks', '-p', action='store_true', help='retain pmx page breaks')
argParser.add_argument('--strip-header', '-H', action='store_true', help='strip header of pmx file, dump rest')
argParser.add_argument('--tex-define', '-D', nargs=3, help='define tex function: <#arguments> <expansion>')
argParser.add_argument('--tex-ignore', '-U', nargs=2, help='ignore tex function: <#arguments>')
argParser.add_argument('input-file', nargs='*', help='pmx input file(s)')

argsNS = argParser.parse_args(sys.argv[1:])
args = vars(argsNS)

out_filename = args['output']
option_line_breaks = args['line_breaks']
option_page_breaks = args['page_breaks']
option_strip_header = args['strip_header']
files = args['input-file']

texDefines = args['tex_define']
texIgnores = args['tex_ignore']

identify()

for f in files:
	if f == '-':
		f = ''

	sys.stderr.write('Processing `%s\'' % f)
	if not out_filename:
		out_filename = os.path.basename(re.sub('(?i).pmx$', '.ly', f))

	if out_filename == f:
		out_filename = os.path.basename(f + '.ly')

	e = Parser()

	left = e.parse(f)

	sys.stderr.write('\nWriting `%s\'' % out_filename)
	if option_strip_header or f.endswith('.tex'):
		ly = left
	else:
		ly = e.dump() + '\n'

	if out_filename[0] == '-':
		fo = sys.stdout
	else:
		fo = open(out_filename, 'w')
	if not option_strip_header:
		fo.write('%% lily was here -- automatically converted by pmx2ly from %s\n' % f)
		fo.write('\\version "2.16.0"\n\n')
	fo.write(ly)
	if fo != sys.stdout:
		fo.close()
	sys.stderr.write(" -- done\n");


