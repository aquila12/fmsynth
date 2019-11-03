#!/usr/bin/env ruby
# frozen_string_literal: true

require 'rubygems'
require 'bundler'
require_relative 'lib/fmcontroller'
require_relative 'lib/fminput'

Bundler.require

drivers = {
  'null' => FMNullDriver,
}

driver = drivers[ENV['FMDRIVER']] || FMStdoutDriver.new
debug = ENV['FMDEBUG'] == '1'

def load_midi_sequence(file)
  track = MIDI::Track.new(nil)
  ppqn = nil
  File.open(file, 'rb') do |mid|
    seq = MIDI::Sequence.new()
    seq.read mid
    ppqn = seq.ppqn

    seq.each { |in_track| track.merge in_track.events }
  end
  [track, ppqn]
end

input_file = ARGV.shift # The file sent to the input module
midi_file = ARGV.shift  # The MIDI file to play

track, ppqn = load_midi_sequence(midi_file)

controller = FMController.new(driver, ppqn)
input = FMInput.new(controller, driver)

input.load input_file

controller.dump_router if debug

track.each do |e|
  e.print_decimal_numbers = true # default = false (print hex)
  e.print_note_names = true # default = false (print note numbers)
  warn e if debug

  controller.handle_event(e)
end
