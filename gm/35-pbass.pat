op.r 0 1.002
op.e 0 100.0 0.7 0.1 0.8 5.0
op.r 1 0.998
op.e 1 100.0 10.0 0.5 0.7 5.0
op.r 2 1.002
op.e 2 160.0 0.5 0.1 0.7 100.0 ; Resonance
op.r 3 0.998
op.e 3 80.0 4.0 10.0 0.7 15.0  ; Pluck
prog
  feed 0
  mix 1 0.25 ; FB
  feed 1
  mix 0 1.2
  feed 2
  mix 1 0.4
  feed 3
  mix 2 1.1
  mix 3 0.4
  out
end
