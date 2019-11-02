# frozen_string_literal: true

FMError = Class.new(StandardError)

FMControllerError = Class.new(FMError)
FMInputError = Class.new(FMError)

FMNoFreeSlotError = Class.new(FMControllerError)
FMNoActiveNoteError = Class.new(FMControllerError)
FMNoInstrumentError = Class.new(FMControllerError)

FMUnknownCommandError = Class.new(FMInputError)
