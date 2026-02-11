OSCAR64 ?= ../../oscar64/build/oscar64

FLAGS = -tm=f256k -n -i=../f256lib

# Library source files (for dependency tracking)
LIB_HEADERS = $(wildcard ../f256lib/*.h)
LIB_SOURCES = $(wildcard ../f256lib/*.c)
LIB_FILES   = $(LIB_HEADERS) $(LIB_SOURCES)

DEPS = $(LIB_FILES)

# All targets
ALL_TARGETS = \
	Aero/Aero.pgz \
	andy/andy.pgz \
	animSprites/animSprites.pgz \
	BachHero/BachHero.pgz \
	backs/backs.pgz \
	badapple/badapple.pgz \
	badapple2x/badapple2x.pgz \
	bigslidesho/bigslidesho.pgz \
	bmptest/bmptest.pgz \
	BitLadder/BitLadder.pgz \
	callisto/callisto.pgz \
	circles/circles.pgz \
	cScale/cScale.pgz \
	digestMidi/digestMidi.pgz \
	ether/ether.pgz \
	f256amp/f256amp.pgz \
	fileSize/fileSize.pgz \
	fileWrite/fileWrite.pgz \
	fileWrite2/fileWrite2.pgz \
	fileWrite3/fileWrite3.pgz \
	fileWrite4/fileWrite4.pgz \
	fileWrite5/fileWrite5.pgz \
	float/float.pgz \
	FusionDrive/FusionDrive.pgz \
	fwrite6/fwrite6.pgz \
	goatt/goatt.pgz \
	grudge/grudge.pgz \
	gui/gui.pgz \
	himem/himem.pgz \
	interr/interr.pgz \
	lcd/lcd.pgz \
	lightshow/lightshow.pgz \
	mandel/mandel.pgz \
	memtest/memtest.pgz \
	mid/mid.pgz \
	midisam/midisam.pgz \
	midisam2/midisam2.pgz \
	midisam3/midisam3.pgz \
	midiStuff/midiStuff.pgz \
	midiTest/midiTest.pgz \
	minsid/minsid.pgz \
	modu/modu.pgz \
	mousing/mousing.pgz \
	mp3/mp3.pgz \
	mp3wSA/mp3wSA.pgz \
	multiRead/multiRead.pgz \
	mustart/mustart.pgz \
	myTimer/myTimer.pgz \
	NES/NES.pgz \
	newlines/newlines.pgz \
	openFile/openFile.pgz \
	opl3/opl3.pgz \
	opl3more/opl3more.pgz \
	opl3tweak/opl3tweak.pgz \
	overlay/overlay.pgz \
	overlay2/overlay2.pgz \
	overlay3/overlay3.pgz \
	ParseNPlayMidi/ParseNPlayMidi.pgz \
	pcm/pcm.pgz \
	psidplayer/psidplayer.pgz \
	sidtweak/sidtweak.pgz \
	simpleTiles/simpleTiles.pgz \
	skull/skull.pgz \
	sol/sol.pgz \
	sprite/sprite.pgz \
	struct/struct.pgz \
	textClock/textClock.pgz \
	textColors/textColors.pgz \
	textScroll/textScroll.pgz \
	tileTest/tileTest.pgz \
	timer0/timer0.pgz \
	timer1/timer1.pgz \
	toobig/toobig.pgz \
	vgmplayer/vgmplayer.pgz \
	vgmplayer2/vgmplayer2.pgz \
	via/via.pgz \
	vsrtmidi/vsrtmidi.pgz \
	vstest/vstest.pgz \
	VStype0/VStype0.pgz \
	write-on-screen/writeonscreen.pgz

# Note: NoFussDemos is excluded - contains pre-compiled binaries only.

.PHONY: all clean

all: $(ALL_TARGETS)

# Build rules - one per doodle
Aero/Aero.pgz: Aero/src/Aero.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

andy/andy.pgz: andy/src/andy.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

animSprites/animSprites.pgz: animSprites/src/animSprites.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

BachHero/BachHero.pgz: BachHero/src/BachHero.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

backs/backs.pgz: backs/src/backs.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

badapple/badapple.pgz: badapple/src/badapple.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

badapple2x/badapple2x.pgz: badapple2x/src/badapple2x.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

bigslidesho/bigslidesho.pgz: bigslidesho/src/bigslidesho.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

bmptest/bmptest.pgz: bmptest/src/bmptest.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

BitLadder/BitLadder.pgz: BitLadder/src/BitLadder.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

callisto/callisto.pgz: callisto/src/callisto.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

circles/circles.pgz: circles/src/circles.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

cScale/cScale.pgz: cScale/src/cScale.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

digestMidi/digestMidi.pgz: digestMidi/src/digestMidi.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

ether/ether.pgz: ether/src/ether.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

f256amp/f256amp.pgz: f256amp/src/f256amp.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fileSize/fileSize.pgz: fileSize/src/fileSize.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fileWrite/fileWrite.pgz: fileWrite/src/fileWrite.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fileWrite2/fileWrite2.pgz: fileWrite2/src/fileWrite2.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fileWrite3/fileWrite3.pgz: fileWrite3/src/fileWrite3.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fileWrite4/fileWrite4.pgz: fileWrite4/src/fileWrite4.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fileWrite5/fileWrite5.pgz: fileWrite5/src/fileWrite5.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

float/float.pgz: float/src/float.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

FusionDrive/FusionDrive.pgz: FusionDrive/src/FusionDrive.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

fwrite6/fwrite6.pgz: fwrite6/src/fwrite6.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

goatt/goatt.pgz: goatt/src/goatt.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

grudge/grudge.pgz: grudge/src/grudge.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

gui/gui.pgz: gui/src/gui.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

himem/himem.pgz: himem/src/himem.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

interr/interr.pgz: interr/src/interr.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

lcd/lcd.pgz: lcd/src/lcd.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

lightshow/lightshow.pgz: lightshow/src/lightshow.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

mandel/mandel.pgz: mandel/src/mandel.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

memtest/memtest.pgz: memtest/src/memtest.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

mid/mid.pgz: mid/src/mid.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

midisam/midisam.pgz: midisam/src/midisam.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

midisam2/midisam2.pgz: midisam2/src/midisam2.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

midisam3/midisam3.pgz: midisam3/src/midisam3.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

midiStuff/midiStuff.pgz: midiStuff/src/midiStuff.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

midiTest/midiTest.pgz: midiTest/src/midiTest.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

minsid/minsid.pgz: minsid/src/minsid.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

modu/modu.pgz: modu/src/modu.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

mousing/mousing.pgz: mousing/src/mousing.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

mp3/mp3.pgz: mp3/src/mp3.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

mp3wSA/mp3wSA.pgz: mp3wSA/src/mp3wSA.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

multiRead/multiRead.pgz: multiRead/src/multiRead.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

mustart/mustart.pgz: mustart/src/mustart.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

myTimer/myTimer.pgz: myTimer/src/myTimer.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

NES/NES.pgz: NES/src/NES.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

newlines/newlines.pgz: newlines/src/newlines.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

openFile/openFile.pgz: openFile/src/openFile.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

opl3/opl3.pgz: opl3/src/opl3.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

opl3more/opl3more.pgz: opl3more/src/opl3more.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

opl3tweak/opl3tweak.pgz: opl3tweak/src/opl3tweak.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

overlay/overlay.pgz: overlay/src/overlay.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

overlay2/overlay2.pgz: overlay2/src/overlay2.c overlay2/src/extra.c overlay2/src/extra.h $(DEPS)
	$(OSCAR64) $(FLAGS) -i=overlay2/src -o=$@ $<

overlay3/overlay3.pgz: overlay3/src/overlay3.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

ParseNPlayMidi/ParseNPlayMidi.pgz: ParseNPlayMidi/src/ParseNPlayMidi.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

pcm/pcm.pgz: pcm/src/pcm.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

psidplayer/psidplayer.pgz: psidplayer/src/psidplayer.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

sidtweak/sidtweak.pgz: sidtweak/src/sidtweak.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

simpleTiles/simpleTiles.pgz: simpleTiles/src/simpleTiles.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

skull/skull.pgz: skull/src/skull.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

sol/sol.pgz: sol/src/sol.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

sprite/sprite.pgz: sprite/src/sprite.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

struct/struct.pgz: struct/src/struct.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

textClock/textClock.pgz: textClock/src/textClock.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

textColors/textColors.pgz: textColors/src/textColors.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

textScroll/textScroll.pgz: textScroll/src/textScroll.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

tileTest/tileTest.pgz: tileTest/src/tileTest.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

timer0/timer0.pgz: timer0/src/timer0.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

timer1/timer1.pgz: timer1/src/timer1.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

toobig/toobig.pgz: toobig/src/toobig.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

vgmplayer/vgmplayer.pgz: vgmplayer/src/vgmplayer.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

vgmplayer2/vgmplayer2.pgz: vgmplayer2/src/vgmplayer2.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

via/via.pgz: via/src/via.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

vsrtmidi/vsrtmidi.pgz: vsrtmidi/src/vsrtmidi.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

vstest/vstest.pgz: vstest/src/vstest.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

VStype0/VStype0.pgz: VStype0/src/VStype0.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

write-on-screen/writeonscreen.pgz: write-on-screen/src/writeonscreen.c $(DEPS)
	$(OSCAR64) $(FLAGS) -o=$@ $<

clean:
	rm -f $(ALL_TARGETS)
	rm -f */*.asm */*.int */*.lbl */*.map
