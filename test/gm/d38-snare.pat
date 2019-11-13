op.a 0 165.7
op.e 0 150 8 6 0.8 6    ; Resonance
op.a 1 45.3
op.e 1 100 0.5 1 0.7 1
op.a 3 80.8
op.e 3 100 0.5 1 0.7 1
op.a 2 165.4
op.e 2 150 30 10 0.6 10 ; Kick
prog
  mix 1 0.35
  feed 1
  mix 3 0.34
  feed 3
  mix 1 0.4
  mix 3 0.25
  feed 0
  mix 1 0.2
  feed 2
  mix 0 1.0
  mix 2 1.2
  out
end
