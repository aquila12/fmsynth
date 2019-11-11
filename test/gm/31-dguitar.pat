lfo 0 5.0
lfo 1 4.0
op.r 0 3.0 ; Operator 0 relative frequency at 2.0*f
op.r 2 2.1
op.r 1 1.5 ; Operator 1 relative frequency at 1.0*f
op.e 1 800.0 10.0 0.2 0.7 10.0 ; Operator 1 envelope
prog
  mix 2 0.13
  feed 2
  lmix 0 0.0
  mix 2 0.08
  feed 0
  mix 0 0.64
  feed 1
  mix 1 1.0
  amod 1 0.1
  out
end
