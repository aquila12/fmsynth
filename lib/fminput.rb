# frozen_string_literal: true

require_relative 'fmerror'

# FM Command input processor
class FMInput
  def initialize(controller, driver)
    @controller = controller
    @driver = driver
    @mapping = {}
    @next_ins = 0
    @patches = {}
    @dir = ['.']
  end

  def emit(str)
    @driver.emit str
  end

  def load(file)
    filename = File.join(@dir.last, file)
    @dir.push File.dirname(filename)
    begin
      warn "Loading #{filename}"
      File.foreach(filename) do |line|
        process(line)
      end
    ensure
      @dir.pop
    end
  end

  def process(line)
    params = line.split(';').first.strip.split(/\s+/)
    return unless params[0]

    if params[0].start_with? '.'
      handle(params)
    else
      driver.emit(line)
    end
  end

  def add_mapping(instrument, str_params)
    unless @mapping.key? instrument
      @mapping[instrument] = @next_ins
      @next_ins += 1
    end

    params = str_params.map { |str| [ str[0].to_s, str[1..-1].to_i ] }
    @controller.instrument_mapping.push [@mapping[instrument], params]
  end

  def add_patch(instrument, polyphony, patchfile)
    @patches[instrument] = {poly: polyphony, file: File.join(@dir.last, patchfile)}
  end

  def emit_settings
    emit 'edit ; Auto config'
    emit "clr #{@next_ins}"
    @mapping.each do |ins, number|
      patch = @patches[ins]
      emit_patch patch, number if patch
      @controller.prepare_instrument number, patch[:poly]
    end
    emit "end ; End config"
  end

  def emit_patch(patchdef, number)
    emit "sel #{number} #{patchdef[:poly]}"
    patch = File.read patchdef[:file]
    ops, lfos, instructions = calibrate_patch patch
    emit "pat #{ops} #{lfos} #{instructions}"
    emit patch
    emit "end"
  end

  def calibrate_patch(patch)
    ops = []
    lfos = []
    instructions = 0
    prog = false
    patch.each_line do |line|
      params = line.split(';').first.strip.split(/\s+/)
      instructions += 1 if prog
      case params[0]
      when 'lfo'
        lfos << params[1].to_i
      when /^op\..$/
        ops << params[1].to_i
      when 'prog'
        prog = true
      when 'end'
        prog = false
      end
    end
    [ops.max.to_i + 1, lfos.max.to_i + 1, instructions - 1]
  end

  def handle(params)
    cmd = params.first
    case cmd
    when '.inc'
      load params[1].to_s
    when '.map'
      add_mapping params[1], params[2..-1]
    when '.pat'
      add_patch params[1], params[2].to_i, params[3].to_s
    when '.set'
      emit_settings
    else
      raise FMUnknownCommandError
    end
  end
end
