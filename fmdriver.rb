# frozen_string_literal: true

# FM Debugging Driver
class FMNullDriver
  def slot_ampl(slot, ampl)
    warn "Slot #{slot} ampl = #{ampl}"
  end

  def slot_freq(slot, fmnn)
    warn "Slot #{slot} note = #{fmnn}"
  end

  def slot_keydown(slot)
    warn "Slot #{slot} keydown"
  end

  def slot_keyup(slot)
    warn "Slot #{slot} keyup"
  end

  def render(n)
    warn "Render #{n} samples"
  end
end

class FMStdoutDriver
  def slot_ampl(slot, ampl)
    puts "ampl 0 #{slot} #{ampl}"
  end

  def slot_freq(slot, fmnn)
    puts "freq 0 #{slot} #{fmnn}"
  end

  def slot_keydown(slot)
    puts "keyd 0 #{slot}"
  end

  def slot_keyup(slot)
    puts "keyu 0 #{slot}"
  end

  def render(n)
    puts "run #{n}"
  end
end
