# fmsynth

A phase-modulation software synthesizer (was FM; they are basically equivalent).

# How to run it

Get require gems with `bundle install`.  You might consider using `rvm`.

To play MIDI files (you might need `bundle exec` in front of it):

```
./midiseq.rb gm/gm.map /tmp/town.mid | ./fmsynth | aplay -r48000 -c1 -fS16_LE
```

Some recommended sample MIDI files:

```
wget https://www.vgmusic.com/music/console/sega/genesis/s3chromegadget.mid
wget https://archive.org/download/win3.0midi/PASSPORT.MID
wget https://www.thewindowsclub.com/downloads/3MIDI.zip
unzip 3MIDI.zip
```

Render synthesizer program from a MIDI file:

```
./midiseq.rb <header program> <midi file> > /tmp/output.prg
```

Render raw audio (1 channel, signed 16-bit little endian, 48kHz):

```
make
./fmsynth < /tmp/output.prg > /tmp/output.raw
```
