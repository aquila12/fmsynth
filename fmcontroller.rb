# frozen_string_literal: true

require 'set'
require_relative 'fmerror'

# Dispatches MIDI events to FM operator slots
class FMController
  SAMPLE_RATE = 48_000
  MIDI_BEAT_DIVISIONS = 96
  DT_CONSTANT = (MIDI_BEAT_DIVISIONS.to_f * 1_000_000 / SAMPLE_RATE)

  def initialize
    reset
  end

  def reset
    @voice_selection = Hash.new { |vs, ch| vs[ch] = [0, 0] } # bank, prog
    @active_notes = Hash.new { |an, ch| an[ch] = Set.new } # note
    @pitch_bend = Hash.new { |pb, ch| pb[ch] = 0x2000 } # pitchbend

    @instr_free_slots = Hash.new { |fs, instr| fs[instr] = Set.new } # slot_number
    @instr_note_slot = Hash.new { |ns, instr| ns[instr] = {} } # h[note_number] => slot

    self.tempo = 500_000 # Default i.e. 120 bpm

    prepare_instrument(0, 0, 32)
  end

  def tempo=(us_per_beat)
    @dt_mult = us_per_beat.to_f / DT_CONSTANT
  end

  def prepare_instrument(instrument, first_slot, num_slots)
    # NB: these may be slot numbers returned from the kernel
    last_slot = first_slot + num_slots - 1
    @instr_free_slots[instrument] += (first_slot..last_slot)
  end

  def get_instrument(channel, note)
    bank, prog = @voice_selection[channel]
    # Check patching rules and return an FM instrument number
    0
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

  def msg_slot_ampl(slot, velo)
    ampl = velo << 17
    warn "Slot #{slot} ampl = #{ampl}"
  end

  def msg_slot_freq(slot, chan, note)
    bend = @pitch_bend[chan] - 0x2000
    fmnn = (bend << 12) + (note << 24)
    warn "Slot #{slot} note = #{fmnn}"
  end

  def msg_slot_keydown(slot)
    warn "Slot #{slot} keydown"
  end

  def msg_slot_keyup(slot)
    warn "Slot #{slot} keyup"
  end

  def msg_render(delta)
    n = (delta * @dt_mult).round
    warn "Render #{n} samples"
  end

  def midi_tempo(event)
    self.tempo = event.tempo
  end

  def midi_noteon(event)
    e = event
    slot = _midi_slot(e.channel, e.note, action: :take)
    msg_slot_ampl(slot, e.velocity)
    msg_slot_freq(slot, e.channel, e.note)
    msg_slot_keydown(slot)
    @active_notes[e.channel].add e.note
  end

  def midi_noteoff(event)
    e = event
    slot = _midi_slot(e.channel, e.note, action: :release)
    msg_slot_keyup(slot)
    @active_notes[e.channel].delete e.note
  end

  def midi_pitchbend(event)
    @pitch_bend[event.channel] = event.value
    @active_notes[event.channel].each do |note|
      slot = _midi_slot(event.channel, note)
      msg_slot_freq(slot, event.channel, note)
    rescue FMNoActiveNoteError => e
      warn "FM Pitchbent note no longer active: #{note}"
    end
  end

  def midi_polypressure(event)
    e = event

    slot = _midi_slot(e.channel, e.note)
    msg_slot_ampl(slot, e.velocity)
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
    msg_render(event.delta_time) if event.delta_time.positive?

    _dispatch(event)
  rescue FMError => e
    warn "FM Handler: #{e}"
  end
end
