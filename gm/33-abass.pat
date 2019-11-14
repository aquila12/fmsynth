lfo 0 0.778
op.r 0 0.998
op.e 0 50.0 1.0 0.1 0.7 10.0
op.r 4 2.007
op.e 4 30.0 0.5 0.3 0.6 5.0
op.r 1 0.998
op.e 1 100.0 10.0 0.5 0.6 5.0
op.r 2 1.002
op.e 2 80.0 0.5 0.1 0.7 100.0 ; Resonance
op.r 3 0.998
op.e 3 80.0 6.0 4.0 0.7 10.0  ; Pluck
prog
  feed 0
  mix 1 0.29 ; FB
  feed 1
  mix 0 0.6
  mix 4 1.7
  lmix 0 0.5
  feed 2
  mix 1 0.3
  feed 3
  mix 2 1.0
  mix 3 0.4
  out
end
