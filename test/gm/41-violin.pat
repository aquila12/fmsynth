lfo 0 0.87
lfo 1 4.0
op.r 0 2.0 ; Operator 0 relative frequency at 2.0*f
op.r 2 1.0
op.r 1 2.0 ; Operator 1 relative frequency at 1.0*f
op.e 1 30.0 30.0 0.0 0.7 20.0 ; Operator 1 envelope
prog
  feed 0
  feed 2
  mix 0 6
  mix 2 3
  lmix 0 0.003
  feed 1
  mix 1 1.0
  amod 1 0.1
  out
end
