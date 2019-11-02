lfo 0 5.0
lfo 1 4.0
op.r 0 2.0 ; Operator 0 relative frequency at 2.0*f
op.r 1 1.0 ; Operator 1 relative frequency at 1.0*f
op.e 1 50.0 10.0 0.2 0.7 25.0 ; Operator 1 envelope
prog
  lmix 0 0.006
  feed 0
  mix 0 1.8
  feed 1
  mix 1 1.0
  amod 1 0.2
  out
end
