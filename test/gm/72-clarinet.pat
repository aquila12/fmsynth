lfo 0 4.0
lfo 1 4.0
op.r 0 2.003 ; Operator 0 relative frequency at 2.0*f
op.r 1 0.997 ; Operator 1 relative frequency at 1.0*f
op.e 1 30.0 10.0 0.0 0.7 20.0 ; Operator 1 envelope
prog
  lmix 0 0.01
  feed 0
  mix 0 0.33
  feed 1
  mix 1 1.0
  amod 1 0.1
  out
end
