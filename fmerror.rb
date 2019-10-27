# frozen_string_literal: true

FMError = Class.new(StandardError)

FMNoFreeSlotError = Class.new(FMError)
FMNoActiveNoteError = Class.new(FMError)
