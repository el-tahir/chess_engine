# Chess Engine

This is a really simple chess engine.

## Build

Linux:
```
make
```

Windows (from WSL, using MinGW cross-compiler):
```
make win
```

Clean:
```
make clean
```

## Commands (UCI)

The engine supports:
- `uci`
- `isready`
- `ucinewgame`
- `position startpos [moves ...]`
- `position fen <fen> [moves ...]`
- `go depth N`
- `quit`
