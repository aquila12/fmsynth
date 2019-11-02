# frozen_string_literal: true

require 'set'
require_relative 'fmerror'
require_relative 'fmdriver'

# Dispatches MIDI events to FM operator slots
class FMController
  SAMPLE_RATE = 48_000
  SAMPLES_PER_US = SAMPLE_RATE / 1_000_000.0

  attr_reader :driver
  attr_accessor :instrument_mapping

  def initialize(driver, divisions_per_beat)
    @driver = driver
    @div_per_beat = divisions_per_beat
    reset
  end

  def vel2ampl(vel)
    vel << 17
  end

  def note2fmnn(ch, note)
    (note << 24) + ((@pitch_bend[ch] - 0x2000) << 12)
  end

  def delta2samples(dt)
    (dt * @dt_mult).round
  end

  def reset
    @voice_selection = Hash.new { |vs, ch| vs[ch] = [0, 0] } # bank, prog
    @active_notes = Hash.new { |an, ch| an[ch] = Set.new } # note
    @pitch_bend = Hash.new { |pb, ch| pb[ch] = 0x2000 } # pitchbend

    @instr_free_slots = Hash.new { |fs, instr| fs[instr] = Set.new } # slot_number
    @instr_note_slot = Hash.new { |ns, instr| ns[instr] = {} } # h[note_number] => slot

    @instrument_mapping = []

    self.tempo = 500_000 # Default i.e. 120 bpm
  end

  def tempo=(us_per_beat)
    @dt_mult = us_per_beat.to_f * SAMPLES_PER_US / @div_per_beat
  end

  def prepare_instrument(instrument, num_slots)
    @instr_free_slots[instrument] += (0..num_slots-1)
  end

  def get_instrument(channel, note)
    bank, prog = @voice_selection[channel]
    match_params = { 'c' => channel, 'n' => note, 'b' => bank, 'p' => prog }
    instrument = @instrument_mapping.each do |instrument, params|
      return instrument if params.all? { |k,v| match_params[k] == v }
    end
    raise FMNoInstrumentError
  end

  def take_slot(instrument, note)
    slot = @instr_note_slot[instrument][note] || @instr_free_slots[instrument].first || raise(FMNoFreeSlotError)
    @instr_free_slots[instrument].delete slot
    @instr_note_slot[instrument][note] = slot
  end

  def release_slot(instrument, note)
    slot = @instr_note_slot[instrument].delete note
    @instr_free_slots[instrument].add slot if slot
    slot
  end

  def _midi_slot(channel, note, action: :none)
    instrument = get_instrument(channel, note)
    chnote = channel << 8 + note
    slot = case action
           when :take
             take_slot(instrument, chnote)
           when :release
             release_slot(instrument, chnote)
           else
             @instr_note_slot[instrument][chnote]
           end
    slot || raise(FMNoActiveNoteError)
  end

  def midi_tempo(event)
    self.tempo = event.tempo
  end

  def midi_noteon(event)
    e = event
    slot = _midi_slot(e.channel, e.note, action: :take)
    driver.slot_ampl(slot, vel2ampl(e.velocity))
    driver.slot_freq(slot, note2fmnn(e.channel, e.note))
    driver.slot_keydown(slot)
    @active_notes[e.channel].add e.note
  end

  def midi_noteoff(event)
    e = event
    slot = _midi_slot(e.channel, e.note, action: :release)
    driver.slot_keyup(slot)
    @active_notes[e.channel].delete e.note
  end

  def midi_pitchbend(event)
    @pitch_bend[event.channel] = event.value
    @active_notes[event.channel].each do |note|
      slot = _midi_slot(event.channel, note)
      driver.slot_freq(slot, note2fmnn(event.channel, note))
    rescue FMNoActiveNoteError => e
      warn "FM Pitchbent note no longer active: #{note}"
    end
  end

  def midi_polypressure(event)
    e = event

    slot = _midi_slot(e.channel, e.note)
    driver.slot_ampl(slot, vel2ampl(e.velocity))
  end

  def _dispatch(event)
    case event
    when MIDI::Tempo
      midi_tempo event
    when MIDI::NoteOn
      midi_noteon event
    when MIDI::PolyPressure
      midi_polypressure event
    when MIDI::NoteOff
      midi_noteoff event
    when MIDI::PitchBend
      midi_pitchbend event
    end
  end

  def handle_event(event)
    driver.render(delta2samples(event.delta_time)) if event.delta_time.positive?

    _dispatch(event)
  rescue FMError => e
    warn "FM Handler: #{e}"
  end
end
