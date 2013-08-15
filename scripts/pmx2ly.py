#!@PYTHON@

# PMX is a Musixtex preprocessor written by Don Simons, see
# http://www.gmd.de/Misc/Music/musixtex/software/pmx/

# TODO:
#  * block openings aren't parsed.

import os
import string
import sys
import re
import getopt

program_name = 'pmx2ly'
version = '@TOPLEVEL_VERSION@'
if version == '@' + 'TOPLEVEL_VERSION' + '@':
	version = '(unknown version)'	   # uGUHGUHGHGUGH


MAX_OCTAVE	= 10
OCTAVE		= 7

def encodeint (i):
	return chr ( i  + ord ('A'))


actab = {
	-2: 'eses',
	-1: 'es',
	0 : '',
	1: 'is',
	2: 'isis'
}


def pitch_to_lily_string (tup):
	(o,n,a,f) = tup

	if n == 's':
		return n

	nm = chr((n + 2) % OCTAVE + ord ('a'))
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
		sys.stderr.write("\n**** Refuse to really suppress the cautionary")
	return nm


def gcd (a,b):
	if b == 0:
		return a
	c = a
	while c:
		c = a % b
		a = b
		b = c
	return a


def rat_simplify (r):
	(n,d) = r
	if d < 0:
		d = -d
		n = -n
	if n == 0:
		return (0,1)
	else:
		g = gcd (n, d)
		return (n/g, d/g)


def rat_multiply (a,b):
	(x,y) = a
	(p,q) = b

	return rat_simplify ((x*p, y*q))


def rat_divide (a,b):
	(p,q) = b
	return rat_multiply (a, (q,p))


tuplet_table = {
	2: 3,
	3: 2,
	5: 4
}


def rat_add (a,b):
	(x,y) = a
	(p,q) = b

	return rat_simplify ((x*q + p*y, y*q))


def rat_neg (a):
	(p,q) = a
	return (-p,q)



def rat_larger (a,b):
	return rat_subtract (a, b )[0] > 0


def rat_subtract (a,b ):
	return rat_add (a, rat_neg (b))


def rat_to_duration (frac):
	log = 1
	d = (1,1)
	while rat_larger (d, frac):
		d = rat_multiply (d, (1,2))
		log = log << 1

	frac = rat_subtract (frac, d)
	dots = 0
	if frac == rat_multiply (d, (1,2)):
		dots = 1
	elif frac == rat_multiply (d, (3,4)):
		dots = 2
	return (log, dots)


class Bar:
	def __init__(self, text):
		self.text = text

	def dump(self):
		return '\\bar "' + self.text + '"'


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
		return "\n    \\repeat volta " + str(volta) + " {\n    "


class RepeatClose:
	def __init__(self, repeat):
		self.repeat = repeat

	def dump(self):
		close = "\n    }"
		if self.repeat.total_volte > 0:
			close = close + "\n    \\alternative {"
		return close


class Volta:
	def __init__(self, repeat, text):
		repeat.begin_volta()
		self.text = text

	def dump(self):
		return "\n    { % open volta\n    "


class VoltaClose:
	def __init__(self, repeat):
		self.repeat = repeat
		self.count = repeat.total_volte
		repeat.end_volta()

	def dump(self):
		close = "\n    } % close volta\n    "
		if self.count == self.repeat.total_volte:
			close = close + "\n    } % close alternative\n    "
		return close


class Barcheck :
	def __init__ (self):
		pass

	def dump (self):
		return ' |'


class Barnumber :
	def __init__ (self, number, meter):
		self.number = number
		self.meter = meter

	def dump (self):
		return ' | % ' + str(self.number + 1) + '\n   '


class Meter :
	def __init__ (self,num,denom):
		if denom == 3:
			raise Exception('weird denom', basic_duration)
		self.num = num
		self.denom = denom

	def to_rat(self):
		return rat_simplify((self.num, self.denom))

	def dump (self):
		return ' \\time ' + str(self.num) + "/" + str(self.denom) + '\n   '


class Beam:
	def __init__ (self, ch):
		self.char = ch

	def dump (self):
		return self.char


class Tie:
	def __init__ (self,id):
		self.id = id
		self.start_chord = None
		self.end_chord = None

	def calculate (self):
		s = self.start_chord
		e = self.end_chord

		if e and s:
			s.note_suffix = s.note_suffix + '~'
			# check if the first note is altered. If so, copy the
			# alteration to the second note.
			for p in s.pitches:
				(op, np, ap, fp) = p
				for i in range(len(e.pitches)):
					(oq, nq, aq, fq) = e.pitches[i]
					if op == oq and np == nq and ap != aq:
						e.pitches[i] = (oq, nq, ap, fq)
		else:
			sys.stderr.write ("\nOrphaned tie")


class Slur:
	def __init__ (self,id):
		self.id = id
		self.start_chord = None
		self.end_chord = None

	def calculate (self):
		s = self.start_chord
		e = self.end_chord

		if e and s:
			s.note_suffix = s.note_suffix + '('
			e.note_suffix = e.note_suffix + ')'
		else:
			sys.stderr.write ("\nOrphaned slur")


class Grace:
	def __init__ (self, items, slashed, slurred, after, direction):
		self.items = items
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
				sys.stderr.write('\nFIXME: for \\afterGrace, invert the grace and the graced note');
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
			sys.stderr.write("\nOrphaned grace")


class Voice:
	def __init__ (self):
		self.entries = []
		self.chords = []
		self.staff = None
		self.meter = None
		self.current_slurs = []
		self.slurs = []
		self.pending_slur = None
		self.current_ties = []
		self.ties = []
		self.pending_tie = None
		self.current_grace = None
		self.pending_grace = None
		self.volta = []
		self.pending_volta = None
		self.graces = []
		self.time = (0, 1)
		self.bar = 0
		self.preset_id = None
		self.nonempty = False
		self.repeats = []
		self.last_name = 0
		self.last_oct = 0

	def set_preset_id(self, preset):
		self.preset_id = preset

	def set_meter(self, meter, pickup):
		self.meter = meter
		(pd, pn) = pickup
		if pd != 0:
			self.time = rat_subtract(meter.to_rat(), pickup)
			self.bar = -1
		(d, n) = self.time
		sys.stderr.write("set_meter(): time %d / %d\n" % (d, n))

	def start_tie(self, id):
		self.pending_tie = Tie(id)

	def end_tie (self, id):
		for s in self.current_ties:
			if s.id == id:
				self.current_ties.remove (s)
				s.end_chord = self.chords[-1]
				return True
		return False

	def toggle_tie (self, id):
		if not self.end_tie(id):
			s = Slur(id)
			s.start_chord = self.chords[-1]
			self.current_ties.append (s)
			self.ties.append (s)

	def start_slur(self, id):
		self.pending_slur = Slur(id)

	def end_slur(self, id):
		for s in self.current_slurs:
			if s.id == id:
				self.current_slurs.remove (s)
				s.end_chord = self.chords[-1]
				return True
		return False

	def toggle_slur (self, id):
		if not self.end_slur(id):
			s = Slur(id)
			s.start_chord = self.chords[-1]
			self.current_slurs.append (s)
			self.slurs.append (s)

	def handle_pending(self):
		if self.pending_slur:
			s = self.pending_slur
			s.start_chord = self.chords[-1]
			self.current_slurs.append (s)
			self.slurs.append (s)
			self.pending_slur = None
		if self.pending_tie:
			s = self.pending_tie
			s.start_chord = self.chords[-1]
			self.current_ties.append (s)
			self.ties.append (s)
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

	def last_chord (self):
		return self.chords[-1]

	def add_chord (self, ch):
		self.chords.append (ch)
		self.entries.append (ch)

	def add_nonchord (self, nch):
		self.entries.append (nch)

	def add_skip_bar(self, count, basic_duration):
		ch = Chord ()
		self.add_chord(ch)
		ch.multibar = 1
		ch.skip = True
		ch.count = count
		ch.basic_duration = basic_duration
		ch.time = (0, 1)

	def idstring (self):
		if self.preset_id:
			return self.preset_id
		else:
			return 'staff%svoice%s ' % (encodeint (self.staff.number) , encodeint(self.number))

	def dump (self):
		out = ''
		ln = ''
		for e in self.entries:
			next = e.dump ()
			if next[-1] == '\n':
				out = out + ln + next
				ln = ''
				continue

			if 0 and len (ln) + len(next) > 72:
				out = out + ln + '\n'
				ln = ''
			ln = ln + next


		out = out  + ln
		id = self.idstring ()

		if self.preset_id:
			out = '%s = {\n   %s}\n' % (self.preset_id, out)
		else:
			out = '%s = <<\n{\n   %s\n}\n>>\n'% (id, out)
		return out

	def calculate_graces (self):
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

	def calculate (self):
		# sys.stderr.write("Skip calculating graces\n")
		# self.calculate_graces ()
		for s in self.slurs:
			s.calculate ()
		for t in self.ties:
			t.calculate ()
		for g in self.graces:
			g.calculate()
		for s in self.chords:
			if not s.skip:
				self.nonempty = True
				break


class Clef:
	def __init__ (self, cl):
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
	def __init__ (self, key, bar, time):
		self.key = key
		self.bar  = bar
		self.time = time

	def dump(self):
		if self.key >= 0:
			key = '+' + str(self.key)
		else:
			key = '-' + str(-self.key)
		return ' \\key ' + key_table[key] + '\n   '


class Accidental:
	def __init__(self, note, octave, alteration, flags, bar, time):
		# if alteration == 'ff' or alteration == 'ss':
		#	sys.stderr.write("\nSee alteration %s" % alteration)
		self.note = note
		self.octave = octave
		self.alteration = alteration
		self.flags = flags
		self.bar = bar
		self.time = time

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
	def __init__ (self):
		self.voices = (Voice (), Voice())
		self.clef = None
		self.instrument = 0
		self.voice_idx = 0
		self.number = None
		self.key = 0
		self.alterations = []
		self.alteration_spans_octaves = False
		self.accidental_mode = ACCIDENTAL_ABSOLUTE

		i = 0
		for v  in self.voices:
			v.staff = self
			v.number = i
			i = i+1

	def set_accidental_mode(self, mode):
		self.accidental_mode = mode

	def set_clef (self, letter):
		if clef_table.has_key (letter):
			clstr = clef_table[letter]
			self.voices[0].add_nonchord (Clef (clstr))
		else:
			sys.stderr.write ("Clef type `%c' unknown\n" % letter)

	def set_key(self, keysig):
		self.key = keysig
		sys.stderr.write("Key sig %d\n" % keysig)
		sys.stderr.write("\n********** FIXME: set pickup time for Key")
		self.alterations.append(Key(keysig, 0, (0, 1)))
		self.voices[0].add_nonchord(Key(keysig, 0, (0, 1)))

	def set_alteration(self, note, octave, alteration, flags):
		self.alterations.append(Accidental(note, octave, alteration, flags,
						   self.current_voice().bar,
						   self.current_voice().time))

	def reset_alterations(self):
		self.alterations.append(AlterationReset(self.current_voice().bar,
							self.current_voice().time))

	def current_voice (self):
		return self.voices[self.voice_idx]

	def next_voice (self):
		self.voice_idx = (self.voice_idx + 1) % len(self.voices)


	def calculate_alteration(self, voice):
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
				elif isinstance(a, Accidental):
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
					if self.accidental_mode == ACCIDENTAL_RELATIVE:
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

		sys.stderr.write("traverse the voice to calculate the alterations\n")


	def calculate (self):
		for v in self.voices:
			self.calculate_alteration(v)
		for v in self.voices:
			v.calculate ()


	def idstring (self):
		return 'staff%s' % encodeint (self.number)


	def dump (self):
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
				voice = '\n    \\' + v.idstring ()
				if v != self.voices[0]:
					refs = voice + ' \\\\' + refs
				else:
					refs = voice + refs
		refs = refs + "\n    \\timeLine"

		out = out + '\n\n%s = \\new Staff = %s <<%s\n>>\n\n' % (self.idstring (), self.idstring (), refs)
		return out


class Tuplet:
	def __init__ (self, number, base, dots):
		self.chords = []
		self.number = number
		self.replaces = tuplet_table[number]
		self.base = base
		self.dots = dots

		length = (1,base)
		if dots == 1:
			length = rat_multiply (length, (3,2))
		elif dots == 2:
			length = rat_multiply (length, (7,4))

		length = rat_multiply (length, (1,self.replaces))

		(nb,nd) =rat_to_duration (length)

		self.note_base = nb
		self.note_dots = nd

	def add_chord (self, ch):
		ch.dots = self.note_dots
		ch.basic_duration = self.note_base
		self.chords.append (ch)

		if len (self.chords) == 1:
			ch.chord_prefix = '\\times %d/%d { ' % (self.replaces, self.number)
		elif len (self.chords) == self.number:
			ch.chord_suffix = ' }'


FLAG_CAUTIONARY = 0x1 << 0
FLAG_SUPPRESS   = 0x1 << 1
FLAG_FORCED     = 0x1 << 2

ACCIDENTAL_ABSOLUTE = 0
ACCIDENTAL_RELATIVE = 1

PAPER_A4     = 0
PAPER_LETTER = 1

class Chord:
	def __init__ (self):
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

	def dump (self):
		if self.multibar != 0:
			if self.grace:
				sys.stderr.write('Ooppsss... multibar grace?????\n')
			if self.skip:
				rest = 's'
			else:
				rest = 'R'
			if self.count != 1:
				multi = str(self.multibar) + "*" + str(self.count)
			else:
				multi = str(self.multibar)
			v = self.chord_prefix + ' ' + rest + str(self.basic_duration) + "*" + str(multi) + self.chord_suffix
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
			v = v + pitch_to_lily_string (p)

		if self.skip:
			v = v + 's'
		elif len (self.pitches) > 1:
			v = '<%s>' % v
		elif len (self.pitches) == 0:
			v = 'r'

		v = v + sd
		for s in self.scripts:
			v = v + '-' + s

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


class Parser:
	def __init__ (self, filename):
		self.staffs = []
		self.timeline = Voice()
		self.timeline.set_preset_id('timeLine')
		self.forced_duration = None
		self.tuplets_expected = 0
		self.tuplets = []
		self.last_basic_duration = 4
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
		self.staff_groups = []

		self.parse (filename)

	def set_staffs (self, number):
		self.staffs = map (lambda x: Staff (), range(0, number))

		self.staff_idx = 0

		i =0
		sys.stderr.write("Key sig %d\n" % self.keysig)
		# self.timeline.set_meter(self.meter, self.pickup)
		# self.timeline.add_nonchord(meter)
		for s in self.staffs:
			s.number = i
			s.set_key(self.keysig)
			s.set_accidental_mode(self.accidental_mode)
			i = i+1

	def current_staff (self):
		return self.staffs[self.staff_idx]

	def current_voice (self):
		return self.current_staff ().current_voice ()

	def next_staff (self):
		self.staff_idx = (self.staff_idx + 1)% len (self.staffs)
		self.current_staff().voice_idx = 0

	def add_skip_bars(self, voice, target_bar):
		bars = target_bar - voice.bar
		for b in range(bars):
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


	def add_markup(self, elevation, text):
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
		ch.chord_suffix = ch.chord_suffix + direction + "\\markup{" + text + "}"


	def parse_grace(self, left):
		left = left[1:]
		slashed = 0
		slurred = 0
		items = -1
		after = 0
		direction = 0
		basic_duration = 8
		# process the grace note options
		while left[0] in '0123456789msxluAWX':
			c = left[0]
			left = left[1:]
			if c == 'm':
				if not left[0] in '0123456789':
					sys.stderr.write("""
Huh? expected number of grace note beams, found %s Left was `%s'""" % (left[0], left[:20]))
				else:
					basic_duration = 4 << (ord(left[0]) - ord('0'))
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
				sys.stderr.write("\nFIXME: no horizontal shift in grace");
				while left[0] in ".0123456789":
					left = left[1:]
			else:
				if not c in '0123456789':
					sys.stderr.write("""
Huh? expected number of grace notes, found %s Left was `%s'""" % (c, left[:20]))
				elif items == -1:
					items = ord(c) - ord('0')
				else:
					items = 10 * items + ord(c) - ord('0')
		if items == -1:
			items = 1
		# sys.stderr.write("Detect grace items %d\n" % items)
		grace = Grace(items, slashed, slurred, after, direction)

		return (left, grace, basic_duration)


	def parse_note (self, left):
		name = None
		ch = None

		grace = None
		if left[0] == 'G':
			(left, grace, basic_duration) = self.parse_grace(left)

		if left[0] == 'z':
			chord_continuation = True
			ch = self.current_voice().last_chord()
			left = left[1:]
		else:
			chord_continuation = False
			ch = Chord ()
			ch.time = self.current_voice().time
			ch.bar = self.current_voice().bar
			self.current_voice().add_chord (ch)

		if grace:
			self.current_voice().pending_grace = grace
		pending_grace = self.current_voice().pending_grace

		self.current_voice().handle_pending()

		# what about 's'?
		alteration = 0
		if left[0] <> 'r':
			name = (ord (left[0]) - ord('a') + 5) % 7
			# sys.stderr.write("Process note '%s' name '%d'\n" % (left[0], name))

		# sys.stderr.write("Process note: name %s from '%s'" % (name, left[:20]))
		left = left[1:]

		count = 1
		forced_duration  = 0
		dots = 0
		octave = None
		durdigit = None
		alteration = None
		multibar = 0
		tupnumber = 0
		extra_oct = 0
		flats = 0
		sharps = 0
		alteration_flags = 0
		while left[0] in 'aber</>dsfmpnul0123456789.,+-\\':
			c = left[0]
			if c == 'a':
				ch.chord_suffix = ch.chord_suffix + '\\noBeam'
			elif c == 'b':
				ch.skip = True
			elif c in 'er</>':
				sys.stderr.write("\nFIXME: horizontal shift not implemented")
			elif c in 'fns':
				alteration = c
				if left[:2] == 'ff' or left[:2] == 'ss':
					alteration = left[:2]
					left = left[1:]
				if len(left) == 1:
					pass
				elif left[1] in '+-':
					sys.stderr.write("\nFIXME: alteration/dot horizontal shift not implemented")
				elif left[1] == 'c':
					alteration_flags = alteration_flags | FLAG_CAUTIONARY;
					left = left[1:]
				elif left[1] == 'i':
					alteration_flags = alteration_flags | FLAG_SUPPRESS;
					left = left[1:]
			elif c == 'm':
				multibar = 1
			elif c == 'p':
				multibar = 1
				if len(left) > 1 and left[1] == 'o':
					# ignore off-center attribute
					left = left[1:]
			elif c == 'd':
				dots = dots + 1
				if left[1] in '+-':
					sys.stderr.write("\nFIXME: alteration/dot horizontal shift not implemented")
			elif c in DIGITS and durdigit == None and \
			     self.tuplets_expected == 0:
				if multibar != 0:
					bars = ""
					while left[0] in DIGITS:
						bars = bars + left[0]
						left = left[1:]
					multibar = string.atoi(bars)
				else:
					durdigit = string.atoi (c)
					if durdigit == 0:
						durdigit = 7
			elif c in DIGITS:
				octave = string.atoi (c) - 3
			elif c == '+':
				extra_oct = extra_oct + 1
			elif c == '-':
				extra_oct = extra_oct - 1
			elif c == '.':
				dots = dots+ 1
				forced_duration = 2
			elif c == ',':
				forced_duration = 2
			left = left[1:]

		if left[0] == 'x':
			left = left[1:]
			tupnumber = string.atoi (left[0])
			left = left[1:]
			left = re.sub (r'\An?f?[+-0-9.]*', '' , left)

		if multibar != 0:
			count = self.meter.num
			basic_duration = self.meter.denom
		elif durdigit:
			try:
				basic_duration =  basicdur_table[durdigit]
			except KeyError:
				sys.stderr.write ("""
Huh? expected duration, found %d Left was `%s'""" % (durdigit, left[:20]))

				basic_duration = 4
		elif not grace:
			basic_duration = self.last_basic_duration

		self.last_basic_duration = basic_duration

		if name <> None and octave == None:
			e = 0
			if self.current_voice().last_name < name and name - self.current_voice().last_name > 3:
				e = -1
			elif self.current_voice().last_name > name and self.current_voice().last_name - name > 3:
				e = 1

			octave = self.current_voice().last_oct  +e + extra_oct

		if name <> None:
			self.current_voice().last_oct = octave
			self.current_voice().last_name = name

		if name <> None:
			ch.pitches.append ((octave, name, 0, 0))

		if alteration:
			self.current_staff().set_alteration(name, octave, alteration, alteration_flags)

		# do before adding to tuplet.
		ch.multibar = multibar
		ch.count = count
		ch.basic_duration = basic_duration

		if multibar == 0:
			ch.dots = dots
			if forced_duration:
				self.forced_duration = ch.basic_duration / forced_duration

			if tupnumber:
				tup =Tuplet (tupnumber, basic_duration, dots)
				self.tuplets_expected = tupnumber
				self.tuplets.append (tup)

		if not pending_grace and not chord_continuation:
			# if tupnumber != 0:
			#	sys.stderr.write("Would add tuplet basic_duration %f tupnumber %d\n" % (basic_duration, tupnumber))
			# calculate the current time in the bar. Correct for tuplets.
			# If current time fills the bar, start a new bar, and reset
			# incidental alterations.
			if multibar != 0:
				self.current_voice().time = self.meter.to_rat()
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
					(d, n) = t
					t = rat_multiply(t, (1, self.tuplets[-1].replaces))
					t = rat_multiply(t, (self.tuplets[-1].replaces, self.tuplets[-1].number))
				self.current_voice().time = rat_add(self.current_voice().time, t)

			if False:
				(dc, nc) = self.current_voice().time
				(dm, nm) = self.meter.to_rat()
				sys.stderr.write("%s: check current time %d/%d against bar time %d/%d\n" % (self.current_voice().idstring(), dc, nc, dm, nm))
			if self.current_voice().time == self.meter.to_rat():
				self.current_voice().time = (0, 1)
				if multibar > 0:
					self.current_voice().bar += multibar
				else:
					self.current_voice().bar += 1
				if self.current_voice() == self.current_staff().voices[0]:
					self.current_staff().reset_alterations()
				barnumber = Barnumber(self.current_voice().bar, self.meter)
				self.current_voice ().add_nonchord(barnumber)
				# sys.stderr.write("%s: increase bar count to %d\n" % (self.current_voice().idstring(), self.current_voice().bar))
				if self.current_voice() == self.staffs[0].voices[0]:
					self.add_skip_bars(self.timeline, self.current_voice().bar)
					# sys.stderr.write("Added bar %d to timeline\n" % barnumber.number)

		if multibar == 0:
			if self.tuplets_expected > 0:
				self.tuplets[-1].add_chord (ch)
				self.tuplets_expected = self.tuplets_expected - 1

		return left

	def parse_basso_continuo (self, left):
		while left[0] in DIGITS +'#n-':
			scr = left[0]

			if scr == '#':
				scr = '\\\\textsharp'

			if len(scr)>1 or scr not in DIGITS:
				scr = '"%s"' % scr

			self.current_voice().last_chord ().scripts.append (scr)
			left=left[1:]
		return left

	def parse_beams (self,left):
		c = left[0]
	#	self.current_voice().add_nonchord (Beam(c))
		if left[0] == '[':
			left = left[1:]
			while left[0] in 'ulfhm+-0123456789':
				left=left[1:]
		else:
			left = left[1:]

		return left

	def parse_key (self, left):
		key = ""
		#The key is changed by a string of the form K[+-]<num>[+-]<num>
		#where the first number is the transposition and the second number is the
		#new key signature.  For now, we won't bother with the transposition.
		if left[2] != '0':
			sys.stderr.write("Transposition not implemented yet: ")
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
		return(left)

	def parse_header  (self, ls):
		def atonum(a):
			if re.search('\\.', a):
				return string.atof (a)
			else:
				return string.atoi (a)

		number_count = 12
		numbers = []

		if ls[0] == '---\n':
			sys.stderr.write("\nFIXME: parse TeX pre-header")
			i = 1
			while ls[i] != '---\n':
				sys.stderr.write('\nFIXME: parse TeX line \'%s\'' % ls[i])
				i = i + 1
			ls = ls[i + 1:]

		while len (numbers) < number_count:
			opening = ls[0]
			ls = ls[1:]

			opening = re.sub ('[ \t\n]+', ' ', opening)
			opening = re.sub ('^ ', '', opening)
			opening = re.sub (' $', '', opening)
			if opening == '':
				continue
			opening = string.split (opening, ' ')

			numbers = numbers + map (atonum, opening)

		off = 0
		(no_staffs, no_instruments) = tuple(numbers[off:2])
		off = off + 2
		if no_instruments < 0:
			no_instruments = -no_instruments
			instrument = [0] * no_instruments
			for i in range(no_instruments):
				instrument[i] = numbers[off]
				off = off + 1
		(timesig_num, timesig_den, ptimesig_num, ptimesig_den, pickup_beats, keysig_number) = tuple (numbers[off:off + 6])
		(no_pages,no_systems, musicsize, fracindent) = tuple (numbers[off + 6:])

		self.meter = Meter(timesig_num, timesig_den)
		self.pickup = (pickup_beats, timesig_den)
		self.timeline.add_nonchord(self.meter)
		self.timeline.set_meter(self.meter, self.pickup)
		self.keysig = keysig_number

		# ignore this.
		# opening = map (string.atoi, re.split ('[\t ]+', opening))

		instruments = []
		while len (instruments) < no_instruments:
			instruments.append (ls[0])
			ls = ls[1:]

		l = ls[0]
		ls = ls[1:]

		self.set_staffs (no_staffs)

		for s in self.staffs:
			s.set_clef(l[0])
			l = l[1:]

		# dump path
		ls = ls[1:]

		# dump more ?
		return ls

	def parse_ornament (self, left):
		left = left[1:]
		e = self.current_voice ().last_chord ()

		id = left[0]
		left = left[1:]
		if left[0] == 'd':
			id = id +'d'
			left = left [1:]

		orn = '"orn"'
		if id == 'e':
			if left[0] in 'sfn':
				e.chord_prefix = e.chord_prefix + '\\once \\set suggestAccidentals = ##t '
				left = left[1:]
		else:
			try:
				orn = ornament_table[id]
			except KeyError:
				sys.stderr.write ("unknown ornament `%s'\n" % id)

			if id == 'T':
				if left[0] == 't':
					sys.stderr.write("\nFIXME: trill wave without \\trill")
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
					sys.stderr.write("\nFIXME: trill spanner")
			e.scripts.append (orn)
		return left

	def parse_barcheck (self, left):
		self.current_voice ().add_nonchord (Barcheck ())

		return left [1:]

	def parse_id(self, left):
		id = None

		if re.match ('[A-Z0-9]', left[0]):
			id = left[0]
			left= left[1:]
		while left[0] in 'uld0123456789+-.':
			left= left[1:]

		return (id, left)

	def parse_slur (self, left):
		c = left[0]
		left = left[1:]

		(id, left) = self.parse_id(left)

		if c == 's':
			self.current_voice().toggle_slur(id)
		elif c == '(':
			self.current_voice().start_slur(id)
		elif c == ')':
			self.current_voice().end_slur(id)
		elif c == 't':
			self.current_voice().toggle_tie(id)
		elif c == '{':
			self.current_voice().start_tie(id)
		elif c == '}':
			self.current_voice().end_tie(id)

		return left

	def parse_mumbo_jumbo (self,left):
		left = left[1:]
		while left and  left[0] <> '\\':
			left = left[1:]

		left  = left[1:]
		return left

	def parsex (self,left):
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
				while left[0] in '.0123456789':
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
				while left[0] in '.0123456789':
					left = left[1:]
			elif c == 'K':
				# ignore multi-voice rest placement directive
				pass
			elif c == 'N':
				f = left.find(' \t\n')
				part_base = left[0:f]
				left = left[f:]
				sys.stderr.write('\nFIXME: set scor2prt file names to \'%s\'' % part_base)
			elif c == 'p':
				# ignore the curvature options
				left = re.sub('\A(([-+][stch])|l)+', '', left)
			elif c == 'r':
				self.accidental_mode = ACCIDENTAL_RELATIVE
				for s in self.staffs:
					s.set_accidental_mode(self.accidental_mode)
			elif c == 'R':
				f = left.find(' \t\n')
				filename = left[0:f]
				left = left[f:]
				sys.stderr.write('\nFIXME: include file \'%s\'' % filename)
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


	def expand_tex(self, left, name, nparam):
		pattern = re.compile(r'\A(' + name + r')[^A-Za-z]')
		m = re.match(pattern, left)
		if not m:
			return (left, None)

		pattern = re.compile(r'\A' + name)
		left = re.sub(pattern, '', left)
		params = []
		for i in range(nparam):
			if left[0] in DIGITS:
				p = ''
				while left[0] in DIGITS:
					p = p + left[0]
					left = left[1:]
				params.append(p)
			elif left[0] == '{':
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


	def parse_meter(self, left):
		left = left[1:]
		m = re.match ('([o0-9]/[o0-9]/[o0-9]/[o0-9])', left)
		if m:
			comps = m.split('/')
			nums = m.group (1)
			left = left[len (nums):]
			nums = map (string.atoi , nums)
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
			sys.stderr.write("\nAssume the repeat restart is handled by the volta")
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
			sys.stderr.write("\nFIXME: volta close outside repeat")


	def parse_M_Tx(self, left):
		left = left[3:]
		(left, params) = self.expand_tex(left, 'InterInstrument', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'StaffBottom', 1)
		# ignore
		(left, params) = self.expand_tex(left, 'Group', 3)
		if params:
			self.staff_groups.append(params)
		(left, params) = self.expand_tex(left, 'SetSize', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'Zchar', 2)
		if params:
			self.add_markup(params[0], params[1])
		# mtxInstrfont{\twelvebf}
		# mtxeightsf{\font\eightsf=cmss8}
		# mtxEightsf{\mtxeightsf\eightsf}
		# mtxtensf{\font\tensf=cmss10}
		# mtxTensf{\mtxtensf\tensf}
		# mtxelevensf{\font\elevensf=cmss10 scaled \magstephalf}
		# mtxElevensf{\mtxelevensf\elevensf}
		# mtxtwelvesf{\font\twelvesf=cmss12}
		# mtxTwelvesf{\mtxtwelvesf\twelvesf}
		# mtxBigsf{\font\Bigtype=cmss9 scaled \magstep2}
		# mtxBIGsf{\font\BIGtype=cmss9 scaled \magstep4}
		# mtxAllsf{\mtxElevensf\mtxBigsf\mtxBIGsf}
		# mtxTinySize{\tinyvalue}
		# mtxSmallSize{\smallvalue}
		# mtxNormalSize{\normalvalue}
		# mtxLargeSize{\largevalue}
		# mtxHugeSize{\Largevalue}
		# mtxSetLyrics, 2):	# #1#2{\setlyrics{#1}{#2}}
		# mtxCopyLyrics, 2):	# #1#2{\copylyrics{#1}{#2}}
		# mtxAssignLyrics, 2):	# #1#2{\assignlyrics{#1}{#2}}
		# mtxAuxLyr#1{\auxlyr{#1}}
		# mtxLyrlink{\lyrlink}
		# mtxLowLyrlink{\lowlyrlink}
		# mtxLyricsAdjust, 2):	# #1#2{\setsongraise{#1}{#2\internote}}
		# mtxAuxLyricsAdjust, 2):	# #1#2{\auxsetsongraise{#1}{#2\internote}}
		# mtxLyrModeAlter#1{\lyrmodealter{#1}}
		# mtxLyrModeNormal#1{\lyrmodenormal{#1}}
		# mtxBM{\beginmel}
		# mtxEM{\endmel}
		# mtxAuxBM{\auxlyr\mtxBM}
		# mtxAuxEM{\auxlyr\mtxEM}
		# mtxTenorClef#1{\settrebleclefsymbol{#1}\treblelowoct}
		# mtxVerseNumber#1{#1 }
		# mtxInterInstrument, 2):	# #1#2{\setinterinstrument{#1}{#2\Interligne}}
		# mtxStaffBottom#1{\gdef\atnextline{\stafftopmarg #1\Interligne}}
		# mtxGroup#1#2#3{\grouptop{#1}{#2}\groupbottom{#1}{#3}}
		# mtxPageHeight#1{\vsize #1}
		# mtxTwoInstruments, 2):	# #1#2{\vbox{\hbox{#1}\hbox{#2}}}
		# mtxTitleLine#1{\gdef\mtxTitle{#1}}
		# mtxComposerLine, 2):	# #1#2{\gdef\mtxPoetComposer{#1\hfill #2}}
		# mtxInstrName#1{{\mtxInstrfont #1}}
		# mtxSetSize, 2):	# #1#2{\setsize{#1}{#2}}
		# mtxDotted{\dotted}
		# %\def\mtxSharp{\raise1ex\hbox{\musicsmallfont\char'064}}
		# %\def\mtxFlat{\raise1ex\hbox{\musicsmallfont\char'062}}
		# mtxSharp{\raise1ex\hbox{\sharp}}
		# mtxFlat{\raise1ex\hbox{\flat}}
		# mtxIcresc{\icresc}
		# mtxTcresc{\tcresc}
		# mtxCresc#1{\crescendo{#1\elemskip}}
		# mtxIdecresc{\icresc}
		# mtxTdecresc{\tdecresc}
		# mtxDecresc#1{\decrescendo{#1\elemskip}}
		# mtxPF{\ppff}
		# mtxLchar, 2):	# #1#2{\lchar{#1}{#2}}
		# mtxCchar, 2):	# #1#2{\cchar{#1}{#2}}
		# mtxZchar, 2):	# #1#2{\zchar{#1}{#2}}
		# mtxVerseNumberOffset{3}
		# mtxVerse{\loffset{\mtxVerseNumberOffset}\lyr}

		# flat{\musixfont\char'062}
		# sharp{\musixfont\char'064}
		return left


	def parse_tex(self, left):
		# handle just a few MusiXTeX commands
		type = 1
		while left[0] == '\\':
			type = type + 1
			left = left[1:]

		(left, params) = self.expand_tex(left, 'zcharnote', 2)
		if params:
			self.add_markup(params[0], params[1])
		(left, params) = self.expand_tex(left, 'lcharnote', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'ccharnote', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'zchar', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'lchar', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'cchar', 2)
		# ignore
		(left, params) = self.expand_tex(left, 'zql', 1)
		if params:
			sys.stderr.write('\nFIXME: handle MusiXTeX macro \\zql, left %s' % left[:20])
		(left, params) = self.expand_tex(left, 'sepbarrules', 0)
		# ignore
		(left, params) = self.expand_tex(left, 'indivbarrules', 0)
		# ignore
		(left, params) = self.expand_tex(left, 'sepbarrule', 1)
		# ignore
		if left.startswith('input'):
			left = left[len('input'):];
			left = re.sub('\A\s+', '', left)
			left = re.sub('\A\S+', '', left)
		# to handle:
		# \zq{note}
		# \zh{note}
		# \rw{note}
		# \lw{note}
		# \rh{note}
		# \lh{note}
		# \rq{note}
		# \lq{note}
		# \zhu{note}
		# \zhl{note}
		# \zqu{note}
		# \zcu{note}
		# \zcl{note}
		# \zccu{note}
		# \zccl{note}
		# \zcccu{note}
		# \zcccl{note}
		# \zccccu{note}
		# \zccccl{note}

		elif left.startswith('smalltype'):
			pass
		elif left.startswith('Smalltype'):
			pass
		elif left.startswith('normtype'):
			pass
		elif left.startswith('medtype'):
			pass
		elif left.startswith('Bigtype'):
			pass
		elif left.startswith('BIgtype'):
			pass
		elif left.startswith('BIGtype'):
			pass

		# and handle a number of M-Tx commands
		elif left.startswith('mtx'):
			left = self.parse_M_Tx(left)

		elif left[1] in ' \n\t':
			left = left[1:]
		else:
			# done = left.find('\\')
			# sys.stderr.write('\nFIXME: ignore TeX command %s' % left[:done+1])
			# left = left[done + 1:]
			pass

		return left


	def parse_body (self, left):
		preamble = 1

		while left:
			c = left[0]
			# Here, don't remove the first char; keep it for some arguments
			if c == '%':
				f = string.find (left, '\n')
				if f < 0:
					left = ''
				left = left[f+1:]
			elif c == 'm':
				left = self.parse_meter(left)
			elif left[0] in 'lh':
				if left[0] == 'l':
					direction = -1
				else:
					direction = 1
				f = string.find (left, '\n')
				if f <0 :
					left = ''
				else:
					left = left[f+1:]

				f = string.find (left, '\n')
				self.add_markup(direction, left[:f])
				left = left[f+1:]
			elif c in 'Gzabcdefgr':
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
				left = self.parse_key (left)
			elif c == 'P':
				# ignore page numbering directive
				left = left[1:]
				left = re.sub(r'\A[lr\d]*', '', left)
			elif c == 'I':
				sys.stderr.write('\nFIXME: support MIDI options')
				left = left[1:]
				left = re.sub(r'\A\S*', '', left)
			elif c in "[]":
				left = self.parse_beams (left)
			elif left[:2] == "//":
				self.current_staff().next_voice()
				left = left[2:]
			elif c == '/':
				bar = self.current_voice().bar
				for v in self.current_staff().voices[self.current_staff().voice_idx + 1:]:
					v = self.current_staff().next_voice()
					self.catch_up(self.current_voice(), bar)
				self.next_staff()
				left = self.parse_barcheck(left)
			# elif c == '\\':
			# 	left = self.parse_mumbo_jumbo(left)
			elif c == '\r':
				left = left[1:]
			elif c == 'T':
				if not left[1] in 'ict':
					sys.stderr.write ("""
Huh? Unknown T parameter `%s', before `%s'""" % (left[1], left[:20] ))
					left = left[1:]
					continue

				s = string.find(left, '\n');
				f = s + 1 + string.find(left[s+1:], '\n');
				sys.stderr.write("\nSee title block '%s'\n" % left[0:f])
				if left[1] == 'i':
					self.instrument = left[s+1:f]
				elif left[1] == 'c':
					self.composer = left[s+1:f]
				elif left[1] == 't':
					self.title = left[s+1:f]
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
					while not left[0] in ' \n\r':
						text = text + left[0]
						left = left[1:]
					self.open_volta(text)
				elif left[0] in 'bx':
					self.close_volta()
					if left[0] == 'b':
						left = left[1:]
						text = ''
						while not left[0] in ' \n\r':
							text = text + left[0]
							left = left[1:]
						self.open_volta(text)
					else:
						left = left[1:]
				else:
					pass
			elif c in 'LPM':
				sys.stderr.write('\nFIXME: handle line/page/movement breaks')
				while not left[0] in ' \n\t':
					left = left[1:]
			elif c == 'X':
				left = left[1:]
				sys.stderr.write("\nFIXME: no horizontal shift");
				if left[0] in 'S:':
					left = left[1:]
				while left[0] in ".0123456789BP":
					left = left[1:]
			elif c == '\\':
				left = self.parse_tex(left)
			else:
				sys.stderr.write ("""
Huh? Unknown directive `%s', before `%s'""" % (c, left[:20] ))
				left = left[1:]

	def dump (self):
		out = "\\header {\n"
		if self.title:
			out = out + "    title = \"" + self.title + "\"\n"
		if self.composer:
			out = out + "    composer = \"" + self.composer + "\"\n"
		if self.instrument:
			out = out + "    instrument = \"" + self.instrument + "\"\n"
		out = out + "}\n\n"

		defaults = '\n\
    \\compressFullBarRests\n\
    \\accidentalStyle modern-cautionary\n\
'
		self.timeline.add_nonchord(Bar('|.'))

		out = out + self.timeline.dump()
		refs = ''
		for s in self.staffs:
			out = out + s.dump()
			refs = '\\' + s.idstring() + refs

		out = out + "\n\n\\score { <<\n    %s\n    %s\n >> }" % (refs , defaults)
		return out


	def compact_timeline(self):
		compacted = []
		multi = 0
		last_bar = None
		last_chord = None
		for b in self.timeline.entries:
			if isinstance(b, Chord):
				multi = multi + 1
				last_chord = b
			elif isinstance(b, Barnumber):
				last_bar = b
			# elif isinstance(b, Meter):
			else:
				# help, do something about the code duplication
				if multi > 0:
					# stop compressing here
					ch = Chord()
					ch.multibar = multi
					ch.skip = True
					ch.basic_duration = last_chord.basic_duration
					ch.count = last_chord.count
					ch.bar = last_chord.bar
					ch.time = last_chord.time
					compacted.append(ch)
					multi = 0
					if last_bar:
						compacted.append(last_bar)
				compacted.append(b)

		# help, do something about the code duplication
		if multi > 0:
			ch = Chord()
			ch.multibar = multi
			ch.skip = True
			ch.basic_duration = last_chord.basic_duration
			ch.count = last_chord.count
			ch.bar = last_chord.bar
			ch.time = last_chord.time
			compacted.append(ch)
			if last_bar:
				compacted.append(last_bar)

		# sys.stderr.write("\nReplace timeline.entries with compacted[%d]" % len(compacted))
		self.timeline.entries = compacted


	def parse (self,fn):
		ls = open (fn).readlines ()
		def subst(s):
			return re.sub ('%.*$', '', s)

		ls = map (subst, ls)

		def newline(s):
			return re.sub('\r\n', '\n', s)
		ls = map(newline, ls)
		# ls = filter (lambda x: x != '\r', ls)
		ls = self.parse_header (ls)
		left = string.join (ls, ' ')

#		print left
		self.parse_body (left)
		for c in self.staffs:
			c.calculate ()
		self.compact_timeline()
		self.timeline.calculate()





def help ():
	sys.stdout.write (
"""Usage: pmx2ly [OPTIONS]... PMX-FILE

Convert PMX to LilyPond.

Options:
  -h, --help          print this help
  -o, --output=FILE   set output filename to FILE
  -b, --line-breaks   keep line breaks in
  -p, --page-breaks   keep page breaks in
  -v, --version       shown version information

PMX is a Musixtex preprocessor written by Don Simons, see
http://www.gmd.de/Misc/Music/musixtex/software/pmx/.

Report bugs to bug-lilypond@gnu.org.

Written by Han-Wen Nienhuys <hanwen@cs.uu.nl>.
Updated by Rutger Hofman <lily@rutgerhofman.nl>.

""")


def print_version ():
	sys.stdout.write ("""pmx2ly (GNU LilyPond) %s

This is free software.  It is covered by the GNU General Public License,
and you are welcome to change it and/or distribute copies of it under
certain conditions.  Invoke as `midi2ly --warranty' for more information.

Copyright (c) 2000--2004 by Han-Wen Nienhuys <hanwen@cs.uu.nl>
Copyright (c) 2013 by Rutger Hofman <lily@rutgerhofman.nl>
""" % version)


def identify():
	sys.stderr.write ("%s from LilyPond %s\n" % (program_name, version))



(options, files) = getopt.getopt (sys.argv[1:], 'vo:h', ['help','version', 'output='])
out_filename = None
for opt in options:
	o = opt[0]
	a = opt[1]
	if o== '--help' or o == '-h':
		help ()
		sys.exit (0)
	if o == '--version' or o == '-v':
		print_version ()
		sys.exit(0)

	if o == '--output' or o == '-o':
		out_filename = a
	# FIXME: Handle -b -v
	else:
		print o
		raise getopt.error

identify()

for f in files:
	if f == '-':
		f = ''

	sys.stderr.write ('Processing `%s\'\n' % f)
	e = Parser(f)
	if not out_filename:
		out_filename = os.path.basename (re.sub ('(?i).pmx$', '.ly', f))

	if out_filename == f:
		out_filename = os.path.basename (f + '.ly')

	sys.stderr.write ('Writing `%s\'' % out_filename)
	ly = e.dump() + '\n'



	fo = open (out_filename, 'w')
	fo.write ('%% lily was here -- automatically converted by pmx2ly from %s\n' % f)
	fo.write('\\version "2.16.0"\n\n')
	fo.write(ly)
	fo.close ()
	sys.stderr.write(" -- done\n");


