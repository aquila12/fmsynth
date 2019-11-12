op.a 0 60.7
op.e 0 80 10 7 0.7 7    ; Resonance
op.a 1 40.3
op.e 1 150 15 8 0.7 8 ; FB
op.a 2 80.4
op.e 2 150 30 10 0.6 10 ; Kick
prog
  mix 1 0.08
  feed 1
  mix 1 0.4
  feed 0
  mix 1 0.34
  feed 2
  mix 0 1.0
  mix 2 1.2
  out
end
