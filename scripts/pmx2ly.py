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

	nm = chr((n + 2) % 7 + ord ('a'))
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


class Barcheck :
	def __init__ (self):
		pass

	def dump (self):
		return '|'


class Barnumber :
	def __init__ (self, number):
		self.number = number

	def dump (self):
		return '| % ' + str(self.number) + '\n   '


class Meter :
	def __init__ (self,num,denom):
		self.num = num
		self.denom = denom

	def to_rat(self):
		return rat_simplify((self.num, self.denom))

	def dump (self):
		return ' \\time ' + str(self.num) + "/" + str(self.denom) + '\n'


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
		self.graces = []
		self.altered = [0] * 7
		self.alteration = [0] * 7
		self.default_alteration = [0] * 7
		self.time = (0, 1)
		self.bar = 0

	def set_meter(self, meter, pickup):
		self.meter = meter
		(pd, pn) = pickup
		if pd != 0:
			self.time = rat_subtract(meter.to_rat(), pickup)
			self.bar = 0
		(d, n) = self.time
		sys.stderr.write("set_meter(): time %d / %d\n" % (d, n))

	def set_keysig(self, keysig):
		a = 6
		for i in range(keysig):
			a = a + 4
			self.default_alteration[a % 7] = (i + 7) / 7
		a = 3
		for i in range(-keysig):
			a = a + 3
			self.default_alteration[a % 7] = -(i + 7) / 7

	def toggle_tie (self, id):
		for s in self.current_ties:
			if s.id == id:
				self.current_ties.remove (s)
				s.end_chord = self.chords[-1]
				return
		self.pending_tie = Tie (id)

	def toggle_slur (self, id):
		for s in self.current_slurs:
			if s.id == id:
				self.current_slurs.remove (s)
				s.end_chord = self.chords[-1]
				return
		self.pending_slur = Slur(id)

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

	def idstring (self):
		return 'staff%svoice%s ' % (encodeint (self.staff.number) , encodeint(self.number))

	def dump (self):
		left = ''
		ln = ''
		for e in self.entries:
			next = ' ' + e.dump ()
			if next[-1] == '\n':
				left  = left + ln + next
				ln = ''
				continue

			if 0 and len (ln) +len (next) > 72:
				left = left+ ln + '\n'
				ln = ''
			ln = ln + next


		left = left  + ln
		id = self.idstring ()

		left = '%s =  { \n %s }\n '% (id, left)
		return left

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


class Clef:
	def __init__ (self, cl):
		self.type = cl

	def dump(self):
		return '\\clef ' + self.type + '\n'


class Key:
	def __init__ (self, key):
		self.type = key

	def dump(self):
		return '\\key ' + self.type + '\n'

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


class Staff:
	def __init__ (self):
		self.voices = (Voice (), Voice())
		self.clef = None
		self.instrument = 0
		self.voice_idx = 0
		self.number = None
		self.key = 0

		i = 0
		for v  in self.voices:
			v.staff = self
			v.number = i
			i = i+1

	def set_clef (self, letter):
		if clef_table.has_key (letter):
			clstr = clef_table[letter]
			self.voices[0].add_nonchord (Clef (clstr))
		else:
			sys.stderr.write ("Clef type `%c' unknown\n" % letter)

	def set_meter(self, meter, pickup):
		for v in self.voices:
			v.add_nonchord(meter)
			v.set_meter(meter, pickup)

	def set_key(self, keysig):
		self.key = keysig
		sys.stderr.write("Key sig %d\n" % keysig)
		if keysig >= 0:
			key = '+' + str(keysig)
		else:
			key = '-' + str(-keysig)
		sys.stderr.write("Key sig[%d] %s\n" % (keysig , key_table[key]))
		keystr = key_table[key]
		for v in self.voices:
			v.add_nonchord(Key(keystr))
			v.set_keysig(keysig)
		for i in range(7):
			sys.stderr.write ("%c -> %d " % (chr(ord('c') + i), self.voices[0].default_alteration[i]))

	def current_voice (self):
		return self.voices[self.voice_idx]

	def next_voice (self):
		self.voice_idx = (self.voice_idx + 1)%len (self.voices)

	def calculate (self):
		for v in self.voices:
			v.calculate ()

	def idstring (self):
		return 'staff%s' % encodeint (self.number)

	def dump (self):

		if len(self.voices) == 1:
			out = v.dump
			refs = v.idstring
		else:
			out = ''
			refs = ''
			for v in self.voices:
				out = v.dump() + "\n\n" + out
			count = 0
			for v in reversed(self.voices):
				refs = refs + '\n   { \\' + v.idstring () + ' }'
				if v != self.voices[0]:
					refs = refs + ' \\\\'
				count = count + 1

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
		self.basic_duration = 0
		self.scripts = []
		self.grace = 0
		self.chord_prefix = ''
		self.chord_suffix = ''
		self.note_prefix = ''
		self.note_suffix = ''
		self.multibar = 0
		self.skip = False

	def dump (self):
		if self.multibar != 0:
			if self.grace:
				sys.stderr.write('Ooppsss... multibar grace?????\n')
			if self.skip:
				rest = 's'
			else:
				rest = 'R'
			multibar = self.multibar
			if multibar == -1:
				multibar = 0
			v = self.chord_prefix + rest + str(self.basic_duration) + "*" + str(multibar) + self.chord_suffix
			return v

		v = ''
		sd = ''
		if self.basic_duration == 0.5:
			sd = '\\breve'
		else:
			sd = '%d' % self.basic_duration
		sd = sd + '.' * self.dots
		for p in self.pitches:
			if v:
				v = v + ' '
			if self.skip:
				v = v + 's'
			else:
				v = v + pitch_to_lily_string (p)

		if len (self.pitches) > 1:
			v = '<%s>' % v
		elif len (self.pitches) == 0:
			v = 'r'

		v = v + sd
		for s in self.scripts:
			v = v + '-' + s

		v = self.note_prefix + v + self.note_suffix
		if self.grace:
			sys.stderr.write('chord is grace: %s %s %s\n' % (self.chord_prefix, v, self.chord_suffix))
		v = self.chord_prefix + v + self.chord_suffix

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
		self.forced_duration = None
		self.last_name = 0
		self.last_oct = 0
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
		self.accidental_mode = ACCIDENTAL_ABSOLUTE
		self.paper_size = PAPER_A4

		self.parse (filename)

	def set_staffs (self, number):
		self.staffs = map (lambda x: Staff (), range(0, number))

		self.staff_idx = 0

		i =0
		sys.stderr.write("Key sig %d\n" % self.keysig)
		for s in self.staffs:
			s.number = i
			s.set_meter(self.meter, self.pickup)
			s.set_key(self.keysig)
			i = i+1

	def current_staff (self):
		return self.staffs[self.staff_idx]

	def current_voice (self):
		return self.current_staff ().current_voice ()

	def next_staff (self):
		self.staff_idx = (self.staff_idx + 1)% len (self.staffs)
		self.current_staff().voice_idx = 0

	def add_skip_bar(self):
		basic_duration = str(self.meter.denom) + "*" + str(self.meter.num)
		ch = Chord ()
		self.current_voice().add_chord(ch)
		ch.multibar = 1
		ch.skip = True
		ch.basic_duration = basic_duration
		self.current_voice().time = self.meter.to_rat()	# should be (0, 1) ????
		(d, s) = self.current_voice().time
		sys.stderr.write("add_skip_bar(): time %d / %d\n" % (d, s))
		self.current_voice().bar += 1
		self.current_voice ().add_nonchord (Barnumber (self.current_voice().bar))

	def add_markup(self, elevation, text):
		ch = Chord()
		self.current_voice().add_chord(ch)
		ch.basic_duration = self.meter.denom
		ch.multibar = -1
		ch.skip = True
		ch.pitches.append((0, 's', 0, 0))
		ch.chord_suffix = ch.chord_suffix + "^\\markup{" + text + "}"

	def parse_note (self, left):
		name = None
		ch = None

		grace = None
		if left[0] == 'G':
			left = left[1:]
			slashed = 0
			slurred = 0
			items = -1
			after = 0
			direction = 0
			basic_duration = 4
			# process the grace note options
			while left[0] in '0123456789msxluAW':
				c = left[0]
				left = left[1:]
				if c == 'm':
					if not left[0] in '0123456789':
						sys.stderr.write("""
Huh? expected number of grace note beams, found %d Left was `%s'""" % (left[0], left[:20]))
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
				else:
					if not c in '0123456789':
						sys.stderr.write("""
Huh? expected number of grace notes, found %s Left was `%s'""" % (c, left[:20]))
					else:
						if items == -1:
							items = ord(c) - ord('0')
						else:
							items = 10 * items + ord(c) - ord('0')
			if items == -1:
				items = 1
			# sys.stderr.write("Detect grace items %d\n" % items)
			grace = Grace(items, slashed, slurred, after, direction)

		if left[0] == 'z':
			ch = self.current_voice().last_chord()
			left = left[1:]
		else:
			ch = Chord ()
			self.current_voice().add_chord (ch)

		if grace:
			self.current_voice().pending_grace = grace

		self.current_voice().handle_pending()

		# what about 's'?
		alteration = 0
		if left[0] <> 'r':
			name = (ord (left[0]) - ord('a') + 5) % 7
			# sys.stderr.write("Process note '%s' name '%d'\n" % (left[0], name))
			if self.current_voice().altered[name]:
				alteration = self.current_voice().alteration[name]
			else:
				alteration = self.current_voice().default_alteration[name]
			# alteration = self.alteration[name]

		left = left[1:]

		forced_duration  = 0
		dots = 0
		oct = None
		durdigit = None
		multibar = 0
		tupnumber = 0
		extra_oct = 0
		flats = 0
		sharps = 0
		accidental_flags = 0
		# sys.stderr.write("Process token '%s'" % left)
		while left[0] in 'aber</>dsfmpnul0123456789.,+-\\':
			c = left[0]
			if c == 'a':
				sys.stderr.write("\nFIXME: Somehow specify \\noBeam")
			elif c == 'b':
				ch.skip = True
			elif c in 'er</>':
				sys.stderr.write("\nFIXME: horizontal shift not implemented")
			elif c in 'fns':
				if c == 'f':
					flats = flats + 1
					if -flats != self.current_voice().alteration[name]:
						self.current_voice().altered[name] = 1
						self.current_voice().alteration[name] = -flats
						alteration = -flats
				elif c == 'n':
					alteration = 0
					self.current_voice().altered[name] = 1
					self.current_voice().alteration[name] = alteration
				elif c == 's':
					sharps = sharps + 1
					if sharps != self.current_voice().alteration[name]:
						self.current_voice().altered[name] = 1
						self.current_voice().alteration[name] = sharps
						alteration = sharps
				# sys.stderr.write("Set alteration to %d\n" % alteration)
				if len(left) == 1:
					pass
				elif left[1] in '+-':
					sys.stderr.write("\nFIXME: accidental/dot horizontal shift not implemented")
				elif left[1] == 'c':
					accidental_flags = accidental_flags | FLAG_CAUTIONARY;
					left = left[1:]
				elif left[1] == 'i':
					accidental_flags = accidental_flags | FLAG_SUPPRESS;
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
					sys.stderr.write("\nFIXME: accidental/dot horizontal shift not implemented")
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
				oct = string.atoi (c) - 3
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

		if not grace:
			if left[0] == 'x':
				left = left[1:]
				tupnumber = string.atoi (left[0])
				left = left[1:]
				left = re.sub (r'^n?f?[+-0-9.]*', '' , left)

			if multibar != 0:
				basic_duration = str(self.meter.denom) + "*" + str(self.meter.num)
			elif durdigit:
				try:
					basic_duration =  basicdur_table[durdigit]
					self.last_basic_duration = basic_duration
				except KeyError:
					sys.stderr.write ("""
Huh? expected duration, found %d Left was `%s'""" % (durdigit, left[:20]))

					basic_duration = 4
			else:
				basic_duration = self.last_basic_duration

		if name <> None and oct == None:
			e = 0
			if self.last_name < name and name -self.last_name > 3:
				e = -1
			elif self.last_name > name and self.last_name -name > 3:
				e = 1

			oct = self.last_oct  +e + extra_oct

		if name <> None:
			self.last_oct = oct
			self.last_name = name

		if name <> None:
			ch.pitches.append ((oct, name, alteration, accidental_flags))

		# do before adding to tuplet.
		ch.multibar = multibar
		ch.basic_duration = basic_duration

		if multibar == 0:
			ch.dots = dots
			if forced_duration:
				self.forced_duration = ch.basic_duration / forced_duration

			if tupnumber:
				tup =Tuplet (tupnumber, basic_duration, dots)
				self.tuplets_expected = tupnumber
				self.tuplets.append (tup)

		if not grace:
			# if tupnumber != 0:
			#	sys.stderr.write("Would add tuplet basic_duration %f tupnumber %d\n" % (basic_duration, tupnumber))
			if multibar != 0:
				self.current_voice().time = self.meter.to_rat()
			else:
				if basic_duration == 0.5:
					t = (2, 1)
				elif basic_duration == 0:
					t = (1, 1)
				else:
					t = (1, int(basic_duration))
				for d in range(dots):
					t = rat_multiply(t, (3, 2))
				if self.tuplets_expected > 0:
					(d, n) = t
					t = rat_multiply(t, (1, self.tuplets[-1].replaces))
					t = rat_multiply(t, (self.tuplets[-1].replaces, self.tuplets[-1].number))
				(d, n) = t
				self.current_voice().time = rat_add(self.current_voice().time, t)
				(d, n) = self.current_voice().time;

			if self.current_voice().time == self.meter.to_rat():
				self.current_voice().time = (0, 1)
				if multibar > 0:
					self.current_voice().bar += multibar
				else:
					self.current_voice().bar += 1
				for i in range(7):
					self.current_voice().altered[i] = 0
					self.current_voice().alteration[i] = self.current_voice().default_alteration[i]
				self.current_voice ().add_nonchord (Barnumber (self.current_voice().bar))

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
			keystr = key_table[key]
			self.current_voice().add_nonchord (Key(keystr))
		return(left)

	def parse_header  (self, ls):
		def atonum(a):
			if re.search('\\.', a):
				return string.atof (a)
			else:
				return string.atoi (a)

		number_count = 12
		numbers = []

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

		(no_staffs, no_instruments, timesig_num, timesig_den, ptimesig_num, ptimesig_den, pickup_beats, keysig_number) = tuple (numbers[0:8])
		(no_pages,no_systems, musicsize, fracindent) = tuple (numbers[8:])

		self.meter = Meter(timesig_num, timesig_den)
		self.pickup = (pickup_beats, timesig_den)
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

	def parse_tie (self, left):
		left = left[1:]

		(id, left) = self.parse_id(left)
		self.current_voice ().toggle_tie(id)

		return left

	def parse_slur (self, left):
		left = left[1:]

		(id, left) = self.parse_id(left)

		self.current_voice ().toggle_slur (id)
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
				while True:
					m = re.match('([-+][stc]|l)+', left)
					if not m:
						break
			elif c == 'r':
				self.accidental_mode = ACCIDENTAL_RELATIVE
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

	def expand_tex(self, left, nparam):
		type = 1
		while left[0] == '\\':
			type += type
			left = left[1:]
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
				start = 1
				if left[1:b].startswith('\\ref'):
					start = start + len('\\ref')
				params.append(left[start:b])
				left = left[b + 1:]
			else:
				raise Exception('TeX parameter must start with \'{\'', left[0])
		return (left, params)

	def parse_body (self, left):
		preamble = 1

		while left:
			c = left[0]
			if c == '%':
				f = string.find (left, '\n')
				if f < 0:
					left = ''
				left = left[f+1:]
			elif c == 'm':
				left = left[1:]
				m = re.match ('([o0-9]/[o0-9]/[o0-9]/[o0-9])', left)
				if m:
					comps = m.split('/')
					nums = m.group (1)
					left = left[len (nums):]
					nums = map (string.atoi , nums)
					self.current_voice ().add_nonchord (Meter (comps[0], comps[1]))
					self.meter = Meter(comps[0], comps[1])
					continue

				sys.stderr.write("See meter in %s\n" % left[0:20])
				m = re.match ('([0-9o]+)', left)
				if m:
					nums = m.group (1)
					syms = map(string.atoi, nums)
					meter = Meter(syms[0], syms[1])
					self.current_voice ().add_nonchord (meter)
					self.meter = meter
					continue

			elif left[0] in 'lh':
				f = string.find (left, '\n')
				if f <0 :
					left = ''
				else:
					left = left[f+1:]

				f = string.find (left, '\n')
				title = left[:f]
				left=left[f+1:]
			elif c in 'Gzabcdefgr':
				left = self.parse_note (left)
			elif c in DIGITS + 'n#-':
				left = self.parse_basso_continuo (left)
			elif c in SPACE:
				left = left[1:]
			elif c in '{}':
				left = self.parse_tie (left)
			elif c in 's()':
				left = self.parse_slur (left)
			elif c == '|':
				left = self.parse_barcheck (left)
			elif c == 'o':
				left = self.parse_ornament (left)
			elif c == 'x':
				left = self.parsex (left)
			elif c == 'C':
				self.current_staff().set_clef(str(left[1]))
				left = left[2:]
			elif c == 'K':
				left = self.parse_key (left)
			elif c in "[]":
				left = self.parse_beams (left)
			elif left[:2] == "//":
				self.current_staff().next_voice ()
				left = left[2:]
			elif c == '/':
				for v in self.current_staff().voices[self.current_staff().voice_idx + 1:]:
					v = self.current_staff().next_voice()
					self.add_skip_bar()
				for s in self.staffs[self.staff_idx + 1:]:
					self.next_staff()
					for v in s.voices:
						v = s.next_voice()
						self.add_skip_bar()
				self.next_staff ()
				left = self.parse_barcheck (left)
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

				f = string.find(left, '\n');
				left = left[f+1:];
				f = string.find(left, '\n');
				sys.stderr.write("\nSee title block '%s'\n" % left[0:f])
				if left[0] == 'i':
					self.instrument = left[0:f]
				elif left[0] == 'c':
					self.composer = left[0:f]
				elif left[0] == 't':
					self.title = left[0:f]
				left = left[f:]
			elif c == 'A':
				left = self.parse_global(left[1:])
			elif c == '\\':
				# handle just a few MusiXTeX commands
				if False:
					pass
				elif left.startswith('\\zcharnote'):
					(left, params) = self.expand_tex(left[len('\\zcharnote'):], 2)
					self.add_markup(params[0], params[1])

					# self.current_voice().last_chord().chord_suffix = self.current_voice().last_chord().chord_suffix + "^\markup{" + params + "}"
					sys.stderr.write("Where can I append \\markup{%s} ?\n" % params[1])
				elif left.startswith('\\lcharnote'):
					(left, params) = self.expand_tex(left[len('\\lcharnote'):], 2)
				elif left.startswith('\\ccharnote'):
					(left, params) = self.expand_tex(left[len('\\lcharnote'):], 2)
				elif left.startswith('\\zchar'):
					(left, params) = self.expand_tex(left[len('\\lcharnote'):], 2)
				elif left.startswith('\\lchar'):
					(left, params) = self.expand_tex(left[len('\\lcharnote'):], 2)
				elif left.startswith('\\cchar'):
					(left, params) = self.expand_tex(left[len('\\lcharnote'):], 2)
				elif left.startswith('\\zql'):
					sys.stderr.write('\nFIXME: handle MusiXTeX macro \\zql, left %s' % left[:20])
					(left, params) = self.expand_tex(left[len('\\zql'):], 1)
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

				elif left[1] in ' \n\t':
					left = left[1:]
				else:
					sys.stderr.write('\nFIXME: ignore TeX command %s' % left[:20])
					done = left.find('\\')
					left = left[done + 1:]
			else:
				sys.stderr.write ("""
Huh? Unknown directive `%s', before `%s'""" % (c, left[:20] ))
				left = left[1:]

	def dump (self):
		sys.stderr.write("Title %s composer %s instrument %s\n" % (self.title, self.composer, self.instrument))

		defaults = '\n\
    \\compressFullBarRests\n\
    \\accidentalStyle modern-cautionary\n\
'
		left = ''

		refs = ''
		for s in self.staffs:
			left = s.dump() + left
			refs = '\\' + s.idstring() + refs

		left = left + "\n\n\\score { <<\n    %s\n    %s\n >> }" % (refs , defaults)
		return left


	def parse (self,fn):
		ls = open (fn).readlines ()
		def subst(s):
			return re.sub ('%.*$', '', s)

		ls = map (subst, ls)
		# ls = filter (lambda x: x <> '\n', ls)
		ls = self.parse_header (ls)
		left = string.join (ls, ' ')

#		print left
		self.parse_body (left)
		for c in self.staffs:
			c.calculate ()





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


