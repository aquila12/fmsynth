#!/usr/bin/env ruby
# frozen_string_literal: true

require 'rubygems'
require 'bundler'
require_relative 'fmcontroller'

Bundler.require

# Create a new, empty sequence.
seq = MIDI::Sequence.new()

# Read the contents of a MIDI file into the sequence.
ARGF.binmode

seq.read(ARGF) do | track, num_tracks, i |
    # Print something when each track is read.
    warn "read track #{i} of #{num_tracks}"
end

controller = FMController.new

# Merge down to a single event list
track = MIDI::Track.new(nil)
seq.each { |in_track| track.merge in_track.events }

track.each do |e|
  e.print_decimal_numbers = true # default = false (print hex)
  e.print_note_names = true # default = false (print note numbers)
  warn e

  controller.handle_event(e)
end
