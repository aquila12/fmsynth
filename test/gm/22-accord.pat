lfo 0 5.0
lfo 1 4.0
op.r 0 2.0 ; Operator 0 relative frequency at 2.0*f
op.r 1 1.01 ; Operator 1 relative frequency at 1.0*f
op.r 2 1.98
op.e 1 20.0 10.0 0.2 0.7 15.0 ; Operator 1 envelope
op.e 2 10.0 10.0 0.2 0.7 15.0
prog
  lmix 0 0.001
  feed 0
  mix 0 5.0
  feed 1
  mix 0 6.0
  feed 2
  mix 1 1.0
  mix 2 1.0
  amod 1 0.2
  out
end
